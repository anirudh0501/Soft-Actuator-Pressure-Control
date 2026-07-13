#define INFLATE_PIN  25
#define DEFLATE_PIN  26
#define PRESSURE_PIN 34

float target_kpa = 30.0;
float h_on = 2.0;  // switch ON threshold — tune this

// State tracking (paper uses separate on/off logic)
bool inflating  = false;
bool deflating  = false;

float readPressure() {
  float gpioVoltage   = analogReadMilliVolts(PRESSURE_PIN) / 1000.0;
  float sensorVoltage = gpioVoltage * (32.0 / 22.0);
  float pressure      = ((sensorVoltage - 0.5) / 4.0) * 200.0;
  if (pressure < 0)   pressure = 0;
  if (pressure > 200) pressure = 200;
  return pressure;
}

void setup() {
  Serial.begin(115200);
  analogSetAttenuation(ADC_11db);
  pinMode(INFLATE_PIN, OUTPUT);
  pinMode(DEFLATE_PIN, OUTPUT);
  digitalWrite(INFLATE_PIN, LOW);
  digitalWrite(DEFLATE_PIN, LOW);
}

void loop() {
  float pressure = readPressure();
  float error    = target_kpa - pressure;

  // ── Charge valve logic (Eq. 1a, 1b) ──
  if (error > h_on) {
    inflating = true;   // Eq. 1a: switch on
  } else if (error <= 0) {
    inflating = false;  // Eq. 1b: switch off
  }

  // ── Discharge valve logic (Eq. 2a, 2b) ──
  if (error < -h_on) {
    deflating = true;   // Eq. 2a: switch on
  } else if (error >= 0) {
    deflating = false;  // Eq. 2b: switch off
  }

  // ── Apply valve states (never both on) ──
  if (inflating && !deflating) {
    digitalWrite(INFLATE_PIN, HIGH);
    digitalWrite(DEFLATE_PIN, LOW);
  } else if (deflating && !inflating) {
    digitalWrite(INFLATE_PIN, LOW);
    digitalWrite(DEFLATE_PIN, HIGH);
  } else {
    digitalWrite(INFLATE_PIN, LOW);
    digitalWrite(DEFLATE_PIN, LOW);
  }

  Serial.print("Target: ");   Serial.print(target_kpa);
  Serial.print(" | Pressure: "); Serial.print(pressure, 2);
  Serial.print(" | Error: ");    Serial.print(error, 2);
  Serial.print(" | State: ");
  Serial.println(inflating ? "INFLATE" : deflating ? "DEFLATE" : "HOLD");

  delay(20);
}
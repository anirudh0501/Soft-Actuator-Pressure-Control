#define INFLATE_PIN  25
#define DEFLATE_PIN  26
#define PRESSURE_PIN 34

float target_kpa = 30.0;
float h = 5.0; // hysteresis band in kPa — tune this

float readPressure() {
  float gpioVoltage = analogReadMilliVolts(PRESSURE_PIN) / 1000.0;
  float sensorVoltage = gpioVoltage * (32.0 / 22.0);
  float pressure = ((sensorVoltage - 0.5) / 4.0) * 200.0;
  if (pressure < 0) pressure = 0;
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
  float error = target_kpa - pressure;

  if (error > h) {
    digitalWrite(INFLATE_PIN, HIGH);
    digitalWrite(DEFLATE_PIN, LOW);
  } else if (error < -h) {
    digitalWrite(INFLATE_PIN, LOW);
    digitalWrite(DEFLATE_PIN, HIGH);
  } else {
    digitalWrite(INFLATE_PIN, LOW);
    digitalWrite(DEFLATE_PIN, LOW);
  }

  Serial.print("Target: "); Serial.print(target_kpa);
  Serial.print(" | Pressure: "); Serial.print(pressure, 2);
  Serial.print(" | Error: "); Serial.println(error, 2);

  delay(20);
}
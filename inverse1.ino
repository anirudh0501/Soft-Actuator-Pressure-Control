#define INFLATE_PIN  25
#define DEFLATE_PIN  26
#define PRESSURE_PIN 34

// Active LOW relay
#define VALVE_ON  LOW
#define VALVE_OFF HIGH

float readPressure() {
  float gpioVoltage   = analogReadMilliVolts(PRESSURE_PIN) / 1000.0;
  float sensorVoltage = gpioVoltage * (32.0 / 22.0);
  float pressure      = ((sensorVoltage - 0.5) / 4.0) * 200.0;
  if (pressure < 0)   pressure = 0;
  if (pressure > 200) pressure = 200;
  return pressure;
}

void deflateToZero() {
  Serial.println("Deflating...");
  digitalWrite(DEFLATE_PIN, VALVE_ON);
  delay(5000);
  digitalWrite(DEFLATE_PIN, VALVE_OFF);
  delay(2000);
}

void inflateToReference(float ref_kpa) {
  Serial.print("Inflating to "); Serial.println(ref_kpa);
  while (readPressure() < ref_kpa) {
    digitalWrite(INFLATE_PIN, VALVE_ON);
    delay(200);
    digitalWrite(INFLATE_PIN, VALVE_OFF);
    delay(500);
  }
  delay(2000);
}

float measureStep(int pin, unsigned long ton_ms) {
  delay(1000);
  float p_before = readPressure();
  Serial.print("Before: "); Serial.println(p_before);

  digitalWrite(pin, VALVE_ON);
  delay(ton_ms);
  digitalWrite(pin, VALVE_OFF);

  delay(2000);
  float p_after = readPressure();
  Serial.print("After: "); Serial.println(p_after);

  return p_after - p_before;
}

void setup() {
  Serial.begin(115200);
  analogSetAttenuation(ADC_11db);
  pinMode(INFLATE_PIN, OUTPUT);
  pinMode(DEFLATE_PIN, OUTPUT);
  digitalWrite(INFLATE_PIN, VALVE_OFF);
  digitalWrite(DEFLATE_PIN, VALVE_OFF);
  delay(3000);

  int on_times[] = {20, 40, 60, 80, 100, 150, 200, 300, 400};
  int n = 9;

  // ── INFLATE calibration ──
  Serial.println("=== INFLATE ===");
  Serial.println("ton_ms,delta_kpa");
  for (int i = 0; i < n; i++) {
    deflateToZero();
    float delta = measureStep(INFLATE_PIN, on_times[i]);
    Serial.print(on_times[i]);
    Serial.print(",");
    Serial.println(delta, 2);
  }

  // ── DEFLATE calibration ──
  Serial.println("=== DEFLATE ===");
  Serial.println("ton_ms,delta_kpa");
  for (int i = 0; i < n; i++) {
    inflateToReference(40.0);
    float delta = measureStep(DEFLATE_PIN, on_times[i]);
    Serial.print(on_times[i]);
    Serial.print(",");
    Serial.println(delta, 2);
  }

  Serial.println("=== DONE ===");
}

void loop() {}
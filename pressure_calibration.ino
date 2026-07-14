const int PRESSURE_PIN = 34;

void setup() {
  Serial.begin(115200);
  analogSetAttenuation(ADC_11db);
}

void loop() {
  float gpioVoltage = analogReadMilliVolts(PRESSURE_PIN) / 1000.0;
  float sensorVoltage = gpioVoltage * (32.0 / 22.0);

  float pressure = ((sensorVoltage - 0.5) / 4.0) * 200.0;

  // Clamp the value
  if (pressure < 0) pressure = 0;
  if (pressure > 200) pressure = 200;

  Serial.print("Pressure: ");
  Serial.print(pressure, 2);
  Serial.println(" kPa");

  delay(200);
}
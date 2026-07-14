const int relayPin = 25;

void setup() {
  pinMode(relayPin, OUTPUT);

  // Start with relay OFF
  digitalWrite(relayPin, HIGH);   // Many modules are active LOW
}

void loop() {
  digitalWrite(relayPin, LOW);    // Relay ON
  delay(1000);

  digitalWrite(relayPin, HIGH);   // Relay OFF
  delay(1000);
}
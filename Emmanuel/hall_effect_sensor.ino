int hallPin = 2; // Digital pin connected to the Hall sensor's output


void setup() {
  pinMode(hallPin, INPUT); // Set the digital pin as input
  Serial.begin(9600); // Start serial communication at 9600 baud
}

void loop() {
  int hallState = digitalRead(hallPin); // Read the state of the Hall sensor
  if (hallState == HIGH) {
    Serial.println("Magnet detected!");
  } else {
    Serial.println("No magnet detected.");
  }
  delay(1000); // Delay for a second
}

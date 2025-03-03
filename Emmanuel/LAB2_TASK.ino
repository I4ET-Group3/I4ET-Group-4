char Turn_On = 'Z';
char Turn_Off = 'H';
char received_input;


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);

}

void loop() {
  if (Serial.available() > 0) {  // Check if there is something to read
    received_input = Serial.read();  // Read the incoming byte
    Serial.println(received_input);  // Echo the received byte back to the Serial Monitor

    if (received_input == Turn_On) {
      digitalWrite(LED_BUILTIN, LOW);  // Turn LED on
      delay(3000);
    } else if (received_input == Turn_Off) {
      digitalWrite(LED_BUILTIN, HIGH);  // Turn LED off
    }
  }
}


int MQ2_PIN = D2;    // Digital pin for smoke sensor
int GREEN_LED = D5;  // Green LED pin (No smoke)
int RED_LED = D6;    // Red LED pin (Smoke detected)
int BUZZER = D7;     // Buzzer pin (Alarm)

bool smokeDetected = false;

void setup() {
    Serial.begin(115200);
    
    pinMode(MQ2_PIN, INPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    pinMode(BUZZER, OUTPUT);

    // Default: No Smoke, Green LED ON
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);
    
    Serial.println("Smoke Detection System");
}

void loop() {
    int sensorValue = digitalRead(MQ2_PIN);  // Read smoke sensor

    if (sensorValue == HIGH) { // Smoke detected
    
            Serial.println("Smoke Detected! Alarm Activated!");
            digitalWrite(GREEN_LED, LOW);
            digitalWrite(RED_LED, HIGH);
            digitalWrite(BUZZER, HIGH);
    
    } else { // No smoke
        
            Serial.println("Air is Clean. System Normal.");
            digitalWrite(GREEN_LED, HIGH);
            digitalWrite(RED_LED, LOW);
            digitalWrite(BUZZER, LOW);
    }
    delay(1000); 
}

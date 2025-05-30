#include <Servo.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

Servo motor1ESC;  
Servo motor2ESC;  

const int ESC_Motor1_Pin = D7;
const int ESC_Motor2_Pin = D6;  
const int relayPin = D2;  

int velocityMotor1 = 1500;  
int velocityMotor2 = 1500;
int leftProp = 5;
int rightProp = 5;  
bool batteryOn = false;  
bool engState = false;
bool batState = false;


// Wi-Fi credentials
const char* ssid     = "DIGIFIBRA-eK9t";
const char* password = "qjaH49AUhvN9";

// MQTT broker info
const char* mqtt_server     = "test.mosquitto.org";
const int   mqtt_port       = 1883;
const char* clientId        = "i4etG3";
const char* mqtt_pub_topic  = "esp8266/engines";


const char* sub_topic_engOnOff  = "engines/on-off";
const char* sub_topic_batOnOff  = "bat/on-off";
const char* sub_topic_leftProp  = "engines/left-prop";
const char* sub_topic_rightProp = "engines/right-prop";


WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n¡WiFi connected!");
}

void reconnect() {
    while (!client.connected()) {
        Serial.println("Connecting to the MQT broker...");
        if (client.connect(clientId)) {
            Serial.println("Connected to the MQTT broker.");
            client.subscribe(sub_topic_engOnOff);
            client.subscribe(sub_topic_batOnOff);
            client.subscribe(sub_topic_leftProp);
            client.subscribe(sub_topic_rightProp);

        } else {
            Serial.print("Connection error: ");
            Serial.println(client.state());
            Serial.println("Trying again in 5 seconds...");
            delay(5000);
        }
    }
}

String msgReceived;
void callback(char* topic, byte* payload, unsigned int length) {
    msgReceived = ""; 
    for (int i = 0; i < length; i++) {
        msgReceived += (char)payload[i];
    }
    msgReceived.trim();

    if (strcmp(topic, sub_topic_engOnOff) == 0) {
        if (msgReceived == "ON") {
            engState = true;
        } else if (msgReceived == "OFF") {
            engState = false;
        } else {
            Serial.println("Unknown command");
        }
    } else if (strcmp(topic, sub_topic_batOnOff) == 0) {
        if (msgReceived == "ON") {
            batState = true;
        } else if (msgReceived == "OFF") {
            batState = false;
        } else {
            Serial.println("Unknown command");
        }
    } else if (strcmp(topic, sub_topic_leftProp) == 0) {
        leftProp = msgReceived.toInt();
    } else if (strcmp(topic, sub_topic_rightProp) == 0) {
        rightProp = msgReceived.toInt();
    } 
}

void setup() {
    Serial.begin(9600);
    setup_wifi();
    client.setServer(mqtt_server,mqtt_port);
    client.setCallback(callback);
    reconnect();

    motor1ESC.attach(ESC_Motor1_Pin);
    motor2ESC.attach(ESC_Motor2_Pin);
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, LOW); 

    motor1ESC.writeMicroseconds(1500);
    motor2ESC.writeMicroseconds(1500);
}

void loop() {

    if (!client.connected()) {
        reconnect();
    }
    client.loop();


    if (engState) {

        velocityMotor1 = map(leftProp,0,100,1500,1900);
        Serial.println(velocityMotor1);
         
        velocityMotor2 = map(rightProp,0,100,1500,1900);
        Serial.println(velocityMotor2);
 
        if (batState) {
            digitalWrite(relayPin, HIGH);
            batteryOn = true;
            Serial.println("Battery connected");
        } else {
            digitalWrite(relayPin, LOW);
            batteryOn = false;
            Serial.println("Battery disconnected");
        } 

    } else {
        velocityMotor1 = 1500;
        velocityMotor2 = 1500;
        Serial.println("Engines Off");
    }

    motor1ESC.writeMicroseconds(velocityMotor1);
    motor2ESC.writeMicroseconds(velocityMotor2);
    
    char payload[128];  
    snprintf(payload, sizeof(payload),  
        "{\"Left_Prop_RPM\":%d,\"Right_Prop_RPM\":%d}",
        velocityMotor1, velocityMotor2);  
    client.publish(mqtt_pub_topic, payload);
}
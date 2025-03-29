#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "DIGIFIBRA-eK9t";
const char* password = "qjaH49AUhvN9";

// MQTT Broker configuration
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic_in = "devices/LAB5-IN";
const char* mqtt_topic_out = "devices/LAB5-OUT";

// Devices Pins
const int ledPin = LED_BUILTIN;
const int buzzerPin = D1;
const int flamePin = D2;

// Global Variables
WiFiClient espClient;
PubSubClient client(espClient);
char messageToSend[5];       //  Message to send
bool waitingForResponse = true; // Control the logical flow between publication and reception
bool responseReceived = false;  // Check if the message was received in the callback
String receivedMessage;         // Save the received message

// WiFi connection
void setup_wifi() {
    Serial.println();
    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\n¡WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

// Retry connection to the MQTT broker
void reconnect() {
    while (!client.connected()) {
        Serial.print("Connecting to the MQT broker...");
        if (client.connect("nodoLAB5-IN")) {
            Serial.println("Connected to the MQTT broker.");
            client.subscribe(mqtt_topic_out); // Subscribe to the output topic
            Serial.println("Subscribed to output MQTT topic.");
        } else {
            Serial.print("Connection error: ");
            Serial.println(client.state());
            Serial.println("Trying again in 5 seconds...");
            delay(5000);
        }
    }
}

// Callback to process received messages
void callback(char* topic, byte* payload, unsigned int length) {
    receivedMessage = "";
    for (int i = 0; i < length; i++) {
        receivedMessage += (char)payload[i];
    }

    // Validate whether the received message is "1" or "0"
    if (receivedMessage == "1" || receivedMessage == "0") {
        Serial.print("Message received: ");
        Serial.println(receivedMessage);

        // Update reception flag
        responseReceived = true;
    } else {
        Serial.println("Unexpected response from the broker, ignoring...");
    }
}

// Initial setup
void setup() {
    Serial.begin(9600);

    // Pin configuration
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);
    pinMode(buzzerPin, OUTPUT);
    pinMode(flamePin, INPUT);

    // Connect to WiFi
    setup_wifi();

    // Configuring the MQTT client
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);

    // Connect to the MQTT broker
    reconnect();
}

// Main Loop
void loop() {
    if (!client.connected()) {
        reconnect();
    }
    // Maintain connection and process incoming messages
    client.loop();

    // Step 1: Generate and publish a new value
    if (waitingForResponse) {
        int valueToSend = digitalRead(flamePin); // Read the digital sensor value
        sprintf(messageToSend, "%u", valueToSend);

        Serial.print("Generating and publishing messages: ");
        Serial.println(messageToSend);

        client.publish(mqtt_topic_in, messageToSend); // Publish message to MQTT topic
        waitingForResponse = false; // Wait for the answer
        responseReceived = false;  // Reset reception flag
    }

    // Step 2: Wait for and process the message received in the callback
    if (!waitingForResponse && responseReceived) {
        Serial.print("Processing response received: ");
        Serial.println(receivedMessage);

        // Actions based on the response
        if (receivedMessage == "1") {
            Serial.println("LED ON");
            digitalWrite(ledPin, LOW); // Turn on LED as an indicator
            tone(buzzerPin,1000);
        } else if (receivedMessage == "0") {
            Serial.println("LED OFF.");
            digitalWrite(ledPin, HIGH); // Turn off LED as an indicator
            noTone(buzzerPin);
        }

        waitingForResponse = true; // Ready to start a new cycle
        Serial.println("...............");
        delay(1000); // Pause to observe the cycle on the serial monitor
    }
}
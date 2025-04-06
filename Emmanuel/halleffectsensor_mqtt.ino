#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Wi-Fi credentials
const char* ssid = "Emmanuel's S24 Ultra";
const char* password = "123456789";

// MQTT broker details
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* subscribeTopic = "sensor/hall";
const char* publishTopic = "sensor/hall";

WiFiClient espClient;
PubSubClient client(espClient);

#define RELAY_PIN D1       // Relay to power Hall Effect sensor
#define HALL_SENSOR_PIN D5 // Digital pin connected to Hall sensor

bool sensorPowered = true;
unsigned long lastMsg = 0;
const long interval = 2000; // Publish every 2 seconds when ON

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Message received: ");
  Serial.println(message);

  if (message == "ON") {
    digitalWrite(RELAY_PIN, HIGH);
    sensorPowered = true;
    Serial.println("Sensor powered ON");
  } else if (message == "OFF") {
    digitalWrite(RELAY_PIN, LOW);
    sensorPowered = false;
    Serial.println("Sensor powered OFF");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client_Hall")) {
      Serial.println("connected");
      client.subscribe(subscribeTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}

void setup_wifi() {
  delay(10);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected. IP: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(HALL_SENSOR_PIN, INPUT_PULLUP);
  digitalWrite(RELAY_PIN, LOW);  // Start OFF
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (sensorPowered && millis() - lastMsg > interval) {
    lastMsg = millis();
    int sensorState = digitalRead(HALL_SENSOR_PIN);
    const char* state = sensorState == LOW ? "1" : "0"; // LOW = magnet detected
    client.publish(publishTopic, state);
    Serial.print("Published hall sensor: ");
    Serial.println(state);
  }
}

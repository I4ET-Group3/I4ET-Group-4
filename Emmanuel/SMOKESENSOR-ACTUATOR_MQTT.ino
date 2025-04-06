#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Wi-Fi credentials
const char* ssid = "Emmanuel's S24 Ultra";
const char* password = "123456789";

// MQTT broker details
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* subscribeTopic = "sensor/mq135/control";  // Receive ON/OFF
const char* publishTopic   = "sensor/mq135/data";     // Publish readings

WiFiClient espClient;
PubSubClient client(espClient);

#define RELAY_PIN D1       // Relay to power MQ135
#define MQ135_PIN A0       // Analog pin for MQ135 sensor

bool sensorPowered = true;
unsigned long lastMsg = 1;
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
    Serial.println("MQ135 powered ON");
  } else if (message == "OFF") {
    digitalWrite(RELAY_PIN, LOW);
    sensorPowered = false;
    Serial.println("MQ135 powered OFF");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client_MQ135")) {
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
  digitalWrite(RELAY_PIN, HIGH);  // Start ON
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
    int mqValue = analogRead(MQ135_PIN);  // Read analog value
    char payload[10];
    snprintf(payload, sizeof(payload), "%d", mqValue);
    client.publish(publishTopic, payload);
    Serial.print("Published MQ135 value: ");
    Serial.println(payload);
  }
}

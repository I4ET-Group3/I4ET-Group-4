#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// WiFi credentials
const char* ssid = "GalaxyM21";
const char* password = "123456789";

// MQTT broker settings
const char* mqttServer = "test.mosquitto.org";
const int mqttPort = 1883;
const char* mqttUser = "";         
const char* mqttPassword = "";     

WiFiClient espClient;
PubSubClient client(espClient);

// Pins
const int LED_R = D2;
const int oneWireBus = D1; // DS18B20 data line

// DS18B20 setup
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

// MQTT topics
const char* mqttPubTopic = "devices/sensor/ESP8266_wifi/temperature_status";
const char* mqttSubTopic = "devices/sensor/ESP8266_wifi/control001"; // FlowFuse control

// Handle incoming MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Received MQTT control: ");
  Serial.println(message);

  if (message == "1") {
    digitalWrite(LED_R, HIGH);
    Serial.println("Remote ON: LED");
  } else if (message == "0") {
    digitalWrite(LED_R, LOW);
    Serial.println("Remote OFF: LED");
  } else {
    Serial.println("Unknown command");
  }
}

void setup() {
  pinMode(LED_R, OUTPUT);
  Serial.begin(115200);
  delay(500);

  // WiFi connection
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // MQTT setup
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("esp8266_sensor_client", mqttUser, mqttPassword)) {
      Serial.println("Connected to MQTT!");
      client.subscribe(mqttSubTopic);
    } else {
      Serial.print("MQTT connection failed, rc=");
      Serial.print(client.state());
      Serial.println(". Retrying...");
      delay(2000);
    }
  }

  sensors.begin(); // Start temperature sensor
}

void loop() {
  client.loop(); // Handle incoming MQTT messages

  // Read and publish temperature
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);

  String msg;
  if (tempC > 50.0) {
    msg = "High temperature detected!";
    digitalWrite(LED_R, HIGH);
  } else {
    msg = "Temperature normal.";
  }

  Serial.print("Temperature: ");
  Serial.print(tempC);
  Serial.println(" °C");

  client.publish(mqttPubTopic, msg.c_str());

  delay(3000); // Wait before next read
}

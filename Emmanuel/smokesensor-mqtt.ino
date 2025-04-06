#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Hall effect sensor pin (digital)
#define HALL_PIN D5  // Or GPIO14

// WiFi credentials
const char* ssid = "Emmanuel's S24 Ultra";
const char* password = "123456789";

// MQTT broker info
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* topic = "sensor/hall";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
const long interval = 2000;  // Publish every 2 seconds

void setup_wifi() {
  delay(10);
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi connection failed. Restarting...");
    ESP.restart();
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(HALL_PIN, INPUT_PULLUP);  // Enable internal pull-up
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > interval) {
    lastMsg = now;

    int hallValue = digitalRead(HALL_PIN);
    const char* state = (hallValue == LOW) ? "Magnet Detected" : "No Magnet";

    char msg[50];
    sprintf(msg, "Hall Sensor: %s", state);
    Serial.print("Publishing: ");
    Serial.println(msg);
    client.publish(topic, msg);
  }
}


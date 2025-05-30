#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// —— PIN MAPPINGS ——
#define SMOKE_PIN     A0    // analog smoke sensor
#define MAG_PIN       D0    // magnetic pickup
#define GREEN_LED     D5
#define RED_LED       D6
#define BUZZER_PIN    D7
#define FLAME_SENSOR  D4
#define DHTPIN        D3     // GPIO3 (RX)
#define DHTTYPE       DHT11
#define RELAY         D8

// Wi-Fi credentials
const char* ssid     = "udcdocencia";
const char* password = "SenFios.Especial";

// MQTT broker info
const char* mqtt_server     = "test.mosquitto.org";
const int   mqtt_port       = 1883;
const char* mqtt_sub_topic  = "sensor/hall";
const char* mqtt_pub_topic  = "esp8266/monitor";

WiFiClient     espClient;
PubSubClient   client(espClient);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // adjust 0x27 if your backpack uses a different address
DHT dht(DHTPIN, DHTTYPE);

// RPM measurement
volatile unsigned long pulseCount = 0;
unsigned long lastMeasure = 0;
unsigned int  rpm         = 0;

// Interrupt routine for hall sensor pulses
void ICACHE_RAM_ATTR onPulse() {
  pulseCount++;
}

void setup_wifi() {
  Serial.print("\nConnecting to ");
  Serial.print(ssid);
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
    Serial.println("\nFailed to connect – rebooting");
    ESP.restart();
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection…");
    String clientId = "ESP8266Client-";
    clientId += String(ESP.getChipId(), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(mqtt_sub_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("; retrying in 5s");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // LCD init
  Wire.begin(D2, D1);
  lcd.init();
  lcd.backlight();

  // Wi-Fi + MQTT
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);

  // I/O setup
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(FLAME_SENSOR, INPUT);
  pinMode(MAG_PIN, INPUT_PULLUP);
  pinMode(RELAY, OUTPUT);

  // Hall-sensor interrupt
  attachInterrupt(digitalPinToInterrupt(MAG_PIN), onPulse, FALLING);

  dht.begin();
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  // RPM Calculation
  unsigned long now = millis();
  if (now - lastMeasure >= 1000) {
    noInterrupts();
    unsigned long count = pulseCount;
    pulseCount = 0;
    interrupts();

    rpm = count * 60;
    lastMeasure = now;
  }

  // Read sensors
  int smokeRaw       = analogRead(SMOKE_PIN);
  int smokePct       = map(smokeRaw, 0, 1023, 0, 100);
  int flameDetected = digitalRead(FLAME_SENSOR);
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Serial output
  Serial.print("Smoke: ");   Serial.print(smokePct);   Serial.print("%\t");
  Serial.print("RPM: ");     Serial.print(rpm);        Serial.print("\t");
  Serial.print("Flame: ");   Serial.println(flameDetected ? "FIRE" : "No Fire");
  Serial.print("Temperature: ");   Serial.println(t);
  Serial.print("Humidity: ");      Serial.println(h);

  // LCD display
  lcd.setCursor(0, 1);
  lcd.printf("Smoke:%3d%%", smokePct);
  lcd.setCursor(0, 0);
  lcd.printf("T:%.1fC H:%.1f%%", t, h);

  // Alerts
  if (smokePct > 50) {
    Serial.println("ALERT: Smoke > 50%!, COOLING SYSTEM ACTIVATED");
    digitalWrite(RED_LED, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RELAY, HIGH);
  }
  else if (flameDetected == HIGH) {
    Serial.println("ALERT: FIRE detected! SPRINKLER ACTIVATED");
    digitalWrite(RED_LED, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RELAY, HIGH);
  }
  else if (t > 30) {
    Serial.println("ALERT: High Temperature detected  COOLING SYSTEM ACTIVATED");
    digitalWrite(RED_LED, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RELAY, HIGH);
  }
  else {
    Serial.println("✅ Status: All normal.");
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RELAY, LOW);
  }

  // Publish JSON payload
  char payload[128];
  snprintf(payload, sizeof(payload),
           "{\"Temperature\":%.1f,\"Humidity\":%.1f,\"smoke\":%d,\"rpm\":%d,\"flame\":\"%s\"}",
           t, h, smokePct, rpm, flameDetected ? "FIRE" : "NO_FIRE");
  client.publish(mqtt_pub_topic, payload);

  delay(1000);
}

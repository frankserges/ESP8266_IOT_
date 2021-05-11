#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Pins Config
const int LightPin = D0;
//const int PhotoPin = A0;
const int DefaultPin = D4;
int DruckSensorval;

// Wifi Config
const char* ssid = "4G-WiFi-208C";
const char* password = "12345678";

// MQTT Config
const char* mqtt_server = "mqtt.tingg.io";
const int port = 1883; // 8883 for mqtts connections.

// Thing Config
const char* thing_id = "c447ce1e-0eec-47c5-8e72-aa7dc30852a0";
const char* thing_key = "s3haunylbp5b618k1ogpo12m8qwusat9";

// Topics Config
const char* pubTopic = "Drucksensor";
const char* subTopic = "<sub_resource>";


// Default Config
const char* username = "thing";

WiFiClient espClient;
PubSubClient client(espClient);

// Vars
int val;
int prevVal = 0;
char buf[12];
long lastUpdateMillis = 0;


// Function to validate connection with the board
  void blinking_loop(){
    while (ssid == "SSID") {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
  }
}

void setup_wifi() {
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.print("Congratulations!! WiFi connected on IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
  Serial.println("Now, follow up on the steps for the MQTT configuration. ");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting to connect MQTT...");
    if (client.connect(thing_id, username, thing_key)) {
      Serial.println("connected");
      client.subscribe(subTopic);
    } else {
      Serial.print(" Still not connected...");      // Serial.print(client.state());
      Serial.println(" trying again in 5 seconds"); // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

String message(byte* payload, unsigned int length) {
  payload[length] = '\0';
  String s = String((char*)payload);
  return s;
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println(topic);
  String msg = message(payload, length);

  if ((msg == "ON") || (msg == "on") || (msg == "1")) {
    Serial.println("Light turned On");
    digitalWrite(LightPin,HIGH);
  }
  else if ((msg == "OFF") || (msg == "off") || (msg == "0")) {
    Serial.println("Light turned Off");
    digitalWrite(LightPin,LOW);
  }
  else {
    Serial.println("Unknown command. Please try: ON, on, 1, OFF, off or 0.");
  }
}

void setup() {
//  pinMode(LightPin,OUTPUT);
//  pinMode(PhotoPin,INPUT);
//  pinMode(DefaultPin,OUTPUT);
  pinMode(A0, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  blinking_loop();
  setup_wifi();
  client.setServer(mqtt_server, port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  DruckSensorval= analogRead(A0);
 // val = analogRead(PhotoPin);

  if (prevVal != DruckSensorval || millis() - lastUpdateMillis > 3000) {
    lastUpdateMillis = millis();
    prevVal = DruckSensorval;
    client.publish(pubTopic,itoa(DruckSensorval, buf, 10));
    Serial.print("Updating value to ");
    Serial.println(DruckSensorval);
    delay(250);
  }
  delay(50);
}

/*
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <stdlib.h>
#include <time.h>
#include <AccelStepper.h>


// создание экземпляра объекта:
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11
AccelStepper s28BYJ-48(8, IN1, IN3, IN2, IN4);


// Update these with values suitable for your network.

const char* ssid = "wi-fi-2G";
const char* password = "q18R29B_star_star";
const char* mqtt_server = "hairdresser.cloudmqtt.com";
const int mqtt_port = 16526;
const char* mqtt_user="xhogwvvx";
const char* mqtt_pass="MRvu0A3txMIw";
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
char charBuffer[10];
String strData;
bool debug=1;
#define MSG_BUFFER_SIZE  (20)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
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
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

int hexToDec(String hexString)
{
  unsigned int decValue = 0;
  int nextInt;

  for (int i = 0; i < hexString.length(); i++)
  {
    nextInt = int(hexString.charAt(i));
    if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
    if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
    if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
    nextInt = constrain(nextInt, 0, 15);
    decValue = (decValue * 16) + nextInt;
  }
  return decValue;
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
    // Initialize the BUILTIN_LED pin as an output
  Serial1.begin(115200); //UART для связи с NodeMCU wiFi 
  Serial.begin(115200); //Основной uart для связи с компьютером
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  s28BYJ-48.setMaxSpeed(900.0);
  s28BYJ-48.setAcceleration(100.0);
  s28BYJ-48.setSpeed(200);
  s28BYJ-48.moveTo(2000);
  
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
  }


  if (strData !="") {
    strData.trim();
    Serial.print("Принято от MQTT@"+strData+"LEN"+strData.length()+"@");
    String tmp=strData;
    int pos1=tmp.indexOf('=');
    Serial.println("pos1="+String(pos1));
    int pos2=tmp.indexOf('=',2);
    Serial.println("pos2="+String(pos2));
    String topic=strData.substring(pos1+1,pos2); //нарезаем до решетки топик
    String value="";
    Serial.println("topic["+topic+"]");
  }
}

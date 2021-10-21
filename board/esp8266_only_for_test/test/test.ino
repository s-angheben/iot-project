
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define LED 16

#define OPEN 1
#define CLOSE 0

struct s_info {
  bool water;
  int operationMode;
  int umidity;
  float temperature;
  float light;
} state;


const char* ssid = "uaifai";
const char* password = "passwd";
const char* mqtt_server = "192.168.1.6";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (100)
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

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();


  if ((char)payload[0] == 'w') {
    if ((char)payload[1] == '1') {
      digitalWrite(LED, HIGH);
      state.water = 1;
    } else {
      digitalWrite(LED, LOW);
      state.water = 0;
    }  
  } else if ((char)payload[0] == 'm') {
    if ((char)payload[1] == '1') {
      state.operationMode = 1;
    } else if ((char)payload[1] == '2') {
      state.operationMode = 2;
    } else if ((char)payload[1] == '3') {
      state.operationMode = 3;
    }
  } 

  snprintf (msg, MSG_BUFFER_SIZE, "{\"water\":%d,\"mode\":%d,\"umidity\":%d, \"temperature\":%f, \"light\":%f}", state.water, state.operationMode, state.umidity, state.temperature, state.light);
  client.publish("infoTopic", msg);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("infoTopic", "hello world");
      // ... and resubscribe
      client.subscribe("controlTopic");
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

  state.water = 0;
  state.operationMode = 1;
  state.umidity = 10;
  state.temperature = 19.8;
  state.light = 20;
  
  pinMode(LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();



  unsigned long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "{\"water\":%d,\"mode\":%d,\"umidity\":%d, \"temperature\":%f, \"light\":%f}", state.water, state.operationMode, state.umidity, state.temperature, state.light);
  //  Serial.print("Publish message: ");
  //  Serial.println(msg);
    client.publish("infoTopic", msg);
  }
}

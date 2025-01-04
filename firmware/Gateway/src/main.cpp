#include <Arduino.h>
#include <Firebase_ESP_Client.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "config.h"

// Token generation and RTDB helper functions
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Display Configuration
Adafruit_SSD1306 display1(-1);
Adafruit_SSD1306 display2(-1);
#define OLED1 0x3C
#define OLED2 0x3D

// Firebase Objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

String databasePath;
String tempPath;
String humPath;
String flamePath;
String COPath;

// Sensor Data Structure
typedef struct {
  uint8_t node_id;
  float temperature;
  float humidity;
  bool flame_detected;
  bool co_detected;
} SensorData;

SensorData data;

// Control Variables
bool dataReady = false;
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 10000;

// WiFi Initialization
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

// ESP-NOW Initialization
void initESPNow() {
  WiFi.mode(WIFI_STA);
  esp_wifi_set_ps(WIFI_PS_NONE);
  esp_wifi_set_max_tx_power(78);
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESP-NOW initialized");
  } else {
    Serial.println("ESP-NOW initialization failed");
  }
}

// Firebase Initialization
void initFirebase() {
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);
  config.token_status_callback = tokenStatusCallback;
  config.max_token_generation_retry = 5;
  Firebase.begin(&config, &auth);
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
}

// Send Float to Firebase
void sendFloat(String path, float value) {
  if (Firebase.RTDB.setFloat(&fbdo, path.c_str(), value)) {
    Serial.println("PASSED");
  } else {
    Serial.print("FAILED: ");
    Serial.println(fbdo.errorReason());
  }
}

// Callback for Receiving Data via ESP-NOW
void onDataRecv(const uint8_t *recvInfo, const uint8_t *incomingData, int len) {
  memcpy(&data, incomingData, sizeof(data));
  
  Serial.printf("\n=== Sensor Data from Node %d ===\n", data.node_id);
  Serial.printf("Temperature: %.2f C\n", data.temperature);
  Serial.printf("Humidity: %.2f %%\n", data.humidity);
  Serial.printf("Flame: %s\n", data.flame_detected ? "Detected" : "Not Detected");
  Serial.printf("CO: %s\n", data.co_detected ? "Danger" : "Safe");
  
  dataReady = true;
  databasePath = "/Node_";
  databasePath += String(data.node_id);
  tempPath = databasePath;
  tempPath += "/temperature";
  humPath = databasePath;
  humPath += "/humidity";
  flamePath = databasePath ;
  flamePath += "/flame";
  COPath = databasePath ;
  COPath += "/CO";
}

// Send request to Node
void sendRequestToNode(uint8_t nodeIndex) {
  uint8_t nodeID = nodeIndex + 1;  // ID của Node
  esp_now_send(nodeMACs[nodeIndex], (uint8_t *)&nodeID, sizeof(nodeID));
  Serial.print("Request sent to Node ");
  Serial.println(nodeID);
}

// Update Display
void updateDisplay(Adafruit_SSD1306 &display, SensorData &data, int node_id) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.printf("Temp %d: %.2f C\n", node_id, data.temperature);
  display.setCursor(0, 8);
  display.printf("Humidity %d: %.2f %%\n", node_id, data.humidity);
  display.setCursor(0, 16);
  display.printf("Flame %d: %s\n", node_id, data.flame_detected ? "Detected" : "Not Detected");
  display.setCursor(0, 24);
  display.printf("CO %d: %s\n", node_id, data.co_detected ? "Danger" : "Safe");
  display.display();
}

// led status
void updateStatusLEDs(int node_id, SensorData &data) {
  if (node_id == 1) {
    if (data.temperature < 30 && !data.flame_detected && !data.co_detected) {
      digitalWrite(LG1, LOW);
      digitalWrite(LR1, HIGH);
      digitalWrite(LY1, HIGH);
    } else if (data.temperature > 30 || data.flame_detected || data.co_detected) {
      digitalWrite(LG1, LOW);
      digitalWrite(LR1, HIGH);
      digitalWrite(LY1, LOW);
    } else {
      digitalWrite(LG1, LOW);
      digitalWrite(LR1, LOW);
      digitalWrite(LY1, HIGH);
    }
  } else if (node_id == 2) {
    if (data.temperature < 30 && !data.flame_detected && !data.co_detected) {
      digitalWrite(LG2, LOW);
      digitalWrite(LR2, HIGH);
      digitalWrite(LY2, HIGH);
    } else if (data.temperature > 30 || data.flame_detected || data.co_detected) {
      digitalWrite(LG2, LOW);
      digitalWrite(LR2, HIGH);
      digitalWrite(LY2, LOW);
    } else {
      digitalWrite(LG2, LOW);
      digitalWrite(LR2, LOW);
      digitalWrite(LY2, HIGH);
    }
  }
}

void setup() {
  Serial.begin(115200);
  initWiFi();
  initESPNow();
  initFirebase();

  // Add Nodes to list
  esp_now_peer_info_t peerInfo = {};
  for (int i = 0; i < NODE_COUNT; i++) {
    memcpy(peerInfo.peer_addr, nodeMACs[i], 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      Serial.print("Failed to add Node ");
      Serial.println(i + 1);
    }
  }
  esp_now_register_recv_cb(onDataRecv);

  display1.begin(SSD1306_SWITCHCAPVCC, OLED1);
  display2.begin(SSD1306_SWITCHCAPVCC, OLED2);
  display1.clearDisplay();
  display2.clearDisplay();

  pinMode(LG1, OUTPUT);
  pinMode(LY1, OUTPUT);
  pinMode(LR1, OUTPUT);
  pinMode(LG2, OUTPUT);
  pinMode(LY2, OUTPUT);
  pinMode(LR2, OUTPUT);
}

void loop() {
  if (Firebase.ready() && dataReady) {
    sendFloat(tempPath, data.temperature);
    sendFloat(humPath, data.humidity);
    sendFloat(flamePath, data.flame_detected);
    sendFloat(COPath, data.co_detected);
    updateDisplay(data.node_id == 1 ? display1 : display2, data, data.node_id);
    updateStatusLEDs(data.node_id, data);
    dataReady = false;
  }
}

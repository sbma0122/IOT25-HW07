//sever code
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLEAdvertising.h>

#define SERVICE_UUID "81a1c112-d7c0-48fc-bc38-7032db57e3f9"

void setup() {
  Serial.begin(115200);
  BLEDevice::init("IoT_E");
  BLEServer*   pServer  = BLEDevice::createServer();
  BLEService*  pService = pServer->createService(SERVICE_UUID);
  pService->start();

  BLEAdvertising* pAdv = BLEDevice::getAdvertising();
  pAdv->addServiceUUID(SERVICE_UUID);
  pAdv->setScanResponse(true);
  pAdv->start();

  Serial.println("BLE Server advertising as IoT_E");
}

void loop() {
  delay(1000);
}

//client code(withAdvanced)
#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

const char* WIFI_SSID = "KT_GiGA_E882";
const char* WIFI_PWD  = "private";

#define TARGET_NAME  "IoT_E"
#define TARGET_UUID  "81a1c112-d7c0-48fc-bc38-7032db57e3f9"

//Calculate txPower directly
const int   txPower     = -80;
const float PATH_LOSS   = 2.0;
const int   SCAN_SEC    = 2;
const float ALERT_DIST  = 1.0;
const int   LED_PIN     = 2;

const int   WINDOW_SIZE = 5;
float       distBuf[WINDOW_SIZE];
int         bufIdx = 0;

WiFiServer web(80);
float currentDistance = -1.0;

float estimateDistance(int rssi) {
  return pow(10.0f, float(txPower - rssi) / (10.0f * PATH_LOSS));
}

void doBleScan() {
  BLEScan* scan = BLEDevice::getScan();
  scan->setActiveScan(true);
  BLEScanResults* res = scan->start(SCAN_SEC, false);

  float d = -1;
  for (int i = 0; i < res->getCount(); ++i) {
    auto dev = res->getDevice(i);
    if (dev.haveName() && dev.getName() == TARGET_NAME
      && dev.haveServiceUUID() && dev.isAdvertisingService(BLEUUID(TARGET_UUID))) {
      d = estimateDistance(dev.getRSSI());
      break;
    }
  }
  scan->clearResults();

  distBuf[bufIdx++] = (d > 0 ? d : 0);
  if (bufIdx >= WINDOW_SIZE) bufIdx = 0;
  float sum = 0;
  for (int i = 0; i < WINDOW_SIZE; i++) sum += distBuf[i];
  currentDistance = sum / WINDOW_SIZE;

  digitalWrite(LED_PIN, (currentDistance > 0 && currentDistance <= ALERT_DIST));
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.begin(WIFI_SSID, WIFI_PWD);
  while (WiFi.status() != WL_CONNECTED) delay(200);
  web.begin();

  BLEDevice::init("");
  for (int i = 0; i < WINDOW_SIZE; i++) distBuf[i] = 0;
}

void loop() {
  doBleScan();

  Serial.printf("Smoothed Distance: %.2f m\n", currentDistance);

  WiFiClient client = web.available();
  if (!client) { delay(50); return; }
  while (client.connected() && client.available() && client.read() != '\n');
  client.print(F(
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Connection: close\r\n\r\n"
    "<!DOCTYPE html><html><body>"
  ));
  client.printf("<h1>Distance: %.2f m</h1>", currentDistance);
  if (currentDistance > 0 && currentDistance <= ALERT_DIST) {
    client.print(F("<p style='color:red;'>⚠ Within 1 m</p>"));
  }
  client.print(F("</body></html>"));
  client.stop();
}

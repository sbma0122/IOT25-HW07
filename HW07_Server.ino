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

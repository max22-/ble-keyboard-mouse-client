#include <Arduino.h>
#include "ble_hid_client.h"



BLEHIDClient hid;

void setup() {
  Serial.begin(115200);
  hid.begin("uxn", true, true);
}

void loop() {
  hid.loop();
}
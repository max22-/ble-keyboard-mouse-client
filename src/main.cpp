#include <Arduino.h>
#include "ble_hid_client.h"



BLEHIDClient hid;

void on_key_pressed(uint8_t k) {
  Serial.printf("key 0x%02x pressed !\n", k);
}

void on_key_released(uint8_t k) {
  Serial.printf("key 0x%02x released !\n", k);
}

void setup() {
  Serial.begin(115200);
  hid.begin("uxn", true, true);
  BLEKeyboard& keyboard = hid.get_keyboard();
  keyboard.on_key_pressed(on_key_pressed);
  keyboard.on_key_released(on_key_released);
  keyboard.debug();
}

void loop() {
  hid.loop();
}
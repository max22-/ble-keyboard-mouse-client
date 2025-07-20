#include <Arduino.h>
#include "ble_hid_client.h"

BLEHIDClient hid;

void on_key_pressed(bool is_modifier, uint8_t k) {
  if(is_modifier) {
    Serial.printf("modifier 0x%02x pressed !\n", k);
  } else {
    Serial.printf("key 0x%02x pressed !\n", k);
  }
}

void on_key_released(bool is_modifier, uint8_t k) {
  if(is_modifier) {
    Serial.printf("modifier 0x%02x released !\n", k);
  } else {
    Serial.printf("key 0x%02x released !\n", k);
  }
}

void setup() {
  Serial.begin(115200);
  hid.begin("BLE HID demo", true, true);
  BLEKeyboard& keyboard = hid.get_keyboard();
  keyboard.on_key_pressed(on_key_pressed);
  keyboard.on_key_released(on_key_released);
  keyboard.debug();
}

void loop() {
  hid.loop();
}
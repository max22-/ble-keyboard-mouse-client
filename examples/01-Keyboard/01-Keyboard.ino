#include <Arduino.h>
#include <ble_keyboard_mouse_client.h>

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
  hid.begin("BLE Keyboard demo", true, false);
  BLEKeyboard& keyboard = hid.get_keyboard();
  keyboard.on_key_pressed(on_key_pressed);
  keyboard.on_key_released(on_key_released);
}

void loop() {
  hid.loop();  // this function handles automatic connections / reconnections
}
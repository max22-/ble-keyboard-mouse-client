ble-keyboard-mouse-client
=========================

A library to connect a Bluetooth Low Energy Keyboard and/or Mouse to ESP32, nRF51 and nRF52 series boards. It is based on [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino), i suggest you read its documentation, especially if you are using a nRF board.  
If you find it useful you can <a href='https://ko-fi.com/C0C7NIYDY' target='_blank'><img height='36' style='border:0px;height:36px;' src='https://storage.ko-fi.com/cdn/kofi2.png?v=3' border='0' alt='Buy Me a Coffee at ko-fi.com' /></a> if you want, or sponsor the creator of [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino).  

This library is a work in progress. For the moment, it only handles keyboards, and only reports HID key codes. Mice will be supported in the future. For a list of the key codes, see here: [usb_hid_keys.h](https://gist.github.com/MightyPork/6da26e382a7ad91b5496ee55fdc73db2)

Quick start
-----------

```
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
```

Future work
-----------
- Interpret keycodes, make keyboard inherit from Stream so that we can do: if(keyboard.available()) { char c = keyboard.read(); }
- Add mouse support
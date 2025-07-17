#include <Arduino.h>
#include "ble_keyboard_client.h"

#define FATAL(...) \
  do { \
    Serial.printf("%s:%d: ", __FILE__, __LINE__); \
    Serial.printf(__VA_ARGS__); \
    Serial.println(); \
    for(;;) delay(1000); \
  } while(0)

BLEKeyboardClient kbd;

void setup() {
  Serial.begin(115200);
  kbd.begin("uxn");
  pinMode(0, INPUT_PULLUP);
  while(digitalRead(0) == HIGH);
  if(!kbd.start_scan(5000))
    FATAL("failed to start scan");
  while(kbd.is_scanning()) delay(1000);
  if(!kbd.keyboard_found())
    FATAL("no keyboard found");
  Serial.printf("found keyboard: %s\n", kbd.keyboard_name());
  if(!kbd.connect())
    FATAL("failed to connect to keyboard");
}

void loop() {

}
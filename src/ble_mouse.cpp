#include "ble_hid_client.h"

void BLEMouse::handle_report(uint8_t *report, size_t len) {
    Serial.print("mouse : ");
    for(size_t i = 0; i < len; i++) {
        Serial.printf("0x%02x ", report[i]);
    }
    Serial.println();
}
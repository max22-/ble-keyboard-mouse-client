#include "ble_hid_client.h"

void BLEKeyboard::on_key_pressed(std::function<void (bool, uint8_t)> callback) {
    BLE_HID_DEBUG("setting key pressed callback");
    key_pressed_callback = callback;
    if(key_pressed_callback)
        BLE_HID_DEBUG("success");
    else
        BLE_HID_DEBUG("failure");
}

void BLEKeyboard::on_key_released(std::function<void (bool, uint8_t)> callback) {
    BLE_HID_DEBUG("setting key released callback");
    key_released_callback = callback;
}

void BLEKeyboard::handle_report(uint8_t *report, size_t len) {
    if(len != 8) return;
    Serial.print("keyboard : ");
    for(size_t i = 0; i < len; i++) {
        Serial.printf("0x%02x ", report[i]);
    }
    Serial.println();

    if(report[0] != modifiers_states) {
        uint8_t pressed = ~modifiers_states & report[0];
        uint8_t released = modifiers_states & ~report[0];
        for(int i = 0; i < 8; i++) {
            uint8_t m = 1 << i;
            if(released & m)
                key_released_callback(true, m);
        }
        for(int i = 0; i < 8; i++) {
            uint8_t m = 1 << i;
            if(pressed & m)
                key_pressed_callback(true, m);
        }
        modifiers_states = report[0];
    }

    for(int i = 1; i < ARRAY_SIZE(keys_states);  i++) {
        if(keys_states[i]) {
            bool found = false;
            for(int j = 2; j < len; j++) {
                if(report[j] == i) {
                    found = true;
                    break;
                }
            }
            if(!found) {
                BLE_HID_DEBUG("key 0x%02x released", i);
                keys_states[i] = false;
                if(key_released_callback)
                    key_released_callback(false, i);
                else 
                    BLE_HID_DEBUG("key released callback not set ");
            }
        }
    }
    for(int j = 2; j < len; j++) {
        uint8_t k = report[j];
        if(k != 0 && !keys_states[k]) {
            BLE_HID_DEBUG("key 0x%02x pressed", k);
            keys_states[k] = true;
            if(key_pressed_callback)
                key_pressed_callback(false, k);
            else 
                BLE_HID_DEBUG("key pressed callback not set");
        }
        
    }
}

bool BLEKeyboard::connect(const NimBLEAdvertisedDevice* advDevice) {
    if(!BLEHIDDevice::connect(advDevice))
        return false;
    NimBLERemoteCharacteristic *protocol_mode_chr = pSvc->getCharacteristic(NimBLEUUID("2a4e"));
    if(protocol_mode_chr) {
        if(protocol_mode_chr->writeValue<uint8_t>(0)) {
            BLE_HID_DEBUG("protocol mode set to 'boot'");
        }
        else {
            BLE_HID_DEBUG("failed to set protocol to 'boot'");
            return false;
        }
    } else {
        BLE_HID_DEBUG("protocol mode chararacteristic not found");
        return false;
    }
    return true;
}

void BLEKeyboard::debug() {
    if(key_pressed_callback)
        BLE_HID_DEBUG("key_pressed_callback is set");
    else
        BLE_HID_DEBUG("key_pressed_callback is not set");
}
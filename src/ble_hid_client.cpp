#include "ble_hid_client.h"

void BLEHIDClient::begin(const char *device_name, bool keyboard_enabled, bool mouse_enabled) {
    NimBLEDevice::init(device_name);
    NimBLEDevice::setSecurityAuth(true, true, true);
    NimBLEDevice::setSecurityPasskey(123456);
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_KEYBOARD_ONLY);
    this->keyboard_enabled = keyboard_enabled;
    this->mouse_enabled = mouse_enabled;
    pScan = NimBLEDevice::getScan();
    pScan->setScanCallbacks(&scan_callbacks, false);
    pScan->setInterval(100);
    pScan->setWindow(100);
    pScan->setActiveScan(true);
    if(this->keyboard_enabled || this->mouse_enabled)
        start_scan();
}

void BLEHIDClient::loop() {
    if(!(keyboard_enabled || mouse_enabled)) return;
    const NimBLEScanResults *results = scan_callbacks.results;
    scan_callbacks.results = nullptr;
    if(results && results->getCount() > 0) {
        BLE_HID_DEBUG("found devices!!!");
        for(auto device: *results) {
            bool device_known = NimBLEDevice::getClientByPeerAddress(device->getAddress()) != nullptr;
            if(device_known)
                BLE_HID_DEBUG("the device is already known");
            else
                BLE_HID_DEBUG("the device is unknown");
            if(device->isAdvertisingService(NimBLEUUID("1812")) || device_known) {
                uint16_t appearance = device->getAppearance();
                if(appearance == 0x3c1 && keyboard_enabled && !keyboard.is_connected()) {
                    BLE_HID_DEBUG("found a keyboard: 0x%x", device);
                    keyboard.connect(device);
                } else if(appearance == 0x3c2 && mouse_enabled && !mouse.is_connected()) {
                    BLE_HID_DEBUG("found a mouse: 0x%x", device);
                    mouse.connect(device);
                }
            }
        }
    } else if((keyboard_enabled && !keyboard.is_connected()) || (mouse_enabled && !mouse.is_connected())) {
        start_scan();
    }
}

void BLEHIDClient::start_scan() {
    if(last_scan == 0 || (millis() - last_scan >= BLE_HID_SCAN_PERIOD)) {
        BLE_HID_DEBUG("starting scan");
        last_scan = millis();
        pScan->start(BLE_HID_SCAN_DURATION);
    }
}

void BLEHIDClient::enable_keyboard() {
    keyboard_enabled = true;
}

void BLEHIDClient::disable_keyboard() {
    keyboard_enabled = false;
}

BLEKeyboard& BLEHIDClient::get_keyboard() {
    return keyboard;
}

void BLEHIDClient::enable_mouse() {
    mouse_enabled = true;
}

void BLEHIDClient::disable_mouse() {
    mouse_enabled = false;
}

BLEMouse& BLEHIDClient::get_mouse() {
    return mouse;
}


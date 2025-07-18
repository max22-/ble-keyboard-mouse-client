#ifndef BLE_HID_CLIENT_H
#define BLE_HID_CLIENT_H

#include <NimBLEDevice.h>

class BLEHIDClientScanCallbacks : public NimBLEScanCallbacks {
public:
    const NimBLEAdvertisedDevice* get_device();
private:
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override;
    void onScanEnd(const NimBLEScanResults& results, int reason) override;
    const NimBLEAdvertisedDevice* device = nullptr;
};

class BLEHIDClient {
public:
    void begin(const char *device_name);
    bool start_scan(uint32_t duration);
    bool is_scanning();
    bool keyboard_found();
    const char *keyboard_name();
    bool connect();

protected:
    NimBLEScan *pScan = nullptr;
    BLEHIDClientScanCallbacks scan_callbacks;
    NimBLEClient *pClient = nullptr;
};

#endif
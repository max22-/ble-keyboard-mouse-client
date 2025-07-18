#ifndef BLE_HID_CLIENT_H
#define BLE_HID_CLIENT_H

#include <NimBLEDevice.h>

#define BLE_HID_SCAN_DURATION 5000

class BLEHIDClient;

class BLEHIDClientScanCallbacks : public NimBLEScanCallbacks {
public:
    const NimBLEScanResults* results;
private:
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override;
    void onScanEnd(const NimBLEScanResults& results, int reason) override;
};

class BLEHIDClientCallbacks : public NimBLEClientCallbacks {
public:
    bool connected = false;
private:
    void onConnect(NimBLEClient* pClient) override;
    void onDisconnect(NimBLEClient* pClient, int reason) override;
    void onAuthenticationComplete(NimBLEConnInfo& connInfo) override;
};


class BLEHIDDevice {
protected:
    virtual void handle_report(uint8_t *report, size_t len) = 0;
private:
    bool connect(const NimBLEAdvertisedDevice* advDevice);
    bool is_connected();
    NimBLEAdvertisedDevice *device = nullptr;
    NimBLEClient *pClient = nullptr;
    bool enabled = false;
    friend class BLEHIDClient;
    BLEHIDClientCallbacks callbacks;
};

class BLEKeyboard : public BLEHIDDevice {

private:
    void handle_report(uint8_t *report, size_t len) override;
};

class BLEMouse : public BLEHIDDevice {

private:
    void handle_report(uint8_t *report, size_t len) override;
};

class BLEHIDClient {
public:
    void begin(const char *device_name, bool keyboard_enabled, bool mouse_enabled);
    void loop();

    void enable_keyboard();
    void disable_keyboard();
    BLEKeyboard& get_keyboard();

    void enable_mouse();
    void disable_mouse();
    BLEMouse& get_mouse();

private:

    NimBLEScan *pScan = nullptr;
    BLEHIDClientScanCallbacks scan_callbacks;
    NimBLEClient *pClient = nullptr;

    bool keyboard_enabled = false;
    BLEKeyboard keyboard;

    bool mouse_enabled = false;
    BLEMouse mouse;
};

#define BLE_HID_DEBUG(...) \
  do { \
    Serial.printf("%s:%d: ", __FILE__, __LINE__); \
    Serial.printf(__VA_ARGS__); \
    Serial.println(); \
  } while(0)

#endif
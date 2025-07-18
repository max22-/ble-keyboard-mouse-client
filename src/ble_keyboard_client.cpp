#include "ble_keyboard_client.h"

class ClientCallbacks : public NimBLEClientCallbacks {
    void onConnect(NimBLEClient* pClient) override { Serial.printf("Connected\n"); }

    void onDisconnect(NimBLEClient* pClient, int reason) override {
        Serial.printf("%s Disconnected, reason = %d\n", pClient->getPeerAddress().toString().c_str(), reason);
        //NimBLEDevice::getScan()->start(scanTimeMs, false, true);
    }

    /** Pairing process complete, we can check the results in connInfo */
    void onAuthenticationComplete(NimBLEConnInfo& connInfo) override {
        Serial.println("onAuthenticationComplete");
        Serial.printf("authenticated: %d, bonded: %d, encrypted: %d, key size: %u\n",
            connInfo.isAuthenticated(),
            connInfo.isBonded(),
            connInfo.isEncrypted(),
            connInfo.getSecKeySize()
        );
        if (!connInfo.isEncrypted()) {
            Serial.printf("Encrypt connection failed - disconnecting\n");
            /** Find the client with the connection handle provided in connInfo */
            NimBLEDevice::getClientByHandle(connInfo.getConnHandle())->disconnect();
            return;
        }
    }
} ClientCallbacks;










void BLEKeyboardClientScanCallbacks::onResult(const NimBLEAdvertisedDevice* advertisedDevice) {
    Serial.printf("device found: %s (appearance = 0x%x)\n", advertisedDevice->toString().c_str(), advertisedDevice->getAppearance());
    if(advertisedDevice->isAdvertisingService(NimBLEUUID("1812"))) {
        NimBLEDevice::getScan()->stop();
        device = advertisedDevice;
    }
}

void BLEKeyboardClientScanCallbacks::onScanEnd(const NimBLEScanResults& results, int reason) {
    Serial.println("scan end");
}

const NimBLEAdvertisedDevice* BLEKeyboardClientScanCallbacks::get_device() {
        return device;
}


static void notifyCB(NimBLERemoteCharacteristic* pRemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
    std::string str  = (isNotify == true) ? "Notification" : "Indication";
    str             += " from ";
    str             += pRemoteCharacteristic->getClient()->getPeerAddress().toString();
    str             += ": Service = " + pRemoteCharacteristic->getRemoteService()->getUUID().toString();
    str             += ", Characteristic = " + pRemoteCharacteristic->getUUID().toString();
    //str             += ", Value = " + std::string((char*)pData, length);
    Serial.printf("%s\n", str.c_str());
    for(size_t i = 0; i < length; i++) {
        Serial.printf("0x%02x ", pData[i]);
    }
    Serial.println();
}


void BLEKeyboardClient::begin(const char *device_name) {
    NimBLEDevice::init(device_name);
    NimBLEDevice::setSecurityAuth(false, false, false);
    NimBLEDevice::setSecurityPasskey(123456);
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_KEYBOARD_ONLY);
}


/**
 * @brief Start scanning.
 * @param [in] duration The duration in milliseconds for which to scan. 0 == scan forever.
 * @return True if scan started or false if there was an error.
 */
bool BLEKeyboardClient::start_scan(uint32_t duration) {
    pScan = NimBLEDevice::getScan();
    pScan->setScanCallbacks(&scan_callbacks, false);
    pScan->setInterval(100);
    pScan->setWindow(100);
    pScan->setActiveScan(true);
    Serial.println("starting scan");
    return pScan->start(duration);
}

bool BLEKeyboardClient::is_scanning() {
    return pScan && pScan->isScanning();
}

bool BLEKeyboardClient::keyboard_found() {
    return scan_callbacks.get_device() != nullptr;
}

const char* BLEKeyboardClient::keyboard_name() {
    const NimBLEAdvertisedDevice *device = scan_callbacks.get_device();
    if(device) return device->getName().c_str();
    else return nullptr;
}

bool BLEKeyboardClient::connect() {
    if(!keyboard_found()) {
        Serial.println("no keyboard found");
        return false;
    }
    if(!pClient) {
        if(NimBLEDevice::getCreatedClientCount() >= NIMBLE_MAX_CONNECTIONS) {
            Serial.println("too many clients");
            return false;
        }
        pClient = NimBLEDevice::createClient();
        pClient->setClientCallbacks(&ClientCallbacks, false);
        pClient->setConnectionParams(12, 12, 0, 150);
        pClient->setConnectTimeout(5000);
        if(!pClient->connect(scan_callbacks.get_device())) {
            Serial.println("failed to connect");
            goto cleanup1;
        }
    }
    if(!pClient->isConnected()) {
        Serial.println("failed to connect");
        return false;
    }
    pSvc = pClient->getService("1812");
    if(!pSvc) {
        Serial.println("service not found");
        goto cleanup2;
    }

    pChr = nullptr;
    for (auto* chr : pSvc->getCharacteristics(true)) {
        if(chr->getUUID() == NimBLEUUID("2a4d") && chr->canNotify()) {
            if(!chr->subscribe(true, notifyCB)) {
                Serial.println("failed to subscribe");
            }
            pChr = chr;
        }
    }
    if(!pChr) {
        Serial.println("characteristic not found");
        goto cleanup2;
    }
    return true;

cleanup2:
    pClient->disconnect();
cleanup1:
    pSvc = nullptr;
    pChr = nullptr;
    NimBLEDevice::deleteClient(pClient);
    pClient = nullptr;
    return false;
}
//
// Keyboard
//
#if !defined(KEYBOARD_H)
#define KEYBOARD_H

#include <M5Unified.h>
#include <Wire.h>

class KeyBoard
{
protected:
    uint8_t _board;
public:
    KeyBoard() : _board(0) {}
    KeyBoard(uint8_t b) : _board(b) {}
    virtual ~KeyBoard() {}

    virtual bool wait_any_key() = 0;
    virtual bool fetch_key(uint8_t &c) = 0;
    virtual bool exists() = 0;

    static const int I2C_ADDR = 0x08;
};

class M5StackKeyBoard : public KeyBoard
{
protected:
public:
    M5StackKeyBoard() : KeyBoard(m5::board_t::board_M5Stack)
    {
        Wire.begin();
        pinMode(INTR, INPUT);
        digitalWrite(INTR, HIGH);
    }
    virtual ~M5StackKeyBoard() { Wire.end(); }
    virtual bool wait_any_key() override;
    virtual bool fetch_key(uint8_t &c) override;
    virtual bool exists() override;

    static const int INTR = 5;
};

class M5Core2KeyBoard : public KeyBoard
{
protected:
public:
    M5Core2KeyBoard() : KeyBoard(m5::board_t::board_M5StackCore2)
    { 
        Wire1.begin();
        pinMode(INTR, INPUT);
        digitalWrite(INTR, HIGH);
    }
    virtual ~M5Core2KeyBoard() { Wire1.end(); }
    virtual bool wait_any_key() override;
    virtual bool fetch_key(uint8_t &c) override;
    virtual bool exists() override;

    static const int INTR = 33;
};

#include <M5Cardputer.h>
#include <queue>
class M5CardputerKeyBoard : public KeyBoard
{
protected:
    std::queue<uint8_t> _buf;
public:
    M5CardputerKeyBoard() : KeyBoard(m5::board_t::board_M5StampS3)
    {
        M5Cardputer.Keyboard.begin();
    }
    virtual ~M5CardputerKeyBoard() { }
    virtual bool wait_any_key() override;
    virtual bool fetch_key(uint8_t &c) override;
    virtual bool exists() override;
};

#include <NimBLEDevice.h>

class  ClientCallbacks : public NimBLEClientCallbacks
{
    void onConnect(NimBLEClient* pClient) override;
    void onDisconnect(NimBLEClient* pClient) override;
    bool onConnParamsUpdateRequest(NimBLEClient* pClient, const ble_gap_upd_params* params) override;

    uint32_t onPassKeyRequest () override;
    bool onConfirmPIN(uint32_t pin) override;
    void onAuthenticationComplete(ble_gap_conn_desc *desc) override;
};

class AdvertisedDeviceCallbacks: public NimBLEAdvertisedDeviceCallbacks
{
    void onResult(NimBLEAdvertisedDevice *advertisedDevice) override;
};

class BTKeyBoard : public KeyBoard
{
protected:
    std::queue<uint8_t> _buf;
    static const uint8_t _keymap[][96];
    bool connectToServer();
    void begin();
    void update();
public:
    BTKeyBoard() : KeyBoard()
    {
        begin();
    }
    virtual ~BTKeyBoard() {}
    virtual bool wait_any_key() override;
    virtual bool fetch_key(uint8_t &c) override;
    virtual bool exists() override;

    static const char *HID_SERVICE;
//  static const char *HID_REPORT_MAP;
    static const char *HID_REPORT_DATA;

    static const uint32_t scanTime;
};

#endif

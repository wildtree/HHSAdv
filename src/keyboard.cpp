//
// KeyBoard
//

#include <keyboard.h>

bool
M5StackKeyBoard::wait_any_key()  
{
    bool r = false;
    if (digitalRead(INTR) == LOW)
    {
        Wire.requestFrom(I2C_ADDR, 1);
        while(Wire.available())
        {
            r = true;
            Wire.read();
        }
    }
    return r;
}

bool
M5StackKeyBoard::fetch_key(uint8_t &c) 
{
    bool r = false;
    if (digitalRead(INTR) == LOW)
    {
        Wire.requestFrom(I2C_ADDR, 1);
        if (Wire.available())
        {
            c = Wire.read();
            r = true;
        }
    }
    return r;
}

bool
M5Core2KeyBoard::wait_any_key() 
{
    bool r = false;
    if (digitalRead(INTR) == LOW)
    {
        Wire1.requestFrom(I2C_ADDR, 1);
        while (Wire1.available())
        {
            Wire1.read();
            r = true;
        }
    }
    return r;
}

bool
M5Core2KeyBoard::fetch_key(uint8_t &c) 
{
    bool r = false;
    if (digitalRead(INTR) == LOW)
    {
        Wire1.requestFrom(I2C_ADDR, 1);
        if (Wire1.available())
        {
            c = Wire1.read();
            r = true;
        }
    }
    return r;
}

bool
M5CardputerKeyBoard::wait_any_key()
{
    bool r = false;
    M5Cardputer.update();
    if (M5Cardputer.Keyboard.isChange())
    {
        r = M5Cardputer.Keyboard.isPressed();
    }
    return r;
}

bool
M5CardputerKeyBoard::fetch_key(uint8_t &c)
{
    bool r = false;
    if (!_buf.empty())
    {
        c = _buf.front();
        _buf.pop();
        return true;
    }
    M5Cardputer.update();
    if (M5Cardputer.Keyboard.isChange())
    {
        if (M5Cardputer.Keyboard.isPressed())
        {
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
            for(auto i:status.word)
            {
                _buf.push(i);
            }
            if (status.del) _buf.push(0x08);
            if (status.enter) _buf.push('\r');
            c = _buf.front();
            _buf.pop();
            
            r = true;
        }
    }
    return r;
}
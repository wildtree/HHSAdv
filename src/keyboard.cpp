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

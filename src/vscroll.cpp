/*
 * VScroll Contoller
 */

#include <vscroll.h>

// Hardware Scroll does not work for screen orientation 0 (320x240 mode)
// Instead of it, software scroll is implemented by this module.
// Constructer
ZVScroll::ZVScroll(uint16_t top, uint16_t bottom)
    :_top(top), _bottom(bottom), _y((int)top), _ty((int)top), _tx(0)
{
#if 0
    M5.Display.writecommand(ILI9342C_VSCRDEF);
    this->writeWordData(_top);
    this->writeWordData(YMax - _top - _bottom);
    this->writeWordData(_bottom);
    this->scrollLine();
#else
    _h = YMax - _top - _bottom;
    _buf = new uint16_t [XMax];
#endif
}

ZVScroll::ZVScroll(const ZVScroll &x)
    : _top(x._top), _bottom(x._bottom), _y(x._y), _ty(x._ty), _tx(x._tx),_h(x._h)
{
#if 0
    M5.Display.writecommand(ILI9342C_VSCRDEF);
    this->writeWordData(_top);
    this->writeWordData(YMax - _top - _bottom);
    this->writeWordData(_bottom);
    this->scrollAddress(_y);
#else
    _buf = new uint16_t [XMax];
#endif
}

// Destructer
ZVScroll::~ZVScroll() {
    if (_buf) delete[] _buf;
}

// writeWordData
void
ZVScroll::writeWordData(uint16_t data)
{
    M5.Display.writedata(data >> 8);
    M5.Display.writedata(data);
}

void
ZVScroll::scrollAddress(uint16_t data)
{
    M5.Display.writecommand(ILI9342C_VSCADRS);
    this->writeWordData(data);
}

int
ZVScroll::scrollLine()
{
#if 0
    _ty = _y;
    M5.Display.fillRect(0, _y, XMax, FontHeight, BLACK);
    _y += FontHeight;
    if (_y >= YMax - _bottom) _y -= YMax - _bottom - _top;
    scrollAddress(_y);
#else
    _y += FontHeight;
    if (_y >= YMax - _bottom)
    {
        M5.Display.startWrite();
        for (int y = 0 ; y < _h - FontHeight ; y++)
        {
            M5.Display.readRect(0, y + _top + FontHeight, XMax, 1, _buf);
            M5.Display.pushImage(0, y + _top, XMax, 1, _buf);
        }
        M5.Display.endWrite();
        _y -= FontHeight;
    }
    //Serial.printf("clear line to be written: (%d)\n", _y);
    M5.Display.fillRect(0, _y, XMax, FontHeight, BLACK);
    _ty = _y;
#endif
    _tx = 0;
    return _ty;
}

void
ZVScroll::print(const String &s)
{
    //Serial.printf("String: '%s' (length = %d)\n", s.c_str(), s.length());
    for (int i = 0 ; i < s.length() ; i++)
    {
        uint8_t c = s[i];
        //Serial.printf("idx = %d (code = %#02x) (%d,%d)\n", i, c, _tx, _ty);
        if (isascii(c))
        {
            if (_tx >= 312)
            {
                scrollLine();
            }
            M5.Display.setFont(&fonts::AsciiFont8x16);
            _tx += M5.Display.drawChar(s[i], _tx, _ty, 2);
        }
        else if (c >= 0x80 && c <= 0xbf) // UTF-8 letters (2nd or later byte letters)
        {
            M5.Display.print(s[i]);
        }
        else
        {
            if (_tx >= 312)
            {
                scrollLine();
            }
            M5.Display.setFont(&fonts::lgfxJapanGothic_16);
            M5.Display.setCursor(_tx, _ty);
            M5.Display.print(s[i]);
            _tx += 16;
        }
    }
    M5.Display.setFont(&fonts::AsciiFont8x16);
}

void
ZVScroll::cls(void)
{
    _tx = 0;
    _ty = _y = _top;
    M5.Display.fillRect(0, _top, XMax, YMax - _bottom - _top, BLACK);
}
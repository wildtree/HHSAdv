/*
 * VScroll Contoller
 */

#include <vscroll.h>

// Hardware Scroll does not work for screen orientation 0 (320x240 mode)
// Instead of it, software scroll is implemented by this module.
// Constructer
ZVScroll::ZVScroll(uint16_t top, uint16_t bottom)
    :_top(top), _bottom(bottom), _ty((int)top), _tx(0)
{
    _h = YMax - _top - _bottom;
    _buf = new uint16_t [XMax];
}

ZVScroll::ZVScroll(const ZVScroll &x)
    : _top(x._top), _bottom(x._bottom), _ty(x._ty), _tx(x._tx),_h(x._h)
{
    _buf = new uint16_t [XMax];
}

// Destructer
ZVScroll::~ZVScroll() {
    if (_buf) delete[] _buf;
}

int
ZVScroll::scrollLine()
{
    int ly = _ty + FontHeight;
    if (ly >= YMax - _bottom)
    {
        M5.Display.startWrite();
        for (int y = 0 ; y < _h - FontHeight ; y++)
        {
            M5.Display.readRect(0, y + _top + FontHeight, XMax, 1, _buf);
            M5.Display.pushImage(0, y + _top, XMax, 1, _buf);
        }
        M5.Display.endWrite();
        ly = _ty;
    }
    //Serial.printf("clear line to be written: (%d)\n", _y);
    M5.Display.fillRect(0, ly, XMax, FontHeight, BLACK);
    _ty = ly;
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
            if (_tx >= XMax - FontWidth)
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
            if (_tx >= XMax - FontWidth)
            {
                scrollLine();
            }
            M5.Display.setFont(&fonts::lgfxJapanGothic_16);
            M5.Display.setCursor(_tx, _ty);
            M5.Display.print(s[i]);
            _tx += FontWidth * 2;
        }
    }
    M5.Display.setFont(&fonts::AsciiFont8x16);
}

void
ZVScroll::cls(void)
{
    _tx = 0;
    _ty = _top;
    M5.Display.fillRect(0, _top, XMax, YMax - _bottom - _top, BLACK);
}

// Cardpiuter
CardputerScroll::CardputerScroll(uint16_t top, uint16_t bottom, int x, int y)
    :ZVScroll(top, bottom), _x(x), _y(y)
{
    _canvas = new M5Canvas(&M5.Display);
    _canvas->createSprite(XMax, _h);
}

CardputerScroll::CardputerScroll(const CardputerScroll &x)
    :ZVScroll(x._top, x._bottom), _x(x._x), _y(x._y)
{
    _canvas = new M5Canvas(&M5.Display);
    _canvas->createSprite(XMax, _h);
}

CardputerScroll::~CardputerScroll()
{
    delete _canvas;
}

int 
CardputerScroll::scrollLine()
{
    int ly = _ty + FontHeight;
    if (ly >= _h)
    {
        for (int y = 0 ; y < _h - FontHeight ; y++)
        {
            _canvas->readRect(0, y + FontHeight, XMax, 1, _buf);
            _canvas->pushImage(0, y, XMax, 1, _buf);
        }
        ly = _ty;
    }
    _canvas->fillRect(0, ly, XMax, FontHeight, BLACK);
    invalidate();
    _ty = ly;
    _tx = 0;
    return _ty;
}

void 
CardputerScroll::print(const String &s)
{
    for (int i = 0 ; i < s.length() ; i++)
    {
        uint8_t c = s[i];
        if (isascii(c))
        {
            if (_tx >= XMax - FontWidth)
            {
                scrollLine();
            }
            _canvas->setFont(&fonts::AsciiFont8x16); // it looks no ASCII font having 4x8 size
            _tx += _canvas->drawChar(s[i], _tx, _ty, 2);
        }
        else if (c >= 0x80 && c <= 0xbf) // UTF-8 letters (2nd or later byte letters)
        {
            _canvas->print(s[i]);
        }
        else
        {
            if (_tx >= XMax - FontWidth)
            {
                scrollLine();
            }
            _canvas->setFont(&fonts::lgfxJapanGothic_16);
            _canvas->setCursor(_tx, _ty);
            _canvas->print(s[i]);
            _tx += FontWidth * 2;
        }
    }
    invalidate();
    _canvas->setFont(&fonts::AsciiFont8x16);
}

void 
CardputerScroll::cls(void)
{
    _tx = 0;
    _ty = 0;
    _canvas->fillRect(0, 0, XMax, _h, BLACK);
    invalidate();
}

void 
CardputerScroll::invalidate() const 
{
    float affine[] = {0.5,0,(float)_x,0,0.5,(float)_y};
    M5.Display.startWrite();
    _canvas->pushAffineWithAA(affine);
    M5.Display.endWrite();
}
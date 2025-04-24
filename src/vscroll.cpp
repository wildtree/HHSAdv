/*
 * VScroll Contoller
 */

#include <vscroll.h>

// Hardware Scroll does not work for screen orientation 0 (320x240 mode)
// Instead of it, software scroll is implemented by this module.
// Constructer
ZVScroll::ZVScroll(uint16_t top, uint16_t bottom)
    :_top(top), _bottom(bottom), _ty(0), _tx(0), _scale(1.0), _x(0), _y((int)top)
{
    _h = YMax - _top - _bottom;
    _buf = new uint16_t [XMax];
    _canvas = new M5Canvas(&M5.Display);
    _canvas->setColorDepth(8);
    _canvas->createSprite(XMax, _h);
    float sx = 1.0;
    if (M5.Displays(0).width() < XMax)
    {
        sx = (float)M5.Displays(0).width();
        sx /= (float)XMax;
    }
    float sy = 1.0;
    if (M5.Displays(0).height() < YMax)
    {
        sy = (float)M5.Displays(0).height();
        sy /= (float)YMax;
    }
    _scale = (sx < sy) ? sx : sy;
    _x = (uint16_t)(M5.Displays(0).width() - (uint16_t)(XMax * _scale)) / 2;
    _y = (uint16_t)(M5.Displays(0).height() - (uint16_t)(YMax * _scale)) / 2 + _top * _scale;
    cls();
}
#if 0
ZVScroll::ZVScroll(const ZVScroll &x)
    : _top(x._top), _bottom(x._bottom), _ty(x._ty), _tx(x._tx),_h(x._h), _scale(x._scale), _x(x._x), _y(x._y)
{
    _buf = new uint16_t [XMax];
    _canvas = new M5Canvas(&M5.Display);
    _canvas->createSprite(XMax, _h);

    cls();
}
#endif

// Destructer
ZVScroll::~ZVScroll() {
    if (_buf) delete[] _buf;
    if (_canvas) delete _canvas;
}

int
ZVScroll::scrollLine()
{
    int ly = _ty + FontHeight;
    if (ly >= _h)
    {
        M5.Display.startWrite();
        for (int y = 0 ; y < _h - FontHeight ; y++)
        {
            _canvas->readRect(0, y + FontHeight, XMax, 1, _buf);
            _canvas->pushImage(0, y, XMax, 1, _buf);
        }
        M5.Display.endWrite();
        ly = _ty;
    }
    //Serial.printf("clear line to be written: (%d)\n", _y);
    _canvas->fillRect(0, ly, XMax, FontHeight, M5.Display.color16to8(BLACK));
    invalidate();
    _ty = ly;
    _tx = 0;
    return _ty;
}

void
ZVScroll::print(const String &s)
{
    //Serial.printf("String: '%s' (length = %d)\n", s.c_str(), s.length());
    String t = s;
    while (t.indexOf("−") > 0)
    {
        t.replace("−", "～");
    }
   for (int i = 0 ; i < t.length() ; i++)
    {
        uint8_t c = t[i];
        //Serial.printf("idx = %d (code = %#02x) (%d,%d)\n", i, c, _tx, _ty);
        if (isascii(c))
        {
            if (_tx >= XMax - FontWidth)
            {
                scrollLine();
            }
            _canvas->setFont(&fonts::AsciiFont8x16);
            _tx += _canvas->drawChar(t[i], _tx, _ty, 2);
        }
        else if (c >= 0x80 && c <= 0xbf) // UTF-8 letters (2nd or later byte letters)
        {
            _canvas->print(t[i]);
        }
        else
        {
            if (_tx >= XMax - FontWidth)
            {
                scrollLine();
            }
            _canvas->setFont(&fonts::lgfxJapanGothic_16);
            _canvas->setCursor(_tx, _ty);
            _canvas->print(t[i]);
            _tx += FontWidth * 2;
        }
    }
    invalidate();
    _canvas->setFont(&fonts::AsciiFont8x16);
}

void
ZVScroll::cls(void)
{
    _tx = 0;
    _ty = 0;
    _canvas->fillRect(0, 0, XMax, _h, M5.Display.color16to8(BLACK));
    invalidate();
}

void 
ZVScroll::invalidate() const 
{
    M5.Display.startWrite();
    if (_scale == 1.0)
    {
        _canvas->pushSprite(0, _top);
    }
    else
    {
        float affine[] = {_scale, 0.0, (float)_x, 0.0, _scale, (float)_y};
        //Serial.printf("Vscroll: (x,y,top) = (%d,%d,%d)\r\n", _x, _y, _top);
        _canvas->pushAffineWithAA(affine);
    }
    M5.Display.endWrite();
}


#if defined(CONFIG_IDF_TARGET_ESP32S3)
// Cardpiuter
CardputerScroll::CardputerScroll(uint16_t top, uint16_t bottom, int x, int y)
    :ZVScroll(top, bottom)
{
#if 0
    _canvas = new M5Canvas(&M5.Display);
    _canvas->setColorDepth(8);
    _canvas->createSprite(XMax, _h);
    cls();
#endif
    _x = x;
    _y = y;
    _scale = 0.5;
}
#if 0
CardputerScroll::CardputerScroll(const CardputerScroll &x)
    :ZVScroll(x._top, x._bottom), _x(x._x), _y(x._y), _scale(x._scale)
{
    _canvas = new M5Canvas(&M5.Display);
    _canvas->createSprite(XMax, _h);
    cls();
}
#endif
CardputerScroll::~CardputerScroll()
{
//    delete _canvas;
}

#if 0
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
    _canvas->fillRect(0, ly, XMax, FontHeight, M5.Display.color16to8(BLACK));
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
    _canvas->fillRect(0, 0, XMax, _h, M5.Display.color16to8(BLACK));
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
#endif

#endif
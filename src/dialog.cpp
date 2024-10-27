//
// Dialog class
//

#include <zsystem.h>
#include <dialog.h>

Button::Button()
    :_x(-1), _y(-1), _w(-1), _h(-1), _label(""), _enabled(false), _key(0)
{
    auto board = M5.getBoard();
    _touch = (board == m5::board_t::board_M5StackCore2);
    _canvas = nullptr;
}

Button::Button(int x, int y, int w, int h, const String &label, uint8_t key, M5Canvas *canvas)
    : _x(x), _y(y), _w(w), _h(h), _label(label), _enabled(true), _key(key), _canvas(canvas)
{
    auto board = M5.getBoard();
    _touch = (board == m5::board_t::board_M5StackCore2);
    if (_label.isEmpty()) _enabled = false;
}

Button::~Button()
{

}

bool
Button::is_pressed(uint8_t c) const
{
    if (!is_enabled()) return false;
    if (c != 0 && c == _key) return true;
    if (!_touch) return false;
    int cnt = M5.Touch.getCount();
    if (cnt == 0) return false;
    bool r = false;
    for (int i = 0 ; i < cnt ; i++)
    {
        auto t = M5.Touch.getDetail(i);
        if (!t.isPressed()) continue;
        if (t.x >= _x && t.x < _x + _w && t.y >= _y && t.y < _y + _h)
        {
            r = true;
            break;
        }
    }
    return r;
}

void
Button::draw(bool is_pressed) const
{
    static uint16_t col;
    if (_enabled)
    {
        if (is_pressed)
        {
            _canvas->fillRoundRect(_x, _y, _w, _h, 8, M5.Display.color16to8(BLACK));
            col = WHITE;
        }
        else
        {
            _canvas->drawRoundRect(_x, _y, _w, _h, 8, _canvas->color16to8(BLACK));
            col = BLACK;
        }
    }
    else
    {
        _canvas->fillRoundRect(_x, _y, _w, _h, 8, _canvas->color16to8(LIGHTGREY));
        col = DARKGREY;
    }
    auto text_datum = _canvas->getTextDatum();
    auto font = _canvas->getFont();
    _canvas->setTextDatum(middle_center);
    _canvas->setFont(&fonts::lgfxJapanGothic_16);
    _canvas->setTextColor(_canvas->color16to8(col));
    _canvas->drawString(_label, _x + _w / 2, _y + _h / 2);
    _canvas->setFont(font);
    _canvas->setTextDatum(text_datum);
}

Dialog::Dialog()
   : _x(8), _y(8), _w(304), _h(144),_title("dialog"),_result(-1)
{
    _canvas = new M5Canvas(&M5.Display);
    _canvas->setColorDepth(1);
    _canvas->createSprite(_w, _h);
    _btnA = new Button(  9,120,90,20,"A",'1', _canvas);
    _btnB = new Button(107,120,90,20,"B",'2', _canvas);
    _btnC = new Button(205,120,90,20,"C",'3', _canvas);
    float sx = (M5.Displays(0).width() < 320.0) ? (float)M5.Displays(0).width() / 320.0 : 1.0;
    float sy = (M5.Displays(0).height() < 240.0) ? (float)M5.Displays(0).height() / 240.0 : 1.0;
    _scale = (sx < sy) ? sx : sy;
    _dx = (uint16_t)(M5.Displays(0).width() - (uint16_t)(320.0 * _scale)) / 2;
    _dy = (uint16_t)(M5.Displays(0).height() - (uint16_t)(240.0 * _scale)) / 2;
}

Dialog::~Dialog()
{
    dismiss();
    delete _btnA;
    delete _btnB;
    delete _btnC;
    delete _canvas;
}


void
Dialog::_print() const
{
    int x = 8;
    int y = 36;

    auto font = _canvas->getFont();
    _canvas->setTextColor(_canvas->color16to8(BLACK));
    for(int i = 0 ; i < _message.length() ; i++)
    {
        uint8_t c = _message[i];
        if (isascii(c)||c >= 0xc0)
        {
            if (x >= _w - FontWidth)
            {
                x = FontWidth;
                y += _canvas->fontHeight(_canvas->getFont());
            }
            _canvas->setCursor(x, y);
            if (isascii(c))
            {
                _canvas->setFont(&fonts::AsciiFont8x16);
                x += FontWidth;
            }
            else
            {
                _canvas->setFont(&fonts::lgfxJapanGothic_16);
                x += FontWidth * 2;
            }
        }
        _canvas->print((char)c);
    }
}

void
Dialog::setScale(float scale)
{
    _scale = scale;
    _dx = (uint16_t)(M5.Displays(0).width() - (uint16_t)(320.0 * _scale)) / 2;
    _dy = (uint16_t)(M5.Displays(0).height() - (uint16_t)(240.0 * _scale)) / 2;
}

int
Dialog::draw(void)
{
    //_canvas->fillRect(0,0,_w,_h, _canvas->color16to8(WHITE));
    _canvas->fillSprite(_canvas->color16to8(WHITE));
    _canvas->fillRect(0,0,_w,20, _canvas->color16to8(BLACK));
    _canvas->drawRect(0,0,_w,20, _canvas->color16to8(WHITE));
    auto font = _canvas->getFont();
    _canvas->setFont(&fonts::lgfxJapanGothic_16);
    auto text_datum = _canvas->getTextDatum();
    _canvas->setTextDatum(middle_center);
    _canvas->setTextColor(_canvas->color16to8(WHITE));
    _canvas->drawString(_title, _w / 2, 8);
    _canvas->setTextDatum(text_datum);
 
    _print(); // put message
    _canvas->setFont(font);

    _btnA->draw();
    _btnB->draw();
    _btnC->draw();

    invalidate();
    while(true)
    {
        uint8_t c = 0;
        M5.update();
        ZSystem::getInstance().getKeyboard()->fetch_key(c);
        if ((_btnA->is_enabled() && M5.BtnA.isPressed())||_btnA->is_pressed(c))
        {
            _btnA->draw(true);
            _result = 1;
            invalidate();
            break;
        }
        if ((_btnB->is_enabled() && M5.BtnB.isPressed())||_btnB->is_pressed(c))
        {
            _btnB->draw(true);
            _result = 2;
            invalidate();
            break;
        }
        if ((_btnC->is_enabled() && M5.BtnC.isPressed())||_btnC->is_pressed(c))
        {
            _btnC->draw(true);
            _result = 3;
            invalidate();
            break;
        }
    }
    _canvas->fillSprite(_canvas->color16to8(BLACK));
    //_canvas->fillRect(0,0,_w,_h,_canvas->color16to8(BLACK));
    invalidate();
    return _result;
}

void
Dialog::dismiss()
{

}

void
Dialog::button(const String &labelA, const String &labelB, const String &labelC)
{
    _btnA->setLabel(labelA);
    _btnB->setLabel(labelB);
    _btnC->setLabel(labelC);

    if (labelA.isEmpty()) _btnA->disable();
    if (labelB.isEmpty()) _btnB->disable();
    if (labelC.isEmpty()) _btnC->disable();
}

void
Dialog::invalidate() const
{
    M5.Display.startWrite();
    //Serial.printf("(x,y,w,h,dx,dy) = (%d,%d,%d,%d,%d,%d)\r\n",_x,_y,_w,_h,_dx,_dy);
    if (_scale == 1.0)
    {
        _canvas->pushSprite(_x,_y);
    }
    else
    {
        float affine[] = {_scale, 0.0, (float)(_dx + _x * _scale), 0.0, _scale, (float)(_dy + _y * _scale)};
        _canvas->pushAffineWithAA(affine);
    }
    M5.Display.endWrite();
}

#if defined(CONFIG_IDF_TARGET_ESP32S3)
CardputerButton::CardputerButton(M5Canvas *canvas)
    : Button()
{
    _touch = false; // Cardputer does not have touch button
    _canvas = canvas;
}

CardputerButton::CardputerButton(int x, int y, int w, int h, const String &label, uint8_t key, M5Canvas *canvas)
    : Button(x, y, w, h, label, key, canvas)
{
    _touch = false;
}
 
CardputerButton::~CardputerButton()
{

}

void
CardputerButton::draw(bool is_pressed) const
{
    uint16_t col;
    if (_enabled)
    {
        if (is_pressed)
        {
            _canvas->fillRoundRect(_x, _y, _w, _h, 8, BLACK);
            col = WHITE;
        }
        else
        {
            _canvas->drawRoundRect(_x, _y, _w, _h, 8, BLACK);
            col = BLACK;
        }
    }
    else
    {
        _canvas->fillRoundRect(_x, _y, _w, _h, 8, LIGHTGREY);
        col = DARKGREY;
    }
    auto text_datum = _canvas->getTextDatum();
    auto font = _canvas->getFont();
    String s = _label;
    if (s != String((char)_key))
    {
        s = String((char)_key) + String(". ") + _label;
    }
    _canvas->setTextDatum(middle_center);
    _canvas->setFont(&fonts::lgfxJapanGothic_16);
    _canvas->setTextColor(col);
    _canvas->drawString(s, _x + _w / 2, _y + _h / 2);
    _canvas->setFont(font);
    _canvas->setTextDatum(text_datum);
}

CardputerDialog::CardputerDialog()
   : Dialog()
{
    _x = 18;
    _y = 0;
    _canvas = new M5Canvas(&M5.Display);
    _canvas->createSprite(_w, _h);
    delete _btnA;
    delete _btnB;
    delete _btnC;
    _btnA = new CardputerButton(  9,120,90,20,"A",'1', _canvas);
    _btnB = new CardputerButton(107,120,90,20,"B",'2', _canvas);
    _btnC = new CardputerButton(205,120,90,20,"C",'3', _canvas);
}

CardputerDialog::~CardputerDialog()
{
}

void
CardputerDialog::_print() const
{
    int x = 8;
    int y = 36;

    auto font = _canvas->getFont();
    _canvas->setTextColor(BLACK);
    for(int i = 0 ; i < _message.length() ; i++)
    {
        uint8_t c = _message[i];
        if (isascii(c)||c >= 0xc0)
        {
            if (x >=_w - FontWidth)
            {
                x = FontWidth;
                y += _canvas->fontHeight(_canvas->getFont());
            }
            _canvas->setCursor(x, y);
            if (isascii(c))
            {
                _canvas->setFont(&fonts::AsciiFont8x16);
                x += FontWidth;
            }
            else
            {
                _canvas->setFont(&fonts::lgfxJapanGothic_16);
                x += FontWidth * 2;
            }
        }
        _canvas->print((char)c);
    }

}

#include <M5Cardputer.h>
int
CardputerDialog::draw()
{
    _canvas->fillRect(0, 0, _w, _h, WHITE);
    _canvas->fillRect(0, 0, _w, 20, BLACK);
    _canvas->drawRect(0, 0, _w, 20, WHITE);
    auto font = _canvas->getFont();
    _canvas->setFont(&fonts::lgfxJapanGothic_16);
    auto text_datum =_canvas->getTextDatum();
    _canvas->setTextDatum(middle_center);
    _canvas->setTextColor(WHITE);
    _canvas->drawString(_title, (int)(_x * 0.67) + _w / 2, (int)(_y * 0.67) + 8);
    _canvas->setTextDatum(text_datum);

    _print(); // put message
    _canvas->setFont(font);

    _btnA->draw();
    _btnB->draw();
    _btnC->draw();
    invalidate();

    while(true)
    {
    #if 0
        M5Cardputer.update();
        if (M5Cardputer.Keyboard.isChange())
        {
            if (_btnA->is_enabled() && M5Cardputer.Keyboard.isKeyPressed('1'))
            {
                _btnA->draw(true);
                _result = 1;
                invalidate();
                break;
            }
            if (_btnB->is_enabled() && M5Cardputer.Keyboard.isKeyPressed('2'))
            {
                _btnB->draw(true);
                _result = 2;
                invalidate();
                break;
            }
            if (_btnC->is_enabled() && M5Cardputer.Keyboard.isKeyPressed('3'))
            {
                _btnC->draw(true);
                _result = 3;
                invalidate();
                break;
            }
        }
#else
        uint8_t c = 0;
        if (ZSystem::getInstance().getKeyboard()->fetch_key(c))
        {
            if (_btnA->is_pressed(c))
            {
                _btnA->draw(true);
                _result = 1;
                invalidate();
                break;
            }
            if (_btnB->is_pressed(c))
            {
                _btnB->draw(true);
                _result = 2;
                invalidate();
                break;
            }
            if (_btnC->is_pressed(c))
            {
                _btnC->draw(true);
                _result = 3;
                invalidate();
                break;
            }
#endif
        }
    }
    _canvas->fillRect(0, 0, _w, _h, BLACK);
    invalidate();
    return _result;
}

void
CardputerDialog::invalidate() const
{
    float af[] = { 0.67, 0.0, (float)_x, 0.0, 0.67, (float)_y};
    M5.Display.startWrite();
    _canvas->pushAffineWithAA(af);
    M5.Display.endWrite();
}
#endif
//
// Dialog class
//

#include <dialog.h>

Button::Button()
    :_x(-1), _y(-1), _w(-1), _h(-1), _label(""), _enabled(false)
{
    auto board = M5.getBoard();
    _touch = (board == m5::board_t::board_M5StackCore2);
}

Button::Button(int x, int y, int w, int h, const String &label)
    : _x(x), _y(y), _w(w), _h(h), _label(label), _enabled(true)
{
    auto board = M5.getBoard();
    _touch = (board == m5::board_t::board_M5StackCore2);
    if (_label.isEmpty()) _enabled = false;
}

Button::~Button()
{

}

bool
Button::is_pressed() const
{
    if (!is_enabled()||!_touch) return false;
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
    uint16_t col;
    if (_enabled)
    {
        if (is_pressed)
        {
            M5.Display.fillRoundRect(_x, _y, _w, _h, 8, BLACK);
            col = WHITE;
        }
        else
        {
            M5.Display.drawRoundRect(_x, _y, _w, _h, 8, BLACK);
            col = BLACK;
        }
    }
    else
    {
        M5.Display.fillRoundRect(_x, _y, _w, _h, 8, LIGHTGREY);
        col = DARKGREY;
    }
    auto text_datum = M5.Display.getTextDatum();
    auto font = M5.Display.getFont();
    M5.Display.setTextDatum(middle_center);
    M5.Display.setFont(&fonts::lgfxJapanGothic_16);
    M5.Display.setTextColor(col);
    M5.Display.drawString(_label, _x + _w / 2, _y + _h / 2);
    M5.Display.setFont(font);
    M5.Display.setTextDatum(text_datum);
}

Dialog::Dialog()
   : _x(8), _y(8), _w(304), _h(144),_title("dialog"),_result(-1)
{
    _btnA = new Button(17,128,90,20,"A");
    _btnB = new Button(115,128,90,20,"B");
    _btnC = new Button(213,128,90,20,"C");
}

Dialog::~Dialog()
{
    dismiss();
    delete _btnA;
    delete _btnB;
    delete _btnC;
}


void
Dialog::_print() const
{
    int x = _x + 8;
    int y = _y + 36;

    auto font = M5.Display.getFont();
    M5.Display.setTextColor(BLACK);
    for(int i = 0 ; i < _message.length() ; i++)
    {
        uint8_t c = _message[i];
        if (isascii(c)||c >= 0xc0)
        {
            if (x >= _x + _w - FontWidth)
            {
                x = _x + FontWidth;
                y += M5.Display.fontHeight(M5.Display.getFont());
            }
            M5.Display.setCursor(x, y);
            if (isascii(c))
            {
                M5.Display.setFont(&fonts::AsciiFont8x16);
                x += FontWidth;
            }
            else
            {
                M5.Display.setFont(&fonts::lgfxJapanGothic_16);
                x += FontWidth * 2;
            }
        }
        M5.Display.print((char)c);
    }

}

int
Dialog::draw(void)
{
    M5.Display.fillRect(_x,_y,_w,_h, WHITE);
    M5.Display.fillRect(_x,_y,_w,20, BLACK);
    M5.Display.drawRect(_x,_y,_w,20, WHITE);
    auto font = M5.Display.getFont();
    M5.Display.setFont(&fonts::lgfxJapanGothic_16);
    auto text_datum = M5.Display.getTextDatum();
    M5.Display.setTextDatum(middle_center);
    M5.Display.setTextColor(WHITE);
    M5.Display.drawString(_title, _x + _w / 2, _y + 8);
    M5.Display.setTextDatum(text_datum);

    _print(); // put message
    M5.Display.setFont(font);

    _btnA->draw();
    _btnB->draw();
    _btnC->draw();

    while(true)
    {
        M5.update();
        if ((_btnA->is_enabled() && M5.BtnA.isPressed())||_btnA->is_pressed())
        {
            _btnA->draw(true);
            _result = 1;
            break;
        }
        if ((_btnB->is_enabled() && M5.BtnB.isPressed())||_btnB->is_pressed())
        {
            _btnB->draw(true);
            _result = 2;
            break;
        }
        if ((_btnC->is_enabled() && M5.BtnC.isPressed())||_btnC->is_pressed())
        {
            _btnC->draw(true);
            _result = 3;
            break;
        }
    }
    M5.Display.fillRect(_x,_y,_w,_h,BLACK);
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

#if defined(CONFIG_IDF_TARGET_ESP32S3)
CardputerButton::CardputerButton(M5Canvas *canvas)
    : Button(), _canvas(canvas)
{
    _touch = false; // Cardputer does not have touch button
}

CardputerButton::CardputerButton(M5Canvas *canvas, int x, int y, int w, int h, const String &label, char key)
    : Button(x, y, w, h, label),_canvas(canvas), _key(key)
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
    if (s != String(_key))
    {
        s = String(_key) + String(". ") + _label;
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
    _btnA = new CardputerButton(_canvas,9,120,90,20,"A",'1');
    _btnB = new CardputerButton(_canvas,107,120,90,20,"B",'2');
    _btnC = new CardputerButton(_canvas,205,120,90,20,"C",'3');
}

CardputerDialog::~CardputerDialog()
{
    delete _canvas;
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
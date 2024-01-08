//
// Dialog class
//

#if !defined(DIALOG_H)
#define DIALOG_H

#include <M5Unified.h>

class Button
{
public:
    Button();
    Button(int x, int y, int w, int h, const String &label);
    ~Button();

    void setRect(int x, int y, int w, int h) { _x = x; _y = y; _w = w; _h = h; }
    void setLabel(const String &label) { _label = label; }

    void draw(bool is_pressed = false) const;
    void enable() { _enabled = true; }
    void disable() { _enabled = false;}
    bool is_enabled() const { return _enabled; }
    bool is_pressed() const;
protected:
    bool _enabled;
    int _x, _y, _w, _h;
    String _label;
    bool _touch;
};

class Dialog
{
protected:
    String _title;
    String _message;
    Button _btnA, _btnB, _btnC;
    int _x, _y, _w, _h;
    int _result;

    void _print() const;
public:
    Dialog();
    ~Dialog();

    void setTitle(const String &title) { _title = title; }
    void setMessage(const String &message) { _message = message; }
    int draw();
    void dismiss();
    void button(const String &labelA, const String &labelB, const String &labelC);
    Button &btnA() { return _btnA; }
    Button &btnB() { return _btnB; }
    Button &btnC() { return _btnC; }

    static const int FontHeight = 16;
    static const int FontWidth = 8;
};


#endif
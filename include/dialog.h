//
// Dialog class
//

#if !defined(DIALOG_H)
#define DIALOG_H

#include <M5Unified.h>
//#include "zsystem.h"

class Button
{
public:
    Button();
    Button(int x, int y, int w, int h, const String &label, uint8_t key = 0, M5Canvas *canvas = nullptr);
    virtual ~Button();

    void setRect(int x, int y, int w, int h) { _x = x; _y = y; _w = w; _h = h; }
    void setLabel(const String &label) { _label = label; _enabled = (_label != ""); }

    virtual void draw(bool is_pressed = false) const;
    void enable() { _enabled = true; }
    void disable() { _enabled = false;}
    bool is_enabled() const { return _enabled; }
    bool is_pressed(uint8_t c = 0) const;
protected:
    bool _enabled;
    int _x, _y, _w, _h;
    String _label;
    bool _touch;
    uint8_t _key;
    M5Canvas *_canvas;
};

#if defined(CONFIG_IDF_TARGET_ESP32S3)
class CardputerButton : public Button
{
public:
    CardputerButton(M5Canvas *canvas = nullptr);
    CardputerButton(int x, int y, int w, int h, const String &label, uint8_t key = 0, M5Canvas *canvas = nullptr);
    virtual ~CardputerButton();

    virtual void draw(bool is_pressed = false) const override;
protected:
    //M5Canvas *_canvas;
    //char _key;
};
#endif

class Dialog
{
protected:
    String _title;
    String _message;
    Button *_btnA, *_btnB, *_btnC;
    int _x, _y, _w, _h, _dx, _dy;
    int _result;
    M5Canvas *_canvas;
    float _scale;

    virtual void _print() const;
public:
    Dialog();
    virtual ~Dialog();

    void setTitle(const String &title) { _title = title; }
    void setMessage(const String &message) { _message = message; }
    virtual int draw();
    virtual void invalidate() const;
    void dismiss();
    void button(const String &labelA, const String &labelB, const String &labelC);
    Button *btnA() { return _btnA; }
    Button *btnB() { return _btnB; }
    Button *btnC() { return _btnC; }
    void setScale(float s);
    float getScale() const { return _scale; }

    static const int FontHeight = 16;
    static const int FontWidth = 8;
};

#if defined(CONFIG_IDF_TARGET_ESP32S3)
class CardputerDialog : public Dialog
{
protected:
//    M5Canvas *_canvas;

    virtual void _print() const override;
public:
    CardputerDialog();
    virtual ~CardputerDialog();
    
    virtual int draw() override;
    virtual void invalidate() const override;
};
#endif
#endif
/*
 * Graphic Library
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <M5Unified.h>
#include <queue>

#define ZRGB332

class Canvas
{
protected:
    uint16_t _ox, _oy;
    uint16_t _w, _h, _dx, _dy;
    const uint16_t _col[8] = {
        BLACK, BLUE, RED, MAGENTA, GREEN, CYAN, YELLOW, WHITE
    };
#ifdef ZRGB332
    uint8_t *_v;
    inline uint16_t col8to16(uint8_t c) const {
        return (((((uint16_t)(c & 0xe0) >> 5) * 31) / 7) << 11)
             | (((((uint16_t)(c & 0x1c) >> 2) * 63) / 7) << 5)
             | (((uint16_t)(c & 3) * 31) / 3);
    }
    inline uint8_t col16to8(uint16_t c) const {
#if 0
        uint16_t r = ((c >> 11) * 7) & 0x7f;
        uint16_t g = ((c >> 5) * 7) & 0xff;
        uint16_t b = ((c & 3) * 3) & 0x7f;
        if (c) Serial.printf("c -> (r,g,b)  %2x -> (%2x,%2x,%2x) ",c, r, g, b);
        r /= 31;
        g /= 63;
        b /= 31;
        if (c) Serial.printf("/ (r,g,b) -> (%2x,%2x,%2x) \r\n",r, g, b);
        return (r << 11)|(g << 5)|b;
#else
        return ((uint8_t)((uint16_t)(((c >> 11) & 0x1f) * 7) / 31) << 5)
            |  ((uint8_t)((uint16_t)(((c >> 5) & 0x3f) * 7) / 63) << 2)
            |  (uint8_t)((uint16_t)((c & 0x1f) * 3) / 31);
#endif
    }
#else
    uint16_t *_v;
#endif
    float *_colorFilter;
    float _scale;
    uint16_t applyFilter(uint16_t);
public:
#if 0
    void *operator new(std::size_t sz) { return heap_caps_malloc(sz, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT|MALLOC_CAP_DMA); }
    void *operator new[](std::size_t sz) { return heap_caps_malloc(sz, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT|MALLOC_CAP_DMA); }
    void operator delete(void *p) { free(p); }
    void operator delete[](void *p) { free(p); }
#endif
    Canvas(uint16_t x = 0, uint16_t y = 0, uint16_t w = 320, uint16_t h = 240);
#if 0
    Canvas(const Canvas &x);
#endif
    virtual ~Canvas();

    virtual void cls(uint16_t c = BLACK);
    void line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t col);
    virtual void pset(uint16_t x, uint16_t y, uint16_t col);
    inline uint16_t pget(uint16_t x, uint16_t y) const { 
#ifdef ZRGB332
        //Serial.printf("col8to16(%2x) = %4x\r\n", _v[x + y * _w], col8to16(_v[x = y * _w]));
        return col8to16(_v[x + y * _w]);
#else
        return _v[x + y * _w]; 
#endif
    }
    void paint(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc);
    void tonePaint(const uint8_t tone[], bool tiling = false);
    uint16_t getColor(int c) const { return _col[c]; }
    void drawRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t c);
    void fillRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t c);

    void colorFilter(void);
    void setColorFilter(const float filter[]) { _colorFilter = (float*)filter; }
    void resetColorFilter() { setColorFilter(nullptr); }

    virtual void invalidate(bool force = false) const;

    float getScale() const { return _scale; }
    void setScale(float scale) { 
        _scale = scale; 
        _dx = (M5.Display.width() - (uint16_t)(320.0 * _scale)) / 2;
        _dy = (M5.Display.height() - (uint16_t)(240.0 * _scale)) / 2;
    }

    static const float blueFilter[], redFilter[], sepiaFilter[];
};

class Point
{
public:
    uint16_t x, y;
    Point(uint16_t x_ = 0, uint16_t y_ = 0) : x(x_), y(y_) {}
    Point(const Point &p) : x(p.x), y(p.y) {}
};

#if 0
class Queue
{
protected:
    Point *_fifo;
    int _size;
    int _head, _tail;
public:
    Queue(int size = 1024);
    Queue(const Queue &x);
    ~Queue();

    void push(const Point p);
    Point pop();
    bool empty() const { return _head == _tail;}
};
#endif 

#if defined(CONFIG_IDF_TARGET_ESP32S3)
class CardputerCanvas : public Canvas
{
protected:
public:
    CardputerCanvas(uint16_t x = 0, uint16_t y = 0, uint16_t w = 320, uint16_t h = 240) : Canvas(x, y, w, h) {}
    CardputerCanvas(const Canvas &x) : Canvas(x) {}
    virtual ~CardputerCanvas();

    virtual void cls(uint16_t c = BLACK);
    virtual void pset(uint16_t x, uint16_t y, uint16_t col);
    virtual void invalidate(bool force = false) const;
};
#endif

#endif /* GRAPH_H */
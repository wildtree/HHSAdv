/*
 * Graphic Library
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <M5Unified.h>
#include <queue>

class Canvas
{
protected:
    uint16_t _ox, _oy;
    uint16_t _w, _h;
    uint16_t *_v;
    const uint16_t _col[8] = {
        BLACK, BLUE, RED, MAGENTA, GREEN, CYAN, YELLOW, WHITE
    };
    float *_colorFilter;
    uint16_t applyFilter(uint16_t);
public:
#if 0
    void *operator new(std::size_t sz) { return heap_caps_malloc(sz, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT|MALLOC_CAP_DMA); }
    void *operator new[](std::size_t sz) { return heap_caps_malloc(sz, MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT|MALLOC_CAP_DMA); }
    void operator delete(void *p) { free(p); }
    void operator delete[](void *p) { free(p); }
#endif
    Canvas(uint16_t x = 0, uint16_t y = 0, uint16_t w = 320, uint16_t h = 240);
    Canvas(const Canvas &x);
    virtual ~Canvas();

    virtual void cls(uint16_t c = BLACK);
    void line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t col);
    virtual void pset(uint16_t x, uint16_t y, uint16_t col);
    uint16_t pget(uint16_t x, uint16_t y) const { return _v[x + y * _w]; }
    void paint(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc);
    void tonePaint(const uint8_t tone[], bool tiling = false);
    uint16_t getColor(int c) const { return _col[c]; }
    void drawRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t c);
    void fillRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t c);

    void colorFilter(void);
    void setColorFilter(const float filter[]) { _colorFilter = (float*)filter; }
    void resetColorFilter() { setColorFilter(nullptr); }

    virtual void invalidate() const {}

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

class CardputerCanvas : public Canvas
{
protected:
public:
    CardputerCanvas(uint16_t x = 0, uint16_t y = 0, uint16_t w = 320, uint16_t h = 240) : Canvas(x, y, w, h) {}
    CardputerCanvas(const Canvas &x) : Canvas(x) {}
    virtual ~CardputerCanvas();

    virtual void cls(uint16_t c = BLACK);
    virtual void pset(uint16_t x, uint16_t y, uint16_t col);
    virtual void invalidate() const;
};
#endif /* GRAPH_H */
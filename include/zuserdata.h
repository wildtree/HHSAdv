//
// ZUser Data
//

#ifndef ZUSERDATA_H
#define ZUSERDATA_H

#include <SD.h>
#include <M5Unified.h>

class ZMapLink
{
protected:
    union ZLink
    {
        uint8_t l[8];
        struct {
            uint8_t n,s,w,e,u,d,i,o;
        } dir;
    } _link;
public:
#if 0
    void *operator new(std::size_t sz) { return heap_caps_malloc(sz, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT); }
    void *operator new[](std::size_t sz) { return heap_caps_malloc(sz, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT); }
    void operator delete(void *p) { free(p); }
    void operator delete[](void *p) { free(p); }
#endif
    enum {
        North = 0,
        South,
        West,
        East,
        Up,
        Down,
        In,
        Out,
    };
    ZMapLink();
    ZMapLink(const uint8_t b[]);
    ZMapLink(const ZMapLink &x);
    void setLink(int id, uint8_t value);
    uint8_t get(int id) const { return _link.l[id]; }
    void write(File &f) const { f.write(_link.l, sizeof(_link)); };
    uint8_t *pack(void) const { return (uint8_t*)_link.l; }
    void unpack(const uint8_t buf[]) { memcpy(_link.l, buf, sizeof(ZMapLink)); }
};

class ZUserData 
{
protected:
    ZMapLink *_map;
    uint8_t *_places, *_flags;
    uint8_t *_buf;
public:
#if 0
    void *operator new(std::size_t sz) { return heap_caps_malloc(sz, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT); }
    void *operator new[](std::size_t sz) { return heap_caps_malloc(sz, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT); }
    void operator delete(void *p) { free(p); }
    void operator delete[](void *p) { free(p); }
#endif
    ZUserData();
    ZUserData(const ZUserData &x);
    ZUserData(const String &file);
    ~ZUserData();

    void init(const String &file);
    void unpack(const uint8_t buf[]);
    uint8_t *pack() const;

    uint8_t getFact(int id) const { return _flags[id]; }
    uint8_t getPlace(int id) const { return _places[id]; }

    ZMapLink &getMap(int id) const { return _map[id]; }

    void setFact(int id, uint8_t v) { _flags[id] = v; }
    void setPlace(int id, uint8_t v) { _places[id] = v; }

    uint8_t decFact(int id) { return --_flags[id]; }
    uint8_t incFact(int id) { return ++_flags[id]; }

    String itemList() const;

    static const int links = 87;
    static const int items = 12;
    static const int flags = 15;

    static const int packed_size = links * sizeof(ZMapLink) + items + flags;
    static const String item_labels[items];
};

#endif /* ZUSERDATA_H  */
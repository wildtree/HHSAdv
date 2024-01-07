//
// High High School Adventure for M5 Series
//

#include <SD.h>
#include <Wire.h>
#include <M5Unified.h>
#include <graph.h>
#include <vscroll.h>
#include <lineeditor.h>
#include <zwords.h>
#include <zmap.h>
#include <zrule.h>
#include <zobjdata.h>
#include <zuserdata.h>
#include <zsystem.h>
#include <dialog.h>

static ZSystem *game = NULL; // game system

#if 0
void *operator new(std::size_t sz)
{
  return heap_caps_malloc(sz, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT);
}

void *operator new[](std::size_t sz)
{
  return heap_caps_malloc(sz, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT);
}

void operator delete(void *p)
{
  free(p);
}

void operator delete[](void *p)
{
  free(p);
}
#endif

void setup() {
    // put your setup code here, to run once:
    auto cfg = M5.config();
    cfg.clear_display = true;
    M5.begin(cfg);
    M5.Display.setRotation(1);
    Serial.printf("Free heap size: %6d\r\n", esp_get_free_heap_size());
    // mount SD (need for M5Unified library)
    while (false == SD.begin(GPIO_NUM_4, SPI, 25000000))
    {
      M5.Display.println("SD Wait ...");
      delay(500);
    }
    game = new ZSystem();
    game->loadDict("/HHSAdv/highds.com");
    game->loadMap("/HHSAdv/map.dat");
    game->loadObjs("/HHSAdv/thin.dat");
    game->loadUser("/HHSAdv/data.dat");
    game->loadRules("/HHSAdv/rule.dat");
    game->loadMsgs("/HHSAdv/msg.dat");

#if 0
    Wire1.begin(); 
    pinMode(game->KEYBOARD_INT,INPUT);
    digitalWrite(game->KEYBOARD_INT, HIGH);
#endif
#if 0
    Dialog d;
    d.setTitle("テスト");
    d.setMessage("真ん中のボタンを押してください。");
    d.button(String(), "中", String());
    d.draw();
#endif
    game->title(); // draw title
}

void loop() {
  game->loop();
}
//
// High High School Adventure for M5 Series
//

#ifdef M5ATOM_LITE
#include <SPIFFS.h>
#include <M5GFX_ST7789.h>
#else
#include <SD.h>
#endif
#include <Wire.h>
#include <M5Unified.h>
#if defined(CONFIG_IDF_TARGET_ESP32S3)
#include <M5Cardputer.h>
#endif
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

//static ZSystem game = ZSystem::getInstance(); // game system
#ifdef M5ATOM_LITE
M5GFX_ST7789 extDisplay;
#else
static SPIClass spi; // spi is not allowed as local variable
#endif
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

void 
setup() 
{
    // put your setup code here, to run once:
    auto cfg = M5.config();
    cfg.clear_display = true;
    M5.begin(cfg);
#ifdef M5ATOM_LITE
    extDisplay.initExt();
    M5.addDisplay(extDisplay);
#else
    uint8_t ssPin = M5.getPin(m5::pin_name_t::sd_spi_ss);
  #if defined(CONFIG_IDF_TARGET_ESP32S3)
    if (M5.getBoard() == m5::board_t::board_M5Cardputer)
    {
      M5Cardputer.begin(cfg);
      spi.begin(
        M5.getPin(m5::pin_name_t::sd_spi_sclk),
        M5.getPin(m5::pin_name_t::sd_spi_miso),
        M5.getPin(m5::pin_name_t::sd_spi_mosi),
        M5.getPin(m5::pin_name_t::sd_spi_ss)
      );
    }
    else
    {
      spi = SPI;
    }
#else
    spi = SPI;
#endif
#endif
#ifdef M5ATOM_LITE
    M5.Display.setRotation(3);
#else
    M5.Display.setRotation(1);
#endif
    Serial.begin(115200);
    Serial.printf("Free heap size: %6d\r\n", esp_get_free_heap_size());
    Serial.printf("KB check: 5 == %d, 33 == %d, LOW == %d\r\n", digitalRead(5), digitalRead(33), LOW);
    // mount SD (need for M5Unified library)
#ifdef M5ATOM_LITE
    while (false == SPIFFS.begin())
    {
      M5.Display.println("SD Wait ...");
      delay(500);
    }
#else
    while (false == SD.begin(ssPin /*GPIO_NUM_4*/, spi, 25000000))
    {
      M5.Display.println("SD Wait ...");
      delay(500);
    }
#endif
    M5.Display.clear(BLACK);
    //game = new ZSystem();
    ZSystem::getInstance().loadDict("/HHSAdv/highds.com");
    ZSystem::getInstance().loadMap("/HHSAdv/map.dat");
    ZSystem::getInstance().loadObjs("/HHSAdv/thin.dat");
    ZSystem::getInstance().loadUser("/HHSAdv/data.dat");
    ZSystem::getInstance().loadRules("/HHSAdv/rule.dat");
    ZSystem::getInstance().loadMsgs("/HHSAdv/msg.dat");

    ZSystem::getInstance().title(); // draw title
}

void 
loop() 
{
  ZSystem::getInstance().loop();
}
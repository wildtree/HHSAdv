#ifndef __M5GFX_ST7789__
#define __M5GFX_ST7789__

#include <lgfx/v1/panel/Panel_ST7789.hpp>
#include <M5GFX.h>

class M5GFX_ST7789 : public M5GFX
{
  lgfx::Bus_SPI::config_t _bus_cfg;

public:

  using lgfx::LGFX_Device::init;
  bool initExt()//super.init()と区別するため…
  {
    setup();
    return init();
  }

  void setup()
  {
    _board = lgfx::board_t::board_unknown;

    {
      _bus_cfg.spi_host = HSPI_HOST;
      _bus_cfg.spi_mode = 3;             // SPI通信モードを設定 (0 ~ 3)
      _bus_cfg.freq_write = 40000000;    // 送信時のSPIクロック (最大80MHz, 80MHzを整数で割った値に丸められます)
      _bus_cfg.freq_read  = 16000000;    // 受信時のSPIクロック
      _bus_cfg.spi_3wire  = false;        // 受信をMOSIピンで行う場合はtrueを設定
      _bus_cfg.use_lock   = true;        // トランザクションロックを使用する場合はtrueを設定
      _bus_cfg.dma_channel = SPI_DMA_CH_AUTO; // 使用するDMAチャンネルを設定 (0=DMA不使用 / 1=1ch / 2=ch / SPI_DMA_CH_AUTO=自動設定)
      _bus_cfg.pin_sclk = EXT_SPI_SCLK;            // SPIのSCLKピン番号を設定
      _bus_cfg.pin_mosi = EXT_SPI_MOSI;            // SPIのMOSIピン番号を設定
      _bus_cfg.pin_miso = -1;            // SPIのMISOピン番号を設定 (-1 = disable)
      _bus_cfg.pin_dc   = EXT_SPI_DC;            // SPIのD/Cピン番号を設定  (-1 = disable)
    }
  }

  bool init_impl(bool use_reset, bool use_clear)
  {
    if (_panel_last.get() != nullptr) {
      return true;
    }
    auto p = new lgfx::Panel_ST7789(); // SSD1309
    auto b = new lgfx::Bus_SPI();
    b->config(_bus_cfg);
    {
      p->bus(b);
      auto cfg = p->config();
      
      cfg.pin_cs           =    EXT_SPI_CS;  // CSが接続されているピン番号   (-1 = disable)
      cfg.pin_rst          =    EXT_SPI_RST;  // RSTが接続されているピン番号  (-1 = disable)

      cfg.pin_busy         =    -1;  // BUSYが接続されているピン番号 (-1 = disable)

      // ※ 以下の設定値はパネル毎に一般的な初期値が設定されていますので、不明な項目はコメントアウトして試してみてください。

      cfg.memory_width     =   240;  // ドライバICがサポートしている最大の幅
      cfg.memory_height    =   320;  // ドライバICがサポートしている最大の高さ
      cfg.panel_width      =   240;  // 実際に表示可能な幅
      cfg.panel_height     =   240;  // 実際に表示可能な高さ
      cfg.offset_x         =     0;  // パネルのX方向オフセット量
      cfg.offset_y         =     0;  // パネルのY方向オフセット量
      cfg.offset_rotation  =     0;  // 回転方向の値のオフセット 0~7 (4~7は上下反転)
      cfg.dummy_read_pixel =     8;  // ピクセル読出し前のダミーリードのビット数
      cfg.dummy_read_bits  =     1;  // ピクセル以外のデータ読出し前のダミーリードのビット数
      cfg.readable         =  false;  // データ読出しが可能な場合 trueに設定
      cfg.invert           =  true;  // パネルの明暗が反転してしまう場合 trueに設定
      cfg.rgb_order        = false;  // パネルの赤と青が入れ替わってしまう場合 trueに設定
      cfg.dlen_16bit       = false;  // データ長を16bit単位で送信するパネルの場合 trueに設定
      cfg.bus_shared       =  false;  // SDカードとバスを共有している場合 trueに設定(drawJpgFile等でバス制御を行います)
      p->config(cfg);
      
    }
    setPanel(p);
    if (lgfx::LGFX_Device::init_impl(use_reset, use_clear)) {
      _panel_last.reset(p);
      _bus_last.reset(b);
      return true;
    }
    setPanel(nullptr);
    delete p;
    delete b;
    return false;
  }
};

#endif
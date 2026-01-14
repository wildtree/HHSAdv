//
// System Wrapper
//

#include <Wire.h>
#include <M5Unified.h>
#include <zsystem.h>
#include <dialog.h>

const String ZSystem::_credit[] = {
    "ハイハイスクールアドベンチャー",
    "Copyright(c)1995-2026",
    "ZOBplus",  
    "hiro"
};

ZSystem::ZSystem()
    : _dict(nullptr), _zmap(nullptr), _obj(nullptr), _user(nullptr), _msg(nullptr), _rules(nullptr),_mode(Title)
{
    _le = new LineEditor(30);
    _core = new ZCore();
    Serial.println("Init system.");
    switch (M5.getBoard())
    {
#ifndef M5ATOM_LITE
        case m5::board_t::board_M5Stack:
            _cv = new Canvas(32, 8, 256, 152);
            _zvs = new ZVScroll(160, 16);
            _prompt = new ZVScroll(224, 0);
            _keyboard = new M5StackKeyBoard;
            _dialog = new Dialog;
            break;
        case m5::board_t::board_M5StackCore2:
            _cv = new Canvas(32, 8, 256, 152);
            _zvs = new ZVScroll(160, 16);
            _prompt = new ZVScroll(224, 0);
            _keyboard = new M5Core2KeyBoard;
            _dialog = new Dialog;
            break;
#else
        case m5::board_t::board_M5Atom: // Special thanks to @shikarunochi san!!
            M5.Display.setSwapBytes(true);
            _cv = new Canvas(32, 8, 256, 152);
            _zvs = new ZVScroll(160, 16);
            _prompt = new ZVScroll(224, 0);
            _keyboard = new M5StackKeyBoard; // dummy -- BT keyboard should be used
            _dialog = new Dialog;
            break;
#endif
#if defined(CONFIG_IDF_TARGET_ESP32S3)
        case m5::board_t::board_M5Cardputer:
            M5.Display.setSwapBytes(true);
            _cv = new CardputerCanvas(64, 0, 256, 152);
            _zvs = new CardputerScroll(160, 16, 0, 95);
            _prompt = new CardputerScroll(224, 0, 0, 127);
            _keyboard = new M5CardputerKeyBoard;
            _dialog = new CardputerDialog;
            break;
#endif
        default:
            break;
    }
    Serial.printf("Free heap size: %6d\r\n", esp_get_free_heap_size());
}

ZSystem::~ZSystem()
{
    if (_cv) delete _cv;
    if (_zvs) delete _zvs;
    if (_le) delete _le;
    if (_core) delete _core;
    if (_dict) delete _dict;
    if (_zmap) delete _zmap;
    if (_obj) delete _obj;
    if (_user) delete _user;
    if (_msg) delete _msg;
    if (_rules) delete _rules;
    if (_keyboard) delete _keyboard;
    if (_dialog) delete _dialog;
}

ZSystem &
ZSystem::getInstance()
{
    static ZSystem _self;
    //Serial.printf("Free heap size: %6d\r\n", esp_get_free_heap_size());
    //Serial.printf("Singleton: ZSystem: %08x\r\n",&_self);
    return _self;
}

void
ZSystem::blekbdchk()
{
    if (_keyboard->keyboard_type() == KeyBoard::ble) return; // BLE keyboard in use.
    M5.update();
    if (M5.BtnA.wasHold())
    {
        _prompt->cls();
        _prompt->setFont(&fonts::lgfxJapanGothic_16);
        _prompt->setTextColor(BLUE);
        _prompt->print("Connecting to BLE Keyboard...");
        _prompt->invalidate();

        Serial.println("Connect to BLE Keybord.");
        delete _keyboard;
        _keyboard = new BTKeyBoard;
        _prompt->cls();
    }
}

void
ZSystem::chgscale()
{
    if (M5.getBoard() == m5::board_t::board_M5Cardputer) return;
    
    float s = _cv->getScale();
    if (s == 1.0) s = 0.75;
    else if (s == 0.75) s = 0.67;
    else s = 1.0;

    M5.Display.setColor(BLACK);
    M5.Display.fillRect(0, 0, M5.Displays(0).width(), M5.Displays(0).height());
 
    _cv->setScale(s);
    _cv->invalidate(true);
 
    _zvs->setScale(s);
    _zvs->invalidate();
 
    _prompt->setScale(s);
    _prompt->invalidate();
}

bool
ZSystem::loadDict(const String &file)
{
    _dict = new ZWords(file);
    return _dict != nullptr;
}

bool
ZSystem::loadMap(const String &file)
{
    _zmap = new ZMapRoot(file);
    return _zmap != nullptr;
}

bool
ZSystem::loadObjs(const String &file)
{
    _obj = new ZObjRoot(file);
    return _obj != nullptr;
}

bool
ZSystem::loadUser(const String &file)
{
    _user = new ZUserData(file);
    return _user != nullptr;
}

bool
ZSystem::loadMsgs(const String &file)
{
    _msg = new ZMessage(file);
    return _msg != nullptr;
}

bool
ZSystem::loadRules(const String &file)
{
    _rules = new ZRules(file);
    return _msg != nullptr;
}

void
ZSystem::title(void) const
{
    _zvs->setTextColor(WHITE);

    _cv->paint(0,0,BLUE,BLUE);
    _zmap->setCursor(76);
    _zmap->curMapData().draw(_cv);
    _zvs->print("");
    for(int i = 0 ; i < 4 ; i++)
    {
        _zvs->scrollLine();
        _zvs->print(_credit[i]);
    }
    _cv->invalidate();
    _zvs->invalidate();
}

void
ZSystem::prompt(void) const
{
    _prompt->home();
    _prompt->setFont(&fonts::lgfxJapanGothic_16);
    _prompt->setTextColor(GREEN);
    _prompt->print((_mode == Play) ? "どうする? " : "何かキーを押してください。");
    _prompt->invalidate();
}

void ZSystem::game_over(void)
{
    _mode = GameOver;
    _prompt->cls();
    prompt();
}

void
ZSystem::time_elapsed(void)
{
  // battery
  if (_user->getFact(3) > 0 && _user->getFact(7) == 1)
  {
    // light is ON
    _user->decFact(3); // consume battery life
    if (_user->getFact(3) < 8 && _user->getFact(3) > 0)
    {
      // low battery
      _user->setFact(6,1); // dim mode
      _zvs->print(_msg->getMessage(0xd9));
    }
    if (_user->getFact(3) == 0)
    {
      // battery ware out
      _user->setFact(7, 0); // light off
      _zvs->print(_msg->getMessage(0xc0));
    }
  }
  // dynamite
  if (_user->getFact(11) > 0)
  {
    if (_user->decFact(11) == 0)
    {
      // play(2); // explosion sound.
      _zvs->scrollLine();
      _zvs->print(_msg->getMessage(0xd8));
      if (_user->getPlace(7) == 48)
      {
        _user->getMap(75 - 1).setLink(ZMapLink::North, 77);
        _user->getMap(68-  1).setLink(ZMapLink::West, 77);
        _zvs->scrollLine();
        _zvs->print(_msg->getMessage(0xda));
      }
      else if (_user->getPlace(7) == 255 || _user->getPlace(7) == _zmap->getCursor())
      {
        // screen to red.
        _zvs->print(_msg->getMessage(0xcf));
        _zvs->print(_msg->getMessage(0xcb));
        game_over();
      }
      else
      {
        _user->setPlace(7,0); // lost dynamite in somewhere else
      }
    }
  }
}

void
ZSystem::check_teacher(void)
{
  if (_mode == GameOver || _user->getFact(1) == _core->mapId()) return;
  int rd = 100 + _core->mapId() + ((_user->getFact(1) > 0) ? 1000 : 0);
  int rz = random(0, 3000);
  _user->setFact(1, (rd < rz) ? 0 : _core->mapId());
  switch (_core->mapId())
  {
    case 1:
    case 48:
    case 50:
    case 51:
    case 52:
    case 53:
    case 61:
    case 64:
    case 65:
    case 66:
    case 67:
    case 68:
    case 69:
    case 70:
    case 71:
    case 72:
    case 73:
    case 74:
    case 75:
    case 76:
    case 77:
    case 83:
    case 86:
      _user->setFact(1,0);
      break;
  }
}

bool
ZSystem::check_darkness(void)
{
    bool dim = false;
    switch (_core->mapId())
    {
        case 47:
        case 48:
        case 49:
        case 61:
        case 64:
        case 65:
        case 67:
        case 68:
        case 69:
        case 71:
        case 74:
        case 75:
        case 77:
            if (_user->getFact(7) != 0)
            {
                if (_user->getFact(6) != 0)
                {
                    // dark mode. (color in blue)
                    _cv->setColorFilter(Canvas::blueFilter);
                    dim = true;
                }
            }
            else
            {
                // black-out
                _core->mapView(_core->mapId());
                _core->mapId(84);
                _zmap->setCursor(84);
            }
            break;
        default:
            if (_user->getFact(6) != 0)
            {
                // back to normal (remove color filter)
                _cv->resetColorFilter();
            }
            break;
    }
    return dim;
}

void
ZSystem::draw_screen(bool with_msg)
{
    //Serial.print("check darkness.\r\n");
    check_darkness();
    //Serial.print("draw map.\r\n");
    _zmap->curMapData().draw(_cv);
    _cv->colorFilter();
    //Serial.printf("map_id:%d\r\n", _zmap->getCursor());
    if (with_msg && _mode != GameOver)
    {
        _zvs->scrollLine();
        if (_zmap->curMapData().isBlank())
        {
            _zvs->print(_msg->getMessage(0xcc));
        }
        else
        {
            _zvs->print(_zmap->curMapData().getMessage());
        }
    }
    for (int i = 0 ; i < ZUserData::items ; i++)
    {
        //Serial.printf(" item_check: %d = %d ... ", i, _user->getPlace(i));
        if (_user->getPlace(i) == _zmap->getCursor())
        {
            if (i == 1 && _user->getFact(0) != 1)
            {
                _obj->draw(_cv, i + 1, true);
            }
            else
            {
                _obj->draw(_cv, i + 1);
            }
            if (with_msg && _mode != GameOver)
            {
                _zvs->print(_msg->getMessage(0x96 + i));
            }
        }
        //Serial.print("done.\r\n");
    }
    if (_user->getFact(1) == _zmap->getCursor())
    {
        _obj->draw(_cv, 14); // draw teacher
        if (with_msg && _mode != GameOver)
        {
            _zvs->print(_msg->getMessage(0xb4));
        }
    }
    _cv->invalidate();
    _zvs->invalidate();
}

void
ZSystem::save_game(int f)
{
    const String filename = "/HHSAdv/" + String(f) + ".dat";
    File fp = SD.open(filename, FILE_WRITE, true);
    fp.write(_core->pack(), ZCore::packed_size);
    fp.write(_user->pack(), ZUserData::packed_size);
    fp.close();
}

void
ZSystem::load_game(int f)
{
    const String filename = "/HHSAdv/" + String(f) + ".dat";
    uint8_t *buf = new uint8_t [max(ZCore::packed_size, ZUserData::packed_size)];
    File fp = SD.open(filename, FILE_READ);
    if (fp.read(buf,ZCore::packed_size) > 0)
    {
        _core->unpack(buf);
    }
    if (fp.read(buf,ZUserData::packed_size) > 0)
    {
        _user->unpack(buf);
    }
    fp.close();
    delete[] buf;
}

void
ZSystem::dialog(uint8_t id)
{
#if defined(CONFIG_IDF_TARGET_ESP32S3)
    //Dialog *dialog = (M5.getBoard() == m5::board_t::board_M5Cardputer) ? new CardputerDialog() : new Dialog();
    Dialog *dialog = _dialog;
#else
    float s = _cv->getScale();
    Dialog *dialog = _dialog; // new Dialog();
    dialog->setScale(s);
#endif
    int r;
    switch(id)
    {
        case 0: // boy or girl?
            _user->setFact(0, 1); // default (boy)
            dialog->setTitle("男子/女子");
            dialog->setMessage(_msg->getMessage(0xe7));
            dialog->button("男子","女子",String());
            _user->setFact(0, dialog->draw());
            _core->mapId(3);
            _zmap->setCursor(3);
            break;
        case 1: // file select
            dialog->setTitle("ファイルを選んでください。");
            dialog->setMessage(_msg->getMessage(0xe8));
            dialog->button("1","2","3");
            if (_core->cmdId() != 0x0f)
            {
                // load game
                bool file_exists = false;
                File f = SD.open("/HHSAdv/1.dat",FILE_READ);
                if (f)
                {
                    f.close();
                    file_exists = true;
                }
                else
                {
                    dialog->btnA()->disable();
                }
                f = SD.open("/HHSAdv/2.dat",FILE_READ);
                if (f)
                {
                    f.close();
                    file_exists = true;
                }
                else
                {
                    dialog->btnB()->disable();
                }
                f = SD.open("/HHSAdv/3.dat",FILE_READ);
                if (f)
                {
                    f.close();
                    file_exists = true;
                }
                else
                {
                    dialog->btnC()->disable();
                }
                if (!file_exists)
                {
                    dialog->setTitle("ファイルがありません");
                    dialog->setMessage("セーブデータが存在しません。");
                    dialog->button(String(), "了解", String());
                    dialog->btnB()->enable();
                    dialog->draw();
                    break;
                }
            }
            r = dialog->draw();
            if (_core->cmdId() == 0x0f)
            {
                // save
                save_game(r);
            }
            else
            {
                // load
                load_game(r);
            }
            break;
        case 2: // show items
            dialog->setTitle("持ち物");
            // 持ち物リストを作成
            dialog->setMessage(_user->itemList());
            dialog->button(String(), "OK", String());
            dialog->draw();
            break;
        case 3: // cut calbe (yellow or red)
            dialog->setTitle("どちらを切りますか？");
            dialog->setMessage(_msg->getMessage(0xe9));
            dialog->button("黄","赤", String());
            r = dialog->draw();
            if (_user->getPlace(11) != 0xff)
            {
                _zvs->scrollLine();
                _zvs->print(_msg->getMessage(0xe0));
            }
            if (r == 1 || _user->getPlace(11) != 0xff)
            {
                // gameover
                _cv->setColorFilter(Canvas::redFilter);
                draw_screen(false);
                _zvs->scrollLine();
                _zvs->print(_msg->getMessage(0xc7));
                _zvs->scrollLine();
                _zvs->print(_msg->getMessage(0xee));
                game_over();
            }
            else
            {
                // gameclear
                _user->setPlace(11,0);
                _core->mapId(74);
                _zmap->setCursor(74);
                // play sound #3
            }
            break;
        default:
            break;
    }
    //delete dialog;
}

void
ZSystem::interpreter(void)
{
    bool ok = false;
    int j = 0;
    for (ZRulesIterator i = _rules->begin() ; i != _rules->end() ; i++)
    {
        // Serial.printf("%03d: rule check\r\n", j++);
        if ((*i).run(_core, _user))
        {
            _zmap->setCursor(_core->mapId());
            while (!_core->is_empty())
            {
                ZCommand c = _core->pop();
                uint8_t op = c.getOperand();
                switch (c.getCommand())
                {
                    case ZCommand::Nop:
                        break;
                    case ZCommand::Message:
                        _zvs->scrollLine();
                        _zvs->print((op & 0x80) ? _msg->getMessage(op) : _zmap->curMapData().find(_core->cmdId(), _core->objId()));
                        break;
                    case ZCommand::Sound:
                        break;
                    case ZCommand::Dialogue:
                        dialog(op);
                        break;
                    case ZCommand::GameOver:
                        switch(op)
                        {
                            case 1: // arrested by the teacher
                                _cv->setColorFilter(Canvas::sepiaFilter);
                                draw_screen(false);
                                break;
                            case 2: // died due to invloved the explosion
                                _cv->setColorFilter(Canvas::redFilter);
                                draw_screen(false);
                                break;
                            case 3: // game clear
                                break;
                            default:
                                break;
                        }
                        game_over();
                        break;
                    default:
                        break;
                }
                
            }
            if (_mode == GameOver) return;
            _zvs->scrollLine();
            _zvs->print(_msg->getMessage(0xed)); // O.K.
            ok = true;
            break;
        }
        // Serial.printf("rule done. (ok = %d)\r\n", ok);
    }
    // Serial.printf("map id = %d\r\n", _core->mapId());
    _zmap->setCursor(_core->mapId());
    // if (dialog_id >= 0) return;
    if (!ok)
    {
        String m = _zmap->curMapData().find(_core->cmdId(), _core->objId());
        if (m.isEmpty())
        {
            m = _msg->getMessage(0xec); // "ダメ";
        }
        _zvs->scrollLine();
        _zvs->print(m);
    }
    if (_zmap->getCursor() == 74)
    {
        int m_id = 0;
        switch(_user->incFact(13))
        {
            case 4:  m_id = 0xe2; break;
            case 6:  m_id = 0xe3; break;
            case 10: m_id = 0xe4; break;
        }
        if (m_id != 0)
        {
            _zvs->scrollLine();
            _zvs->print(_msg->getMessage(m_id));
        }
    }
}

void
ZSystem::run(const String &cmd)
{
    Serial.printf("Free heap size: %6d\r\n", esp_get_free_heap_size());
    _zvs->setTextColor(CYAN);
    _zvs->scrollLine();
    _zvs->print(">>> ");
    _zvs->print(cmd);
    _zvs->setTextColor(WHITE);
    //_zvs->scrollLine();
    // parse command line
    _core->cmdId(_dict->findVerb(cmd.substring(0, cmd.indexOf(' '))));
    if (cmd.indexOf(' ') < 0)
    {
        _core->objId(-1);
    }
    else
    {
        _core->objId(_dict->findObjs(cmd.substring(cmd.indexOf(' ')+1)));
    }
    //Serial.printf("(v,o) = (%d,%d)\r\n", _core->cmdId(), _core->objId());
    // interpret
    time_elapsed();
    //Serial.print("time_elapsed: ok\r\n");
    if (_mode == GameOver) return;
    interpreter();
    if (_mode == GameOver) return;
    //Serial.print("interpreter: ok\r\n");
    // if (_dialog_id>= 0) return;
    check_teacher();
    //Serial.print("teacher check done.");
    draw_screen(true);
}

void
ZSystem::start(void)
{
    if (_mode != Play) return;
    _cv->resetColorFilter();
    _prompt->cls();
    //M5.Display.fillRect(0, 224, 320, 16, BLACK);
    _zmap->setCursor(1);
    _core->mapId(1);
    M5.Display.setTextColor(WHITE);
    _zvs->cls();
    draw_screen(true);  
}

void
ZSystem::loop(void)
{
    //Serial.printf("Stack size = %d\r\n", uxTaskGetStackHighWaterMark(nullptr));
    blekbdchk();
    prompt();
    if (_mode != Play)
    {
        if (_keyboard->wait_any_key())
        {
            
            if (_mode == Title)
            {
                playing();
                start();
            }
            else
            {
                setMode(Title);
                title();
                loadUser("/HHSAdv/data.dat"); // initialize data
            }
            _prompt->cls();
        }
    }
    else
    {
        uint8_t c;
        if (_keyboard->fetch_key(c))
        {
            if (c == 7) 
            {
                chgscale();
                return;
            }
            if (c == 0x1e || c == 0x1f)
            {
                _zvs->scroll((c == 0x1e) ? -1 : 1);
                return;
            }

            String cmd = _le->putChar(c);
            if (c == '\r')
            {
                //M5.Display.fillRect(80, 224, 240, 16, BLACK);
                run(cmd);
                _prompt->cls();
                return;
            }
            _prompt->cls();
            prompt();
            _prompt->setTextColor(WHITE);
            _prompt->print((String)*_le);
            _prompt->invalidate();
            //_le->draw(80, 224, 240, 16, WHITE, BLACK);
        }
    }
}
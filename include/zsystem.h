//
// System Wrapper
//

#if !defined(ZSYSTEM_H)
#define ZSYSTEM_H

#ifdef M5ATOM_LITE
#include <SPIFFS.h>
#else
#include <SD.h>
#endif
#include <M5Unified.h>
#include <zmap.h>
#include <zobjdata.h>
#include <zrule.h>
#include <zuserdata.h>
#include <zwords.h>
#include <lineeditor.h>
#include <vscroll.h>
#include <keyboard.h>
#include <dialog.h>

#ifdef M5ATOM_LITE
#define SD SPIFFS
#endif

class ZSystem
{
protected:
    LineEditor *_le;
    ZVScroll *_zvs, *_prompt;
    Canvas *_cv;
    ZCore *_core;
    ZWords *_dict;
    ZMapRoot *_zmap;
    ZObjRoot *_obj;
    ZUserData *_user;
    ZMessage *_msg;
    ZRules *_rules;
    KeyBoard *_keyboard;
    Dialog *_dialog;

    static const String _credit[];
    enum Mode {
        Title = 0,
        Play,
        GameOver,
    } _mode;

    Mode getMode(void) const { return _mode; }
    void setMode(Mode m) { _mode = m; }
    void dialog(uint8_t id);
    void blekbdchk();
    void chgscale();
    ZSystem();
    ~ZSystem();
public:
    ZSystem(const ZSystem&) = delete;
    ZSystem &operator=(const ZSystem&) = delete;
    ZSystem(ZSystem&&) = delete;
    ZSystem &operator=(ZSystem&&) = delete;
    static ZSystem &getInstance();

    bool loadDict(const String &file);
    bool loadMap(const String &file);
    bool loadObjs(const String &file);
    bool loadUser(const String &file);
    bool loadMsgs(const String &file);
    bool loadRules(const String &file);

    void load_game(int f);
    void save_game(int f);
    void playing(void) { setMode(Play); }
    void game_over(void);
    void time_elapsed(void);
    void check_teacher(void);
    bool check_darkness(void);
    void draw_screen(bool);
    void title(void) const;
    void prompt(void) const;
    void interpreter(void);
    void run(const String &cmd);
    void start(void);
    void loop(void);

    KeyBoard *getKeyboard() { return _keyboard; }

    static const int KEYBOARD_I2C_ADDR = 0x08;
    static const int KEYBOARD_INT = 33; // For M5 stack GPIO5 / Core2 GPIO33 5;
};
#endif
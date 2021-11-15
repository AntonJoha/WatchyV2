#ifndef MENU_H
#define MENU_H
#include "AppFrame.h"

extern RTC_DATA_ATTR byte position;

typedef struct MenuList{
    struct MenuList* next;
    AppFrame* (*factory)(void);
    char *name;
}MenuList;

class AppMenu{
    public:
        void draw();
        int handleButtonPress(uint64_t wakeupBit);
        AppMenu();
        ~AppMenu();
        void addApp(const char * name, AppFrame*(*factory)(void));
        int apphandleButtonPress(uint64_t wakeupBit);
        void appDraw();
        MenuList* head;
        AppFrame* frame;

    private:
        int fastMenu();
        int menuButton();
        void downButton();
        void upButton();

};


#endif

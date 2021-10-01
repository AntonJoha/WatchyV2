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
        void draw(void * data);
        int handleButtonPress(uint64_t wakeupBit, void * data);
        AppMenu();
        ~AppMenu();
        void addApp(char * name, AppFrame*(*factory)(void));
    private:
        MenuList* head;
};


#endif
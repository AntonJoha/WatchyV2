#include "AppMenu.h"
#include "../Watchy.h"
#include <cstring>

RTC_DATA_ATTR byte position;


byte menuLenght(MenuList *head)
{
    byte value = 0;
    while(head != nullptr)
    {
        value++;
        head = head->next;
    }
    return value;
}

MenuList * getPos(unsigned int pos, MenuList* head)
{
    MenuList * toReturn = head;
    for (unsigned int i = 0; i < pos && toReturn != nullptr; ++i)
        toReturn = toReturn->next;
    return toReturn;
}

AppMenu::AppMenu() {
    head = nullptr;
    frame = nullptr;
}

AppMenu::~AppMenu()
{

    if (head != nullptr)
    {
        MenuList* next = head->next;

        while (next != nullptr)
        {
            MenuList * temp = next->next;
            free(next->name);
            free(next);
            next = temp;
        } 
        free(head);
    }
}


void AppMenu::draw(void * data){
    auto display = Watchy::getDisplay();

    display->init(0, false);
    display->setFullWindow();
    display->fillScreen(GxEPD_BLACK);
    display->setFont(&FreeMonoBold9pt7b);

    int16_t x1, y1;
    uint16_t w, h;

    byte page = position/6;
    byte relativePos = position%6;

    int16_t ypos;
    MenuList* current = getPos(page*6, head);   


    for (int i = 0; i < MENU_LENGTH && current != nullptr; ++i)
    {
        ypos = 30+(MENU_HEIGHT*i);
        display->setCursor(0, ypos);
        if (i == relativePos)
        {
            display->getTextBounds(current->name, 0, ypos, &x1, &y1, &w, &h);
            display->fillRect(x1-1, y1-10, 200, h+15, GxEPD_WHITE);
            display->setTextColor(GxEPD_BLACK);
            display->println(current->name);
        }
        else
        {
            display->setTextColor(GxEPD_WHITE);
            display->println(current->name);
        }
        current = current->next;
    }
    
    /*
    display.init(0, false); //_initial_refresh to false to prevent full update on init
    display.setFullWindow();
    display.fillScreen(GxEPD_BLACK);
    display.setFont(&FreeMonoBold9pt7b);

    int16_t  x1, y1;
    uint16_t w, h;
    int16_t yPos;
    const char *menuItems[] = {"Check Baaattery", "Vibrate Motor", "Show Accelerometer", "Set Time", "Setup WiFi", "Update Firmware"};
    for(int i=0; i<MENU_LENGTH; i++){
        yPos = 30+(MENU_HEIGHT*i);
        display.setCursor(0, yPos);
        if(i == position){
            display.getTextBounds(menuItems[i], 0, yPos, &x1, &y1, &w, &h);
            display.fillRect(x1-1, y1-10, 200, h+15, GxEPD_WHITE);
            display.setTextColor(GxEPD_BLACK);
            display.println(menuItems[i]);      
        }
        else{
            display.setTextColor(GxEPD_WHITE);
            display.println(menuItems[i]);
        }   
    }
    */
    display->display(false);
    display->hibernate();

}

int AppMenu::handleButtonPress(uint64_t wakeupBit, void * data)
{
    if (wakeupBit & MENU_BTN_MASK)
    {
        MenuList * item = getPos(position, head);
        AppFrame *frame = item->factory();
        if (frame->runnable() == true)
        return frame->run();
        return APP_STATE;
    }
    else if (wakeupBit & BACK_BTN_MASK)
    {
        return WATCHFACE_STATE;
    }
    else if (wakeupBit & UP_BTN_MASK)
    {
        position--;
        if (position < 0) position = menuLenght(head) - 1;
        if (position < 0) position = 0;
        draw(data);
    }
    else if (wakeupBit & DOWN_BTN_MASK)
    {
        position++;
        if (position >= menuLenght(head)) position = 0;
        draw(data);
    }

    //TODO MAKE A NEW FAST MENU

    return MAIN_MENU_STATE;
}

MenuList * assignMenuList(char *s , AppFrame*(*factory)(void))
{
    MenuList * toReturn = (MenuList*) malloc(sizeof(MenuList));
    toReturn->next = nullptr;
    toReturn->name = s;
    toReturn->factory = factory;

    return toReturn;
}

void AppMenu::addApp(const char * s, AppFrame*(*factory)(void))
{
   MenuList* h = head;
   size_t len = strlen(s);
   char* name = (char*) malloc(sizeof(char)*len);
   strcpy(name, s);
   if (h == nullptr)
   {
       head = assignMenuList(name, factory);
   } 
   else
   {
        while (h->next != nullptr) h = h->next;
        h->next = assignMenuList(name, factory);
   }
}


void AppMenu::appDraw(void * data)
{
    MenuList* item = getPos(position, head);
    frame = item->factory();
    frame->draw(data);
}

int AppMenu::apphandleButtonPress(uint64_t wakeupBit, void* data)
{
    return frame->handleButtonPress(wakeupBit, data);
}
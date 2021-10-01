#include "AppMenu.h"
#include "../Watchy.h"
#include <cstring>

RTC_DATA_ATTR byte position;

byte menuLenght(MenuList *head)
{
    return 6;
    byte value = 0;
    while(head != nullptr)
    {
        value++;
        head = head->next;
    }
    return value;
}

AppMenu::AppMenu() {};

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

    display.display(false);
    display.hibernate();

}

int AppMenu::handleButtonPress(uint64_t wakeupBit, void * data)
{
    if (wakeupBit & MENU_BTN_MASK)
    {
        switch(position)
        {
            case 0:
                Watchy::showBattery();
                break;
            case 1:
                Watchy::showBuzz();
                break;          
            case 2:
                Watchy::showAccelerometer();
                break;
            case 3:
                Watchy::setTime();
                break;
            case 4:
                Watchy::setupWifi();
                break;                    
            case 5:
                Watchy::showUpdateFW();
                break;
            default:
                break;                              
        }
    }
    else if (wakeupBit & BACK_BTN_MASK)
    {
        return WATCHFACE_STATE;
    }
    else if (wakeupBit & UP_BTN_MASK)
    {
        position--;
        if (position < 0) position = menuLenght(head) - 1;
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

void AppMenu::addApp(char * s, AppFrame*(*factory)(void))
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
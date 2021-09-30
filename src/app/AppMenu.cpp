#include "AppMenu.h"
#include "../Watchy.h"


AppMenu::~AppMenu()
{

    if (head != nullptr)
    {
        MenuList* next = head->next;

        while (next != nullptr)
        {
            MenuList * temp = next->next;
            free(next);
            next = temp;
        } 
        free(head);
    }
}


RTC_DATA_ATTR uint8_t position;

void AppMenu::draw(void * data){
    auto display = Watchy::getDisplay();

    display.init(0, false); //_initial_refresh to false to prevent full update on init
    display.setFullWindow();
    display.fillScreen(GxEPD_BLACK);
    display.setFont(&FreeMonoBold9pt7b);

    int16_t  x1, y1;
    uint16_t w, h;
    int16_t yPos;

    const char *menuItems[] = {"Check Battery", "Vibrate Motor", "Show Accelerometer", "Set Time", "Setup WiFi", "Update Firmware"};
    for(int i=0; i<MENU_LENGTH; i++){
        yPos = 30+(MENU_HEIGHT*i);
        display.setCursor(0, yPos);
        if(i == menuIndex){
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


MenuList * assignMenuList(char *s , AppFrame*(*factory)(void))
{
    MenuList * toReturn = (MenuList*) malloc(sizeof(MenuList));
    toReturn->next = nullptr;
    toReturn->name = s;
    toReturn->factory = factory;

    return toReturn;
}

void AppMenu::addItem(char * s, AppFrame*(*factory)(void))
{
   MenuList* h = head;
   if (h == nullptr)
   {
       head = assignMenuList(s, factory);
   } 
   else
   {
        while (h->next != nullptr) h = h->next;
        h->next = assignMenuList(s, factory);
   }
}
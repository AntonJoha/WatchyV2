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


void AppMenu::draw(){
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
    
    display->display(true);
    display->hibernate();

}

int AppMenu::fastMenu(){

    draw();

    long lastTimeout = millis();
    
    pinMode(MENU_BTN_PIN, INPUT);
    pinMode(BACK_BTN_PIN, INPUT);
    pinMode(UP_BTN_PIN, INPUT);
    pinMode(DOWN_BTN_PIN, INPUT);

    while(true){
        //If it has been inactive for too long
        if(millis() - lastTimeout > 5000)
        {
            break;
        }
        
        if (digitalRead(MENU_BTN_PIN) == 1)
        {
            return menuButton();
        }
        else if (digitalRead(BACK_BTN_PIN) == 1){
            return WATCHFACE_STATE;
        }
        else if (digitalRead(UP_BTN_PIN) == 1){
            upButton();
            lastTimeout = millis();
            draw();
        }
        else if (digitalRead(DOWN_BTN_PIN) == 1){
            downButton();
            lastTimeout = millis();
            draw();
        }

    }

    return MAIN_MENU_STATE;
}

int AppMenu::handleButtonPress(uint64_t wakeupBit)
{
    if (wakeupBit & MENU_BTN_MASK)
    {
        return menuButton();
    }
    else if (wakeupBit & BACK_BTN_MASK)
    {
        return WATCHFACE_STATE;
    }
    else if (wakeupBit & UP_BTN_MASK)
    {
        upButton();
    }
    else if (wakeupBit & DOWN_BTN_MASK)
    {
        downButton();
    }


    return fastMenu();
}

void AppMenu::upButton(){
    
    position--;
    if (position > menuLenght(head)) position = menuLenght(head) - 1;
}

void AppMenu::downButton(){
    
    position++;
    if (position >= menuLenght(head)) position = 0;
}

int AppMenu::menuButton(){
    MenuList * item = getPos(position, head);
    AppFrame *frame = item->factory();
    if (frame->runnable() == true){
		int state = frame->run();
		if (state == MAIN_MENU_STATE) draw();
		return state;
	}
	frame->draw();
    return APP_STATE;
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


void AppMenu::appDraw()
{
    MenuList* item = getPos(position, head);
    frame = item->factory();
    frame->draw();
}

int AppMenu::apphandleButtonPress(uint64_t wakeupBit)
{
	MenuList* item = getPos(position, head);
	frame = item->factory();
    int state = frame->handleButtonPress(wakeupBit);
	if (state == MAIN_MENU_STATE) draw();
	return state;
}

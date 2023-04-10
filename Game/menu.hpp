#ifndef MENU_HPP
#define MENU_HPP

#define NAME_LEN 20
struct Button {
    char name[NAME_LEN];
    float vert[8], buffer[50*NAME_LEN];
    int num_quads;
    float textPosX, textPosY, textS;
    bool isHover, isDown, isActive;
};

Button* getButtons();
int getState();
void setState(int st);

int Menu_AddButton(char *name, float x, float y, float width, float height, float textS);
void ShowButton(int buttonId);
void Menu_ShowMenu();
int Menu_MouseMove(float x, float y);
int Menu_MouseDown();
void Menu_MouseUp();
char* Menu_GetButtonName(int buttonID);
void Menu_Clear();
void MouseDown();

#endif // MENU_HPP

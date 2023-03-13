#include "menu.hpp"
#include <gl/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "../stb-master_lib/stb_easy_font.h"
#include "texture.hpp"

Button *btns;
int btnCnt = 0;

float mouseX, mouseY;
int menuState = 0;

Button* getButtons() { return btns; }
int getState() { return menuState; }

int Menu_AddButton(char *name, float x, float y, float width, float height, float textS) {
    btns = (Button*)realloc(btns, sizeof(btns[0])*(++btnCnt));

    snprintf(btns[btnCnt-1].name, NAME_LEN, "%s", name);
    float *vert = btns[btnCnt-1].vert;
    vert[0]=vert[6]=x;
    vert[2]=vert[4]=x+width;
    vert[1]=vert[3]=y;
    vert[5]=vert[7]=y+height;
    btns[btnCnt-1].isHover = false;
    btns[btnCnt-1].isDown = false;
    btns[btnCnt-1].isActive = true;

    Button *b = btns + btnCnt - 1;
    b->num_quads = stb_easy_font_print(0, 0, name,0,b->buffer, sizeof(b->buffer));
    b->textPosX = x +(width-stb_easy_font_width(name)*textS)/2.0;
    b->textPosY = y +(height-stb_easy_font_height(name)*textS)/2.0;
    b->textPosY+= textS*2;
    b->textS = textS;

    return btnCnt-1;
}

void ShowButton(int buttonId) {
    Button btn1 = btns[buttonId];
    glVertexPointer(2, GL_FLOAT, 0, btn1.vert);
    glEnableClientState(GL_VERTEX_ARRAY);
    if (!btn1.isActive) glColor3f(0.3, 0.3, 0.3);
    else if(btn1.isDown)glColor3f(0.2,1,0.2);
    else if (btn1.isHover)glColor3f(0.8,0.8,1);
    else glColor3f(0.6,0.6,0.6);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glColor3f(0.5,0.5,0.5);
    glLineWidth(1);
    glDrawArrays(GL_LINE_LOOP,0,4);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPushMatrix();
    glColor3f(0.1,0.1,0.1);
    glTranslatef(btn1.textPosX,btn1.textPosY,0);
    glScalef(btn1.textS,btn1.textS,0);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, btn1.buffer);
    glDrawArrays(GL_QUADS, 0, btn1.num_quads*4);
    glDisableClientState(GL_VERTEX_ARRAY);
    glPopMatrix();
}

void Menu_ShowMenu() {
    int start, finish;
    switch (menuState) {
        case 1: start = 3; finish = btnCnt; break;
        default: start = 0; finish = 3;
    }
    for(int i=start; i<finish; i++) ShowButton(i);
}

bool isCordInButton(int buttonID, float x, float y) {
    float *vert=btns[buttonID].vert;
    return (x>vert[0]) && (y>vert[1]) && (x<vert[4]) && (y<vert[5]);
}

int Menu_MouseMove (float x, float y) {
    mouseX = x;
    mouseY = y;
    int movebtn=-1;
    int start, finish;
    switch (menuState) {
        case 1: start = 3; finish = btnCnt; break;
        default: start = 0; finish = 3;
    }
    for(int i=start; i<finish; i++)
        if (isCordInButton(i, mouseX, mouseY)) {
            btns[i].isHover = 1;
            movebtn = i;
        } else {
            btns[i].isDown = 0;
            btns[i].isHover = 0;
        }
    return movebtn;
}

int Menu_MouseDown() {
    int downbtn=-1;
    int start, finish;
    switch (menuState) {
        case 1: start = 3; finish = btnCnt; break;
        default: start = 0; finish = 3;
    }
    for(int i=start; i<finish; i++)
        if (btns[i].isActive && isCordInButton(i, mouseX, mouseY)) {
            btns[i].isDown = 1;
            downbtn = i;
        }
    return downbtn;
}

void Menu_MouseUp() { for (int i=0; i < btnCnt; i++) btns[i].isDown=0; }

char* Menu_GetButtonName(int buttonID) { return btns[buttonID].name; }

void Menu_Clear() { btnCnt=0; free(btns); btns=0; }

void MouseDown() {
    int buttonId = Menu_MouseDown();
    if (buttonId<0) return;
    char *name = Menu_GetButtonName(buttonId);
    printf("%s\n",name, &buttonId);

    switch (buttonId) {
    case 1: menuState = 1; break;
    case 3: nextLine(-1); break;
    case 4: nextLine(1); break;
    case 5: menuState = 0; break;
    case 2:
        PostQuitMessage(0);
        break;
    }

}

#include <gl/gl.h>
#include <math.h>
#include <string>

#include "game.hpp"
#include "texture.hpp"
#include "stb-master_lib/stb_image.h"
#include "menu.hpp"

short tileSize = 16;

Hero hero = {WIDTH/2 - 225/2, 3*tileSize};

void Game(GLuint texture, GLuint background) {
    static float svertixFrame[] = { 0,0, 0,0, 0,0, 0,0 };
    static float TexCordFrame[] = { 0,0, 0,0, 0,0, 0,0 };
    static float tileCords[] = { 0,0, 0,0, 0,0, 0,0 };
    static float sv_bg[] = {0,0, 0,HEIGHT, WIDTH,HEIGHT, WIDTH,0};
    static float tc_bg[] = {0,1, 0,0, 1,0, 1,1};

    short space = GetKeyState(VK_SPACE), left = GetKeyState(VK_LEFT), right = GetKeyState(VK_RIGHT);
    if (space < 0) hero.jump();
    if (left < 0 || right < 0) {
        hero.vx = 4;
        hero.animation = 0;
        hero.direction = left<0 ? LEFT : RIGHT;
    } else if (hero.onGround) {
        hero.vx = 0;
        hero.animation = 2;
    }

    hero.iter();

    svertixFrame[0] = svertixFrame[2] = hero.x;
    svertixFrame[1] = svertixFrame[7] = hero.y;
    svertixFrame[3] = svertixFrame[5] = hero.y+hero.h*hero.scale;
    svertixFrame[4] = svertixFrame[6] = hero.x+hero.w*hero.scale;

    TexCordFrame[3] = TexCordFrame[5] = hero.animation/3.;
    TexCordFrame[1] = TexCordFrame[7] = (hero.animation+1)/3.;
    if (hero.direction == RIGHT) {
        TexCordFrame[0] = TexCordFrame[2] = hero.frame/8.;
        TexCordFrame[4] = TexCordFrame[6] = (hero.frame+1)/8.;
    } else {
        TexCordFrame[4] = TexCordFrame[6] = hero.frame/8.;
        TexCordFrame[0] = TexCordFrame[2] = (hero.frame+1)/8.;
    }

    GlSettingsOn(background);
        glVertexPointer(2,GL_FLOAT,0, sv_bg);
        glTexCoordPointer(2,GL_FLOAT,0, tc_bg);
        glDrawArrays(GL_TRIANGLE_FAN,0,4);
    glBindTexture(GL_TEXTURE_2D, texture);
        glVertexPointer(2,GL_FLOAT,0,svertixFrame);
        glTexCoordPointer(2,GL_FLOAT,0,TexCordFrame);
        glDrawArrays(GL_TRIANGLE_FAN,0,4);
    glBindTexture(GL_TEXTURE_2D, 0);

    int rows = 44, cols = 80;
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            if (TileMap[rows-i-1][j] == ' ') continue;
            if (TileMap[rows-i-1][j] == 'X') {
                tileCords[0] = tileCords[2] = j*tileSize;
                tileCords[1] = tileCords[7] = i*tileSize;
                tileCords[3] = tileCords[5] = (i+1)*tileSize;
                tileCords[4] = tileCords[6] = (j+1)*tileSize;

                glVertexPointer(2, GL_FLOAT, 0, tileCords);
                glColor3f(0.99f, 0.87f, 0.46f);
                glDrawArrays(GL_QUADS, 0, 4);
            } else if (TileMap[rows-i-1][j] == 'F') {
                GlSettingsOff();

                glEnable(GL_POINT_SMOOTH);
                glPointSize(32);
                glBegin(GL_POINTS);
                    if (hero.score == 0) glColor3f(0.0f, 0.9f, 0.0f);
                    else glColor3f(0.9f, 0.0f, 0.0f);
                    glVertex2f((j+0.5f)*tileSize, (i+0.5f)*tileSize);
                glEnd();
                glDisable(GL_POINT_SMOOTH);

                GlSettingsOn(texture);
                glBindTexture(GL_TEXTURE_2D, 0);
            } else if ((hero.score & (1 << (TileMap[rows-i-1][j]-'0'-1))) != 0) {
                GlSettingsOff();

                glEnable(GL_POINT_SMOOTH);
                glPointSize(20);
                glBegin(GL_POINTS);
                    glColor3f(0.0f, 0.5f, 0.5f);
                    glVertex2f((j+0.5f)*tileSize, (i+0.5f)*tileSize);
                glEnd();
                glDisable(GL_POINT_SMOOTH);

                GlSettingsOn(texture);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        }
    }

    GlSettingsOff();

}

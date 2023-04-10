#include <gl/gl.h>
#include <math.h>
#include <string>

#include "game.hpp"
#include "texture.hpp"
#include "stb-master_lib/stb_image.h"

short tileSize = 32;

Hero hero = {WIDTH/2 - 225/2, 3*tileSize};

void Game(GLuint texture) {
    static float svertixFrame[] = { 0,0, 0,0, 0,0, 0,0 };
    static float TexCordFrame[] = { 0,0, 0,0, 0,0, 0,0 };
    static float tileCords[] = { 0,0, 0,0, 0,0, 0,0 };

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

    GlSettingsOn(texture);

    glVertexPointer(2,GL_FLOAT,0,svertixFrame);
    glTexCoordPointer(2,GL_FLOAT,0,TexCordFrame);
    glDrawArrays(GL_TRIANGLE_FAN,0,4);

    glBindTexture(GL_TEXTURE_2D, 0);
    int rows = 22, cols = 40;
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            if (TileMap[rows-i-1][j] == ' ') continue;
            if (TileMap[rows-i-1][j] == 'X') {
                tileCords[0] = tileCords[2] = j*tileSize;
                tileCords[1] = tileCords[7] = i*tileSize;
                tileCords[3] = tileCords[5] = (i+1)*tileSize;
                tileCords[4] = tileCords[6] = (j+1)*tileSize;

                //glEnableClientState(GL_VERTEX_ARRAY);
                glVertexPointer(2, GL_FLOAT, 0, tileCords);
                glColor3f(0.95f, 0.64f, 0.37f);
                glDrawArrays(GL_QUADS, 0, 4);
                //glDisableClientState(GL_VERTEX_ARRAY);
            }
        }
    }

    GlSettingsOff();

}

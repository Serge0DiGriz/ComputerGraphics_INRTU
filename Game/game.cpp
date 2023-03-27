#include <gl/gl.h>
#include <math.h>

#include "game.hpp"
#include "texture.hpp"
#include "stb-master_lib/stb_image.h"

Hero hero = {WIDTH/2 - 225/2, 1};

void Game(GLuint texture) {
    static float svertixFrame[] = {
        hero.x,hero.y,  hero.x,hero.y+hero.h,
        hero.x+hero.w,hero.y+hero.h,  hero.x+hero.w,hero.y};
    static float TexCordFrame[] = {0,1/3., 0,0, 1/8.,0, 1/8.,1/3. };

    if (GetKeyState(VK_SPACE) < 0) hero.jump();
    if (GetKeyState(VK_LEFT) < 0) {
        hero.direction = LEFT;
        hero.vx = 5;
        hero.animation = 0;
    } else if (GetKeyState(VK_RIGHT) < 0) {
        hero.direction = RIGHT;
        hero.vx = 5;
        hero.animation = 0;
    } else if (hero.onGround) {
        hero.vx = 0;
        hero.animation = 2;
    }

    hero.iter();

    svertixFrame[0] = svertixFrame[2] = hero.x;
    svertixFrame[1] = svertixFrame[7] = hero.y;
    svertixFrame[3] = svertixFrame[5] = hero.y+hero.h;
    svertixFrame[4] = svertixFrame[6] = hero.x+hero.w;

    TexCordFrame[3] = TexCordFrame[5] = hero.animation/3.;
    TexCordFrame[1] = TexCordFrame[7] = (hero.animation+1)/3.;
    TexCordFrame[0] = TexCordFrame[2] = hero.frame/8.;
    TexCordFrame[4] = TexCordFrame[6] = (hero.frame+1)/8.;

    GlSettingsOn(texture);

    glVertexPointer(2,GL_FLOAT,0,svertixFrame);
    glTexCoordPointer(2,GL_FLOAT,0,TexCordFrame);
    glDrawArrays(GL_TRIANGLE_FAN,0,4);

    GlSettingsOff();

}

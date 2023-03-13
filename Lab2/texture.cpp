#include <gl/gl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <malloc.h>

#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb-master_lib/stb_image.h"

#ifndef WINDOW_SIZE
#define WINDOW_SIZE
#define WIDTH 1280
#define HEIGHT 720
#endif // WINDOW_SIZE

using namespace std;

struct spriteCoord { float left, right, top, bottom; };
spriteCoord vrtcoord;
int spriteLine = 0;

void nextLine(int k) {
    spriteLine = (spriteLine+3+k)%3;
}

void Load_Texture( char *filename, GLuint *textureID, int swarp, int twarp, int filter)
{
    int twidth, thight, tcnt;
    unsigned char *data=stbi_load(filename,&twidth,&thight,&tcnt,0);

    glGenTextures(1, textureID);
    glBindTexture(GL_TEXTURE_2D, *textureID);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,swarp);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,twarp);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,filter);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,filter);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,twidth,thight,
                 0, tcnt == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
}

void Show_Background(GLuint texture)
{
    static float svertix[]= {10,HEIGHT/2-50,  10,HEIGHT/2+78,  WIDTH-25,HEIGHT/2+78,  WIDTH-25,HEIGHT/2-50};
    static float TexCord[]= {0,1, 0,0, 1,0, 1,1 };
    TexCord[1] = TexCord[7] = (spriteLine+1)/3.;
    TexCord[3] = TexCord[5] = spriteLine/3.;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(2,GL_FLOAT,0,svertix);
    glTexCoordPointer(2,GL_FLOAT,0,TexCord);

    glDrawArrays(GL_TRIANGLE_FAN,0,4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

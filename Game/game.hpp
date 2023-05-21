#ifndef GAME_HPP
#define GAME_HPP

#ifndef WINDOW_SIZE
#define WINDOW_SIZE
#define WIDTH 1282
#define HEIGHT 732
#endif // WINDOW_SIZE

#include <string>
#include <iostream>
#include "menu.hpp"

#define LEFT -1
#define RIGHT 1

inline float min(float a, float b) { return a<b ? a : b; }
inline float max(float a, float b) { return a>b ? a : b; }

const std::string TileMap[] = {
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
    "X                                                                              X",
    "X                                                                              X",
    "X                                                                         F    X",
    "X                                                                              X",
    "X                      1                                                       X",
    "X                                                                   XXXXXXXXXXXX",
    "X                 XXXXXXXXXXXX                                                 X",
    "X                                                    XXXXXXXXX                 X",
    "X                                                                              X",
    "X                                  XXXXXXXXXXXXXX                              X",
    "X                                                                              X",
    "X                                                                              X",
    "X           XXXXXXXXXXXXXXXXX                                                  X",
    "X                                                                              X",
    "X                                XXXXXXXXXXXX                                  X",
    "XXXXX                  6                                                       X",
    "X                                                                              X",
    "X                XXXXXXXXX                         XXXXXXXXXXXXXX              X",
    "X                                                                              X",
    "X  3                                                                           X",
    "X                                                                      XXXXXXXXX",
    "XXXXXXX                                                                        X",
    "X               XXXXXXXXXXX                                                2   X",
    "X                                                                              X",
    "X                                                             XXXXXXXXXXXXXXXXXX",
    "X                                  XXXXXXXXXXXXXXXX                            X",
    "X    4                         XXXXXXXXXXXXXXXXXXXXXXXX                        X",
    "X                                                                              X",
    "XXXXXXXXXXXXXXXXXXXXXXXX                                                       X",
    "X                      XX                                XXXXXXXXX             X",
    "X                       XXXXXX                                                 X",
    "X    5                       X                                                 X",
    "X                            X                                                 X",
    "XXXXXXXXXXXXXXXX             X                                        XXXXXXXXXX",
    "XXXXXXXXXXXXXXXX             X                                                 X",
    "XXXXXXXXXXXXXXXX             X       XXXXXXXXXXXXXXX                           X",
    "X                                                                              X",
    "X                                                      XXXXXXXXXX              X",
    "X                                                                              X",
    "X                                                                              X",
    "X                                                                              X",
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
};



struct Hero {
    float x,y, vx=0,vy=0, ax=0,ay=-1;
    bool onGround = true;
    short direction = RIGHT;
    const short w = 225, h=128;
    float scale = 0.3;
    int score = (1 << 6) - 1;

    int animation = 0, frame = 0, itr = 0;

    void iter() {
        move(vx*direction, vy);

        if (vx < 0.001) vx = ax = 0;
        checkGround();

        if (onGround) {
            ++itr %= 40;
            frame = itr/5;
        } else {
            animation = 1;
            if (vy > 13) frame = 1;
            else if (vy > 10) frame = 2;
            else if (vy > 4) frame = 3;
            else if (vy > -4) frame = 4;
            else if (vy > -10) frame = 5;
            else if (vy > -13) frame = 6;
            else frame = 7;
        }
    }

    void move(float dx, float dy) {
        int row=0, col=0;
        bool check;
        // check y
        check = true;
        if (dy != 0) {
            row = (dy < 0 ? y+dy : min(HEIGHT-16, y+dy+h*scale))/16;
            for (float cx=x; check && cx<min(WIDTH-17, x+w*scale); cx+=8) {
                col = cx/16;
                check = check && (TileMap[43-row][col] != 'X');
                if('0' <= TileMap[43-row][col] || TileMap[43-row][col] <= '9') {
                    score = score & ~(1 << (TileMap[43-row][col]-'0'-1));
                }
                if (TileMap[43-row][col] == 'F' && score == 0) setState(3);
            }
        }
        if (check) {
            y += dy;
            vy += ay;
        }
        else {
            y = (dy < 0) ? ((int)y/16)*16 : ((int) min(HEIGHT-16, y+h*scale)/16 + 1)*16 - h*scale - 1;
            vy = 0;
        }

        // check x
        check = true;
        if (dx != 0) {
            col = (dx < 0 ? x+dx : min(WIDTH-17, x+dx+w*scale))/16;
            for (float cy=y; check && cy<min(HEIGHT-16, y+h*scale); cy+=8) {
                row = cy/16;
                check = check && (TileMap[43-row][col] != 'X');
                if('0' <= TileMap[43-row][col] || TileMap[43-row][col] <= '9') {
                    score = score & ~(1 << (TileMap[43-row][col]-'0'-1));
                }
            }
        }
        if (check) {
            x += dx;
            vx = max(0, vx+ax);
        }
        else {
            x = (dx < 0) ? ((int) x/16)*16 : ((int) min(WIDTH-33, x+w*scale)/16 + 1)*16 - w*scale - 1;
            vx = ax = 0;
        }
    }

    void checkGround() {
        bool check = false;
        for (int mx=x; (!check) && mx<min(WIDTH, x+w*scale); mx+=8) {
            int row = (y-2)/16, col = mx/16;
            check = check || (TileMap[43-row][col] == 'X');
        }

        if (check && !onGround) {
            onGround = true;
            itr=0;
            frame=0;
            ay = 0;
        } else if (!check && onGround) {
            ay = -1;
            onGround = false;
        }
    }

    void jump() {
        if (onGround) {
            onGround = false;
            ay = -1;
            vy = 12.5;
            ax = -0.2;
            vx = 8;
        }
    }
};

void Game(GLuint texture, GLuint background);

#endif // GAME_HPP

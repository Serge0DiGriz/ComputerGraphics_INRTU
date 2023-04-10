#ifndef GAME_HPP
#define GAME_HPP

#ifndef WINDOW_SIZE
#define WINDOW_SIZE
#define WIDTH 1282
#define HEIGHT 732
#endif // WINDOW_SIZE

#include <string>

#define LEFT -1
#define RIGHT 1

inline float min(float a, float b) { return a<b ? a : b; }
inline float max(float a, float b) { return a>b ? a : b; }

const std::string TileMap[] = {
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
    "X                           X          X",
    "X                           X          X",
    "X                           X          X",
    "X                           X          X",
    "X                           X          X",
    "X                           X          X",
    "X                                      X",
    "X                                      X",
    "X              XXXXXXXXXXX             X",
    "X                                      X",
    "X                                      X",
    "XXXXXXXXX                              X",
    "X                                      X",
    "X                                      X",
    "X         XXXXXXXXXXXXXXX              X",
    "X                              XXXXXXXXX",
    "X                                      X",
    "X                                      X",
    "X                    XXXXXXXXXX        X",
    "X                                      X",
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
};

struct Hero {
    float x,y, vx=0,vy=0, ax=0,ay=-1;
    bool onGround = true;
    short direction = RIGHT;
    const short w = 225, h=128;
    float scale = 0.5;

    int animation = 0, frame = 0, itr = 0;

    void iter() {
        /*
        if (checkMove(x + vx*direction, y + vy)) {
            x += vx*direction;
            y += vy;
            vx = max(0, vx+ax);
            vy = vy+ay;
        } else {
            vx /= 2.0f;
            vy /= 2.0f;
        }
        */
        //x = min(max(0, x + vx*direction), WIDTH-w*scale);
        //y = min(max(1, y + vy), HEIGHT-h*scale);
        //vx = max(0, vx+ax);
        //vy = vy+ay;

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
            row = (dy < 0 ? y+dy : min(HEIGHT-32, y+dy+h*scale))/32;
            for (float cx=x; check && cx<min(WIDTH-33, x+w*scale); cx+=16) {
                col = cx/32;
                check = check && (TileMap[row][col] == ' ');
            }
        }
        if (check) {
            y += dy;
            vy += ay;
        }
        else {
            y = (dy < 0) ? ((int)y/32)*32 : ((int) min(HEIGHT-32, y+h*scale)/32 + 1)*32 - 1;
            vy = 0;
        }

        // check x
        check = true;
        if (dx != 0) {
            col = (dx < 0 ? x+dx : min(WIDTH-33, x+dx+w*scale))/32;
            for (float cy=y; check && cy<min(HEIGHT-32, y+h*scale); cy+=16) {
                row = cy/32;
                check = check && (TileMap[row][col] == ' ');
            }
        }
        if (check) {
            x += dx;
            vx = max(0, vx+ax);
        }
        else {
            x = (dx < 0) ? ((int) x/32)*32 : ((int) min(WIDTH-33, x+w*scale)/32 + 1)*32 - 1;
            vx = ax = 0;
        }
    }

    bool checkMove(float x, float y) {
        float limx = min(x+w*scale, WIDTH), limy = min(y+h*scale, HEIGHT);
        int row, col;
        bool check = true;
        for (int mx=x; check && mx<limx; mx+=16)
        for (int my=y; check && my<limy; my+=16) {
            // 1
            row = my/32; col = x/32;
            check = check && (TileMap[row][col] == ' ');

            // 2
            row = my/32; col = limx/32;
            check = check && (TileMap[row][col] == ' ');

            // 3
            row = y/32; col = mx/32;
            check = check && (TileMap[row][col] == ' ');

            // 4
            row = limy/32; col = mx/32;
            check = check && (TileMap[row][col] == ' ');
        }
        return check;
    }

    void checkGround() {
        bool check = false;
        for (int mx=x; (!check) && mx<min(WIDTH, x+w*scale); mx+=16) {
            int row = (y-1)/32, col = mx/32;
            check = check || (TileMap[row][col] == 'X');
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
            vy = 15;
            ax = -0.2;
            vx = 8;
        }
    }
};

void Game(GLuint texture);

#endif // GAME_HPP

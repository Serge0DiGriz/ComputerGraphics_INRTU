#ifndef GAME_HPP
#define GAME_HPP

#ifndef WINDOW_SIZE
#define WINDOW_SIZE
#define WIDTH 1280
#define HEIGHT 720
#endif // WINDOW_SIZE

#define LEFT -1
#define RIGHT 1

inline float min(float a, float b) { return a<b ? a : b; }
inline float max(float a, float b) { return a>b ? a : b; }

struct Hero {
    float x,y, vx=0,vy=0, ax=0,ay=0;
    bool onGround = true;
    short direction = RIGHT;
    const short w = 225, h=128;

    int animation = 0, frame = 0, itr = 0;

    void iter() {
        x = min(max(0, x + vx*direction), WIDTH-w);
        y = min(max(1, y + vy), HEIGHT-h);
        vx = max(0, vx+ax);
        if (vx == 0) ax = 0;
        vy = vy+ay;
        checkGround();

        if (onGround) {
            ++itr %= 40;
            frame = itr/5;
        } else if (vy > 13) frame = 1;
        else if (vy > 10) frame = 2;
        else if (vy > 4) frame = 3;
        else if (vy > -4) frame = 4;
        else if (vy > -10) frame = 5;
        else if (vy > -13) frame = 6;
        else frame = 7;
    }

    void checkGround() {
        if (y == 1 && !onGround) {
            onGround = true;
            itr=0;
            frame=0;
            ay = 0;
        }
    }

    void jump() {
        if (onGround) {
            onGround = false;
            animation = 1;
            ay = -1;
            vy = 15;
            ax = -0.2;
            vx = 8;
        }
    }
};

void Game(GLuint texture);

#endif // GAME_HPP

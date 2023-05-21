#include <windows.h>
#include <gl/gl.h>
#include "menu.hpp"
#include "texture.hpp"
#include "game.hpp"

#ifndef WINDOW_SIZE
#define WINDOW_SIZE
#define WIDTH 1282
#define HEIGHT 732
#endif // WINDOW_SIZE

using namespace std;

GLuint texture, game_background;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

void init_buttons() {
    float btn_w = 300, btn_h = 50, sp = 30;
    Menu_AddButton("Start", (WIDTH-btn_w)/2., (HEIGHT+btn_h)/2., btn_w, btn_h, 2);
    Menu_AddButton("Show Animations", (WIDTH-btn_w)/2., (HEIGHT+3*btn_h+sp)/2., btn_w, btn_h, 2);
    Menu_AddButton("Exit", (WIDTH-btn_w)/2., (HEIGHT+5*btn_h+2*sp)/2., btn_w, btn_h, 2);
    Menu_AddButton("<", (WIDTH-sp)/2.-btn_h, HEIGHT/2.+sp, btn_h, btn_h, 3);
    Menu_AddButton(">", (WIDTH+sp)/2., HEIGHT/2.+sp, btn_h, btn_h, 3);
    Menu_AddButton("Back", (WIDTH-btn_w)/2., HEIGHT/2.+2*sp+btn_h, btn_w, btn_h, 2);
    Menu_AddButton("Menu", 4, 4, btn_w/3, btn_h/2.1, 1.5);
}
/*
void finish() {
    char name[] = "Finish!";
    float vert[8] = {WIDTH/2-50, HEIGHT/2+20, };
    float buffer[1000];
    int num_quads = stb_easy_font_print(0, 0, name,0, buffer, sizeof(buffer));

    glVertexPointer(2, GL_FLOAT, 0, vert);
    glEnableClientState(GL_VERTEX_ARRAY);
    glColor3f(0.8,0.8,0.8);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glLineWidth(1);
    glDrawArrays(GL_LINE_LOOP,0,4);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPushMatrix();
    glColor3f(0.1,0.1,0.1);
    glTranslatef(vert[0]+5,vert[1]+5,0);
    glScalef(3,3,0);
    glEnableClientState(GL_VERTEX_ARRAY);



    glVertexPointer(2, GL_FLOAT, 16, btn1.buffer);
    glDrawArrays(GL_QUADS, 0, btn1.num_quads*4);
    glDisableClientState(GL_VERTEX_ARRAY);
    glPopMatrix();
}
*/
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_MINIMIZEBOX|WS_SYSMENU,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          WIDTH,
                          HEIGHT,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    RECT rct;
    GetClientRect(hwnd, &rct);
    glOrtho(0,rct.right,0,rct.bottom,1,-1);
    init_buttons();
    //getButtons()[0].isActive = false;
    Load_Texture( "./Assets/crab.png", &texture, GL_REPEAT, GL_REPEAT, GL_NEAREST);
    Load_Texture( "./Assets/background.png", &game_background, GL_REPEAT, GL_REPEAT, GL_NEAREST);

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */

            //if (getState() == 2) glClearColor(0.4f, 0.6f, 1.0f, 0.0f);
            //else  glClearColor(0.8f, 0.8f, 0.6f, 0.0f);
            glClearColor(0.8f, 0.8f, 0.6f, 0.0f);
            if (getState() != 3) glClear(GL_COLOR_BUFFER_BIT);

            switch (getState()) {
                case 1: Show_Background(texture); break;
                case 2: Game(texture, game_background); break;
                case 3: break;
            }

            glPushMatrix();
            glLoadIdentity();
            glOrtho(0,rct.right,rct.bottom,0,1,-1);
            Menu_ShowMenu();
            glPopMatrix();


            SwapBuffers(hDC);
            Sleep(1);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {

        case WM_MOUSEMOVE: Menu_MouseMove(LOWORD(lParam), HIWORD(lParam)); break;
        case WM_LBUTTONDOWN: MouseDown(); break;
        case WM_LBUTTONUP: Menu_MouseUp(); break;

        case WM_CLOSE: PostQuitMessage(0); break;

        case WM_DESTROY: return 0;

        case WM_KEYDOWN: {
            switch (wParam) {
                case VK_ESCAPE: {
                    if (getState() == 0) PostQuitMessage(0);
                    else setState(0);
                    break;
                }
            }
        } break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}


#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include "camera.h"

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

float vert[] = {1,1,0, 1,-1,0, -1,-1,0, -1,1,0};

void WndResize(int x, int y){
    glViewport(0,0,x,y); //перестраивает размеры окна
    float k=x/(float)y; //соотношение сторон
    float sz = 0.1; //единица размера
    glLoadIdentity(); //загрузка единичной матрицы
    glFrustum(-k*sz, k*sz, -sz, sz, sz*2, 100); //установка перспективной проэкции
}

void MoveCamera(){
    Camera_MoveDirectional(
        GetKeyState('W')<0 ? 1 : GetKeyState('S')<0 ? -1 : 0,
        GetKeyState('D')<0 ? 1 : GetKeyState('A')<0 ? -1 : 0,
        0.1);
    Camera_Rotation(
        GetKeyState(VK_UP)<0 ? 1 : GetKeyState(VK_DOWN)<0 ? -1 : 0,
        GetKeyState(VK_RIGHT)<0 ? 1 : GetKeyState(VK_LEFT)<0 ? -1 : 0);
    Camera_Zoom(
        GetKeyState(VK_ADD)<0||GetKeyState(VK_SPACE)<0 ? 0.1 :
        GetKeyState(VK_SUBTRACT)<0||GetKeyState(VK_SHIFT)<0 ? -0.1 : 0);
    //Camer_AutoMoveByMouse(400,400,0.1);
}

void DrawGrid(int n) {
    glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, &vert);
        for (int i=-n/2; i<n/2+n%2; i++) {
            for (int j=-n/2; j<n/2+n%2; j++) {
                glPushMatrix();
                    if ((i+j)%2) glColor3f(0.8, 0.8, 0.8);
                    else glColor3f(0.2, 0.2, 0.2);
                    glTranslatef(i*2,j*2,0);
                    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
                glPopMatrix();
            }
        }
    glDisableClientState(GL_VERTEX_ARRAY);
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow) {
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    int theta = 0;

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
                          1024,
                          768,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    RECT rct;
    GetClientRect(hwnd,&rct);
    WndResize(rct.right,rct.bottom);

    glEnable(GL_DEPTH_TEST);
    /* program main loop */
    while (!bQuit) {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT) {
                bQuit = TRUE;
            } else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        } else {
            /* OpenGL animation code goes here */
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


            glPushMatrix();
                MoveCamera();
                Camera_Apply();
                DrawGrid(15);

                glRotatef(theta, 0.0f, 0.0f, 1.0f);
                glBegin(GL_TRIANGLES);
                    glColor3f(1.0f, 0.0f, 0.0f);
                    glVertex3f(0.0f,   1.0f, 1+sin(M_PI*theta/180.0));
                    glColor3f(0.0f, 1.0f, 0.0f);
                    glVertex3f(0.87f,  -0.5f, 1+sin(M_PI*(theta/180.0+1/3.0)));
                    glColor3f(0.0f, 0.0f, 1.0f);
                    glVertex3f(-0.87f, -0.5f, 1+sin(M_PI*(theta/180.0+2/3.0)));
                glEnd();

            glPopMatrix();


            SwapBuffers(hDC);

            ++theta%=360;
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
        case WM_CLOSE: PostQuitMessage(0); break;
        case WM_DESTROY: return 0;
        case WM_KEYDOWN: {
            switch (wParam) {
                case VK_ESCAPE: PostQuitMessage(0); break;
            }
        } break;
        default: return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC) {
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

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC) {
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}


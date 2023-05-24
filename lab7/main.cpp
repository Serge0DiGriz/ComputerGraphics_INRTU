#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include "camera.h"

#define N 15

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

float grid_vert[] = {1,1,0, 1,-1,0, -1,-1,0, -1,1,0};
float normal_vert[] = {0,0,1, 0,0,1, 0,0,1, 0,0,1};

void WndResize(int x, int y){
    glMatrixMode(GL_PROJECTION);
    glViewport(0,0,x,y); //перестраивает размеры окна
    float k=x/(float)y; //соотношение сторон
    float sz = 0.1; //единица размера
    glLoadIdentity(); //загрузка единичной матрицы
    glFrustum(-k*sz, k*sz, -sz, sz, sz*2, 100); //установка перспективной проэкции
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Init_Light() {
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    GLfloat light_position[] = { 15.0f, 0.0f, 7.0f, 1.0f };
    GLfloat light_spot_direction[] = {-14.0, 0.0, -8.0};
    GLfloat light_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_specular[] = { 0.2f, 0.2f, 0.2f, 32.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_spot_direction);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 1.0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glEnable(GL_LIGHT0);
}

void Init_Material(float alpha) {
    GLfloat material_diffuse[] = {1.0, 1.0, 1.0, alpha};
    GLfloat material_ambient[] = {1.0, 1.0, 1.0, 0.5};
    GLfloat material_specular[] = { 1.0f, 1.0f, 1.0f, 32.0f };
    GLfloat material_shininess[] = { 50.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, &grid_vert);
        glNormalPointer(GL_FLOAT,0,&normal_vert);
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
    glDisable(GL_NORMAL_ARRAY);
}

void DrawPrism(float x, float y, float z, float r, float h, float alpha) {
    float cos40 = cos(M_PI/9.0*2), sin40 = sin(M_PI/9.0*2);
    float xi,yi,zi, nx,ny;
    int i,j;
    Init_Material(alpha);
    glColor4f(0.1, 0.1, 0.8, alpha);

    float vert[60];
    float norm[60];
    for (i=0, zi=z; i<2; i++, zi+=h) {
        vert[i*30] = x; vert[i*30+1] = y; vert[i*30+2] = zi;
        norm[i*30] = 0; norm[i*30+1] = 0; norm[i*30+2] = (i*2)-1;
        for (j=1, xi=x+r, yi=y; j<=10; j++, xi=nx, yi=ny){
            vert[i*30+j*3] = xi; vert[i*30+j*3+1] = yi; vert[i*30+j*3+2] = zi;
            norm[i*30+j*3] = xi-x; norm[i*30+j*3+1] = yi-y; norm[i*30+j*3+2] = h/2.0*(2*i-1);
            nx = x + (xi-x)*cos40 - (yi-y)*sin40;
            ny = y + (xi-x)*sin40 + (yi-y)*cos40;
        }
    }
    int indices[108];
    for (int b=0; b<2; b++)
    for (int i=0; i<9; i++) {
        indices[i*3+27*b] = 10*b;
        indices[i*3+27*b+1] = i+1 + 10*b;
        indices[i*3+27*b+2] = (i+2)%10+(i+2)/10 + 10*b;
    }
    for (int i=0; i<9; i++) {
        indices[54+i*6] = i+1;
        indices[54+i*6+1] = i+1+10;
        indices[54+i*6+2] = (i+2)%10+(i+2)/10+10;

        indices[54+i*6+3] = i+1;
        indices[54+i*6+4] = (i+2)%10+(i+2)/10;
        indices[54+i*6+5] = (i+2)%10+(i+2)/10+10;
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vert);

    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, norm);

    glDrawElements(GL_TRIANGLES, 108, GL_UNSIGNED_INT, indices);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
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
                if (GetForegroundWindow()==hwnd)
                    MoveCamera();
                Camera_Apply();

                Init_Light();

                DrawGrid(N);
                //DrawPrism(5, 0, 1, 2, 2, 1);
                glPushMatrix();
                for (int i=0; i<10; i++) {
                    DrawPrism(5, 0, 1, 1, 1.5, i/10.0);
                    glRotatef(36, 0,0,1);
                }
                glPopMatrix();
            glPopMatrix();


            SwapBuffers(hDC);
            theta++;
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


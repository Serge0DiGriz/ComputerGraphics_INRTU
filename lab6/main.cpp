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

void DrawCube(float x, float y, float z,  float s, int color){
    GLfloat vertices[] = {
        x,y,z,    x+s,y,z,    x+s,y+s,z,    x,y+s,z ,
        x,y,z+s,  x+s,y,z+s,  x+s,y+s,z+s,  x,y+s,z+s };

    GLuint indices[] = {
        0, 1, 2,
        2, 3, 0,
        1, 5, 6,
        6, 2, 1,
        7, 6, 5,
        5, 4, 7,
        4, 0, 3,
        3, 7, 4,
        4, 5, 1,
        1, 0, 4,
        3, 2, 6,
        6, 7, 3
    };
    GLfloat normals[] = {
         0.0f, 0.0f,-1.0f,   0.0f, 0.0f,-1.0f,   0.0f, 0.0f,-1.0f,   0.0f, 0.0f,-1.0f ,
         0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f ,
        -1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f ,
         1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f ,
         0.0f,-1.0f, 0.0f,   0.0f,-1.0f, 0.0f,   0.0f,-1.0f, 0.0f,   0.0f,-1.0f, 0.0f ,
         0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f };
    glEnableClientState(GL_VERTEX_ARRAY);
    glColor3f(color&4, color&2, color&1);
    glVertexPointer(3, GL_FLOAT, 0, vertices);

    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, normals);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, indices);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

void Init_Light() {
    glEnable(GL_LIGHTING); //общее освещения для всего пространства
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    GLfloat light_position[] = { 10.0f, 0.0f, 4.0f, 1.0f }; //позиция источника
    GLfloat light_spot_direction[] = {-15.0, 0.0, -1.0}; // позиция цели
    GLfloat light_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f }; //параметры
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; //параметры
    GLfloat light_specular[] = { 0.2f, 0.2f, 0.2f, 32.0f }; //параметры
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 20); // конус для направленного источника
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_spot_direction);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 8.0); // экспонента убывания интенсивности задействование настроек для источника LIGHT0
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glEnable(GL_LIGHT0); // источник света LIGHT0
}

void Init_Material() {
    glEnable(GL_COLOR_MATERIAL); //разрешения использования материала
    glShadeModel(GL_SMOOTH); // сглаживает границы
    GLfloat material_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat material_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat material_specular[] = { 1.0f, 1.0f, 1.0f, 32.0f };
    GLfloat material_shininess[] = { 50.0f }; //блеск материала
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);
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

                glPushMatrix();
                glRotatef(theta, 0,0,1);
                    Init_Light();
                    Init_Material();
                glPopMatrix();

                float colors[] = {4,6,2,6,7,3,2,3,1};
                DrawGrid(N);
                for (int x=-9, i=0; x<=9; x+=8)
                    for (int y=-9; y<=9; y+=8)
                        DrawCube(x, y, 1, 2, colors[i++]);
                /*
                glRotatef(theta, 0.0f, 0.0f, 1.0f);
                glBegin(GL_TRIANGLES);
                    glColor3f(1.0f, 0.0f, 0.0f);
                    glVertex3f(0.0f,   1.0f, 1+sin(M_PI*theta/180.0));
                    glColor3f(0.0f, 1.0f, 0.0f);
                    glVertex3f(0.87f,  -0.5f, 1+sin(M_PI*(theta/180.0+1/3.0)));
                    glColor3f(0.0f, 0.0f, 1.0f);
                    glVertex3f(-0.87f, -0.5f, 1+sin(M_PI*(theta/180.0+2/3.0)));
                glEnd();
                */
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


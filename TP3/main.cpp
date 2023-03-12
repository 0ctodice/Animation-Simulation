#include <iostream>
#include <raylib.h>
#include <raymath.h>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include <../styles/cyber/cyber.h>

using namespace std;

#define screenWidth 1280
#define screenHeight 720

#define FLAGWIDTH 5
#define FLAGHEIGHT 4

// #define NBM FLAGWIDTH *FLAGHEIGHT
// #define NBL 55

#define NBM 13
#define NBL NBM - 1

#define m 1

float k = 0.1;
float z = 0.01;
float Fe = 100;
float gravity = 0;
float mouseForce = 0;

bool drawGUI = true;

class OwnVector3
{
public:
    double x, y, z = 0.0;

    OwnVector3(double _x, double _y, double _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }

    // surcharge de l'opérateur +
    OwnVector3 operator+(OwnVector3 const &obj)
    {
        OwnVector3 res(x + obj.x, y + obj.y, z + obj.z);
        return res;
    }

    // surcharge de l'opérateur -
    OwnVector3 operator-(OwnVector3 const &obj)
    {
        OwnVector3 res(x - obj.x, y - obj.y, z - obj.z);
        return res;
    }

    // surcharge de l'opérateur *
    OwnVector3 operator*(double const &obj)
    {
        OwnVector3 res(x * obj, y * obj, z * obj);
        return res;
    }

    // surcharge de l'opérateur /
    OwnVector3 operator/(double const &obj)
    {
        OwnVector3 res(x / obj, y / obj, z / obj);
        return res;
    }

    // surcharge de l'opérateur =
    OwnVector3 operator=(OwnVector3 const &obj)
    {
        x = obj.x;
        y = obj.y;
        z = obj.z;
        return *this;
    }

    // surcharge de l'opérateur +=
    OwnVector3 operator+=(OwnVector3 const &obj)
    {
        x += obj.x;
        y += obj.y;
        z += obj.z;
        return *this;
    }

    // surcharge de l'opérateur -=
    OwnVector3 operator-=(OwnVector3 const &obj)
    {
        x -= obj.x;
        y -= obj.y;
        z -= obj.z;
        return *this;
    }

    // surcharge de l'opérateur *=
    OwnVector3 operator*=(double const &obj)
    {
        x *= obj;
        y *= obj;
        z *= obj;
        return *this;
    }

    // surcharge de l'opérateur /=
    OwnVector3 operator/=(double const &obj)
    {
        x /= obj;
        y /= obj;
        z /= obj;
        return *this;
    }

    // surcharge de l'opérateur ==
    bool operator==(OwnVector3 const &obj)
    {
        return (x == obj.x && y == obj.y && z == obj.z);
    }

    // surcharge de l'opérateur !=
    bool operator!=(OwnVector3 const &obj)
    {
        return (x != obj.x || y != obj.y || z != obj.z);
    }

    Vector3 operator!(void)
    {
        return {(float)x, (float)y, (float)z};
    }
};

class PMat
{
public:
    OwnVector3 pos{0.0, 0.0, 0.0};
    OwnVector3 iniPos{0.0, 0.0, 0.0};
    OwnVector3 vit{0.0, 0.0, 0.0};
    OwnVector3 frc{0.0, 0.0, 0.0};
    Color color = BLACK;
    bool fixed = true;

    void initPos()
    {
        iniPos = pos;
    }

    void setup(double h)
    {
        vit += (frc / m) * h;
        pos += vit * h;
        frc = {0.0, 0.0, 0.0};
    }

    void setup_pfixe()
    {
        frc = {0.0, 0.0, 0.0};
    }

    void reset()
    {
        vit = {0.0, 0.0, 0.0};
        frc = {0.0, 0.0, 0.0};
        pos = iniPos;
    }

    void draw()
    {
        DrawSphereEx(!pos, 2, 10, 10, color);
    }
};

class Link
{
public:
    double l0 = 0.0;
    PMat *M1, *M2;
    bool connected = false;

    void reset()
    {
        OwnVector3 delta = M2->pos - M1->pos;
        l0 = sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
    }

    void connect(PMat *_M1, PMat *_M2)
    {
        M1 = _M1;
        M2 = _M2;
        OwnVector3 delta = M2->pos - M1->pos;
        l0 = sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
        connected = true;
    }

    void setup_ressort_frein()
    {
        double h = Fe;
        OwnVector3 delta = M2->pos - M1->pos;
        double d = sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);

        double fX = h * (k / m) * (d - l0) + h * (z / m) * (M2->vit.x - M1->vit.x);
        double fY = h * (k / m) * (d - l0) + h * (z / m) * (M2->vit.y - M1->vit.y);
        double fZ = h * (k / m) * (d - l0) + h * (z / m) * (M2->vit.z - M1->vit.z);

        double ux = delta.x / d;
        double uy = delta.y / d;
        double uz = delta.z / d;

        M1->frc.x += fX * ux;
        M1->frc.y += fY * uy;
        M1->frc.z += fZ * uz;
        M2->frc.x -= fX * ux;
        M2->frc.y -= fY * uy;
        M2->frc.z -= fZ * uz;
    }

    void apply_gravity()
    {
        for (PMat *M = M1; M <= M2; M++)
        {
            M->frc.y += gravity * 1000;
        }
    }
};

void CordeModeleur(PMat *tabM, Link *tabL)
{
    PMat *M = tabM;

    M->pos.x = (-NBM / 2) * 10.0;
    M->pos.y = 0.0;
    M->pos.z = 0.0;
    M->color = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_FOCUSED));
    M->initPos();

    M++;

    for (int i = (-NBM / 2) + 1; i < NBM / 2; i++)
    {
        M->pos.x = i * 10.0;
        M->pos.y = 0.0;
        M->pos.z = 0.0;
        M->color = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_PRESSED));
        M->fixed = false;
        M->initPos();
        M++;
    }

    M->pos.x = (NBM / 2) * 10.0;
    M->pos.y = 0.0;
    M->pos.z = 0.0;
    M->color = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_FOCUSED));
    M->initPos();

    M = tabM;

    for (Link *L = tabL; L < tabL + NBL; L++)
    {
        L->connect(M, M + 1);
        M++;
    }
}

void test(PMat *tabM, Link *tabL)
{
    int i = -NBM / 2;
    for (PMat *M = tabM; M < tabM + NBM; M++)
    {
        M->pos.x = (double)i * 10.0;
        M->pos.y = 0;
        M->pos.z = 0;
        M->color = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_PRESSED));
        M->fixed = false;
        M->initPos();
        i++;
    }
}

void Drapeau2DModeleur(PMat *tabM, Link *tabL)
{
    PMat *M = tabM;
    for (int j = 0; j < FLAGHEIGHT; j++)
    {
        M = tabM + j * FLAGWIDTH;
        M->pos.x = 100.0;
        M->pos.y = 150.0 + j * (screenHeight - 300.0) / (FLAGHEIGHT - 1.0);
        M->color = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_FOCUSED));
        M->initPos();
    }

    for (int j = 0; j < FLAGHEIGHT; j++)
    {
        for (int i = 1; i < FLAGWIDTH; i++)
        {
            M = tabM + j * FLAGWIDTH + i;
            M->pos.x = 100.0 + i * (screenWidth - 500.0) / (FLAGWIDTH - 1.0);
            M->pos.y = 150.0 + j * (screenHeight - 300.0) / (FLAGHEIGHT - 1.0);
            M->color = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_PRESSED));
            M->fixed = false;
            M->initPos();
        }
    }

    int linkIndex = 0;
    Link *L = tabL;

    for (int j = 0; j < FLAGHEIGHT - 1; j++)
    {
        for (int i = 0; i < FLAGWIDTH - 1; i++)
        {
            M = tabM + j * FLAGWIDTH + i;
            L = tabL + linkIndex;
            L->connect(M, M + 1);
            linkIndex++;
            L = tabL + linkIndex;
            L->connect(M, M + FLAGWIDTH);
            linkIndex++;
            L = tabL + linkIndex;
            L->connect(M, M + FLAGWIDTH + 1);
            linkIndex++;
            if (j != 0)
            {
                L = tabL + linkIndex;
                L->connect(M, M - FLAGWIDTH + 1);
                linkIndex++;
            }
        }
    }

    for (int i = FLAGWIDTH - 1; i < FLAGWIDTH; i++)
    {
        for (int j = 0; j < FLAGHEIGHT - 1; j++)
        {
            M = tabM + j * FLAGWIDTH + i;
            L = tabL + linkIndex;
            L->connect(M, M + FLAGWIDTH);
            linkIndex++;
        }
    }

    for (int j = FLAGHEIGHT - 1; j < FLAGHEIGHT; j++)
    {
        for (int i = 0; i < FLAGWIDTH - 1; i++)
        {
            M = tabM + j * FLAGWIDTH + i;
            L = tabL + linkIndex;
            L->connect(M, M + 1);
            linkIndex++;
            L = tabL + linkIndex;
            L->connect(M, M - FLAGWIDTH + 1);
            linkIndex++;
        }
    }
}

void MoteurRessortFrein(PMat *tabM, Link *tabL)
{

    for (Link *L = tabL; L < tabL + NBL; L++)
    {
        L->setup_ressort_frein();
    }

    for (PMat *M = tabM; M < tabM + NBM; M++)
    {
        M->frc.y -= gravity * 1000;
    }

    for (PMat *M = tabM; M < tabM + NBM; M++)
    {
        M->fixed ? M->setup_pfixe() : M->setup(1.0 / Fe);
    }
}

void DrawGUI(PMat *tabM, Link *tabL)
{
    gravity = GuiSliderBar((Rectangle){screenWidth - 150, 50, 100, 30}, "GRAVITY", std::to_string(gravity).substr(0, std::to_string(gravity).find(".") + 3).c_str(), gravity, 0, 1);
    mouseForce = GuiSliderBar((Rectangle){screenWidth - 150, 90, 100, 30}, "FORCE", std::to_string(mouseForce).substr(0, std::to_string(mouseForce).find(".") + 3).c_str(), mouseForce, 0, 1);
    k = GuiSliderBar((Rectangle){screenWidth - 150, 130, 100, 30}, "K", std::to_string(k).substr(0, std::to_string(k).find(".") + 3).c_str(), k, 0, 1);
    z = GuiSliderBar((Rectangle){screenWidth - 150, 170, 100, 30}, "Z", std::to_string(z).substr(0, std::to_string(z).find(".") + 3).c_str(), z, 0, 1);
    Fe = GuiSliderBar((Rectangle){screenWidth - 150, 210, 100, 30}, "Fe", std::to_string(Fe).substr(0, std::to_string(Fe).find(".")).c_str(), Fe, 0, 1000);
    const char *FPS = ("FPS: " + std::to_string(GetFPS())).c_str();
    GuiLabel((Rectangle){screenWidth - 130, 250, 100, 30}, FPS);
    if (GuiButton((Rectangle){screenWidth - 150, 280, 100, 30}, "RESET"))
    {
        gravity = 0;
        mouseForce = 0;
        k = 0.1;
        z = 0.01;
        Fe = 100;
        for (PMat *M = tabM; M < tabM + NBM; M++)
        {
            M->reset();
        }

        for (Link *L = tabL; L < tabL + NBL; L++)
        {
            L->reset();
        }
    }
}

int main()
{
    PMat *tabM = new PMat[NBM];
    Link *tabL = new Link[NBL];

    InitWindow(screenWidth, screenHeight, "TP3");
    GuiLoadStyleCyber();
    SetTargetFPS(60);

    // Drapeau2DModeleur(tabM, tabL);
    CordeModeleur(tabM, tabL);

    Camera camera = {0};
    camera.position = (Vector3){0.0f, 0.0f, 100.0f};
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    SetCameraMode(camera, CAMERA_ORBITAL);

    while (WindowShouldClose() == false)
    {
        // if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        // {

        //     for (PMat *M = tabM; M < tabM + NBM; M++)
        //     {
        //         Vector2 mV = {(float)M->pos.x, (float)M->pos.y};
        //         if (abs(Vector2Distance(GetMousePosition(), mV)) < 15.0)
        //         {
        //             M->frc.y += mouseForce * 1000000;
        //         }
        //     }
        // }

        MoteurRessortFrein(tabM, tabL);
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        BeginMode3D(camera);
        UpdateCamera(&camera);

        for (Link *L = tabL; L < tabL + NBL; L++)
        {
            DrawLine3D(!(L->M1->pos), !(L->M2->pos), GetColor(GuiGetStyle(BUTTON, BASE_COLOR_FOCUSED)));
        }

        for (PMat *M = tabM; M < tabM + NBM; M++)
        {
            M->draw();
        }

        EndMode3D();

        drawGUI = IsKeyPressed(KEY_G) ? !drawGUI : drawGUI;

        if (drawGUI)
            DrawGUI(tabM, tabL);

        EndDrawing();
    }
    CloseWindow();
    delete[] tabM;
    delete[] tabL;
    return 0;
}
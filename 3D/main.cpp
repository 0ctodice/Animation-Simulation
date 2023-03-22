#include <iostream>
#include <raylib.h>
#include <raymath.h>

#define RAYGUI_IMPLEMENTATION
#include <../GUI/raygui.h>
#include <../GUI/terminal/terminal.h>

using namespace std;

#define screenWidth 1280
#define screenHeight 720

#define FLAGWIDTH 5
#define FLAGHEIGHT 4
#define FLAGRES 5

#define NBM FLAGWIDTH *FLAGHEIGHT
#define NBL FLAGWIDTH *(FLAGHEIGHT - 1) + FLAGHEIGHT *(FLAGWIDTH - 1) + 2 * (FLAGWIDTH - 1) * (FLAGHEIGHT - 1) + (FLAGWIDTH - 2) * FLAGHEIGHT + (FLAGHEIGHT - 2) * FLAGWIDTH

// #define NBM 13
// #define NBL NBM - 1

#define m 1

float k = 0.1;
float z = 0.01;
float Fe = 400;
float g = 0;

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
        DrawSphereEx(!pos, 1, 10, 10, color);
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

        double fX = h * h * (k / m) * (d - l0) + h * (z / m) * (M2->vit.x - M1->vit.x);
        double fY = h * h * (k / m) * (d - l0) + h * (z / m) * (M2->vit.y - M1->vit.y);
        double fZ = h * h * (k / m) * (d - l0) + h * (z / m) * (M2->vit.z - M1->vit.z);

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
        M1->frc.y -= g * 1000;
        M2->frc.y -= g * 1000;
    }

    void draw()
    {
        // DrawLine3D(!(M1->pos), !(M2->pos), GetColor(GuiGetStyle(BUTTON, BASE_COLOR_FOCUSED)));
        DrawCylinderEx(!M1->pos, !M2->pos, 0.1, 0.1, 10, GetColor(GuiGetStyle(BUTTON, BASE_COLOR_FOCUSED)));
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

void Drapeau3DModeleur(PMat *tabM, Link *tabL)
{
    PMat *M = tabM;
    for (int j = 0; j < FLAGHEIGHT; j++)
    {
        M = tabM + j * FLAGWIDTH;
        M->pos.x = (-FLAGWIDTH / 2) * FLAGRES;
        M->pos.y = (j - FLAGHEIGHT / 2) * FLAGRES;
        M->color = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_FOCUSED));
        M->initPos();
    }

    for (int j = 0; j < FLAGHEIGHT; j++)
    {
        for (int i = 1; i < FLAGWIDTH; i++)
        {
            M = tabM + j * FLAGWIDTH + i;
            M->pos.x = (i - FLAGWIDTH / 2) * FLAGRES;
            M->pos.y = (j - FLAGHEIGHT / 2) * FLAGRES;
            M->color = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_PRESSED));
            M->fixed = false;
            M->initPos();
        }
    }

    int linkIndex = 0;
    Link *L = tabL;

    // PARCOURS LINK GEOMETRIQUE

    for (int j = 0; j < FLAGHEIGHT; j++)
    {
        for (int i = 0; i < FLAGWIDTH; i++)
        {
            M = tabM + j * FLAGWIDTH + i;

            if (i < FLAGWIDTH - 1)
            {
                L = tabL + linkIndex;
                L->connect(M, M + 1);
                linkIndex++;
            }

            if (j < FLAGHEIGHT - 1)
            {
                L = tabL + linkIndex;
                L->connect(M, M + FLAGWIDTH);
                linkIndex++;
            }
        }
    }

    // PARCOURS LINK CISAILLEMENT

    for (int j = 0; j < FLAGHEIGHT; j++)
    {
        for (int i = 0; i < FLAGWIDTH - 1; i++)
        {
            M = tabM + j * FLAGWIDTH + i;

            if (j < FLAGHEIGHT - 1)
            {
                L = tabL + linkIndex;
                L->connect(M, M + FLAGWIDTH + 1);
                linkIndex++;
            }

            if (j > 0)
            {
                L = tabL + linkIndex;
                L->connect(M, M - FLAGWIDTH + 1);
                linkIndex++;
            }
        }
    }

    // PARCOURS LINK COURBE

    for (int j = 0; j < FLAGHEIGHT; j++)
    {
        for (int i = 0; i < FLAGWIDTH; i++)
        {
            M = tabM + j * FLAGWIDTH + i;

            if (i < FLAGWIDTH - 2)
            {
                L = tabL + linkIndex;
                L->connect(M, M + 2);
                linkIndex++;
            }

            if (j < FLAGHEIGHT - 2)
            {
                L = tabL + linkIndex;
                L->connect(M, M + 2 * FLAGWIDTH);
                linkIndex++;
            }
        }
    }
}

void MoteurRessortFrein(PMat *tabM, Link *tabL)
{

    for (Link *L = tabL; L < tabL + NBL; L++)
    {
        L->setup_ressort_frein();
        L->apply_gravity();
    }

    for (PMat *M = tabM; M < tabM + NBM; M++)
    {
        M->fixed ? M->setup_pfixe() : M->setup(1.0 / Fe);
    }
}

void DrawGUI(PMat *tabM, Link *tabL)
{
    g = GuiSliderBar((Rectangle){screenWidth - 150, 50, 100, 30}, "G", std::to_string(g).substr(0, std::to_string(g).find(".") + 3).c_str(), g, 0, 1);
    k = GuiSliderBar((Rectangle){screenWidth - 150, 90, 100, 30}, "K", std::to_string(k).substr(0, std::to_string(k).find(".") + 3).c_str(), k, 0, 1);
    z = GuiSliderBar((Rectangle){screenWidth - 150, 130, 100, 30}, "Z", std::to_string(z).substr(0, std::to_string(z).find(".") + 3).c_str(), z, 0, 1);
    Fe = GuiSliderBar((Rectangle){screenWidth - 150, 170, 100, 30}, "Fe", std::to_string(Fe).substr(0, std::to_string(Fe).find(".")).c_str(), Fe, 0, 1000);
    if (GuiButton((Rectangle){screenWidth - 150, 210, 100, 30}, "RESET"))
    {
        g = 0;
        k = 0.1;
        z = 0.01;
        Fe = 400;

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
    GuiLoadStyleTerminal();
    SetTargetFPS(120);

    Drapeau3DModeleur(tabM, tabL);
    // CordeModeleur(tabM, tabL);

    Camera camera = {0};
    camera.position = (Vector3){0.0f, 0.0f, 100.0f};
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    SetCameraMode(camera, CAMERA_ORBITAL);

    while (WindowShouldClose() == false)
    {
        MoteurRessortFrein(tabM, tabL);
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        BeginMode3D(camera);
        UpdateCamera(&camera);

        for (Link *L = tabL; L < tabL + NBL; L++)
        {
            L->draw();
        }

        for (PMat *M = tabM; M < tabM + NBM; M++)
        {
            M->draw();
        }

        EndMode3D();

        drawGUI = IsKeyPressed(KEY_G) ? !drawGUI : drawGUI;

        if (drawGUI)
        {
            DrawGUI(tabM, tabL);
            DrawFPS(10, 10);
        }

        EndDrawing();
    }
    CloseWindow();
    delete[] tabM;
    delete[] tabL;
    return 0;
}
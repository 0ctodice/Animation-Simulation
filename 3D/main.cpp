////////////////////////////////////////////////////////////////////////////////////
//
// ---------------------------------------------------------------------------------
//   _____      _            _ _
//  |  _  |    | |          | (_)
//  | |/' | ___| |_ ___   __| |_  ___ ___
//  |  /| |/ __| __/ _ \ / _` | |/ __/ _ \
//  \ |_/ / (__| || (_) | (_| | | (_|  __/
//   \___/ \___|\__\___/ \__,_|_|\___\___|
//
// ---------------------------------------------------------------------------------
//
//  Project for the course "Animation et Simulation" at the University Gustave Eiffel
//  ~ Thomas DUMONT A.K.A 0ctodice
//
// ---------------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include "OwnVector3.hpp"
#include <vector>
#include <algorithm>

#define RAYGUI_IMPLEMENTATION
#include <../GUI/raygui.h>
#include <../GUI/terminal/terminal.h>

using namespace std;

#define screenWidth 1280
#define screenHeight 720

#define FLAGWIDTH 30
#define FLAGHEIGHT 30
#define FLAGRES 1

#define NBM FLAGWIDTH *FLAGHEIGHT
#define NBL FLAGWIDTH *(FLAGHEIGHT - 1) + FLAGHEIGHT *(FLAGWIDTH - 1) + 2 * (FLAGWIDTH - 1) * (FLAGHEIGHT - 1) + (FLAGWIDTH - 2) * FLAGHEIGHT + (FLAGHEIGHT - 2) * FLAGWIDTH

#define m 1

float k = 0.1;
float z = 0.01;
float Fe = 400;
float g = 0;
float a = 1;
float b = 1;
float w = 0;
float r = 1000000;
float wx = 0;
float wy = 0;
float wz = 0;
OwnVector3 windDir = {0.0, 0.0, 0.0};

bool drawGUI = true;

void resetParam()
{
    g = 0;
    k = 0.1;
    z = 0.01;
    Fe = 400;
    a = 1;
    b = 1;
    w = 0;
    r = 1000000;
    wx = 0;
    wy = 0;
    wz = 0;
}

// OwnVector3 wind()
// {
//     double alphaT = cos(2 * PI * GetFrameTime() / (Fe));
//     double betaT = cos(2 * PI * 0.5 * GetFrameTime() * (1.0 / Fe));
//     return {cos(alphaT) * sin(betaT), sin(alphaT) * cos(betaT), cos(betaT)};
// }

class PMat
{
public:
    OwnVector3 pos{0.0, 0.0, 0.0};
    OwnVector3 iniPos{0.0, 0.0, 0.0};
    OwnVector3 vit{0.0, 0.0, 0.0};
    OwnVector3 frc{0.0, 0.0, 0.0};
    float coef = 1.0;
    Color color = BLACK;
    bool fixed = true;
    bool exist = true;

    void initPos()
    {
        iniPos = pos;
    }

    void setup(double h)
    {
        vit += (frc / m) * h * coef;
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
        exist = true;
    }

    void draw()
    {
        DrawSphereEx(!pos, 1, 10, 10, color);
    }
};

enum LINKTYPE
{
    STRUCTURAL,
    SHEAR,
    BEND
};

class Link
{
public:
    double l0 = 0.0;
    PMat *M1, *M2;
    bool connected = false;
    LINKTYPE type;

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
        if (!M1->exist || !M2->exist)
        {
            return;
        }

        double h = Fe;
        OwnVector3 delta = M2->pos - M1->pos;
        double d = sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
        double K = 0.0;
        double Z = 0.0;

        switch (type)
        {
        case STRUCTURAL:
            K = h * h * (k / m);
            Z = h * (z / m);
            break;
        case SHEAR:
            K = a * h * h * (k / m);
            Z = a * h * (z / m);
            break;
        case BEND:
            K = b * h * h * (k / m);
            Z = b * h * (z / m);
            break;
        }

        double ux = delta.x / d;
        double uy = delta.y / d;
        double uz = delta.z / d;

        double fX = K * (d - l0) * ux + Z * (M2->vit.x - M1->vit.x) + windDir.x * w;
        double fY = K * (d - l0) * uy + Z * (M2->vit.y - M1->vit.y) + windDir.y * w;
        double fZ = K * (d - l0) * uz + Z * (M2->vit.z - M1->vit.z) + windDir.z * w;

        double totalFrc = sqrt(fX * fX + fY * fY + fZ * fZ);

        if (totalFrc > r)
        {
            M1->exist = false;
            M2->exist = false;
            return;
        }

        M1->frc.x += fX;
        M1->frc.y += fY;
        M1->frc.z += fZ;
        M2->frc.x -= fX;
        M2->frc.y -= fY;
        M2->frc.z -= fZ;
    }

    void apply_gravity()
    {
        M1->frc.y -= m * g * Fe;
        M2->frc.y -= m * g * Fe;
    }

    void draw()
    {
        if (M1->exist && M2->exist)
            DrawLine3D(!(M1->pos), !(M2->pos), GetColor(GuiGetStyle(BUTTON, BASE_COLOR_FOCUSED)));
    }
};

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
            M->coef = max(1.0 - ((float)i / (float)FLAGWIDTH), 0.1);
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
                L->type = STRUCTURAL;
                linkIndex++;
            }

            if (j < FLAGHEIGHT - 1)
            {
                L = tabL + linkIndex;
                L->connect(M, M + FLAGWIDTH);
                L->type = STRUCTURAL;
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
                L->type = SHEAR;
                linkIndex++;
            }

            if (j > 0)
            {
                L = tabL + linkIndex;
                L->connect(M, M - FLAGWIDTH + 1);
                L->type = SHEAR;
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
                L->type = BEND;
                linkIndex++;
            }

            if (j < FLAGHEIGHT - 2)
            {
                L = tabL + linkIndex;
                L->connect(M, M + 2 * FLAGWIDTH);
                L->type = BEND;
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
    a = GuiSliderBar((Rectangle){screenWidth - 150, 210, 100, 30}, "A", std::to_string(a).substr(0, std::to_string(a).find(".") + 3).c_str(), a, 0.5, 1);
    b = GuiSliderBar((Rectangle){screenWidth - 150, 250, 100, 30}, "B", std::to_string(b).substr(0, std::to_string(b).find(".") + 3).c_str(), b, 0.75, 1);
    w = GuiSliderBar((Rectangle){screenWidth - 150, 290, 100, 30}, "W", std::to_string(w).substr(0, std::to_string(w).find(".")).c_str(), w, 0, 10000);
    r = GuiSliderBar((Rectangle){screenWidth - 150, 330, 100, 30}, "R", std::to_string(r).substr(0, std::to_string(r).find(".")).c_str(), r, 0, 100000);
    wx = GuiSliderBar((Rectangle){screenWidth - 150, 370, 100, 30}, "WX", std::to_string(wx).substr(0, std::to_string(wx).find(".") + 3).c_str(), wx, -1, 1);
    wy = GuiSliderBar((Rectangle){screenWidth - 150, 410, 100, 30}, "WY", std::to_string(wy).substr(0, std::to_string(wy).find(".") + 3).c_str(), wy, -1, 1);
    wz = GuiSliderBar((Rectangle){screenWidth - 150, 450, 100, 30}, "WZ", std::to_string(wz).substr(0, std::to_string(wz).find(".") + 3).c_str(), wz, -1, 1);
    if (GuiButton((Rectangle){screenWidth - 150, 490, 100, 30}, "RESET"))
    {
        resetParam();

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

    Camera camera = {0};
    camera.position = (Vector3){0.0f, 0.0f, 100.0f};
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    while (WindowShouldClose() == false)
    {
        windDir = {wx, wy, wz};
        windDir = windDir.normalized();
        MoteurRessortFrein(tabM, tabL);
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        BeginMode3D(camera);
        UpdateCameraPro(&camera, {GetMouseWheelMove(), 0, 0}, {0, 0, 0}, 0);

        for (Link *L = tabL; L < tabL + NBL; L++)
        {
            L->draw();
        }

        EndMode3D();

        drawGUI = IsKeyPressed(KEY_G) ? !drawGUI : drawGUI;

        if (drawGUI)
        {
            DrawGUI(tabM, tabL);
            DrawFPS(10, 10);
        }

        if (IsKeyPressed(KEY_F))
        {
            ToggleFullscreen();
        }

        EndDrawing();
    }
    CloseWindow();
    delete[] tabM;
    delete[] tabL;
    return 0;
}
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

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include <../styles/cyber/cyber.h>

using namespace std;

#define screenWidth 1280
#define screenHeight 720

#define NBM 13
#define NBL (NBM - 1)

#define m 1

float k = 0.1;
float z = 0.01;
float Fe = 100;
float gravity = 0;
float mouseForce = 0;

bool drawGUI = true;

class PMat
{
public:
    double pos = 0.0;
    double vit = 0.0;
    double frc = 0.0;
    double x, y = 0.0;
    double initX, initY = 0.0;
    Color color = BLACK;
    bool fixed = true;

    void initXY()
    {
        initX = x;
        initY = y;
    }

    void setup(double h)
    {
        vit += (frc / m) * h;
        pos += vit * h;
        frc = 0.0;
        y += vit * h;
    }

    void setup_pfixe()
    {
        frc = 0.0;
    }

    void reset()
    {
        pos = 0.0;
        vit = 0.0;
        frc = 0.0;
        x = initX;
        y = initY;
    }
};

class Link
{
public:
    double l0 = 0.0;
    PMat *M1, *M2;

    void reset()
    {
        l0 = M2->pos - M1->pos;
    }

    void connect(PMat *_M1, PMat *_M2)
    {
        M1 = _M1;
        M2 = _M2;
        l0 = M2->pos - M1->pos;
    }

    void setup_ressort_frein()
    {
        double h = 1.0 / Fe;
        double f = h * h * (k / m) * ((M2->pos - M1->pos) - l0) + h * (z / m) * (M2->vit - M1->vit);
        M1->frc += f;
        M2->frc -= f;
    }

    void apply_gravity()
    {
        for (PMat *M = M1; M <= M2; M++)
        {
            M->frc += gravity * 1000;
        }
    }
};

void Modeleur(PMat *tabM, Link *tabL)
{
    PMat *M = tabM;

    M->pos = 0.0;
    M->x = 100.0;
    M->y = screenHeight / 2.0;
    M->color = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_FOCUSED));
    M->initXY();

    M++;

    for (int i = 1; i < NBM - 1; i++)
    {
        M->pos = 0.0;
        M->x = 100.0 + i * (screenWidth - 200.0) / (NBM - 1.0);
        M->y = screenHeight / 2.0;
        M->color = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_PRESSED));
        M->fixed = false;
        M->initXY();
        M++;
    }

    M->pos = 0.0;
    M->x = screenWidth - 100.0;
    M->y = screenHeight / 2.0;
    M->color = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_FOCUSED));
    M->initXY();

    M = tabM;
    Link *L = tabL;

    for (int i = 0; i < NBL; i++)
    {
        L->connect(M, M + 1);
        L++;
        M++;
    }
}

void MoteurRessortFrein(PMat *tabM, Link *tabL)
{
    Link *L = tabL;

    for (int i = 0; i < NBL; i++)
    {
        L->setup_ressort_frein();
        L->apply_gravity();
        L++;
    }

    PMat *M = tabM;

    for (int i = 0; i < NBM; i++)
    {
        M->fixed ? M->setup_pfixe() : M->setup(1.0 / Fe);
        M++;
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
        PMat *M = tabM;
        for (int i = 0; i < NBM; i++)
        {
            M->reset();
            M++;
        }

        Link *L = tabL;
        for (int i = 0; i < NBL; i++)
        {
            L->reset();
            L++;
        }
    }
}

int main()
{
    PMat *tabM = new PMat[NBM];
    Link *tabL = new Link[NBL];

    InitWindow(screenWidth, screenHeight, "TP1");
    GuiLoadStyleCyber();
    SetTargetFPS(60);

    Modeleur(tabM, tabL);

    while (WindowShouldClose() == false)
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            PMat *M = tabM;

            for (int i = 1; i < NBM; i++)
            {
                Vector2 mV{(float)M->x, (float)M->y};
                if (abs(Vector2Distance(GetMousePosition(), mV)) < 15.0)
                {
                    M->frc += mouseForce * 1000000;
                }
                M++;
            }
        }

        MoteurRessortFrein(tabM, tabL);
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
        GuiPanel((Rectangle){0, 0, screenWidth, screenHeight}, "WELCOME TO THE SIMULATION");

        PMat *M = tabM;

        for (int i = 0; i < NBM; i++)
        {
            if (i < NBM - 1)
                DrawLineEx(Vector2{(float)M->x, (float)M->y}, Vector2{(float)(M + 1)->x, (float)(M + 1)->y}, 3.0, GetColor(GuiGetStyle(BUTTON, BASE_COLOR_FOCUSED)));
            DrawCircle(M->x, M->y, 10, M->color);
            M++;
        }
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
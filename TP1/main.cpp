#include <iostream>
#include <raylib.h>
#include <raymath.h>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include <styles/cyber/cyber.h>

using namespace std;

#define screenWidth 1280
#define screenHeight 720

#define Fe 100
#define H (1.0 / Fe)
#define k (0.865086 * pow(Fe, 2.0))
#define z (0.08 * Fe)
#define H (1.0 / Fe)
#define NBM 12
#define NBL (NBM - 1)

class PMat
{
public:
    double m = 15.0;
    double pos = 0.0;
    double vit = 0.0;
    double frc = 0.0;
    double x, y = 0.0;
    Color color = BLACK;
    bool fixed = true;

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
};

class Link
{
public:
    double l0 = 0.0;
    PMat *M1, *M2;
    void setup()
    {
        double f = k * ((M2->pos - M1->pos) - l0);
        M1->frc += f;
        M2->frc -= f;
    }
    void connect(PMat *_M1, PMat *_M2)
    {
        M1 = _M1;
        M2 = _M2;
        l0 = M2->pos - M1->pos;
    }
    void setup_frein()
    {
        double f = z * (M2->vit - M1->vit);
        M1->frc += f;
        M2->frc -= f;
    }
};

int main()
{
    int FPS = 60;

    PMat *tabM = new PMat[NBM];
    Link *tabL = new Link[NBL];

    InitWindow(screenWidth, screenHeight, "TP1");
    GuiLoadStyleCyber();
    SetTargetFPS(FPS);

    PMat *M = tabM;

    M->pos = 0.0;
    M->x = 100.0;
    M->y = screenHeight / 2.0;
    M->color = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_FOCUSED));

    M++;

    for (int i = 1; i < NBM - 1; i++)
    {
        M->pos = 0.0;
        M->x = 100.0 + i * (screenWidth - 200.0) / (NBM - 1.0);
        M->y = screenHeight / 2.0;
        M->color = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_PRESSED));
        M->fixed = false;
        M++;
    }

    M->pos = 0.0;
    M->x = screenWidth - 100.0;
    M->y = screenHeight / 2.0;
    M->color = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_FOCUSED));

    tabM[1].frc = 1000.0;

    M = tabM;
    Link *L = tabL;

    for (int i = 0; i < NBL; i++)
    {
        L->connect(M, M + 1);
        L++;
        M++;
    }

    while (WindowShouldClose() == false)
    {
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
        GuiPanel((Rectangle){0, 0, screenWidth, screenHeight}, "WELCOME TO THE SIMULATION");

        FPS = GuiSliderBar((Rectangle){100, screenHeight - 50, screenWidth - 200, 30}, "FPS", "", FPS, 0, 120);
        SetTargetFPS(FPS);

        Link *L = tabL;

        for (int i = 0; i < NBL; i++)
        {
            L->setup_frein();
            L++;
        }

        L = tabL;

        for (int i = 0; i < NBL; i++)
        {
            L->setup();
            L++;
        }

        PMat *M = tabM;

        for (int i = 0; i < NBM; i++)
        {
            if (M->fixed)
                M->setup_pfixe();
            else
                M->setup(H);

            if (i < NBM - 1)
                DrawLineBezier(Vector2{(float)M->x, (float)M->y}, Vector2{(float)(M + 1)->x, (float)(M + 1)->y}, 3.0, GetColor(GuiGetStyle(BUTTON, BASE_COLOR_FOCUSED)));
            DrawCircle(M->x, M->y, 10, M->color);
            M++;
        }

        EndDrawing();
    }

    CloseWindow();
    delete[] tabM;
    delete[] tabL;
    return 0;
}
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
#define NBM 13
#define NBL (NBM - 1)
#define m 15
int gravity = 0;
int mouseForce = 0;

bool drawGUI = true;
bool gravityValueFocused = false;
bool mouseForceValueFocused = false;

class PMat
{
public:
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

void Modeleur(PMat *tabM, Link *tabL)
{
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

    M = tabM;
    Link *L = tabL;

    for (int i = 0; i < NBL; i++)
    {
        L->connect(M, M + 1);
        L++;
        M++;
    }
}

void MoteurPhyisique(PMat *tabM, Link *tabL)
{
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
        M->frc += !M->fixed ? gravity : 0;
        M++;
    }

    M = tabM;

    for (int i = 0; i < NBM; i++)
    {
        M->fixed ? M->setup_pfixe() : M->setup(H);
        M++;
    }
}

void DrawGUI()
{
    Rectangle gravityValueBox{screenWidth - 130, 50, 100, 30};
    Rectangle mouseForceValueBox{screenWidth - 130, 90, 100, 30};

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        gravityValueFocused = CheckCollisionPointRec(GetMousePosition(), gravityValueBox) ? !gravityValueFocused : false;
        mouseForceValueFocused = CheckCollisionPointRec(GetMousePosition(), mouseForceValueBox) ? !mouseForceValueFocused : false;
    }

    GuiValueBox(gravityValueBox, "GRAVITY", &gravity, 0, 100000, gravityValueFocused);
    GuiValueBox(mouseForceValueBox, "FORCE", &mouseForce, 0, 1000000000, mouseForceValueFocused);
}

int main()
{
    PMat *tabM = new PMat[NBM];
    Link *tabL = new Link[NBL];

    InitWindow(screenWidth, screenHeight, "TP1");
    GuiLoadStyleCyber();
    SetTargetFPS(30);

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
                    M->frc += mouseForce;
                }
                M++;
            }
        }

        MoteurPhyisique(tabM, tabL);
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
            DrawGUI();

        EndDrawing();
    }
    CloseWindow();
    delete[] tabM;
    delete[] tabL;
    return 0;
}
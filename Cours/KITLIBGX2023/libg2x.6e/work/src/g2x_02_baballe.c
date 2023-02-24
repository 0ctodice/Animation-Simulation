/*!=================================================================!*/
/*!= E.Incerti - eric.incerti@univ-eiffel.fr                       =!*/
/*!= Université Gustave Eiffel                                     =!*/
/*!= Code "squelette" pour prototypage avec libg2x.6e              =!*/
/*!=================================================================!*/

#include <g2x.h>

/* tailles de la fenêtre graphique (en pixels)     */
static int WWIDTH=512, WHEIGHT=512;
/* limites de la zone reelle associee a la fenetre */
static double   wxmin=-10.,wymin=-10.,wxmax=+10.,wymax=+10.;

/* variables géométriques */
static G2Xpoint  ctr; /* un point   : centre du cercle    */
static double    ray; /* un réel    : rayon du cercle     */
static G2Xvector vit; /* un vecteur : vitesse de la balle */

/* variable (FLAG) de contrôle */
static bool      FILLDISK;

/* la fonction d'initialisation */
static void init(void)
{
  /* les objets de la scène */
  ctr=(G2Xpoint ){-4.,+1.};
  vit=(G2Xvector){0.005,0.003};
  ray=1.;
  FILLDISK=false; /* au départ, le cercle est 'vide' */
}

/* la fonction de contrôle : appelée 1 seule fois, juste après <init> */
static void ctrl(void)
{
  /* le centre devient actif (drag_&_drop), "cliquable" dans un rayon de 10 pixels */
  g2x_SetControlPoint(&ctr,10);
  /* un scrollbar vertical associé à la variable <ray> : varie dans [0.1,5.]       */
  g2x_CreateScrollv_d("ray",&ray,0.1,5.,"rayon du cercle");
  /* un bouton on/off pour 'remplir/vider' le cercle                               */
  g2x_CreateSwitch("F.",&FILLDISK,"Disque plein ou pas");
}

/* la fonction de dessin */
static void draw()
{
  switch (FILLDISK)
  {
    case true : g2x_DrawFillCircle(ctr,ray,G2Xo);
    default   : g2x_DrawCircle(ctr,ray,G2Xr,2);
  }
}

/* cette fois, on veut que le cercle se déplace tout seul
 * à la vitesse <vit> et rebondisse sur les bords de la fenêtre */
static void anim()
{
  /* 1°) le cercle se déplace */
  ctr.x+=vit.x;
  ctr.y+=vit.y;
  /* 2°) si il est sorti de la fenêtre, on inverse la vitesse */
  if (ctr.x<g2x_GetXMin()+ray || ctr.x>g2x_GetXMax()-ray) vit.x=-vit.x;
  if (ctr.y<g2x_GetYMin()+ray || ctr.y>g2x_GetYMax()-ray) vit.y=-vit.y;
}


/***************************************************************************/
/* La fonction principale : NE CHANGE (presque) JAMAIS                     */
/***************************************************************************/
int main(int argc, char **argv)
{
  /* 1°) creation de la fenetre - titre et tailles (pixels) */
  g2x_InitWindow(*argv,WWIDTH,WHEIGHT);
  /* 2°) définition de la zone de travail en coord. réeelles *
   *     ATTENTION : veiller à respecter les proportions
   *                 (wxmax-wxmin)/(wymax-wymin) = WWIDTH/WHEIGHT
   *     si cette fonction n'est pas appelée, la zone réelle
   *     par défaut est wxmin=0., wymin=0., wxmax=WWIDTH, wymax=WHEIGHT
   */
  g2x_SetWindowCoord(wxmin,wymin,wxmax,wymax);

  /* 3°) association des fonctions */
  g2x_SetInitFunction(init); /* fonction d'initialisation */
  g2x_SetCtrlFunction(ctrl); /* fonction de contrôle      */
  g2x_SetEvtsFunction(NULL); /* fonction d'événements     */
  g2x_SetDrawFunction(draw); /* fonction de dessin        */
  g2x_SetAnimFunction(anim); /* fonction d'animation      */
  g2x_SetExitFunction(NULL); /* fonction de sortie        */

  /* 4°) lancement de la boucle principale */
  return g2x_MainStart();
  /* RIEN APRES CA */
}

/*=================================================================*/
/*= E.Incerti - eric.incerti@univ-eiffel.fr                       =*/
/*= Université Gustave Eiffel                                     =*/
/*= Code "squelette" pour prototypage avec libg2x.6c              =*/
/*=================================================================*/

#include <g2x.h>

static int WWIDTH=512, WHEIGHT=512;
static double wxmin=-10.,wymin=-10.,wxmax=+10.,wymax=+10.;

G2Xpart  a,b;
G2Xpoint Ea,Eb;

/**---------------------------------------------------------*
 *---------------------------------------------------------**/
static void init(void)
{
  srand(getpid());
  g2x_SetWindowCoord(wxmin,wymin,wxmax,wymax);
  g2x_SetBkGdCol(0.);

  a  = g2x_Part_pud(g2x_RandPoint(5.),g2x_RandVector(1.),g2x_Rand_Percent(8.,0.1));
  Ea = g2x_PartNextPos(&a);

  b  = g2x_Part_pud(g2x_RandPoint(5.),g2x_RandVector(1.),g2x_Rand_Percent(8.,0.1));
  Eb = g2x_PartNextPos(&b);

}

/**---------------------------------------------------------*
 *---------------------------------------------------------**/
static void ctrl(void)
{
  g2x_SetControlPoint(&Ea,10);
  g2x_SetControlPoint(&Eb,10);
  g2x_SetControlPoint(&(a.p),10);
  g2x_SetControlPoint(&(b.p),10);
}

/**---------------------------------------------------------*
 *---------------------------------------------------------**/
static void evts(void)
{
       if (g2x_GetControlPoint()==&Ea ) { a.u = g2x_Vector2p(a.p,Ea); a.d=g2x_Normalize(&a.u); }
  else if (g2x_GetControlPoint()==&Eb ) { b.u = g2x_Vector2p(b.p,Eb); b.d=g2x_Normalize(&b.u); }
       if (g2x_GetControlPoint()==&a.p) { Ea  = g2x_PartNextPos(&a); }
  else if (g2x_GetControlPoint()==&b.p) { Eb  = g2x_PartNextPos(&b); }
}

/**---------------------------------------------------------*
 *---------------------------------------------------------**/
static void draw(void)
{
  g2x_PartDraw(&a,G2Xr);
  g2x_StaticPrint(10,40,G2Xr,"A : p=(%.2f,%.2f)  u=(%.2f,%.2f)  d=%.2f",a.p.x,a.p.y,a.u.x,a.u.y,a.d);

  g2x_PartDraw(&b,G2Xg);
  g2x_StaticPrint(10,20,G2Xg,"B : p=(%.2f,%.2f)  u=(%.2f,%.2f)  d=%.2f",b.p.x,b.p.y,b.u.x,b.u.y,b.d);

  G2Xpoint I;
  if (g2x_PartInterPart(&a,&b,&I))
  {
    g2x_DrawPoint(I,G2Xy,5);
    g2x_Print(I.x,I.y,G2Xy,"(%.2f,%.2f)",I.x,I.y);
  }
}

/**---------------------------------------------------------*
 *---------------------------------------------------------**/
int main(int argc, char **argv)
{
  g2x_InitWindow(*argv,WWIDTH,WHEIGHT);

  g2x_SetInitFunction(init); /* fonction d'initialisation */
  g2x_SetCtrlFunction(ctrl); /* fonction de contrôle      */
  g2x_SetEvtsFunction(evts); /* fonction d'interruptions  */
  g2x_SetDrawFunction(draw); /* fonction de dessin        */
  g2x_SetAnimFunction(NULL); /* fonction d'animation      */
  g2x_SetExitFunction(NULL); /* fonction de sortie        */

  return g2x_MainStart();
}

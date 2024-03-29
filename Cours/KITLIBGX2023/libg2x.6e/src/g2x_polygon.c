/*!==================================================================
  E.Incerti - Universite Gustave Eiffel - eric.incerti@univ-eiffel.fr
       - Librairie G2X - Fonctions de base d'acces public -
                    version 6e - aout 2022
  =================================================================== */

/*======================================================*/
/*! POLYGONES           CONVEXES             POLYGONES !*/
/*!           CONVEXES            POLYGONES            !*/
/*! CONVEXES            POLYGONES            CONVEXES  !*/
/*======================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <g2x.h>
#include <g2x_draw.h>
#include <g2x_geoalgo.h>
#include <g2x_polygon.h>


/*! avril 2022
 *  R�vision du mode d'allocation
 *  => un gros polyg�ne "vide" est cr�� � la 1� demande de Vertex par l'utilisateur.
 *     il sert de r�serve : lorsqu'il faut un nouveau Vertex, on vient piocher dedans
 *     et lorsqu'un Vertex est lib�r�, il est r�int�gr� dans le stock
 *     Ca serait mieux avec des blocs + petits et des <realloc>, mais le poly stockant
 *     des adresses de Vertex (prev/next), celles-ci sont invalid�es en cas de d�placement
 *     du bloc m�moire.
 *  => dans cette version, on part avec un poly de taille 1024 (un peu gros...)
 * */
static G2Xpolygon STOCKPOLY=NULL;
static uint       STOCKSIZE=1024;

/*! avril 2022
 * Pour r�gler, si n�cessaire, la taille du stock
 * */
extern void g2x_SetPolygonStockSize(uint n) { STOCKSIZE = n; }

static bool g2x_InitStockPolygon(void)
{
  STOCKPOLY = (G2Xvertex*)calloc(STOCKSIZE,sizeof(G2Xvertex));
  G2Xvertex *v=STOCKPOLY,*lastv=STOCKPOLY+STOCKSIZE;
  if (!v) return false;
  v->next = v+1;
  v++;
  while (v<lastv)
  {
    v->prev=v-1;
    v->next=v+1;
    v++;
  }
  v--;
  v->next = STOCKPOLY;
  STOCKPOLY->prev=v;
  return true;
}

/*!-------------------------------------------------------------------------!*/
/*! externe par n�cessit� (?) => appel�e dans <g2x_windows> par g2x_Quit()  !*/
/*!-------------------------------------------------------------------------!*/
extern void g2x_FreeStockPolygon(void)
{
  if (STOCKPOLY) free(STOCKPOLY);
  STOCKPOLY=NULL;
}

/*!-------------------------------------------------------------------------!*/
/*! Equivalent de l'alloc d'un nouveau vertex : pris dans le stock          !*/
/*!-------------------------------------------------------------------------!*/
static G2Xvertex* g2x_GetVertex()
{
  if (STOCKPOLY==NULL) g2x_InitStockPolygon();
  G2Xvertex *newv=STOCKPOLY->prev;
  newv->prev->next = STOCKPOLY;
  STOCKPOLY->prev = newv->prev;
  memset(newv,0,sizeof(G2Xvertex));
  return newv;
}

/*!-------------------------------------------------------------------------!*/
/*! => pas vraiment supprim� : remis dans le stock                          !*/
/*!-------------------------------------------------------------------------!*/
static void g2x_FreeVertex(G2Xvertex *v)
{
  memset(v,0,sizeof(G2Xvertex));
  v->next = STOCKPOLY;
  v->prev = STOCKPOLY->prev;
  STOCKPOLY->prev->next = v;
  STOCKPOLY->prev = v;
}

/*!-------------------------------------------------------------------------!*/
/*! supprime le vertex *V et renvoie le vertex SUIVANT (*V)->next           !*/
/*!-------------------------------------------------------------------------!*/
extern G2Xvertex* g2x_DelVertex(G2Xvertex** V)
{
  G2Xvertex *tmp=(*V)->next;
  (*V)->prev->next=tmp;
  tmp->prev=(*V)->prev;
  g2x_FreeVertex(*V);
  return tmp;
}

/*!-------------------------------------------------------------------------!*/
/*! cr�e un nouveau vertex pour le point de coordonn�es (x,y)               !*/
/*!-------------------------------------------------------------------------!*/
extern G2Xvertex* g2x_Vertex_XY(double x, double y)
{
  G2Xvertex* newptr=g2x_GetVertex();
  newptr->s.x = x;
  newptr->s.y = y;
  newptr->next= NULL;
  newptr->prev= NULL;
  return newptr;
}

/*!-------------------------------------------------------------------------!*/
/*! variante : cr�e un nouveau vertex pour le point P                       !*/
/*!-------------------------------------------------------------------------!*/
extern G2Xvertex* g2x_Vertex(G2Xpoint P)
{
  G2Xvertex* newptr=g2x_GetVertex();
  newptr->s = P;
  newptr->next= NULL;
  newptr->prev= NULL;
  return newptr;
}


static void g2x_SetVertexOpen(G2Xvertex *v)
{

  G2Xvector u1=g2x_NormalVector2p(v->prev->s,v->s);
  G2Xvector u2=g2x_NormalVector2p(v->s,v->next->s);
  v->o = g2x_ProdVect(u1,u2);
}

extern void g2x_SetPolygonOpen(G2Xpolygon pol)
{
  G2Xvertex *v=pol;
  do
  {
    g2x_SetVertexOpen(v);
  } while ((v=v->next)!=pol);
}

/*!-------------------------------------------------------------------------!*/
/*! ins�re le point P dans le polyg�ne (cr�e le vertex automatiquement)     !*/
/*!-------------------------------------------------------------------------!*/
extern void g2x_InsereSommet(G2Xpolygon *Pol, G2Xpoint P)
{
  if (*Pol==NULL) /* si la liste est vide : 1 seul vertex, boucl� sur lui-m�me */
  {
    (*Pol) = g2x_Vertex(P);
    (*Pol)->prev = (*Pol)->next = (*Pol);
    (*Pol)->o = 0.;
    return;
  }
  G2Xvertex* newptr;
  newptr=g2x_Vertex(P); /* nouveau vertex */
  newptr->prev=(*Pol);
  newptr->next=(*Pol)->next;
  newptr->next->prev=newptr;
  (*Pol)->next=newptr;
}

/*!-------------------------------------------------------------------------!*/
/*! m�me chose, mais pour un vertex d�j� existant (dans une autre polyg�ne) !*/
/*! => il est enlev� de son polyg. d'origine et ins�r� dans (Pol)           !*/
/*! => si Pol �tait vide, le nouveau vertex devient le point d'entr�e       !*/
/*!    sinon, il est ins�r� en fin, juste avant (*Pol)                      !*/
/*!-------------------------------------------------------------------------!*/
extern void g2x_InsereVertex(G2Xpolygon *Pol, G2Xvertex *new_vrtx)
{
  /* v est court-circuit� dans son polyg�ne d'origine */
  new_vrtx->prev->next = new_vrtx->next;
  new_vrtx->next->prev = new_vrtx->prev;
  if (*Pol==NULL) /* si la liste est vide : 1 seul vertex, boucl� sur lui-m�me */
  {
    (*Pol) = new_vrtx;
    (*Pol)->prev = (*Pol)->next = (*Pol);
    (*Pol)->o = 0.;
    return;
  }
  /* cas g�n�ral : v est ins�r� en dernier (avant Pol) */
  new_vrtx->prev=(*Pol);
  new_vrtx->next=(*Pol)->next;
  new_vrtx->next->prev=new_vrtx;
  (*Pol)->next=new_vrtx;
}


/*!-------------------------------------------------------------------------!*/
/*! insere le point P (cr�e le vertex) dans le polyg�ne, apr�s le vertex V  !*/
/*! => si V=NULL, �a d�marre un nouveau polyg�ne � 1 vertex ( sommet P)     !*/
/*! => dans tous les cas, la fonction renvoie le nouveau vertex             !*/
/*!-------------------------------------------------------------------------!*/
extern G2Xvertex* g2x_InsereApres(G2Xvertex* V, G2Xpoint P)
{
  G2Xvertex* newptr=g2x_Vertex(P);
  if (V)
  {
    newptr->prev=V;
    newptr->next=V->next;
    V->next->prev=newptr;
    V->next=newptr;
  }
  else /* si V==NULL */
  {
    newptr->prev=newptr;
    newptr->next=newptr;
  }
  return newptr;
}

/*!-------------------------------------------------------------------------!*/
/*! m�me chose, mais pour un vertex d�j� existant                           !*/
/*! => il est enlev� de son polyg. d'origine et ins�r� dans le nouveau      !*/
/*!-------------------------------------------------------------------------!*/
extern G2Xvertex* g2x_InsereVertexApres(G2Xvertex* prev_vrtx, G2Xvertex *new_vrtx)
{
  /* v est court-circuit� dans son polyg�ne d'origine */
  new_vrtx->prev->next = new_vrtx->next;
  new_vrtx->next->prev = new_vrtx->prev;
  if (prev_vrtx)
  {
    new_vrtx->prev=prev_vrtx;
    new_vrtx->next=prev_vrtx->next;
    prev_vrtx->next->prev=new_vrtx;
    prev_vrtx->next=new_vrtx;
  }
  else /* si prev_vrtx==NULL */
  {
    new_vrtx->prev=new_vrtx;
    new_vrtx->next=new_vrtx;
  }
  return new_vrtx;
}

/*!-------------------------------------------------------------------------!*/
/*! supprime les sommets doubles (points identiques)                        !*/
/*!-------------------------------------------------------------------------!*/
extern int g2x_CleanPolygon(G2Xpolygon Pol)
{
  int n=1;
  G2Xvertex *a,*b;

  if (Pol==NULL) return 0;
  a=Pol;
  b=a->next;
  while (b!=Pol)
  {
    if (a->s.x==b->s.x && a->s.y==b->s.y) g2x_DelVertex(&b);
    n++;
    a=b;
    b=a->next;
  }
  return n;
}

/*!-------------------------------------------------------------------------!*/
/*! supprime les sommets algin�s                                            !*/
/*!-------------------------------------------------------------------------!*/
extern int g2x_CleanAlignVertex(G2Xpolygon Pol)
{
  int n=1;
  G2Xvertex *a,*b,*c;

  if (Pol==NULL) return 0;
  a=Pol;
  if (a->next==Pol) return 0;
  b=a->next;
  if (b->next==Pol) return 0;
  c=b->next;
  while (c!=Pol)
  {
    double o = g2x_Orient(a->s,b->s,c->s);
    if (G2Xiszero(o)) g2x_DelVertex(&b);
    n++;
    a=b;
    b=c;
    c=c->next;
  }
  return n;
}


/*!-------------------------------------------------------------------------!*/
/*! Enregistrement en mode texte (ou simple affichage)                      !*/
/*!-------------------------------------------------------------------------!*/
extern void g2x_PrintPolygon(G2Xpolygon Pol, FILE* output)
{
  int i,N=0;
  G2Xvertex *tmp;
  char    ok;

  tmp=Pol;
  while (tmp!=NULL && tmp->next!=Pol)
  {
    N++;
    tmp=tmp->next;
  }
  ok = (tmp==NULL ? 'n':'o');
  fprintf(stderr,"Polygone %s a %d sommets\n",ok=='o'?"ferme":"ouvert",N);

  if (output==stderr) fprintf(stderr," N=");
  fprintf(output,"%d\n",N);
  i=0;
  tmp=Pol;
  while (i<=N)
  {
    if (output==stderr) fprintf(stderr," s[%2d] : ",i);
    fprintf(output,"%lf %lf\n",tmp->s.x,tmp->s.y);
    tmp=tmp->next;
    i++;
  }
  fprintf(output,"%c\n",ok);
}

/*!-------------------------------------------------------------------------!*/
/*! Lecture sur fichier                                                     !*/
/*!-------------------------------------------------------------------------!*/
extern void g2x_ScanPolygon(G2Xpolygon* Pol, FILE* input)
{
  int N,i;
  G2Xvertex *tmp;
  double  x,y;
  char    ok;

  if (input==stdin) fprintf(stderr," N=");
  fscanf(input,"%d\n",&N);
  i=0;
  if (input==stdin) fprintf(stderr," s[%2d] : ",i);
  fscanf(input,"%lf %lf\n",&x,&y);
  *Pol=g2x_Vertex_XY(x,y);

  tmp=*Pol;
  while (++i<N+1)
  {
    if (input==stdin) fprintf(stderr," s[%2d] : ",i);
    fscanf(input,"%lf %lf\n",&x,&y);
    tmp->next=g2x_Vertex_XY(x,y);
    tmp->next->prev=tmp;
    tmp=tmp->next;
  }
  if (input==stdin) fprintf(stderr,"Fermer le G2Xpolygon ? ");
  fscanf(input,"%c\n",&ok);
  if (ok=='o' || ok=='0' || ok=='\n')
  {
    tmp->next=*Pol;
    (*Pol)->prev=tmp;
  }
}

/*!-------------------------------------------------------------------------!*/
/*! Polyg�ne r�gulier � <deg> sommets                                       !*/
/*! - Centr� en <C>, de rayon <r>, oritent� d'un angle <alpha>              !*/
/*!-------------------------------------------------------------------------!*/
extern void g2x_RegulPolygon(G2Xpolygon* Pol, uint deg, G2Xpoint C, double r, double alpha)
{
  G2Xvertex* tmp;
  double apas=2.*PI/deg;

   alpha+=0.5*apas;
  *Pol=g2x_Vertex_XY(C.x+r*cos(alpha),C.y+r*sin(alpha));
  tmp=*Pol;
  while (--deg)
  {
    alpha+=apas;
    tmp->next=g2x_Vertex_XY(C.x+r*cos(alpha),C.y+r*sin(alpha));
    tmp->next->prev=tmp;
    tmp=tmp->next;
  }
  tmp->next=*Pol;
  (*Pol)->prev=tmp;
}

/*!-------------------------------------------------------------------------!*/
/*! Polyg�ne construit sur un tableau de points, ferm� ou non               !*/
/*!-------------------------------------------------------------------------!*/
extern void g2x_TabPolygon(G2Xpolygon* Pol, G2Xpoint S[], uint n, bool closed)
{
  G2Xvertex* tmp;
  uint i;

  *Pol=g2x_Vertex(S[0]);
  tmp=*Pol;
  i=0;
  while (++i<n)
  {
    tmp->next=g2x_Vertex(S[i]);
    tmp->next->prev=tmp;
    tmp=tmp->next;
  }
  if (closed)
  {
    tmp->next=*Pol;
  (*Pol)->prev=tmp;
  }
}


/*!-------------------------------------------------------------------------!*/
/*! v�rifie si le polyg�ne est convexe                                      !*/
/*!-------------------------------------------------------------------------!*/
extern int g2x_IsConvex(G2Xpolygon Pol)
{
  G2Xvertex* v=Pol;
  int orient = g2x_Orient(v->s,v->next->s,v->next->next->s);
  do
  {
    v=v->next;
    if (g2x_Orient(v->s,v->next->s,v->next->next->s)!=orient) return 0;
  } while (v->next!=Pol);
  return orient;
}

extern void g2x_DrawPolygon(G2Xpolygon Pol, G2Xcolor col, uint w)
{
  G2Xvertex* tmp=Pol;
  double  xa,ya,xb,yb;

  if (Pol==NULL)
    return;
  if (Pol==Pol->next)
    return g2x_Plot(Pol->s.x,Pol->s.y,col,w);
  if (Pol==Pol->next->next)
    return g2x_Line(Pol->s.x,Pol->s.y,Pol->next->s.x,Pol->next->s.y,col,w);

  xa=tmp->s.x;
  ya=tmp->s.y;
  do
  {
    tmp=tmp->next;
    xb=tmp->s.x;
    yb=tmp->s.y;
    g2x_Line(xa,ya,xb,yb,col,w);
    xa=xb;
    ya=yb;
  }
  while (tmp!=Pol);
}

extern void g2x_DrawSplinePolygon(G2Xpolygon Pol)
{
  G2Xvertex* tmp=Pol;
  double xa,ya,xb,yb;
  static G2Xcolor  col1={0.75,0.50,0.25};
  static G2Xcolor  col2={0.12,0.25,0.33};
  int    i=0;
  do
  {
    xa=tmp->s.x; ya=tmp->s.y;
    if (i%3==0)
      g2x_Plot(xa,ya,col1,4);
    else
      g2x_Plot(xa,ya,col2,2);
    i++;
    tmp=tmp->next;
    xb=tmp->s.x; yb=tmp->s.y;
    g2x_LineDft(xa,ya,xb,yb);
  }
  while (tmp!=Pol);
}


extern void g2x_DrawFilledPolygon(G2Xpolygon Pol, G2Xcolor col)
{
  G2Xvertex* tmp=Pol;
  double xa,ya,xb,yb,xc,yc;

  if (Pol==NULL)
    return;
  if (Pol==Pol->next)
    return g2x_Plot(Pol->s.x,Pol->s.y,col,1);
  if (Pol==Pol->next->next)
    return g2x_Line(Pol->s.x,Pol->s.y,Pol->next->s.x,Pol->next->s.y,col,1);

  if (!g2x_IsConvex(Pol)) return g2x_DrawPolygon(Pol,col,1);

  xa=tmp->s.x;
  ya=tmp->s.y;
  tmp=tmp->next;
  do
  {
    xb=tmp->s.x;
    yb=tmp->s.y;
    tmp=tmp->next;
    xc=tmp->s.x;
    yc=tmp->s.y;
    g2x_FillTriangle(xa,ya,xb,yb,xc,yc,col);
  }
  while (tmp!=Pol);
}


extern void g2x_FreePolygon(G2Xpolygon* Pol)
{
  G2Xvertex* tmp;

  if (*Pol==NULL) return;
  tmp=(*Pol)->next;
  while (tmp!=*Pol)
  {
    (*Pol)->next=tmp->next;
    g2x_DelVertex(&tmp);
    tmp = (*Pol)->next;
  }
  g2x_DelVertex(Pol);
  *Pol=NULL;
}


extern bool g2x_PointDansPolygon(G2Xpoint P, G2Xpolygon Pol)
{
  /* il faut commencer par determiner */
  /* l'orientation du G2Xpolygon        */
  double sgn = G2Xorient(Pol->s,Pol->next->s,Pol->next->next->s);
  G2Xvertex* tmp=Pol;
  do
  {
    if (sgn*G2Xorient(P,tmp->s,tmp->next->s)<0.) return false;
    tmp=tmp->next;
  }
  while (tmp!=Pol);
  return true;
}

/*!-------------------------------------------------------------------------!*/
/*! version alternative                                                     !*/
/*!  cette version renvoie un vertex (*som) :                               !*/
/*!  - NULL si le  vertex v est dans le polyg�ne                            !*/
/*!  - sinon c'est que P est dehors, par rapport � [(*som),(*som)->next]    !*/
/*!-------------------------------------------------------------------------!*/
extern bool g2x_VertexDansPolygon(G2Xvertex *v, G2Xpolygon Pol, int orient, G2Xvertex** som)
{
  G2Xvertex* tmp=Pol;
  do
  {
    if (orient*G2Xorient(v->s,tmp->s,tmp->next->s)<0.)
    {
     *som = tmp;
      return false;
    }
    tmp=tmp->next;
  }
  while (tmp!=Pol);
 *som=NULL;
  return true;
}

/* une 3� version... */
extern G2Xvertex* g2x_PointDansPolygon3(G2Xpoint P, G2Xpolygon Pol)
{
  double sgn = G2Xorient(Pol->s,Pol->next->s,Pol->next->next->s);
  G2Xvertex* tmp=Pol;
  do
  {
    if (sgn*G2Xorient(P,tmp->s,tmp->next->s)<0.)
      return tmp;
    tmp=tmp->next;
  }
  while (tmp!=Pol);
  return NULL;
}

extern bool g2x_Clipping(G2Xpoint *A, G2Xpoint *B, G2Xpolygon Pol)
{
  G2Xpoint   P,Q;
  G2Xvertex *S;
  bool boolA = g2x_PointDansPolygon(*A,Pol);
  bool boolB = g2x_PointDansPolygon(*B,Pol);
  bool flag  = false;

  if (boolA && boolB )return true;    /* A et B dedans */

  /* on cherche une premiere intersection*/
  S = Pol;
  while (!flag && S->next!=Pol)
  {
    flag = g2x_SegmentInterSegment(*A,*B,S->s,S->next->s,&P);
    S=S->next;
  }

  if (flag) /* si on en a trouve une */
  {
    if (boolB) { *A=P;  return true; } /* B dedans, donc A dehors */
    if (boolA) { *B=P;  return true; } /* A dedans, donc B dehors */
    flag = false;
    while (!flag)                   /* A et B dehors           */
    { /* on cherche la seconde intersection :
         on est s�r qu'elle existe */
      flag = g2x_SegmentInterSegment(*A,*B,S->s,S->next->s,&Q);
      S=S->next;
    }
    if (G2Xpntprodscal(P,*A,Q)<0)
    { *A=P; *B=Q; }
    else
    { *A=Q; *B=P; }
    return true;
  }
  /* si on n'a rien trouve */
  return false;
}

/*= autre version =*/
extern int g2x_Clipping2(G2Xpoint* A, G2Xpoint* B, G2Xpolygon Pol)
{
  G2Xpoint   P,Q;
  G2Xvertex *S;
  bool boolA = g2x_PointDansPolygon(*A,Pol);
  bool boolB = g2x_PointDansPolygon(*B,Pol);
  bool flag  = false;

  if (boolA && boolB )return 0;    /* A et B dedans */

  /* on cherche une premiere intersection*/
  S = Pol;
  while (!flag && S->next!=Pol)
  {
    flag = g2x_SegmentInterSegment(*A,*B,S->s,S->next->s,&P);
    S=S->next;
  }

  if (flag) /* si on en a trouve une */
  {
    if (boolB) { *A=P;  return 1; } /* B dedans, donc A dehors */
    if (boolA) { *B=P;  return 2; } /* A dedans, donc B dehors */
    flag = false;
    while (!flag)                   /* A et B dehors           */
    { /* on cherche la seconde intersection :
         on est s�r qu'elle existe */
      flag = g2x_SegmentInterSegment(*A,*B,S->s,S->next->s,&Q);
      S=S->next;
    }
    if (G2Xpntprodscal(P,*A,Q)<0)
    { *A=P; *B=Q; }
    else
    { *A=Q; *B=P; }
    return 3;
  }
  /* si on n'a rien trouve */
  return -1;
}

extern G2Xpolygon g2x_PolygonInterPolygon(G2Xpolygon Pol1, G2Xpolygon Pol2)
{
  G2Xpolygon Inter=NULL;
  G2Xpoint   A,B;
  G2Xvertex *tmp;

  if (!g2x_IsConvex(Pol1) || !g2x_IsConvex(Pol2)) return NULL;

  tmp=Pol1;
  do
  {
    A = tmp->s;
    B = tmp->next->s;

    switch (g2x_Clipping2(&A,&B,Pol2))
    {
      case 0 : case 1 : Inter=g2x_InsereApres(Inter,A); break;
      case 2 : case 3 : Inter=g2x_InsereApres(Inter,A);
                        Inter=g2x_InsereApres(Inter,B);
                        break;
    }
    tmp =  tmp->next;
  } while (tmp!=Pol1);

  tmp=Pol2;
  do
  {
    if (g2x_PointDansPolygon(tmp->s,Pol1))
    {
      if (Inter==NULL || Inter->next==Inter || Inter->next->next==Inter)
        Inter=g2x_InsereApres(Inter,tmp->s);
      else
        Inter=g2x_InsereApres(g2x_PointDansPolygon3(tmp->s,Inter),tmp->s);
    }
    tmp =  tmp->next;
  } while (tmp!=Pol2);

  return Inter;
}

/*! Clipping d'un polygone sur un cercle :
  *  -1 : les 2 sont disjoints - par d'untersection, pas d'inclusion
  *   0 : le polygone est inclus dans le cercle
  *  +1 : le cercle est inclus dans le polygone
  * n>1 : le polygone intersection a n sommets, certains pouvant
          appartenir au polyg. initial.
                                                                  !*/
extern int g2x_CercleClipPolygon(G2Xpoint C, double r, G2Xpolygon Poly, G2Xpolygon *Clip)
{
  G2Xvertex* v=Poly;
  G2Xpoint   A,B,I1,I2;
  int       sum=0;
  int       n;

  do
  {
    A=v->s;
    v=v->next;
    B=v->s;
    n=0;
    if (G2Xsqrdist(C,A)<r*r)
    { g2x_InsereSommet(Clip,A);
      sum++;
      n=1;
    }
    if (G2Xsqrdist(C,B)<r*r && n==1) continue;

    n=g2x_SegmentInterCercle(A,B,C,r,&I1,&I2);
    switch (n)
    {
      case -1 : case 0 : case +1 : break;
      case +2 : g2x_InsereSommet(Clip,I2); sum++;
                break;
      case +3 : g2x_InsereSommet(Clip,I1); sum++;
                break;
      case +4 : g2x_InsereSommet(Clip,I1); sum++;
                g2x_InsereSommet(Clip,I2); sum++;
                break;
    }

  }
  while (v!=Poly);

  /* Il y a au moins 1 intersection */
  if (sum>0) return sum;

  /* Aucune intersection et au moins 1 sommet est dans le cercle */
  /* donc il y sont tous et le poly est inclu dans le cercle */
  if (G2Xsqrdist(C,Poly->s)<r*r) return 0;

  /* tous les sommets du polyg. sont hors du cercle :
     - si C est dans le polyg, le cercle y estentierement (return 1)
     - sinon les 2 sont disjoints (return 0) */
  return g2x_PointDansPolygon(C,Poly)?1:-1;
}

extern bool g2x_ConvexHull(G2Xpoint *point, int nbpt, G2Xpolygon *EC, int *EClen)
{
  if (nbpt<3) return false;
  /* 1) Initialisation sur les 3 premiers points */
  G2Xpoint *p=point;
  (*EC) = g2x_InsereApres((*EC),*p);
  g2x_InsereApres((*EC),*(p+1));
  /* Pour le 3�, on teste le triangle <P0,P1,P2> pour savoir si il est direct ou pas
     FONDAMENTAL : ca d�termine dans quel sens on tourne */
  if (g2x_Orient(*(p),*(p+1),*(p+2))>=0.)
    g2x_InsereApres((*EC)->next,*(p+2));
  else
    g2x_InsereApres((*EC),*(p+2));
  (*EClen) = 3;
  p+=3;

  G2Xvertex *v,*w;
  while (p<point+nbpt)
  {
    v=*EC;
    do
    {
      if (g2x_Orient(*p,v->s,v->next->s)<0.)
      { /* p est hors de l'enveloppe par rapport au segment [v,v.next] */
        /* On insere p entre v et v->next                              */
        v=g2x_InsereApres(v,*p);
        (*EClen)++;
        /* On nettoie ce qu'il y a avant */
        w=v->prev;
        /*! ATTENTION ICI :
         *  la fonction <g2x_DelVertex> renvoie le vertex suivant celui qui est d�truit
         *  il faut donc RECULER d'un cran et prendre le <prev>                        !*/
        while (g2x_Orient(w->s,w->prev->s,w->next->s)>0)
        {
          w=g2x_DelVertex(&w)->prev;
          (*EClen)--;
        }
        /* On nettoie ce qu'il y a apres */
        w=v->next;
        while (g2x_Orient(w->s,w->prev->s,w->next->s)>0)
        {
          w=g2x_DelVertex(&w);
          (*EClen)--;
        }
        /* On repositionne le d�but de liste sur le point ins�r� */
        *EC=v;
        /* Et on s'en va */
        break;
      }
      /* sinon, on teste le segment suivant */
      v = v->next;
    }
    while(v!=*EC);
    /* Le point �tait � l'int�rieur */
    p++;
  }
  return true;
}

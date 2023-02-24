/*!===============================================================
  E.Incerti - Universite Gustave Eiffel - eric.incerti@univ-eiffel.fr
       - Librairie G2X - Fonctions de base d'acces public -
                    version 6e - aout 2022
  ================================================================!*/

#ifdef __cplusplus
  extern "C" {
#else
  #define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <g2x.h> /* le type G2Xpnm est la */

static int _r_=0;

/* Charge l'image */
static size_t g2x_PnmPullRAW(G2Xpnm *img, FILE* imgfile)
{
  uchar *map=img->map, *end=img->end;
  uchar *p,*q;
  int    i;
  size_t n=img->layer*img->height*img->width;

  switch (img->mode) /* bpm/pgm/ppm ASCII/RAW */
  { /* format ASCII */
    case '1' : /* PBM */
    case '2' : /* PGM */
    case '3' : /* PPM */ for (p=map; !feof(imgfile) && p<end ;++p) n+=_r_=fscanf(imgfile,"%hhu",p); break;
    /* format RAW */
    case '4' : /* PBM */ n/=8;
    case '5' : /* PGM */
    case '6' : /* PPM */ n=fread(map,sizeof(uchar),n,imgfile); break;
  }

  if (img->mode=='4') /* cas BINAIRE  : 1 bit -> 8 bits */
  {
    fprintf(stderr,"\e[0;33mFormat PBM : conversion 1 bit -> 8 bits (%d octets lus)->%d pixels\e[0m\n",(int)n,(int)(8*n));
    p=(uchar*)img->map+n-1;
    q=(uchar*)img->end-1;
    do
    {
      for (i=0;i<8;i++) {  q--; *q=(*p&0x1)?0xFF:0x00; *p>>=1;}
      p--;
    }
    while (p!=img->map);
    n*=8;
    return n;
  }
  return n;
}

/* Charge l'image */
static size_t g2x_PnmPullRGB(G2Xpnm *img, FILE* imgfile)
{
  uchar *map=img->map, *end=img->end;
  uchar *p,*q;
  int    i;
  size_t n=3*img->height*img->width;

  switch (img->mode) /* bpm/pgm/ppm ASCII/RAW */
  {
    /* format ASCII */
    case '1' : /* PBM */
    case '2' : /* PGM */
    case '3' : /* PPM */ for (p=map; !feof(imgfile) && p<end ;++p) n+=_r_=fscanf(imgfile,"%hhu",p); break;
    /* format RAW */
    case '4' : /* PBM */ n/=8;
    case '5' : /* PGM */
    case '6' : /* PPM */ n=fread(map,sizeof(uchar),n,imgfile); break;
  }

  if (img->mode=='4') /* cas BINAIRE  : 1 bit -> 24 bits */
  {
    fprintf(stderr,"\e[0;33mFormat PBM : conversion 1 bit -> 24 bits (%d octets lus)->%d pixels\e[0m\n",(int)n,24*(int)n);
    p=(uchar*)img->map+n-1;
    q=(uchar*)img->end-1;
    do
    {
      for (i=0;i<8;i++) {  q-=3; if (*p&1) memset(q,0xFF,3); *p>>=1;}
      p--;
    }
    while (p!=img->map);
    n*=24;
  }
  if (img->mode=='5') /* cas NIV.GRIS : 8 bit -> 24 bits */
  {
    fprintf(stderr,"\e[0;33mFormat PGM : conversion 8 bits -> 24 bits (%d octets lus)->%d pixels\e[0m\n",(int)n,3*(int)n);
    p=(uchar*)img->map+n-1;
    q=(uchar*)img->end-1;
    do {q-=3; memset(q,*p,3); p--; } while (p!=img->map);
    n*=3;
  }
  return n;
}

/* Charge l'image */
static size_t g2x_PnmPushRAW(G2Xpnm *img, FILE* imgfile)
{
  uchar *map=img->map, *end=img->end;
  uchar *p,*q,a;
  size_t n=img->layer*img->height*img->width,i=0;

  if (img->mode=='4')
  {
    fprintf(stderr,"\e[0;33mFormat PBM : conversion 8 bits -> 1 bit\e[0m\n");
    p=map;
    i=0;
    do { fprintf(stderr,"\e[0;33m%hhu%s",*p++,(++i)%img->width?"":"\e[0m\n");} while (p!=end);
    p=map;
    q=map;
    n=0;
    do
    {
      a = 0;
      a|=(*q++&1)<<7; a|=(*q++&1)<<6; a|=(*q++&1)<<5; a|=(*q++&1)<<4;
      a|=(*q++&1)<<3; a|=(*q++&1)<<2; a|=(*q++&1)<<1; a|=(*q++&1)<<0;
      p--;
     *p++=a;
      n++;
    }
    while (q!=end);
    end=p;
  }

  switch (img->mode)
  {
    case '1' :
    case '2' :
    case '3' : for (p=map;p<end;++p) n+=fprintf(imgfile,"%hhu ",*p); break;
    case '4' :
    case '5' :
    case '6' : n=_r_=fwrite(map,sizeof(uchar),n,imgfile); break;
  }
  return n;
}

/* Charge l'image */
static size_t g2x_PnmPushRGB(G2Xpnm *img, FILE* imgfile)
{
  uchar *map=img->map, *end=img->end;
  uchar *p,*q,a;
  size_t n=3*img->height*img->width,i=0;

  if (img->mode=='4')
  {
    fprintf(stderr,"\e[0;33mFormat PBM : conversion 8 bits -> 1 bit\e[0m\n");
    p=map;
    i=0;
    do { fprintf(stderr,"\e[0;33m%hhu%s",*p++,(++i)%img->width?"":"\e[0m\n");} while (p!=end);
    p=map;
    q=map;
    n=0;
    do
    {
      a = 0;
      a|=(*q++&1)<<7; a|=(*q++&1)<<6; a|=(*q++&1)<<5; a|=(*q++&1)<<4;
      a|=(*q++&1)<<3; a|=(*q++&1)<<2; a|=(*q++&1)<<1; a|=(*q++&1)<<0;
      p--;
     *p++=a;
      n++;
    }
    while (q!=end);
    end=p;
  }

  switch (img->mode)
  {
    case '1' :
    case '2' :
    case '3' : for (p=map;p<end;++p) n+=fprintf(imgfile,"%hhu ",*p); break;
    case '4' :
    case '5' :
    case '6' : n=_r_=fwrite(map,sizeof(uchar),n,imgfile); break;
  }
  return n;
}


#define MAXIMGNAME 32
#define MAXNAMELEN 255
typedef struct
{
  G2Xpnm   *img;
  char     name[MAXNAMELEN+1];
} G2Ximgnames;

static int nbimages=0;
static G2Ximgnames img_names[MAXIMGNAME];

extern bool g2x_PnmFree(G2Xpnm** img)
{
  if (*img==NULL) return false;

  if (nbimages)
  {
    int i=0;
    while (i<nbimages && *img!=img_names[i].img) i++;
    if (i<nbimages)
    {
      if (*img_names[i].name==0)  return true;
     *img_names[i].name=0;
    }
  }
  if ((*img)->map) free((*img)->map);
  free(*img);
  *img=NULL;
  return true;
}

extern bool g2x_PnmAlloc(G2Xpnm **img, int width, int height, int layer, int depth)
{
  if ((*img)!=NULL                               &&
      (*img)->map!=NULL                          &&
      (*img)->height*(*img)->width>=height*width &&
      (*img)->layer>=layer                       &&
      (*img)->depth>=depth                        )
  {
    (*img)->height = height;
    (*img)->width  = width;
    (*img)->layer  = layer;
    (*img)->depth  = depth;
    (*img)->end=(uchar*)(*img)->map + (layer*height*width);
    return true;
  }
  if (*img!=NULL) g2x_PnmFree(img);

  if (!((*img)=(G2Xpnm*)calloc(1,sizeof(G2Xpnm))))
  { fprintf(stderr,"\e[1;31m  <g2x_Pnmalloc_1> : erreur allocation\e[0m\n"); return false;}
  (*img)->height = height;
  (*img)->width  = width;
  (*img)->layer  = layer;
  (*img)->depth  = depth;
  if (!((*img)->map = (uchar*)calloc(layer*width*height,sizeof(uchar))))
  {
    free((void**)img);
    fprintf(stderr,"\e[1;31m  <g2x_Pnmalloc_1> : erreur allocation\e[0m\n");
    return false;
  }
   (*img)->end=(uchar*)(*img)->map+(layer*height*width);
  return true;
}

extern bool g2x_PnmRead(G2Xpnm** img, const char *filename, bool MPEG_SETUP)
{
  int     height,width,depth=1,layer=1;
  char    mode[3],c,*r,comment[256];
  FILE*   imgfile;
  size_t  leng;

  if (!(imgfile=fopen(filename,"r")))
  { fprintf(stderr,"\e[1;31mErreur ouverture %s\e[0m\n",filename); return false; }

  _r_=fscanf(imgfile,"%s\n",mode);
  if (strcmp(mode,"P1")!=0 && strcmp(mode,"P4")!=0 &&
      strcmp(mode,"P2")!=0 && strcmp(mode,"P5")!=0 &&
      strcmp(mode,"P3")!=0 && strcmp(mode,"P6")!=0   )
  { fprintf(stderr,"\e[1;31mpnm : format <%s> invalide\e[0m\n",mode); return false; }

  while ((c=getc(imgfile))=='#') r=fgets(comment,256,imgfile);
  ungetc(c,imgfile);
  _r_=fscanf(imgfile,"%d",&width);
  _r_=fscanf(imgfile,"%d",&height);

  if (MPEG_SETUP)
  {
    bool  flag=((width%16)||(height%16));
    if (flag)
    {
      fprintf(stderr,"\e[0;33mw=%d, h=%d\e[0m\n",width,height);
      width =( width/16)*16;
      height=(height/16)*16;
      fprintf(stderr,"\e[0;33m---------------------------------------------------------------------------------------\n");
      fprintf(stderr,"g2x_PnmRead(%s)\nAjustement - les tailles de la fenetre doivent etre multiples de 16 (BUG dans ppmtomeg) -> %dx%d\n"
                  ,filename,width,height);
      fprintf(stderr,"---------------------------------------------------------------------------------------\e[0m\n");
      fclose(imgfile);
      sprintf(comment,"pamcut 0 0 %d %d %s > .tmppnm2",width,height,filename);
      _r_=system(comment);
      flag=g2x_PnmRead(img,".tmppnm2",G2X_NO_MPEG_SETUP);
      _r_=system("rm -f .tmppnm2");
      return flag;
    }
  }

  if (mode[1]!='1' && mode[1]!='4')
  { _r_=fscanf(imgfile,"%d\n",&depth); depth++; }
  else
    _r_=fscanf(imgfile,"\n");

  if (mode[1]=='3' || mode[1]=='6') layer=3;

  if (!g2x_PnmAlloc(img,width,height,layer,depth)) { fclose(imgfile); return false; }

  (*img)->mode=mode[1];

  leng=g2x_PnmPullRAW(*img,imgfile);
  if (leng<layer*height*width)
  { fprintf(stderr,"\e[0;31mdonnee tronquees : %d octets lus sur %d prevus\e[0m\n",(int)leng,(int)(layer*height*width));}

  fclose(imgfile);
  return true;
}

extern bool g2x_ImageRead(G2Xpnm** img, const char *filename, bool MPEG_SETUP, bool RELOAD)
{
  static char  command[256];
  bool   ok=true;

  if (!RELOAD)
  {
    int i=0;
    while (i<nbimages && strcmp(filename,img_names[i].name)) i++;

    if (i<nbimages)  { *img=img_names[i].img; return true; }
  }
  // mai 2022 : on teste d'abord si c'est du format PNM
  if (!g2x_PnmRead(img,filename,MPEG_SETUP))
  {
    // si c'est pas le cas, on converti en PNM (temporaire)
    sprintf(command,"cat  %s | anytopnm > .tmppnm;",filename);
    if ((_r_=system(command)))
    {
      fprintf(stderr,"\e[1;31m<g2x_ImageRead> : le fichier <%s> n'existe pas ou n'est pas lisible\e[0m\n",filename);
      return false;
    }
   // on charge le PNM temporaire
   ok=g2x_PnmRead(img,".tmppnm",MPEG_SETUP);
   // et on le détruit
    _r_=system("rm -f .tmppnm");
  }

  fprintf(stderr,"\e[0;33mChargement image \e[0;35m%s\e[0;33m (ajust.:[\e[0;41m%s\e[0;33m],recyclage:[\e[0;45m%s\e[0;33m])\e[0m\n"
                ,filename,MPEG_SETUP?"oui":"non",RELOAD?"non":"oui");

  if (!RELOAD)
  {
    if (nbimages==MAXIMGNAME)
      fprintf(stderr,"\e[1;31m<g2x_ImageRead> : impossible de stocker le nom de l'image <%s> - tableau plein\e[0m\n",filename);
    strncpy(img_names[nbimages].name,filename,MAXNAMELEN);
    img_names[nbimages].img=*img;
    nbimages++;
  }
  return ok;
}

// mai 2022
// Chargement du pixamp dans une texture
// Par défaut, la texture est un rectangle à la taille de l'image qui sera 'resizé' à l'affichage
extern void g2x_LoadPixmap(G2Xpnm *pnm)
{
  pnm->id = glGenLists(1);
  glNewList(pnm->id, GL_COMPILE);
    uint tex_id;
    glGenTextures(1,&tex_id);
    glBindTexture(GL_TEXTURE_2D,tex_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    switch (pnm->layer)
    {
      case 1 : glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, pnm->width, pnm->height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,pnm->map); break;
      case 3 : glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8     , pnm->width, pnm->height, 0, GL_RGB      , GL_UNSIGNED_BYTE,pnm->map); break;
      default : fprintf(stderr,"\e[1;31m<g2x_LoadPixmap> : format d'image [%d] non reconnu\e[0m\n",pnm->layer); return;
    }
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glColor4f(1.,1.,1.,0.);
    glBegin(GL_QUADS);
      glTexCoord2d(0,1); glVertex2d(-0.5*pnm->width,-0.5*pnm->height);
      glTexCoord2d(1,1); glVertex2d(+0.5*pnm->width,-0.5*pnm->height);
      glTexCoord2d(1,0); glVertex2d(+0.5*pnm->width,+0.5*pnm->height);
      glTexCoord2d(0,0); glVertex2d(-0.5*pnm->width,+0.5*pnm->height);;
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDeleteTextures(1, &tex_id);
  glEndList();
}

// mai 2022
// simple appel à l'objet Gl précompilé (bcp + économique)
// on lui applique les conditions de zoom_value via les transfos OpenGl
// pour recaler la texture (rectangle réel) à la taille voulue
extern void g2x_CallPixmap(G2Xpnm *pnm, bool PIX_GRID)
{
  double dx = g2x_GetZoom()*g2x_GetXPixSize(); // facteur d'échelle en x
  double dy = g2x_GetZoom()*g2x_GetYPixSize(); // facteur d'échelle en y
  glPushMatrix();
    glScalef(dx,dy,1.);
    glCallList(pnm->id);
    if (PIX_GRID && g2x_GetZoom()>4)
    {
      int x,y;
      for (y=-pnm->height/2;y<pnm->height/2;y++)
        g2x_Line(-pnm->width/2,y,+pnm->width/2,y,G2Xwa_a,1);
      for (x=-pnm->width/2;x<pnm->width/2;x++)
        g2x_Line(x,-pnm->height/2,x,pnm->height/2,G2Xwa_a,1);
    }
  glPopMatrix();
}


// oct.2021
extern void g2x_ShowPixels(int width, int height, double zoom_value, double xoff, double yoff)
{
  double x=zoom_value*width/2.;
  double stepx = 2.*x/width;
  double y=zoom_value*height/2.;
  double stepy = 2.*y/height;
  int l,c;
  x*=(1.-xoff);
  y*=(1.-yoff);
  for (c=0;c<width ;c++) g2x_Line(-x+c*stepx,-y        ,-x+c*stepx,+y        ,G2Xwa_a,1);
  for (l=0;l<height;l++) g2x_Line(-x        ,-y+l*stepy,+x        ,-y+l*stepy,G2Xwa_a,1);
}

extern bool g2x_PnmWrite(G2Xpnm* img, const char *filename, char mode, char* comment)
{
  FILE  *imgfile;
  time_t date=time(NULL);
  size_t leng;

  if (img==NULL)
  { fprintf(stderr,"\e[0;31mRien a ecrire sur %s\e[0m\n",filename); return false; }

  if (!(imgfile=fopen(filename,"w")))
  { fprintf(stderr,"\e[1;31mErreur ouverture %s\e[0m\n",filename); return false; }

  fprintf(imgfile,"P%c\n",mode);
  fprintf(imgfile,"#--------------------------------------\n");
  fprintf(imgfile,"# %s - %s",filename,ctime(&date));
  if (strlen(comment)!=0) fprintf(imgfile,"# %s\n",comment);
  fprintf(imgfile,"#--------------------------------------\n");
  fprintf(imgfile,"%d %d\n",img->width,img->height);

  if (mode!='1' && mode!='4') fprintf(imgfile,"%d\n",img->depth-1);
  fflush(imgfile);

  leng=g2x_PnmPushRAW(img,imgfile);
  if (leng<img->layer*img->height*img->width)
  { fprintf(stderr,"\e[0;31mdonnee tronquees : %d octets ecrits sur %d prevus\e[0m\n",(int)leng,(int)(img->layer*img->height*img->width));}

  fflush(imgfile);
  fclose(imgfile);
  return true;
}

#ifdef __cplusplus
  }
#endif

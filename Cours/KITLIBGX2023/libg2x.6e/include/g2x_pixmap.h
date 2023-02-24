/**@file    g2x_pnm.h
 * @author  Universite Gustave Eiffel
 * @author  E.Incerti - eric.incerti@univ-eiffel.fr
 * @brief   Base public control functions
 * @version 6.e
 * @date    Aug.2022 (doc generation)
 */
/**@cond SHOW_SECURITY_DEFINE */

#ifdef __cplusplus
  extern "C" {
#endif

#ifndef _G2X_PIXMAP_
  #define _G2X_PIXMAP_
/**@endcond                   */

  #include <g2x_types.h>

  /* Le type "pixmap" base sur le format PNM [cf. https://fr.wikipedia.org/wiki/Portable_pixmap]*/
  typedef struct
  {
    int     id;           /* un identificateur (texture) */
    char    mode;         /* P[1-6]                      */
    int     width,height; /* largeur, hauteur            */
    int     layer,depth;  /* nbre de plans, profondeur   */
    uchar  *map,*end;     /* debut et fin du pixmap      */
  } G2Xpixmap;

  /* avec _G2X_NO_RELOAD_ : lorsqu'une image est chargee, son nom est stocke dans un  */
  /* tableau, avec le pointeur de pixmap correspondant. Si le meme nom est rappele  */
  /* l'image n'est pas rechargee : on redirige vers le pixmap corrspondant          */
  /* C'est utile surtout en 3D pour les images de texture qui sont utilisees        */
  /* plusieurs fois. --- RISQUE DE BUG A LA LIBERATION ????????                     */
  #define _G2X_RELOAD_        true
  #define _G2X_NO_RELOAD_     false

  /* Libere *pix et le met a NULL */
  bool g2x_PnmFree(G2Xpixmap** pix);

  /* Si *pix=NULL, on alloue, sinon c'est qu'il existe deja, donc on ne fait rien */
  /* Le principe est donc de ne declarer que des G2Xpixmap* initialises a NULL        */
  bool g2x_PnmAlloc(G2Xpixmap** pix, int width, int height, int layer, int depth);

  /* Charge (alloue, si necessaire) *pix a partir d'un fichier image au format PNM       */
  /* Quel que soit le type du fichier original (pbm,pgm,ppm), on creera un pixmap 24bits */
  bool g2x_PnmRead(G2Xpixmap** pix, const char *filename);
  /* Ecrit les donnee pix dans un fichier PNM au format souhaite (pbm,pgm,ppm) */
  bool g2x_PnmWrite(G2Xpixmap*  pix, const char *filename, char mode, char* comment);

  /* Idem, mais a partir d'un format d'image quelconque    */
  /* L'image est transformee en fichier PNM (pnmlib), puis chargee  */
  /* le fichier PNM intermediaire est detruit immediatement*/
  bool g2x_ImageRead(G2Xpixmap** pix, const char *filename, bool RELOAD);

  void g2x_PreloadPixmap(G2Xpixmap *pix);
  void g2x_RecallPixmap(G2Xpixmap *pix, bool PIXGRID);

  void g2x_ShowPixmap(G2Xpixmap *pnm);

  /* renvoie la valeur du "plan"  du pixel ("line",""col") */
  uchar* g2x_GetPixel(G2Xpixmap* pix, int plan, int line, int col);
  /* renvoie les positions réelles y (ligne) et x (colonne)  */
  double g2x_PixLineToY(G2Xpixmap *pix, int line);
  double g2x_PixRowToX (G2Xpixmap *pix, int row );

#endif

#ifdef __cplusplus
  }
#endif

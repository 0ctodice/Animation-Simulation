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

#ifndef _G2X_PNM_
  #define _G2X_PNM_
/**@endcond                   */

  #include <g2x_types.h>

  /* Le type "image" base sur le format PNM */
  typedef struct
  {
    int     id;           /* un identificateur (texture) */
    char    mode;         /* P[1-6]                      */
    int     width,height; /* largeur, hauteur            */
    int     layer,depth;  /* nbre de plans, profondeur   */
    uchar  *map,*end;     /* debut et fin du pixmap      */
  } G2Xpnm;

  /* si l'image est utilisee comme "fond d'ecran" ses tailles doivent etre        */
  /* multiples de 16 sinon ca produit une erreur dans la compil des fichiers MPEG */
  /* avec G2X_MPEG_SETUP, les tailles sont ajustees au chargement (bords coupes)  */
  #define G2X_MPEG_SETUP    true
  #define G2X_NO_MPEG_SETUP false

  /* avec G2X_NO_RELOAD : lorsqu'une image est charchee, son nom est stocke dans un */
  /* tableau, avec le pointeur de pixmap correspondant. Si le meme nom est rappele  */
  /* l'image n'est pas rechargee : on redirige vers le pixmap corrspondant          */
  /* C'est utile surtout en 3D pour les images de texture qui sont utilisees        */
  /* plusieurs fois. --- RISQUE DE BUG A LA LIBERATION ????????                     */
  #define G2X_RELOAD        true
  #define G2X_NO_RELOAD     false

  /* Libere *img pointe et le met a NULL */
  bool g2x_PnmFree(G2Xpnm** img);

  /* Si *img=NULL, on alloue, sinon c'est qu'il existe deja, donc on ne fait rien */
  /* Le principe est donc de ne declarer que des G2Xpnm* initialises a NULL        */
  bool g2x_PnmAlloc(G2Xpnm** img, int width, int height, int layer, int depth);

  /* Charge (eventuellement alloue) *img a partir d'un fichier image au format PNM       */
  /* Quel que soit le type du fichier original (pbm,pgm,ppm), on creera un pixmap 24bits */
  bool g2x_PnmRead(G2Xpnm** img, const char *filename, bool MPEG_SETUP);

  /* Idem, mais a partir d'un format d'image quelconque    */
  /* L'image est transformee en fichier pnm, puis chargee  */
  /* le fichier pnm intermediaire est detruit immediatement*/
  bool g2x_ImageRead(G2Xpnm** img, const char *filename, bool MPEG_SETUP, bool RELOAD);


  void g2x_LoadPixmap(G2Xpnm *pnm);
  void g2x_CallPixmap(G2Xpnm *pnm, bool PIX_GRID);

  /* Ecrit les donnee img dans un fichier pnm au format souhaite (pbm,pgm,ppm) */
  bool g2x_PnmWrite(G2Xpnm*  img, const char *filename, char mode, char* comment);

#endif

#ifdef __cplusplus
  }
#endif

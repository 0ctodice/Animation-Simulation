#!/bin/bash
printf '#---------------------------------------------------------------------------------\n# ' cat >> $HOME/.bashrc
date | cat >> $HOME/.bashrc
echo "# Variables d'environnement pour la lib. graphique <g2x> - Version 6e - 2022
#---------------------------------------------------------------------------------
# teste la config : 32/64 bits
case \`uname -m\` in
    i686) export PROCBIT=32b;;
    x86_64 | ia64) export PROCBIT=64b;;
esac
# ____________gl/gl/glut_____________
export lGL_PATH=/usr/lib/x86_64-linux-gnu
export iGL_PATH=/usr/include
export libGL='-lm -L\$(lGL_PATH) -lGL -lGLU -lglut'
export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:\$lGL_PATH
# ________________g2x________________
export G2X_VERS=6e
export G2X_PATH=$PWD/libg2x.$G2X_VERS
# variables d'environnement pour Makefile avec la bonne version (32b/64b) de <libg2x>
export incG2X='-I\$(iGL_PATH) -I\$(G2X_PATH)/include'
export libG2X='\$(libGL) -L\$(G2X_PATH) -lg2x.\$(G2X_VERS).\$(PROCBIT)'
# chemins pour l'editeur de liens 'ld'
export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:\$G2X_PATH
# ________________g3x________________
export G3X_VERS=6e
export G3X_PATH=$PWD/libg3x.$G3X_VERS
# variables d'environnement pour Makefile avec la bonne version (32b/64b) de <libg3x>
export incG3X='-I\$(iGL_PATH) -I\$(G3X_PATH)/include'
export libG3X='\$(libGL) -L\$(G3X_PATH) -lg3x.\$(G3X_VERS).\$(PROCBIT)'
# chemins pour l'editeur de liens 'ld'
export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:\$G3X_PATH
#---------------------------------------------------------------------------------"| cat >> $HOME/.bashrc;
bash;

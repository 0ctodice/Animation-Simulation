```cpp
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
```

# Animation-Simulation

## Installation & Compilation

### Prérequis :

- Visual Studio Code

### Windows :

- Vous aurez besoin d'installer la librairie graphique Raylib, disponible ici : https://raysan5.itch.io/raylib/purchase
- Vous n'êtes pas obligé de fairte un don, cliquez juste sur "No thanks, just take me to the downloads".
- Suivez les étapes d'installation et ce sera tout bon : 

### Linux :

- Pour installer Raylib veuillez suivre ces étapes :
```shell
sudo apt install build-essential git
sudo apt install cmake
sudo apt install libasound2-dev mesa-common-dev libx11-dev libxrandr-dev libxi-dev xorg-dev libgl1-mesa-dev libglu1-mesa-dev
sudo apt install gdb
git clone https://github.com/raysan5/raylib.git raylib
cd raylib
mkdir build && cd build
cmake -DBUILD_SHARED_LIBS=ON ..
make
sudo make install
```

---

- Une fois installer, télécharger le projet.
- Allez dans une des dossier selon le niveau de dimensions (La simulation du drapeau est le projet 3D).
- Ouvrez le dossier dans visual studio code, en mode workspace.
- Appuyez sur F5 et ammusez-vous !

# Ce qui fonctionne :

Le drapeau est modélisé correctement avec les 3 maillages (Structurel, Torsion et Courbure).
on peut :
 - appliquer de la gravité.
 - orienter le vents dans les 3 axes.
 - appliquer une force de vent commune aux 3 axes.
 - régler le niveau de résistance du drapeau (il peut donc se déchirer).
 - afin d'éviter l'effet "super-élastique", on a ajouter un coefficient réducteur qui vaut 100% au niveau du mât et minimum 10% à l'extrémité du drapeau.
 - On peut réinitialiser la simulation à tout moment.

# Les pistes que je voulais explorer :

- Ajouter la gestion des collisions avec des objets.
- Ajouter un visuel pour le drapeau (texture, couleur, etc...)

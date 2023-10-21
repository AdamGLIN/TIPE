
#include "structures.h"

#ifndef IMAGE

    #define IMAGE

    #define PI 3.141592653589793238

    matrice* noir_blanc(image* img); // O(hl) ~ O(n²)

    filtre* gauss(uint8_t taille, double sigma); // O(t²) ~ O(1)

    double filtrer(filtre* f, double** t, int i, int j); // O(t²) ~ O(1)

    matrice* applique_filtre(filtre* f, matrice* m); // O(hl) ~ O(n²)

    image* moyenne_images(char* dossier, int k); // O(khl) ~ O(n²)

    image* rognage(image* img, int i1, int j1, int i2, int j2); // O(hl) ~ O(n²)

#endif

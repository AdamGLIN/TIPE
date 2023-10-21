
#include <stdbool.h>

#include "structures.h"

#ifndef DETECTION

    #define DETECTION

    #define RADIANS 0.017453292519943295

    typedef struct parcours
    {
        int x;
        int y;
        int angle;
    } parcours;

    filtre* gradient_x(); // O(1)

    filtre* gradient_y(); // O(1)

    gradient* calcule_gradient(matrice* m); // O(hl) ~ O(n²)

    int arrondi(double x); // O(1)

    bool max_direction(gradient* g, int i, int j); // O(1)

    matrice* sup_non_maxima(gradient* g, double* moyenne); // O(hl) ~ O(n²)

    void seuils(matrice* m, double l_haute, double l_basse); // O(hl) ~ O(n²)

    bool voisin(matrice* m, int i, int j); // O(1)

    void propage_contour(matrice* m, int i, int j); // O(hl) ~ O(n²) au pire

    void hysteresis(matrice* m); // O(hl) ~ O(n²)

    matrice* canny(image* img); // O(hl) ~ O(n²)

    bool remplir(matrice* m, int i, int j); // O(hl) ~ O(n²)

    bool deplace_curseur(matrice* m, parcours* p, int i, int j); // O(hl) ~ O(n²)

    int trace_contour(matrice* m, int i, int j); // O(hl) ~ O(n²)

    int compte(matrice* m); // O(hl) ~ O(n²)

#endif

#include <stdint.h>

#ifndef STRUCTURES

    #define STRUCTURES

    typedef struct image
    {
        uint32_t hauteur;
        uint32_t largeur;
        uint8_t** couleurs[3]; // RGB
    } image;

    typedef struct matrice
    {
        uint32_t hauteur;
        uint32_t largeur; 
        double** valeurs;
    } matrice;

    typedef struct filtre
    {
        uint32_t taille;
        double** noyau;
    } filtre;

    typedef struct gradient
    {
        uint32_t hauteur;
        uint32_t largeur;
        double** amplitudes;
        double** directions;
    } gradient;

    image* allouer_image(uint32_t largeur, uint32_t hauteur); // O(h) ~ O(n)

    void liberer_image(image* img); // O(h) ~ O(n)

    matrice* allouer_matrice(uint32_t largeur, uint32_t hauteur); // O(h) ~ O(n)

    void liberer_matrice(matrice* m); // O(h) ~ O(n)

    matrice* convertir_image_matrice(image* img, uint8_t couleur); // O(hl) ~ O(n²)

    image* convertir_matrice_image(matrice* m); // O(hl) ~ O(n²)

    filtre* allouer_filtre(uint8_t taille); // O(t) ~ O(1)

    void liberer_filtre(filtre* f); // O(t) ~ O(1)

    gradient* allouer_gradient(uint32_t largeur, uint32_t hauteur); // O(hl) ~ O(n²)
    
    void liberer_gradient(gradient* g); // O(h) ~ O(n)

#endif

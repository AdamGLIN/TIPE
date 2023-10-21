
#include <stdlib.h>
#include <math.h>

#include "structures.h"

image* allouer_image(uint32_t largeur, uint32_t hauteur)
{
    // allouer de la mémoire à l'image
    image* img = (image*) malloc(sizeof(image));
    // remplir les champs largeur et hauteur
    img -> largeur = largeur;
    img -> hauteur = hauteur;
    // allouer de la mémoire à chaque couleurs
    img -> couleurs[0] = (uint8_t**) malloc(hauteur * sizeof(uint8_t*));
    img -> couleurs[1] = (uint8_t**) malloc(hauteur * sizeof(uint8_t*));
    img -> couleurs[2] = (uint8_t**) malloc(hauteur * sizeof(uint8_t*));
    for (int i = 0; i < hauteur; i++)
    {
        // allouer de la mémoire à chaque ligne
        img -> couleurs[0][i] = (uint8_t*) malloc(largeur * sizeof(uint8_t));
        img -> couleurs[1][i] = (uint8_t*) malloc(largeur * sizeof(uint8_t));
        img -> couleurs[2][i] = (uint8_t*) malloc(largeur * sizeof(uint8_t));
    }
    // renvoyer l'image
    return img;
}

void liberer_image(image* img)
{
    for (int i = 0; i < img -> hauteur; i++)
    {
        // libérer chaque ligne
        free(img -> couleurs[0][i]);
        free(img -> couleurs[1][i]);
        free(img -> couleurs[2][i]);
    }
    // libérer l'image
    free(img -> couleurs[0]);
    free(img -> couleurs[1]);
    free(img -> couleurs[2]);
    free(img);
}

matrice* allouer_matrice(uint32_t largeur, uint32_t hauteur)
{
    // allouer de la mémoire à la structure
    matrice* m = (matrice*) malloc(sizeof(matrice));
    // remplir les champs largeur et hauteur
    m -> largeur = largeur;
    m -> hauteur = hauteur;
    // allouer de la mémoire à la matrice
    m -> valeurs = (double**) malloc(hauteur * sizeof(double*));
    // allouer de la mémoire à chaque ligne
    for (int i = 0; i < hauteur; i++) m -> valeurs[i] = (double*) malloc(largeur * sizeof(double));
    // renvoyer la matrice
    return m;
}

void liberer_matrice(matrice* m)
{
    // libérer chaque ligne
    for (int i = 0; i < m -> hauteur; i++) free(m -> valeurs[i]);
    // libérer la matrice
    free(m -> valeurs);
    free(m);
}

matrice* convertir_image_matrice(image* img, uint8_t couleur)
{
    // allouer de la mémoire à la matrice
    matrice* m = allouer_matrice(img -> largeur, img -> hauteur);
    // parcourir la matrice
    for (int i = 0; i < m -> hauteur; i++)
    {
        for (int j = 0; j < m -> largeur; j++)
        {
            // remplir les valeurs
            m -> valeurs[i][j] = img -> couleurs[couleur][i][j];
        }
    }
    // renvoyer la matrice
    return m;
}

image* convertir_matrice_image(matrice* m)
{
    // allouer de la mémoire à l'image
    image* img = allouer_image(m -> largeur, m -> hauteur);
    // parcourir l'image
    for (int i = 0; i < m -> hauteur; i++)
    {
        for (int j = 0; j < m -> largeur; j++)
        {
            // remplir les valeurs
            img -> couleurs[0][i][j] = round(abs(m -> valeurs[i][j]));
            img -> couleurs[1][i][j] = round(abs(m -> valeurs[i][j]));
            img -> couleurs[2][i][j] = round(abs(m -> valeurs[i][j]));
        }
    }
    // renvoyer l'image
    return img;
}

filtre* allouer_filtre(uint8_t taille)
{
    // allouer de la mémoire au filtre
    filtre* f = (filtre*) malloc(sizeof(filtre));
    // remplir le champ taille
    f -> taille = taille;
    // allouer de la mémoire au noyau
    f -> noyau = (double**) malloc(taille * sizeof(double*));
    // allouer de la mémoire à chaque ligne
    for (int i = 0; i < taille; i++) f -> noyau[i] = (double*) malloc(taille * sizeof(double));
    // renvoyer le filtre
    return f;
}

void liberer_filtre(filtre* f)
{  
    // libérer chaque ligne
    for (int i = 0; i < f -> taille; i++) free(f -> noyau[i]);
    // libérer le filtre
    free(f -> noyau);
    free(f);
}

gradient* allouer_gradient(uint32_t largeur, uint32_t hauteur)
{
    // allouer de la mémoire au gradient
    gradient* g = (gradient*) malloc(sizeof(gradient));
    // remplir les champs largeur et hauteur
    g -> largeur = largeur;
    g -> hauteur = hauteur;
    // allouer de la mémoire aux différents tableaux
    g -> amplitudes = (double**) malloc(hauteur * sizeof(double*));
    g -> directions = (double**) malloc(hauteur * sizeof(double*));
    // allouer de la mémoire à chaque lignes
    for (int i = 0; i < hauteur; i++)
    {
        g -> amplitudes[i] = (double*) malloc(largeur * sizeof(double));
        g -> directions[i] = (double*) malloc(largeur * sizeof(double));
    } 
    // renvoyer le gradient
    return g;
}

void liberer_gradient(gradient* g)
{
    // libérer chaque ligne
    for (int i = 0; i < g -> hauteur; i++)
    {
        free(g -> amplitudes[i]);
        free(g -> directions[i]);
    }
    // libérer la matrice
    free(g -> amplitudes);
    free(g -> directions);
    free(g);    
}

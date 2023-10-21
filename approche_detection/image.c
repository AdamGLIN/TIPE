
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "image.h"
#include "fichiers.h"

matrice* noir_blanc(image* img)
{
    // allouer de la mémoire à la matrice
    matrice* m = allouer_matrice(img -> largeur, img -> hauteur);
    // parcourir l'image
    for (int i = 0 ; i < img -> hauteur; i++)
    {
        for (int j = 0; j < img -> largeur; j++)
        {
            // appliquer la formule de conversion
            m -> valeurs[i][j] = (img -> couleurs[0][i][j] + img -> couleurs[1][i][j] + img -> couleurs[2][i][j]) / 3.;
        }
    }
    // renvoyer la matrice
    return m;
}

filtre* gauss(uint8_t taille, double sigma)
{
    // allouer de la mémoire au filtre
    filtre* f = allouer_filtre(taille);
    // définir le décalage et la somme
    uint8_t dec = f -> taille / 2;
    double tot = 0.;
    // parcourir le filtre
    for (int i = 0; i < f -> taille; i++)
    {
        for (int j = 0; j < f -> taille; j++)
        {
            // calculer les valeurs de chaque case et l'intégrale
            f -> noyau[i][j] = exp(-((i - dec) * (i - dec) + (j - dec) * (j - dec)) / (2 * sigma * sigma)) / (2 * PI * sigma * sigma);
            tot += f -> noyau[i][j];
        }
    }
    // normaliser pour avoir un filtre de somme 1
    for (int i = 0; i < f -> taille; i++)
    {
        for (int j = 0; j < f -> taille; j++)
        {
            f -> noyau[i][j] /= tot;
        }
    }
    // renvoyer le filtre
    return f;
}

double filtrer(filtre* f, double** t, int i, int j)
{
    // initialiser le résultat
    double total = 0;
    // définir le décalage
    int decalage = f -> taille / 2;
    // parcourir la fenêtre du filtre
    for (int k = 0; k < f -> taille; k++)
    {
        for (int l = 0; l < f -> taille; l++)
        {
            // calculer pixel par pixel
            total += t[i + k - decalage][j + l - decalage] * f -> noyau[k][l];
        }
    }
    // renvoyer le résultat
    return total;
}

matrice* applique_filtre(filtre* f, matrice* m)
{
    // définir le décalage
    int decalage = f -> taille / 2;
    // allouer de la mémoire à la matrice
    matrice* m_new = allouer_matrice(m -> largeur - 2 * decalage, m -> hauteur - 2 * decalage);
    // parcourir la matrice
    for (int i = decalage; i < m -> hauteur - decalage; i++)
    {
        for (int j = decalage; j < m -> largeur - decalage; j++)
        {
            // appliquer le filtre sur chaque pixel
            m_new -> valeurs[i - decalage][j - decalage] = filtrer(f, m -> valeurs, i, j);
        }
    }
    // renvoyer la matrice
    return m_new;
}

image* moyenne_images(char* dossier, int k)
{
    // definir la variable qui va représenter le nom des images
    char fichier[200];
    sprintf(fichier, "%s0.bmp", dossier);
    // initialiser l'image moyenne
    image* m = importer_bitmap(fichier);
    for (int i = 0; i < m -> hauteur; i++)
    {
        for (int j = 0; j < m -> largeur; j++)
        {
            m -> couleurs[0][i][j] /= (double) k;
            m -> couleurs[1][i][j] /= (double) k;
            m -> couleurs[2][i][j] /= (double) k;
        }
    }
    // parcourir chaque image et rajouter les valeurs de chaque pixel
    for (int p = 1; p < k; p++)
    {
        sprintf(fichier, "%s%d.bmp", dossier, p);
        image* img = importer_bitmap(fichier);
        for (int i = 0; i < m -> hauteur; i++)
        {
            for (int j = 0; j < m -> largeur; j++)
            {
                m -> couleurs[0][i][j] += img -> couleurs[0][i][j] / (double) k;
                m -> couleurs[1][i][j] += img -> couleurs[1][i][j] / (double) k;
                m -> couleurs[2][i][j] += img -> couleurs[2][i][j] / (double) k;
            }
        }
        free(img);
    }
    // renvoyer l'image moyenne
    return m;
}

image* rognage(image* img, int i1, int j1, int i2, int j2)
{
    // allouer de la mémoire à la nouvelle image
    image* img_rognee = allouer_image(j2 - j1, i2 - i1);
    // recopier les pixels
    for (int i = 0; i < img_rognee -> hauteur; i++)
    {
        for (int j = 0; j < img_rognee -> largeur; j++)
        {
            img_rognee -> couleurs[0][i][j] = img -> couleurs[0][i1 + i][j1 + j];
            img_rognee -> couleurs[1][i][j] = img -> couleurs[1][i1 + i][j1 + j];
            img_rognee -> couleurs[2][i][j] = img -> couleurs[2][i1 + i][j1 + j];
        }
    }
    // renvoyer la nouvelle image
    return img_rognee;
}

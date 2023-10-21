
#include <stdio.h>
#include <stdlib.h>

#include "fichiers.h"

image* importer_bitmap(char* fichier)
{
    // ouvrir le fichier à lire
    FILE* f = fopen(fichier, "rb");
    // récupérer les dimensions de l'image et l'offset
    uint8_t dim[8];
    uint8_t offset[4];
    fseek(f, 10, SEEK_SET);
    fread(&offset, sizeof(offset), 1, f);
    fseek(f, 18, SEEK_SET);
    fread(&dim, sizeof(dim), 1, f);
    // définir la taille de l'entête, la largeur et la hauteur de l'image
    uint32_t taille_entete = (offset[3] << 24) + (offset[2] << 16) + (offset[1] << 8) + offset[0];
    uint32_t largeur = (dim[3] << 24) + (dim[2] << 16) + (dim[1] << 8) + dim[0];
    uint32_t hauteur = (dim[7] << 24) + (dim[6] << 16) + (dim[5] << 8) + dim[4];
    // allouer de la mémoire à l'image
    image* img = allouer_image(largeur, hauteur);
    // définir le décalage (une image bitmap doit avoir une largeur multiple de 4)
    uint8_t decalage = (4 - (largeur) % 4) % 4;
    // parcourir le bitmap de bas en haut à partir de la fin d'entete
    fseek(f, taille_entete, SEEK_SET);
    for (int y = hauteur - 1; y >= 0; y--)
    {
        // parcourir de gauche à droite
        for (int x = 0; x < largeur; x++)
        {
            // récupérer les valeurs pour chaque pixel
            img -> couleurs[2][y][x] = fgetc(f);
            img -> couleurs[1][y][x] = fgetc(f);
            img -> couleurs[0][y][x] = fgetc(f);
        }
        // décaler à la prochaine ligne de l'image
        fseek(f, decalage, SEEK_CUR);
    }
    // fermer le fichier
    fclose(f);
    // renvoyer la structure
    return img;
}

void remplir_petit_boutisme(uint8_t* entete, uint32_t valeur, int offset, int nb_octet)
{
    // remplir le bon nombre d'octet
    for (int i = 0; i < nb_octet; i++)
    {
        // appliquer la règle du petit boutisme pour décomposer la valeur en octets
        entete[i + offset] = (valeur << ((3 - i) * 8)) >> 24;
    }
}

uint8_t* creer_entete(image* img)
{
    // allouer de la mémoire à l'entete
    uint8_t* entete = (uint8_t*) malloc(TAILLE_ENTETE * sizeof(uint8_t));
    // format du fichier ("BM" = 42 4D = 19778)
    remplir_petit_boutisme(entete, 19778, 0, 2);
    // taille du fichier
    uint8_t decalage = (4 - (img -> largeur) % 4) % 4;
    remplir_petit_boutisme(entete, TAILLE_ENTETE + 3 * img -> hauteur * (img -> largeur + decalage), 2, 4);
    // champ reservé mis à zéro
    remplir_petit_boutisme(entete, 0, 6, 4);
    // offset
    remplir_petit_boutisme(entete, TAILLE_ENTETE, 10, 4);
    // taille de cette partie de l'entete
    remplir_petit_boutisme(entete, 40, 14, 4);
    // largeur de l'image
    remplir_petit_boutisme(entete, img -> largeur, 18, 4);
    // hauteur de l'image
    remplir_petit_boutisme(entete, img -> hauteur, 22, 4);
    // champ de couleur
    remplir_petit_boutisme(entete, 1, 26, 2);
    // bit par pixel
    remplir_petit_boutisme(entete, 24, 28, 2);
    // compression utilisée
    remplir_petit_boutisme(entete, 0, 30, 4);
    // taille de l'image
    remplir_petit_boutisme(entete, 3 * img -> hauteur * (img -> largeur), 34, 4);
    // mettre la résolution et les couleurs à 0
    remplir_petit_boutisme(entete, 0, 38, 16);
    // renvoyer l'entete
    return entete;
}

void exporter_bitmap(image* img, char* fichier)
{
    // ouvrir le fichier pour écrire
    FILE* f = fopen(fichier, "wb");
    // créer et recopier l'entete
    uint8_t* entete = creer_entete(img);
    fwrite(entete, TAILLE_ENTETE, 1, f);
    free(entete);
    // définir le décalage
    uint8_t decalage = (4 - (img -> largeur) % 4) % 4;
    // parcourir de bas en haut
    for (int i = img -> hauteur - 1; i >= 0; i--)
    {
        // parcourir de gauche à droite
        for (int j = 0; j < img -> largeur; j++)
        {
            // recopier les valeurs des pixels
            fputc(img -> couleurs[2][i][j], f);
            fputc(img -> couleurs[1][i][j], f);
            fputc(img -> couleurs[0][i][j], f);
        }
        // décaler pour recopier la ligne suivante
        for (int k = 0; k < decalage; k++) fputc(0, f);
    }
    // fermer le fichier
    fclose(f);
}

void exporter_csv_matrice(matrice* m, char* fichier)
{
    // ouvrir le fichier
    FILE* f = fopen(fichier, "w");
    // parcourir la matrice
    for (int i = 0; i < m -> hauteur; i++)
    {
        for (int j = 0; j < m -> largeur; j++)
        {
            // recopier les valeurs de chaque ligne en les séparant par une virgule
            fprintf(f, "%lf,", m -> valeurs[i][j]);
        }
        // revenir à la ligne à la fin de chaque ligne
        fprintf(f, "\n");
    }
    // fermer le fichier
    fclose(f);
}

void exporter_csv_gradient(gradient* g, char* fichier)
{
    // ouvrir le fichier
    FILE* f = fopen(fichier, "w");
    // parcourir la matrice
    for (int i = 0; i < g -> hauteur; i++)
    {
        for (int j = 0; j < g -> largeur; j++)
        {
            // recopier les valeurs de chaque ligne en les séparant par une virgule
            fprintf(f, "%lf,", g -> amplitudes[i][j]);
            fprintf(f, "%lf,", g -> directions[i][j]);
        }
        // revenir à la ligne à la fin de chaque ligne
        fprintf(f, "\n");
    }
    // fermer le fichier
    fclose(f);
}


#include "structures.h"

#ifndef FICHIER

    #define FICHIER

    #define TAILLE_ENTETE 54

    image* importer_bitmap(char* fichier); // O(hl) ~ O(n²)

    void remplir_petit_boutisme(uint8_t* entete, uint32_t valeur, int offset, int nb_octet); // O(1)

    uint8_t* creer_entete(image* img); // O(1)

    void exporter_bitmap(image* b, char* fichier); // O(hl) ~ O(n²)

    void exporter_csv_matrice(matrice* m, char* fichier); // O(hl) ~ O(n²)

    void exporter_csv_gradient(gradient* g, char* fichier); // O(hl) ~O(n²)

#endif

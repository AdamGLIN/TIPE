
#include <stdio.h>

#include "image.h"
#include "fichiers.h"
#include "detection.h"

int main(int argc, char* argv[])
{
    if (argc > 2)
    {
        // importation de l'image
        image* img = importer_bitmap(argv[1]);
        // extraction des contours
        matrice* m = canny(img);
        // comptage des formes
        printf("%d\n", compte(m));
        // exportation de l'image traitée
        image* img_fin = convertir_matrice_image(m);
        exporter_bitmap(img_fin, argv[2]);

        return 1;
    }
    else return 0;
}

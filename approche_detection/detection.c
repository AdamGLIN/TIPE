
#include <stdlib.h>
#include <math.h>

#include "image.h"
#include "detection.h"

filtre* gradient_x()
{
    // allouer de la mémoire au filtre
    filtre* f = allouer_filtre(3);
    // remplir les cases utiles
    f -> noyau[0][0] = -1;
    f -> noyau[1][0] = -2;
    f -> noyau[2][0] = -1;
    f -> noyau[0][1] = 0;
    f -> noyau[1][1] = 0;
    f -> noyau[2][1] = 0;
    f -> noyau[0][2] = 1;
    f -> noyau[1][2] = 2;
    f -> noyau[2][2] = 1;
    // renvoyer le filtre
    return f;
}

filtre* gradient_y()
{
    // allouer de la mémoire au filtre
    filtre* f = allouer_filtre(3);
    // remplir les cases utiles
    f -> noyau[0][0] = -1;
    f -> noyau[0][1] = -2;
    f -> noyau[0][2] = -1;
    f -> noyau[1][0] = 0;
    f -> noyau[1][1] = 0;
    f -> noyau[1][2] = 0;
    f -> noyau[2][0] = 1;
    f -> noyau[2][2] = 2;
    f -> noyau[2][1] = 1;
    // renvoyer le filtre
    return f;
}

gradient* calcule_gradient(matrice* m)
{
    // allouer de la mémoire à la nouvelle matrice et aux filtres
    filtre* gx = gradient_x();
    filtre* gy = gradient_y();
    // on rogne les bords
    gradient* g = allouer_gradient(m -> largeur - 2, m -> hauteur - 2);
    // calculer les composantes du gradient
    matrice* m_gx = applique_filtre(gx, m);
    matrice* m_gy = applique_filtre(gy, m);
    for (int i = 0; i < m -> hauteur - 2; i++)
    {
        for (int j = 0; j < m -> largeur - 2; j++)
        {
            // calculer l'amplitude et la direction
            g -> amplitudes[i][j] = sqrt(m_gx -> valeurs[i][j] * m_gx -> valeurs[i][j] + m_gy -> valeurs[i][j] * m_gy -> valeurs[i][j]);
            g -> directions[i][j] = atan2(m_gy -> valeurs[i][j], m_gx -> valeurs[i][j]);
        }
    }
    // libérer les composantes et filtres
    liberer_filtre(gx);
    liberer_filtre(gy);
    liberer_matrice(m_gx);
    liberer_matrice(m_gy);
    // renvoyer le gradient
    return g;
}

int arrondi(double x)
{
    // renvoyer l'entier le plus proche à chaque fois
    if (x >= 0) return round(x);
    else return -round(-x);
}

bool max_direction(gradient* g, int i, int j)
{
    // définir le décalage avec la direction du gradient
    int di = arrondi(sin(g -> directions[i][j]));
    int dj = arrondi(cos(g -> directions[i][j]));
    // vérifier que le gradient est maximal suivant cette direction
    return g -> amplitudes[i][j] > g -> amplitudes[i + di][j + dj] && g -> amplitudes[i][j] > g -> amplitudes[i - di][j - dj];;
}

matrice* sup_non_maxima(gradient* g, double* moyenne)
{
    // allouer la mémoire à la nouvelle matrice
    matrice* m = allouer_matrice(g -> largeur - 2, g -> hauteur - 2);
    *moyenne = 0;
    int k = 0;
    // parcourir la matrice
    for (int i = 0; i < m -> hauteur; i++)
    {
        for (int j = 0; j < m -> largeur; j++)
        {
            // si c'est un maximum local on garde
            if (max_direction(g, i + 1, j + 1))
            {
                m -> valeurs[i][j] = g -> amplitudes[i + 1][j + 1];
                *moyenne += g -> amplitudes[i + 1][j + 1] / (g -> hauteur * g -> largeur);
                k++;
            } 
            // sinon on efface
            else m -> valeurs[i][j] = 0.;
        }
    }
    *moyenne = *moyenne * ((g -> hauteur * g -> largeur) / k);
    // renvoyer la matrice des maxima
    return m;
}

void seuils(matrice* m, double l_haute, double l_basse)
{
    // parcourir la matrice
    for (int i = 0; i < m -> hauteur; i++)
    {
        for (int j = 0; j < m -> largeur; j++)
        {
            // distinguer les cas suivant les seuils
            if (m -> valeurs[i][j] > l_haute) m -> valeurs[i][j] = 255;
            else if (m -> valeurs[i][j] < l_basse) m -> valeurs[i][j] = 0;
            else m -> valeurs[i][j] = 128;
        }
    }
}

bool voisin(matrice* m, int i, int j)
{
    // parcourir les voisins du pixel
    for (int k = i - 1; k <= i + 1; k++)
    {
        for (int l = j - 1; l <= j + 1; l++)
        {
            // vérifier si il existe un pixel en blanc autour
            if (m -> valeurs[k][l] == 255) return true;
        }
    }
    // renvoyer faux sinon
    return false;
}

void propage_contour(matrice* m, int i, int j)
{
    // mettre le pixel comme contour
    m -> valeurs[i][j] = 255;
    // si on est sur le bord de l'image, on ne fait plus rien
    if (i == m -> hauteur - 1 || i == 0 || j == m -> largeur - 1 || j == 0) return;
    // sinon parcourir les voisins du pixel
    for (int k = i - 1; k <= i + 1; k++)
    {
        for (int l = j - 1; l <= j + 1; l++)
        {
            // si le pixel voisin est gris, propager à nouveau le contour
            if (m -> valeurs[k][l] == 128)
            {
                propage_contour(m, k, l);
            }
        }
    }
}

void hysteresis(matrice* m)
{
    // parcourir la matrice
    for (int i = 1; i < m -> hauteur - 1; i++)
    {
        for (int j = 1; j < m -> largeur - 1; j++)
        {
            // si le pixel est gris et voisin d'un vrai contour, lancer la propagation, sinon le rejeter
            if (m -> valeurs[i][j] == 128)
            {
                if (voisin(m, i, j)) propage_contour(m, i, j);
                else m -> valeurs[i][j] = 0;
            }
        }
    }
}

matrice* canny(image* img)
{
    // convertir l'image en noir et blanc
    matrice* nb = noir_blanc(img);
    // appliquer un flou gaussien sur l'image
    filtre* f = gauss(7, 2);
    matrice* m_lissee = applique_filtre(f, nb);
    // calculer le gradient
    gradient* g = calcule_gradient(m_lissee);
    // garder uniquement les vrais contours
    double moyenne = 0;
    matrice* m_max = sup_non_maxima(g, &moyenne);
    seuils(m_max, 3 * moyenne / 2, 2 * moyenne / 3);
    hysteresis(m_max);
    // libérer les matrices, filtre et gradient inutiles
    liberer_matrice(nb);
    liberer_matrice(m_lissee);
    liberer_gradient(g);
    liberer_filtre(f);
    // renvoyer la matrice transformée
    return m_max;
}

bool remplir(matrice* m, int i, int j)
{
    // colorier la case en gardant sa valeur
    double v = m -> valeurs[i][j];
    m -> valeurs[i][j] = 64;
    // parcourir les voisins verticaux et horizontaux
    for (int alpha = 0; alpha <= 270; alpha += 90)
    {
        int n = i - arrondi(sin(alpha * RADIANS));
        int k = j + arrondi(cos(alpha * RADIANS));
        // si on est sur le bord de l'image, on le signal et on remet sa valeur au pixel
        if (n >= m -> hauteur || n < 0 || k >= m -> largeur || k < 0)
        {
            m -> valeurs[i][j] = v;
            return false;
        }
        // sinon on est pas sur un bord donc on continue de remplir récursivement
        else if (m -> valeurs[n][k] == 255 || m -> valeurs[n][k] == 0)
        {
            // mais si une erreur est relevée, on annule
            if (!remplir(m, n, k))
            {
                m -> valeurs[i][j] = v;
                return false;
            }
        }
    }
    // on confirme avoir fini de remplir
    return true;
}

bool deplace_curseur(matrice* m, parcours* p, int i, int j)
{
    // définir la variable tour qui va permettre de ne pas boucler sur un même point
    int tour = 0;
    while (tour < 270)
    {
        // on calcule où pointe le curseur et où est l'intérieur
        int n_x = p -> x + arrondi(cos(p -> angle * RADIANS));
        int n_y = p -> y - arrondi(sin(p -> angle * RADIANS));
        int r_x = p -> x - arrondi(sin(p -> angle * RADIANS));
        int r_y = p -> y - arrondi(cos(p -> angle * RADIANS));
        // si le curseur pointe sur un contour, on le déplace et on colorie
        if (m -> valeurs[n_y][n_x] == 255)
        {
            p -> x = n_x;
            p -> y = n_y;
            p -> angle = (p -> angle + 270) % 360;
            m -> valeurs[n_y][n_x] = 128;
            // si on a tracé toute la forme, on s'arrête et on valide la forme
            if (n_y == i && n_x == j) return true;
            // sinon on continue récursivement
            else if (deplace_curseur(m, p, i, j))
            {
                // on regarde à l'intérieur si on peut remplir, et on remplit dans le cas échéant
                if (m -> valeurs[r_y][r_x] != 128 && m -> valeurs[r_y][r_x] != 64)
                {   
                    remplir(m, r_y, r_x);
                }
                return true;
            }
            // si la forme n'est pas fermée, on efface
            else
            {
                m -> valeurs[n_y][n_x] = 0;
                return false;
            }
        }
        // sinon on le tourne de 45°
        else
        {
            p -> angle = (p -> angle + 45) % 360;
            tour += 45;
        }
    }
    // si on a bouclé, on ne compte pas
    return false;
}

int trace_contour(matrice* m, int i, int j)
{
    // allouer de la mémoire et initialiser le curseur
    parcours* p = (parcours*) malloc(sizeof(parcours));
    p -> y = i;
    p -> x = j;
    p -> angle = 180;
    // lancer le traçage, libérer le curseur et renvoyer le résultat
    if (deplace_curseur(m, p, i, j))
    {
        free(p);
        return 1;
    }
    else
    {
        free(p);
        return 0;
    }
}

int compte(matrice* m)
{
    // définir n le nombre de formes
    int n = 0;
    // parcourir la matrice de haut en bas, de gauche à droite
    for (int i = 0; i < m -> hauteur; i++)
    {
        for (int j = 0; j < m -> largeur; j++)
        {
            // si on voit un contour, on trace la forme
            if (m -> valeurs[i][j] == 255)
            {
                n += trace_contour(m, i, j);
            }
        }
    }
    // renvoyer le nombre de formes
    return n;
}

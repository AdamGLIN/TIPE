
import numpy as np
import csv
import cv2

# définir l'objet modele
class Modele :

    def __init__(self, donnees, k, dist) :
        self.donnees = donnees
        self.k = k
        self.dist = dist

    def estimation(self, img) :
        return k_nn(self, img)

def importer_images(dossier) :
    # définir les listes des images et des valeurs associées
    images = []
    valeurs = []
    # ouvrir le fichier csv qui détient les informations des images
    with open(dossier + "image_labels.txt", "rt") as img_info :
        info = csv.reader(img_info, delimiter=',')
        for ligne in info :
            # importer chaque image et la valeur associée
            img = cv2.imread(dossier + "images_réduites/" +  str(ligne[0]) + ".jpg")
            images.append(img)
            valeurs.append(float(ligne[1]))
    # renvoyer les listes
    return images, valeurs

def extraire_caracteristiques(img, seuil_cote=25, seuil_couleur=15) :
    # récupérer la taille de l'image
    hauteur, largeur, _ = np.shape(img)
    # définir le vecteur associé à l'image
    vect = [0, 0, 0, 0]
    # mesure de la densité à partir des contours
    img_cotes = cv2.Canny(cv2.cvtColor(img, cv2.COLOR_BGR2GRAY), 150, 350, L2gradient=True)
    # on divise l'image en 256 fenêtres et on les parcourt
    for i in range(0, hauteur, hauteur // 16) :
        for j in range(0, largeur, largeur // 16) :
            # pour chaque fenêtre, on calcule l'intensité moyenne (bord ou fond)
            moyenne_cote = np.mean(img_cotes[i : i + hauteur // 16, j : j + largeur // 16])
            # si cette valeur est supérieure au seuil, on augmente la première coordonnées
            if moyenne_cote > seuil_cote : 
                vect[0] += moyenne_cote - seuil_cote
            # sinon la deuxième
            else :
                vect[1] += seuil_cote - moyenne_cote
    # mesure de la densité à partir des couleurs
    # on divise l'image en 64 fenêtres et on les parcourt
    for i in range(0, hauteur, hauteur // 8) :
        for j in range(0, largeur, largeur // 8) :
            # on calcule les écarts-type de chacune des distributions de couleurs et on en fait la moyenne
            b = np.std(img[i : i + hauteur // 8, j : j + largeur // 8, 0].flatten())
            g = np.std(img[i : i + hauteur // 8, j : j + largeur // 8, 1].flatten())
            r = np.std(img[i : i + hauteur // 8, j : j + largeur // 8, 2].flatten())
            uniformite_couleur = (r + g + b) / 3
            # si cette valeur est supérieure au seuil, on augmente la troisième coordonnées
            if uniformite_couleur > seuil_couleur :
                vect[2] += uniformite_couleur - seuil_couleur
            # sinon la quatrième
            else :
                vect[3] += seuil_couleur - uniformite_couleur
    # renvoyer le vecteur associé à l'image
    return vect

def entrainer(k, dist, images, valeurs, seuil_cote=25, seuil_couleur=15) :
    # définir la liste qui va contenir les caractéristiques des images
    donnees = []
    # parcourir les images
    for i in range(len(images)) :
        # construire le vecteur associé à l'image et renseigner en dernière coordonnées le compte de personne associé
        vecteur = extraire_caracteristiques(images[i], seuil_cote, seuil_couleur)
        vecteur.append(valeurs[i])
        donnees.append(vecteur)
    # renvoyer le modele associé aux données, k et distance choisies
    return Modele(np.array(donnees), k, dist)

def estimer(modele, images) :
    # définir la liste des estimations des nouvelles images
    estimations = []
    # parcourir les nouvelles images
    for i in range(len(images)) :
        # récupérer l'estimation du modele pour chaque image
        estimation_img = modele.estimation(images[i])
        estimations.append(estimation_img)
    return estimations

def dist_euclidienne(vect1, vect2) :
    # définir la variable qui représente la distance
    d = 0
    # appliquer la formule de la distance euclidienne
    for i in range(len(vect1)) :
        d += (vect1[i] - vect2[i]) ** 2
    return np.sqrt(d)

def k_nn(modele, img) :
    # récupérer le vecteur associé à l'image à estimer
    n_point = extraire_caracteristiques(img)
    # calculer les distances à chaque points du modele
    distances = np.array([modele.dist(n_point, point[:-1]) for point in modele.donnees])
    # récupérer les k plus proches voisins
    plus_proches_voisins = []
    max = np.max(distances)
    for _ in range(modele.k) :
        i = np.argmin(distances)
        d = np.min(distances)
        distances[i] = max
        plus_proches_voisins.append([modele.donnees[i][-1], d])
    # faire la moyenne pondérée des valeurs des voisins pour estimer la valeur de l'image
    n_valeur = 0
    coeffs = 0
    for voisins in plus_proches_voisins :
        n_valeur += voisins[0] / voisins[1]
        coeffs += 1 / voisins[1]
    # renvoyer l'estimation
    return n_valeur / coeffs

def mae(estimations, valeurs) :
    # définir les variables utiles
    n = len(estimations)
    total = 0
    # appliquer la formule de la MAE
    for k in range(n) :
        total += np.abs(estimations[k] - valeurs[k])
    return total / n

def rmse(estimations, valeurs) :
    # définir les variables utiles
    n = len(estimations)
    total = 0
    # appliquer la formule de la RMSE
    for k in range(n) :
        total += (estimations[k] - valeurs[k]) ** 2
    return np.sqrt(total / n)

# construction du modele
print("Importation des images d'entraînement...")
images_entrainement, valeurs_entrainement = importer_images("train/")
print("Construction du modèle...")
modele = entrainer(4, dist_euclidienne, images_entrainement, valeurs_entrainement)
print("Importation des images de test...")
images_test, valeurs_test = importer_images("test/")
print("Estimation du modèle...")
estimations = estimer(modele, images_test)
print("Erreur MAE :", mae(estimations, valeurs_test))
print("Erreur RMSE :", rmse(estimations, valeurs_test))

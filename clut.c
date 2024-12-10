#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "clut.h"

static int compter_bits_necessaires(int n);
static unsigned char get_plus_proche_index(couleur_t couleur, clut_t *clut);

int compter_bits_necessaires(int n) {
  int size = 1, i;

  n = n - 1;
  for (i = 0; i < sizeof(n) * 8; i++) {
    n = n / 2;
    if (n == 0) break;
    size++;
  }

  return size;
}

unsigned char get_plus_proche_index(couleur_t couleur, clut_t *clut) {
  int i, plus_proche_index;
  double distance_min, distance;

  // commencer avec la première couleur puis comparer avec les autres
  plus_proche_index = 0;
  distance_min =
    ((couleur.r - clut->couleurs[0].r) * (couleur.r - clut->couleurs[0].r)) +
    ((couleur.g - clut->couleurs[0].g) * (couleur.g - clut->couleurs[0].g)) +
    ((couleur.b - clut->couleurs[0].b) * (couleur.b - clut->couleurs[0].b));

  for (i = 1; i <= clut->size; i++) {
    distance =
      ((couleur.r - clut->couleurs[i].r) * (couleur.r - clut->couleurs[i].r)) +
      ((couleur.g - clut->couleurs[i].g) * (couleur.g - clut->couleurs[i].g)) +
      ((couleur.b - clut->couleurs[i].b) * (couleur.b - clut->couleurs[i].b));

    if (distance < distance_min) {
      distance_min = distance;
      plus_proche_index = i;
    }
  }
  return plus_proche_index;
}

clut_t* clut_create(image_t *image, int nb_couleurs) {
  clut_t* clut;
  int index, image_size, i;

  image_size = image->width * image->height;

  clut = (clut_t*) malloc(sizeof *clut);
  clut->size = (nb_couleurs > 256 ? 256 : nb_couleurs) - 1;
  clut->couleurs = malloc((clut->size + 1) * sizeof *clut->couleurs);

  for (i = 0; i <= (int) clut->size; i++) {
    index = (rand() % image_size) * 3;

    clut->couleurs[i].r = (float)(image->data[index + 0]);
    clut->couleurs[i].g = (float)(image->data[index + 1]);
    clut->couleurs[i].b = (float)(image->data[index + 2]);
  }

  return clut;
}

void clut_print(clut_t *clut) {
  int i;
  for (i = 0; i <= clut->size; i++) {
    printf("%d : (%d, %d, %d)\n", i, clut->couleurs[i].r, clut->couleurs[i].g, clut->couleurs[i].b);
  }
}

image_t* clut_apply(image_t *image, clut_t *clut) {
  image_t *dst;
  couleur_t *src_data;
  couleur_t *dst_data;
  int index, i;

  int size = image->width * image->height;

  dst = (image_t *) malloc(sizeof *dst);
  dst->width = image->width;
  dst->height = image->height;
  dst->data = malloc(size * 3 * sizeof *dst->data);

  src_data = (couleur_t*) image->data;
  dst_data = (couleur_t*) dst->data;

  for (i = 0; i < size; i++, src_data++, dst_data++) {
    index = get_plus_proche_index(*src_data, clut);

    dst_data->r = clut->couleurs[index].r;
    dst_data->g = clut->couleurs[index].g;
    dst_data->b = clut->couleurs[index].b;
  }
  return dst;
}

void clut_free(clut_t* clut) {
  if (clut->couleurs != NULL) {
    free(clut->couleurs);
    clut->couleurs = NULL;
  }

  free(clut);
}

int clut_kmeans(image_t *image, clut_t *clut) {
  int couleur_index, size, i;
  int somme_distances, d, ra, ga, ba, dr, dg, db;
  long *sommes;
  couleur_t *data;

  // stocker les sommes des valeurs r, g, b de l'image + la somme totale des pixels pour une couleur
  sommes = malloc((clut->size + 1) * 4 * sizeof *sommes);
  memset(sommes, 0, (clut->size + 1) * 4 * sizeof *sommes);

  size = image->width * image->height;
  data = (couleur_t*) image->data;

  // calculer les sommes des couleurs de l'image et le nombre de pixels correspondants pour chaque couleur de la clut
  for (i = 0; i < size; i++, data++) {
    couleur_index = get_plus_proche_index(*data, clut);
    sommes[4 * couleur_index + 0] += (int) data->r;
    sommes[4 * couleur_index + 1] += (int) data->g;
    sommes[4 * couleur_index + 2] += (int) data->b;
    sommes[4 * couleur_index + 3] += 1;
  }

  // redéfinir les couleurs avec la moyenne pour chaque cluster
  somme_distances = 0;

  for (i = 0; i <= clut->size; i++) {
    // aucun pixel dans le point le plus proche pour cette couleur
    if (sommes[4 * i + 3] == 0) continue;

    ra = sommes[4 * i] / sommes[4 * i + 3];
    ga = sommes[4 * i + 1] / sommes[4 * i + 3];
    ba = sommes[4 * i + 2] / sommes[4 * i + 3];

    // somme des carrés des distances parcourues dans le nuage 3D
    dr = ra - clut->couleurs[i].r;
    dg = ga - clut->couleurs[i].g;
    db = ba - clut->couleurs[i].b;
    d = (dr * dr) + (dg * dg) + (db * db);

    somme_distances += d;

    // destination de parcours du centre pour le cluster i
    clut->couleurs[i].r = ra;
    clut->couleurs[i].g = ga;
    clut->couleurs[i].b = ba;
  }

  free(sommes);

  return somme_distances;
}

int clut_kmeans_times(image_t *image, clut_t *clut, int nb_iterations) {
  int i, distance;

  for (i = 0; i < nb_iterations; i++) {
    distance = clut_kmeans(image, clut);

    if (distance < clut->size) {
      printf("stop\n");
      break;
    }
  }

  printf("%d itérations réalisées\n", i);
  return distance;
}

static int comparer_couleurs_lineaires(const void *a, const void *b) {
  couleur_freq_t *couleur_a = (couleur_freq_t *)a;
  couleur_freq_t *couleur_b = (couleur_freq_t *)b;

  // tri décroissant
  return couleur_b->freq - couleur_a->freq;
}

clut_t* clut_create_by_stats(image_t * im, int nb_couleurs) {
  char i, j, found;
  int c;
  unsigned long x, y, size, freq_count;
  couleur_t * data;
  couleur_freq_t * couleur_freqs;
  clut_t* clut;

  clut = (clut_t*) malloc(sizeof *clut);
  clut->size = (nb_couleurs > 256 ? 256 : nb_couleurs) - 1;

  size = im->width * im->height;

  couleur_freqs = malloc(size * sizeof(couleur_freq_t));
  freq_count = 0;

  data = (couleur_t *)im->data;

  for (x = 0; x < size; x++, data++) {
    // temps d'execution beaucoup trop long dans l'état..
    if (x % 1000 == 0) printf("pixel : %d\n", x);
    c = 0;
    for (i = 7; i >= 0; i--) {
      c <<= 3;
      c += (((data->r >> i) & 1) << 2) +
           (((data->g >> i) & 1) << 1) +
           ((data->b >> i) & 1);
    }

    found = 0;
    for (y = 0; y < freq_count; y++) {
      if (couleur_freqs[y].value == c) {
        couleur_freqs[y].freq++;
        found = 1;
        break;
      }
    }

    // si la couleur n'a pas été trouvée, l'ajouter au tableau
    if (!found) {
      couleur_freqs[freq_count].value = c;
      couleur_freqs[freq_count].freq = 1;
      freq_count++;
    }
  }

  // trier les couleurs en décroissant
  qsort(couleur_freqs, freq_count, sizeof(couleur_freq_t), comparer_couleurs_lineaires);

  // ne garder que les couleurs les plus fréquentes
  for (j = 0; j < clut->size; j++) {
    // TODO: ajouter la couleur 3D correspondant à la valeur 1D.  
    // clut->couleurs[j] = couleur_freqs[j].value;
  }

  // Libération de la mémoire du tableau couleur_freqs
  free(couleur_freqs);

  return clut;
}

void clut_write(char* fileName, image_t *image, clut_t *clut) {
  FILE *output;
  couleur_t *data;
  int bits_par_pixel, image_size, i, couleur_index, reste;
  unsigned char buffer;

  // output = fopen(fileName, "wb");
  output = fopen(fileName, "wb");

  // 1. stocker la taille de l'image
  fwrite(&(image->width), sizeof(long), 1, output);
  fwrite(&(image->height), sizeof(long), 1, output);

  // 2. stocker le nombre de couleur
  fputc(clut->size, output);

  // 3. stocker les couleurs de la clut
  for (i = 0; i <= clut->size; i++) {
    fwrite(&(clut->couleurs[i]), sizeof(GLbyte), 3, output);
  }

  // 4. stocker les indexes de couleurs pour chaque pixel
  data = (couleur_t*) image->data;

  image_size = image->width * image->height;
  bits_par_pixel = compter_bits_necessaires((clut->size) + 1);
  buffer = 0; // accumulateur de l'octet en cours d'écriture
  reste = 8;    // nombre de bits restants à remplir dans l'octet
  for (i = 0; i < image_size; i++, data++) {
    couleur_index = get_plus_proche_index(*data, clut);

    // ecriture bit à bit
    if (reste >= bits_par_pixel) {
      buffer <<= bits_par_pixel; // shifter de nbesize bits pour faire de la place
      buffer += couleur_index; // ajouter les bits
      reste -= bits_par_pixel; // décrémenter le reste à écrire
    }
    // si il ne reste pas suffisement de bits pour écrire une couleur
    else {
      // 1. ecrire le nombre de bit correspondant au reste actuel
      buffer <<= reste;
      // extraire dans tmp les bits de gauche qui peuvent rentrer dans l'accumulateur
      buffer += couleur_index >> (bits_par_pixel - reste);
      // 2. ecrire l'octet
      fputc(buffer, output);
      buffer = 0;
      // 3. remplir buffer avec le reste à écrire
      reste = 8 - (bits_par_pixel - reste); // soustraire à reste le nombre de bits restant à écrire
      buffer = couleur_index << reste; // commencer à remplir le nouvel octet avec les bits restants
      buffer >>= reste; // ajuster pour ne garder que les bits de poids faible
    }
  }

  if (reste > 0) {
    fputc(buffer, output);
  }

  fclose(output);
}

// recevoir une clut vide avec une mémoire allouée pour la remplir à la lecture du fichier
image_t* clut_read(char* fileName, clut_t *clut) {
  FILE *input;
  image_t *image = malloc(sizeof *image);
  couleur_t *data;
  int i, image_size, bits_par_pixel, couleur_index, reste;
  unsigned char buffer;

  // remplir la clut et retourner l'image

  // format: width, height, nbe, r, g b * nbe, indexes

  input = fopen(fileName, "rb");
  if (input == NULL) {
    printf("Erreur lors de l'ouverture du fichier %s.\n", fileName);
    return NULL;
  }

  // 1. lire la taille de l'image
  fread(&(image->width), sizeof(long), 1, input);
  fread(&(image->height), sizeof(long), 1, input);

  image_size = image->width * image->height;

  // 2. lire le nombre de couleurs de la clut
  clut->size = fgetc(input);

  // 3. remplir la clut
  clut->couleurs = malloc(clut->size * sizeof(couleur_t));

  for (i = 0; i <= clut->size; i++) {
    fread(&(clut->couleurs[i]), sizeof(GLbyte), 3, input);
  }

  image->data = malloc(image_size * 3 * sizeof *image->data);
  // lire les indexes de l'image

  bits_par_pixel = compter_bits_necessaires((clut->size) + 1);
  buffer = 0; // accumulateur d'un octet pour stocker les bits extraits
  reste = 0; // nombre de bits restant à extraire de buffer

  data = (couleur_t*) image->data;
  for (i = 0; i < image_size; i++, data++) {
    // lecture bit à bit

    if (reste == 0) {
      buffer = fgetc(input);
      if (buffer == EOF) break;
      reste = 8;
    }

    if (reste >= bits_par_pixel) {
      // 1. lire un indice
      couleur_index = buffer >> (reste - bits_par_pixel); // extraire les nbesize bits les plus à gauche

      // 2. Enlever les bits déjà extraits
      buffer = buffer - (couleur_index << (reste - bits_par_pixel)); // bytes_acc - reshift à gauche des nbe bits shiftés précedemment à droite
      // 3. Réduire le nombre de bits restant
      reste = (reste - bits_par_pixel);
    } else {
      // si le reste à lire est inferieur à nbe, c'est que buffer n'a pas assez de bits, lire l'octet suivant
      // 1. décaler les bits du buffer à gauche pour libérer l'espace nécessaire
      couleur_index = buffer << (bits_par_pixel - reste);

      buffer = fgetc(input);

      // 2. compléter temp avec les bits restants shifté à droite pour avoir la couleur
      couleur_index += (buffer >> (8 - (bits_par_pixel - reste)));

      reste = 8 - (bits_par_pixel - reste); // mettre à jour le nombre de bits restants à lire

      // réduire le nombre de bits restants dans buffer
      buffer = buffer - ((buffer >> reste) << reste); // Réduit le nombre de bits restants dans byte_acc
    }

    // Lire la couleur depuis son indice
    data->r = clut->couleurs[couleur_index].r;
    data->g = clut->couleurs[couleur_index].g;
    data->b = clut->couleurs[couleur_index].b;
  }

  fclose(input);
  return image;
}
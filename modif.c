/* il s agit de prendre une image et de la modifier */

#include "ima.h"
#include <limits.h>
#include <math.h> 
#include <time.h>
#include <string.h>

void gris_pondere(image_t * i) {
  int j, size, v;
  GLubyte *imr, *img, *imb, val;

  size = i->sizeY * i->sizeX;

  imr = i->data;
  img = i->data + 1;
  imb = i->data + 2;

  for (j = 0; j < size; j ++) {
    v = *imr + (int) *imr;
    v += 3 * (int) *img;
    v += *imb;
    v /= 6;
    val = (GLubyte) v;

    *imr = val;
    *img = val;
    *imb = val;

    imr += 3;
    img += 3;
    imb += 3;
  }
}

clut_t creerclut(int nbe) {
  clut_t cl;
  int i;

  srand(time(NULL));

  cl.clut = malloc(nbe * sizeof(couleur_t));

  if (nbe > 256) nbe = 256;
  cl.nbe = nbe;

  for (i = 0; i < nbe; i++) {
    cl.clut[i].r = (float)rand() / RAND_MAX * 255;
    cl.clut[i].g = (float)rand() / RAND_MAX * 255;
    cl.clut[i].b = (float)rand() / RAND_MAX * 255;     
  }
  return cl;
}

void afficherclut(clut_t *cl) {
  printf("couleurs de la clut\n");
  for (int i = 0; i < cl->nbe; i++) {
    printf("%d : (%d, %d, %d)\n", i, cl->clut[i].r, cl->clut[i].g, cl->clut[i].b);
  }
}

void freeimage(image_t* image) {
  free(image->data);
  free(image);
}

image_t* creercopie(image_t *source, clut_t *cl) {
  image_t *i;
  int couleurindex;
  GLubyte *sr, *sg, *sb;
  GLubyte *ir, *ig, *ib;

  int size = source->sizeX * source->sizeY;

  i = (image_t *) malloc(sizeof(image_t));

  i->sizeX = source->sizeX;
  i->sizeY = source->sizeY;

  i->data = malloc(size * sizeof(GLubyte) * 3);

  sr = source->data;
  sg = source->data + 1;
  sb = source->data + 2;

  ir = i->data;
  ig = i->data + 1;
  ib = i->data + 2;

  for (int x = 0; x < size; x++) {
    couleurindex = plusproche(sr, sg, sb, cl);

    *ir = cl->clut[couleurindex].r;
    *ig = cl->clut[couleurindex].g;
    *ib = cl->clut[couleurindex].b;

    sr += 3;
	  sg += 3;
	  sb += 3;

    ir += 3;
	  ig += 3;
	  ib += 3;
  }
  return i;
}

GLubyte plusproche(GLubyte *r, GLubyte *g, GLubyte *b, clut_t *cl) {
  GLubyte i, plusproche;
  double dmin, d;

  // commencer avec la première couleur puis comparer avec les autres
  plusproche = 0;
  dmin =
    ((*r - cl->clut[0].r) * (*r - cl->clut[0].r)) + 
    ((*g - cl->clut[0].g) * (*g - cl->clut[0].g)) + 
    ((*b - cl->clut[0].b) * (*b - cl->clut[0].b));

  for (i = 1; i < cl->nbe; i++) {
    d =
    ((*r - cl->clut[i].r) * (*r - cl->clut[i].r)) + 
    ((*g - cl->clut[i].g) * (*g - cl->clut[i].g)) + 
    ((*b - cl->clut[i].b) * (*b - cl->clut[i].b));

    if (d < dmin) {
      dmin = d;
      plusproche = i;
    }
  }
  return plusproche;
}

void kmoyennes(image_t *im, clut_t *cl) {
  int couleurindex, size, i;
  long *sommes;
  GLubyte *imr, *img, *imb;
  
  // stocker les sommes des valeurs r, g, b de l'image + la somme totale des pixels pour une couleur
  sommes = malloc(sizeof(*sommes) * cl->nbe * 4);
  memset(sommes, 0, sizeof(*sommes) * cl->nbe * 4);

  size = im->sizeX * im->sizeY;

  imr = im->data;
  img = im->data + 1;
  imb = im->data + 2;

  // calculer les sommes des couleurs de l'image et le nombre de pixels correspondants pour chaque couleur de la clut
  for (i = 0; i < size; i++) {
    couleurindex = plusproche(imr, img, imb, cl);
    sommes[4 * couleurindex] += (int)*imr;
    sommes[4 * couleurindex + 1] += (int)*img;
    sommes[4 * couleurindex + 2] += (int)*imb;
    sommes[4 * couleurindex + 3] += 1;

    imr += 3;
    img += 3;
    imb += 3;
  }

  // redéfinir les couleurs avec la moyenne pour cette couleur
  for (i = 0; i < cl->nbe; i++) {
    // aucun pixel dans le point le plus proche pour cette couleur
    if (sommes[4 * i + 3] == 0) continue;

    cl->clut[i].r = sommes[4 * i] / sommes[4 * i + 3];
    cl->clut[i].g = sommes[4 * i + 1] / sommes[4 * i + 3];
    cl->clut[i].b = sommes[4 * i + 2] / sommes[4 * i + 3];
  }

  free(sommes);
}

void compresser(char* fileName, image_t *im, clut_t *cl) {
  int nbesize, size, i;
  GLubyte *imr, *img, *imb;
  GLubyte couleurindex, byte_acc, reste, tmp;

  FILE *output;

  size = im->sizeX * im->sizeY;

  // output = fopen(fileName, "wb");
  output = fopen(fileName, "wb");

  // 1. stocker la taille de l'image
  fwrite(&(im->sizeX), sizeof(int), 1, output);
  fwrite(&(im->sizeY), sizeof(int), 1, output);

  // 2. stocker le nombre de couleur
  fputc(cl->nbe, output);

  // 3. stocker les couleurs de la clut
  for (i = 0; i < cl->nbe; i++) {
    fwrite(&(cl->clut[i]), sizeof(GLbyte), 3, output);
  }

  // 4. stocker les indexes de couleurs pour chaque pixel
  imr = im->data;
  img = im->data + 1;
  imb = im->data + 2;

  nbesize = couleurtaillebinaire(cl->nbe);
  byte_acc = 0, reste = 8, tmp = 0;

  for (i = 0; i < size; i ++) {
    couleurindex = plusproche(imr, img, imb, cl);

    // ecriture bit à bit
    if (reste >= nbesize) {
      byte_acc <<= nbesize;
      byte_acc += couleurindex;
      reste -= nbesize;
    }
    else {
      // 1. ecrire le nombre de bit correspondant au reste actuel
      byte_acc <<= reste;
      tmp = couleurindex >> (nbesize - reste);
      byte_acc += tmp;
      // 2. ecrire l'octet
      fputc(byte_acc, output);
      byte_acc = 0;
      // 3. remplir byte_acc avec le reste à écrire
      reste = 8 - (nbesize - reste);
      byte_acc = couleurindex << reste;
      byte_acc >>= reste;
    }

    imr += 3;
    img += 3;
    imb += 3;
  }

  if (reste < 8) {
    fputc(byte_acc, output);
  }

  fclose(output);
}

GLubyte couleurtaillebinaire(GLubyte b) {
  GLubyte size, i;
  for (i = 0; i < sizeof(b) * 8; i++) {
    b = b / 2;
    if (b > 0) size++;
    else break;
  }
  return size;
}

// recevoir une clut vide avec une mémoire allouée pour la remplir à la lecture du fichier
image_t decompresser(char* fileName, clut_t *cl) {
  FILE *input;
  image_t im;
  int i, imsize, nbesize;
  // GLubyte couleurindex, byte_acc, reste, tmp;

  // if (cl == NULL) return;

  // remplir la clut et retourner l'image

  // format: sizeX, sizeY, nbe, r, g b * nbe, indexes

  input = fopen(fileName, "rb");

  // 1. lire la taille de l'image
  fread(&(im.sizeX), sizeof(int), 1, input);
  fread(&(im.sizeY), sizeof(int), 1, input);

  imsize = im.sizeX * im.sizeY;

  im.data = malloc(imsize * sizeof(GLubyte) * 3);

  // 2. lire le nombre de couleurs de la clut
  cl->nbe = fgetc(input);

  // 3. remplir la clut
  cl->clut = malloc(cl->nbe * sizeof(couleur_t));

  for (i = 0; i < cl->nbe; i++) {
    fread(&(cl->clut[i]), sizeof(GLbyte), 3, input);
  }
  
  // lire les indexes de l'image
  
  nbesize = couleurtaillebinaire(cl->nbe);
  // byte_acc = 0, reste = 0, tmp = 0;

  // A faire: lecture bit à bit de l'image

  fclose(input);
  return im;
}
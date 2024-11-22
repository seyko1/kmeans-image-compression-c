/* il s agit de prendre une image et de la modifier */

#include "ima.h"
#include <limits.h>
#include <math.h> 
#include <time.h>

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
  cl.nbe = nbe;

  for (i = 0; i < nbe; i++) {
    cl.clut[i].r = (float)rand() / RAND_MAX * 255;
    cl.clut[i].g = (float)rand() / RAND_MAX * 255;
    cl.clut[i].b = (float)rand() / RAND_MAX * 255;     
  }
  return cl;
}

void afficherclut(clut_t cl) {
  for (int i = 0; i < cl.nbe; i++) {
    printf("(%d, %d, %d)\n", cl.clut[i].r, cl.clut[i].g, cl.clut[i].b);
  }
}

image_t* creercopie(image_t *source, clut_t *cl) {
  image_t *i;
  couleur_t c;
  GLubyte *sr, *sg, *sb;
  GLubyte *ir, *ig, *ib;

  int size = source->sizeX * source->sizeY;

  // i = malloc(sizeof(*i));
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
    // i->data[x] += source->data[x] / 2;
    // i->data[x];
    // calcul de la distance euclidienne pour trouver quel point de la clut est le plus proche
    c = plusproche(sr, sg, sb, cl);

    // printf("r : %d\n", c.r);
    // printf("g : %d\n", c.g);
    // printf("b : %d\n", c.b);

    *ir = c.r;
    *ig = c.g;
    *ib = c.b;
  
    sr += 3;
	  sg += 3;
	  sb += 3;

    ir += 3;
	  ig += 3;
	  ib += 3;
  }
  return i;
}

couleur_t plusproche(GLubyte *r, GLubyte *g, GLubyte *b, clut_t *cl) {
  couleur_t plusproche;
  int i;
  double dmin, d;

  // commencer avec la première couleur puis comparer avec les autres
  plusproche = cl->clut[0];
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
      plusproche = cl->clut[i];
    }
  }
  return plusproche;
}
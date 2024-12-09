#ifndef IMA_H_INCLUDED
#define IMA_H_INCLUDED

#include "ppm.h"

#define __OSX__
#ifndef __OSX__
/* pour Bocal */
#include <GL/gl.h>      
#else
/* pour mac */
#include <GL/glut.h>
#endif

typedef struct couleur {
  GLubyte r;
  GLubyte g;
  GLubyte b;
} couleur_t;

typedef struct clut {
  GLubyte size;
  couleur_t *couleurs;
} clut_t;

typedef struct {
    int value; // couleur codée sous forme d'entier
    int freq;  // La fréquence de cette couleur
} couleur_freq_t;

clut_t* clut_create(image_t *image, int nb_couleurs);
image_t* clut_apply(image_t *image, clut_t *clut);
void clut_print(clut_t *clut);
void clut_free(clut_t *clut);

/* appliquer kmeans*/
int clut_kmeans(image_t *image, clut_t *clut);
/* appliquer plusieurs itérations de kmeans */
int clut_kmeans_times(image_t *image, clut_t *clut, int nb_iterations);

clut_t* clut_create_by_stats(image_t * im, int nb_couleurs);

void clut_write(char* fileName, image_t *image, clut_t *clut);
image_t* clut_read(char* fileName, clut_t *clut);

#endif // IMA_H_INCLUDED
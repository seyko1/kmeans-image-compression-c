#ifndef IMA_H_INCLUDED
#define IMA_H_INCLUDED

#include <stdio.h> 
#include <stdlib.h>
#include <assert.h>

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
  int nbe;
  couleur_t *clut;
} clut_t;

typedef struct image {
    unsigned long sizeX;
    unsigned long sizeY;
    GLubyte *data;
} image_t;

typedef unsigned short utab [3][3][3];

int load_ppm(char *filename, image_t *image);
void save_ppm(char *filename, image_t *image);
void upsidedown(image_t *);
void gris_pondere (image_t *);

clut_t creerclut(int nb);
void afficherclut(clut_t cl);
image_t* creercopie(image_t *source, clut_t *cl);
couleur_t plusproche(GLubyte *r, GLubyte *g, GLubyte *b, clut_t *cl);
void freeImage(image_t *image);

#endif // IMA_H_INCLUDED
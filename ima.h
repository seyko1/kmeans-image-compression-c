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

#define MAX_COULEURS 256

typedef struct couleur {
  GLubyte r;
  GLubyte g;
  GLubyte b;
} couleur_t;

typedef struct clut {
  unsigned short nbe;
  couleur_t *clut;
} clut_t;

typedef struct image {
    unsigned long sizeX;
    unsigned long sizeY;
    GLubyte *data;
} image_t;

int load_ppm(char *filename, image_t *image);
void save_ppm(char *filename, image_t *image);
void upsidedown(image_t *);

clut_t creerclut(image_t *im, unsigned short nbe);
void afficherclut(clut_t *cl);
image_t* creercopie(image_t *source, clut_t *cl);
int* lineariserpixels(image_t * im);
GLubyte plusproche(GLubyte *r, GLubyte *g, GLubyte *b, clut_t *cl);
void appliqueriterations(int nbiter, image_t *source, clut_t  *cl);
int kmoyennes(image_t *source, clut_t *cl);
GLubyte couleurtaillebinaire(unsigned short nbe);
void compresser(char* fileName, image_t *im, clut_t *cl);
image_t decompresser(char* fileName, clut_t *cl);
void freeimage(image_t *image);

#endif // IMA_H_INCLUDED
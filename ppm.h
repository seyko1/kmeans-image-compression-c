#ifndef PPM_H_INCLUDED
#define PPM_H_INCLUDED

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

typedef struct image {
    unsigned int width;
    unsigned int height;
    GLubyte *data;
} image_t;

int ppm_load(char *filename, image_t *image);
void ppm_write(char *filename, image_t *image);
void upsidedown(image_t *);
void image_free(image_t *image);

#endif // PPM_H_INCLUDED
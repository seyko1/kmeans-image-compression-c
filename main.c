#include <unistd.h>     
#include <math.h>
#include "ima.h"

image_t* to_display;
image_t *image;
image_t *copy = NULL;
clut_t clut;
int afficheroriginale = 1;

#define ESCAPE 27

void keyboard(unsigned char key, int x, int y) {
  switch(key) {
    case ESCAPE:
      exit(0);                   
      break;
    default:
      fprintf(stderr, "Unused key\n");
  }
}

void mouse(int button, int state, int x, int y) {
  switch(button) {
    case GLUT_LEFT_BUTTON:
      break;
    case GLUT_MIDDLE_BUTTON:
      break;
    case GLUT_RIGHT_BUTTON:
      break;    
  }
  glutPostRedisplay();
}

int init(char *s) {
  image = (image_t *) malloc(sizeof(image_t));
  to_display = image;
  
  if (image == NULL) {
    fprintf(stderr, "Out of memory\n");
    return(-1);
  }

  if (load_ppm(s, image)==-1) {
	  return(-1);
  }
  printf("tailles %d %d\n",(int) image->sizeX, (int) image->sizeY);

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glutReshapeWindow(image->sizeX, image->sizeY);

  clut = creerclut(image, 10);
  afficherclut(&clut);

  return (0);
}

int reinit() {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glutReshapeWindow(image->sizeX, image->sizeY);
  
  return (0);
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT);

  glDrawPixels(
    to_display->sizeX,
    to_display->sizeY,
    GL_RGB,
    GL_UNSIGNED_BYTE, 
	  to_display->data
  );

  glFlush();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, (GLdouble) w, 0.0, (GLdouble)h);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void menuFunc(int item) {
  // char s[256];
  char fileName[] = "door.km";
  switch(item){
    case 0:
      free(image);
      exit(0);
      break;
    case 1:
      to_display = image;
      display();
      break;
    case 2:
      if (copy != NULL) freeimage(copy);
      
      afficherclut(&clut);
      
      copy = creercopie(image, &clut);
      
      to_display = copy;
      display();
      
      //free(clut.clut);
      break;
    case 3:
      for (int i = 0; i < 10; i++) {
        kmoyennes(image, &clut);
      }

      afficherclut(&clut);

      if (copy != NULL) freeimage(copy);

      copy = creercopie(image, &clut);

      to_display = copy;
      display();

      break;
    case 4:
      // printf("Entrer le nom pour l'image compressée\n");
      // scanf("%s", &s[0]);
      printf("Compression de l'image %s\n", fileName);
      compresser(fileName, image, &clut);
      break;
    case 5:
      printf("Décompression de l'image %s\n", fileName);
      decompresser(fileName, &clut);
    case 6:
      printf("Taille de l image : %ld %ld\n", (long) image->sizeX, (long) image->sizeY);
      break;
    default:
      break;
  }
}

int main(int argc, char **argv) {  
  if (argc<2) {
    fprintf(stderr, "Usage : palette nom_de_fichier\n");
    exit(0);
  }

  glutInit(&argc, argv); 
  glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
  glutInitWindowSize(400, 400);  
  glutInitWindowPosition(100, 100);  
  glutCreateWindow("VPUP8");  

  init(argv[1]);

  glutCreateMenu(menuFunc);
  glutAddMenuEntry("Quitter", 0);
  glutAddMenuEntry("Afficher originale", 1);
  glutAddMenuEntry("Appliquer clut de 10", 2);
  glutAddMenuEntry("Kmeans +10 iter", 3);
  glutAddMenuEntry("Compresser", 4);
  glutAddMenuEntry("Decompresser", 5);
  glutAddMenuEntry("Informations", 6);
  glutAttachMenu(GLUT_LEFT_BUTTON);

  glutDisplayFunc(display);  
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  
  glutMouseFunc(mouse);

  glutMainLoop();  

  return 1;
}

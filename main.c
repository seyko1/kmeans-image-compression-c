#include <unistd.h>     
#include <math.h>
#include "ima.h"

#define NB_COULEURS 10
#define NB_ITER 100

image_t* to_display;
image_t *image;
image_t *copy = NULL;
clut_t clut;
int afficheroriginale = 1;

enum Menu {
  Quitter,
  AfficherOriginale,
  AfficherCopie,
  AppliquerClut,
  KmeansIterations,
  Compresser,
  Decompresser,
  Informations,
  Reinitialiser,
};

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

  clut = creerclut(image, NB_COULEURS);
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
  char fileName[] = "image.km";
  switch(item){
    case Quitter:
      freeimage(image);
      free(clut.clut);
      exit(0);
      break;

    case AfficherOriginale:
      to_display = image;
      display();
      break;

    case AfficherCopie:
      if (copy == NULL) {
        printf("La copie n'est pas initialisée.\n"); 
        return;
      }
      to_display = copy;
      display();
      break;

    case AppliquerClut:
      if (copy != NULL) freeimage(copy);
      
      free(clut.clut);
      clut = creerclut(image, NB_COULEURS);
      
      afficherclut(&clut);
      
      copy = creercopie(image, &clut);
      
      to_display = copy;
      display();
      break;

    case KmeansIterations:
      printf("Nombre d'itérations : %d\n", NB_ITER);
      // for (int i = 0; i < NB_ITER; i++) {
      //   kmoyennes(image, &clut);
      // }

      appliqueriterations(NB_ITER, image, &clut);

      afficherclut(&clut);

      if (copy != NULL) freeimage(copy);

      copy = creercopie(image, &clut);

      to_display = copy;
      display();

      break;

    case Compresser:
      // printf("Entrer le nom pour l'image compressée\n");
      // scanf("%s", &s[0]);
      printf("Compression de l'image %s\n", fileName);
      compresser(fileName, image, &clut);
      break;

    case Decompresser:
      printf("Décompression de l'image %s\n", fileName);

      copy = malloc(sizeof(*copy));

      free(clut.clut);
      *copy = decompresser(fileName, &clut);

      printf("Taille de l image décompressée : %ld %ld\n", (long) copy->sizeX, (long) copy->sizeY);
      printf("Affichage de l'image décompressé\n");

      to_display = copy;
      display();
      break;

    case Informations:
      printf("Taille de l image : %ld %ld\n", (long) image->sizeX, (long) image->sizeY);
      break;

    case Reinitialiser:
      freeimage(image);
      free(clut.clut);
      clut.clut = NULL;
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
  glutAddMenuEntry("Quitter", Quitter);
  glutAddMenuEntry("Afficher originale", AfficherOriginale);
  glutAddMenuEntry("Afficher copie", AfficherCopie);
  glutAddMenuEntry("Appliquer clut", AppliquerClut);
  glutAddMenuEntry("Kmeans iterations", KmeansIterations);
  glutAddMenuEntry("Compresser", Compresser);
  glutAddMenuEntry("Decompresser", Decompresser);
  glutAddMenuEntry("Informations", Informations);
  glutAddMenuEntry("Reinitialiser", Reinitialiser);
  glutAttachMenu(GLUT_LEFT_BUTTON);

  glutDisplayFunc(display);  
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  
  glutMouseFunc(mouse);

  glutMainLoop();  

  return 1;
}

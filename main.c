#include <unistd.h>     
#include <math.h>
#include "ima.h"

image_t *image;
image_t *copy;
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
  image_t *i;

  printf("afficheroriginale ? %d\n", afficheroriginale);

  if (afficheroriginale) {
    i = image;
  } else {
    i = copy;
  }

  GLint w, h;

  glClear(GL_COLOR_BUFFER_BIT);

  w = glutGet(GLUT_WINDOW_WIDTH);
  h = glutGet(GLUT_WINDOW_HEIGHT);

  glDrawPixels(
    i->sizeX,
    i->sizeY,
    GL_RGB,
    GL_UNSIGNED_BYTE, 
	  i->data
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
  char s[256];

  switch(item){
    case 0:
      free(image);
      exit(0);
      break;
    case 1:
      printf("Gris pondéré\n");
      gris_pondere(image);
      afficheroriginale = 1;
      display();
      break;
    case 2:
      clut_t cl = creerclut(5);
      afficherclut(cl);
      copy = creercopie(image, &cl);
      afficheroriginale = 0;
      display();
      break;
    case 3:
      // printf("Entrer le nom pour l'image dans cette taille\n");
      // scanf("%s", &s[0]);
      // save_ppm(s, image);
      break;
    case 4:
      printf("Taille de l image : %ld %ld\n", (int) image->sizeX, (int) image->sizeY);
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
  glutAddMenuEntry("Gris pondere", 1);
  glutAddMenuEntry("Appliquer clut", 2);
  glutAddMenuEntry("Sauvegarder", 3);
  glutAddMenuEntry("Informations", 4);
  glutAttachMenu(GLUT_LEFT_BUTTON);

  glutDisplayFunc(display);  
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  
  glutMouseFunc(mouse);

  glutMainLoop();  

  return 1;
}

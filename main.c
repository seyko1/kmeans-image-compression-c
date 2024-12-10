#include <unistd.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "ppm.h"
#include "clut.h"

#define NB_ITER 100

image_t* to_display = NULL;
image_t *image = NULL;
image_t *copy = NULL;
clut_t *clut = NULL;
int nb_couleurs = 10;
int afficheroriginale = 1;
char* km_filename;

enum Menu {
  Quitter,
  AfficherOriginale,
  AfficherCopie,
  NouvelleClut,
  KmeansIterations,
  Compresser,
  Decompresser,
  Informations,
};

#define KEY_ESCAPE 27
#define KEY_SPACE 32
#define KEY_PLUS 43
#define KEY_MINUS 45
#define KEY_P 112
#define KEY_S 115
#define KEY_T 116

static void on_appliquer_clut();
static void on_creer_clut();
static void on_increase_clut_colors();
static void on_decrease_clut_colors();
static void on_afficher_clut();
static void on_display_original();
static void on_display_copy();
static void on_switch_image();

static char* get_km_name(char* fileName);
static void cleanup();
static void keyboard(unsigned char key, int x, int y);
static void mouse(int button, int state, int x, int y);
static int init(char *s);
static void display();
static void reshape(int w, int h);
static void menuFunc(int item);

void on_appliquer_clut() {
  copy = clut_apply(image, clut);
  to_display = copy;
  display();
}

void on_creer_clut() {
  if (copy != NULL) image_free(copy);
  if (clut != NULL) clut_free(clut);

  clut = clut_create(image, nb_couleurs);
  // clut = clut_create_by_stats(image, nb_couleurs);

  on_appliquer_clut();
}

void on_increase_clut_colors() {
  if (nb_couleurs >= 256) return;

  nb_couleurs ++;
  printf("Nombre de couleurs : %d\n", nb_couleurs);
  on_creer_clut();
}

void on_decrease_clut_colors() {
  if (nb_couleurs < 3) return;

  nb_couleurs--;
  printf("Nombre de couleurs : %d\n", nb_couleurs);
  on_creer_clut();
}

void on_afficher_clut() {
  printf("Couleurs de la clut :\n");
  clut_print(clut);
}

void on_display_original() {
  to_display = image;
  display();
}

void on_display_copy() {
  to_display = copy;
  display();
}

void on_switch_image() {
  to_display = to_display == image ? copy : image;
  display();
}

void keyboard(unsigned char key, int x, int y) {
  // printf("Key pressed : %d\n", key);
  switch(key) {
    case KEY_ESCAPE:
      exit(0);
      break;
    case KEY_SPACE:
      on_creer_clut();
      break;
    case KEY_PLUS:
      on_increase_clut_colors();
      break;
    case KEY_MINUS:
      on_decrease_clut_colors();
      break;
    case KEY_P:
      on_afficher_clut();
      break;
    case KEY_S:
      on_switch_image();
      break;
    case KEY_T:
      clut_kmeans(image, clut);
      on_appliquer_clut();
      break;
    default:
      // fprintf(stderr, "Unused key\n");
      break;
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

char* get_km_name(char* fileName) {
  char* result;
  int len = strlen(fileName);

  result = malloc((len + 3) * sizeof *result);
  strcpy(result, fileName);
  strcat(result, ".km");

  return result;
}

int init(char *fileName) {
  km_filename = get_km_name(fileName);

  image = (image_t *) malloc(sizeof image);
  to_display = image;

  if (image == NULL) {
    fprintf(stderr, "Out of memory\n");
    return(-1);
  }

  if (ppm_load(fileName, image)==-1) {
	  return(-1);
  }

  printf("[Échap] : Quitter le programme\n");
  printf("[Espace] : Créer une nouvelle CLUT avec le même nombre de couleurs que l'actuelle\n");
  printf("[+] : Augmenter le nombre de couleurs de 1\n");
  printf("[-] : Diminuer le nombre de couleurs de 1\n");
  printf("[P] : Afficher le contenu de la CLUT\n");
  printf("[S] : Passer de l'image originale à la copie et vice-versa\n");
  printf("[T] : Appliquer un déplacement des moyennes\n");
  printf("\n");
  printf("Taille de l'image %d %d\n", image->width, image->height);

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glutReshapeWindow(image->width, image->height);

  clut = clut_create(image, nb_couleurs);
  copy = clut_apply(image, clut);

  return (0);
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT);

  glDrawPixels(
    to_display->width,
    to_display->height,
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
  switch(item){
    case Quitter:
      exit(0);
      break;

    case AfficherOriginale:
      on_display_original();
      break;

    case AfficherCopie:
      on_display_copy();
      break;

    case NouvelleClut:
      on_creer_clut();
      break;

    case KmeansIterations:
      clut_kmeans_times(image, clut, NB_ITER);

      if (copy != NULL) image_free(copy);

      copy = clut_apply(image, clut);

      to_display = copy;
      display();

      break;

    case Compresser:
      // printf("Entrer le nom pour l'image compressée\n");
      // scanf("%s", &s[0]);
      printf("Compression de l'image %s\n", km_filename);
      clut_write(km_filename, image, clut);
      break;

    case Decompresser:
      printf("Décompression de l'image %s\n", km_filename);

      if (copy != NULL) image_free(copy);
      if (clut != NULL) {
        free(clut->couleurs);
      } else {
        clut = malloc(sizeof *clut);
      }

      copy = clut_read(km_filename, clut);
      if (copy == NULL) {
        break;
      }

      printf("Taille de l image décompressée : %ld %ld\n", (long) copy->width, (long) copy->height);

      to_display = copy;
      display();
      break;

    case Informations:
      printf("Taille de l image : %ld %ld\n", (long) image->width, (long) image->height);
      break;

    default:
      break;
  }
}

void cleanup() {
  image_free(image);
  clut_free(clut);
  free(km_filename);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage : palette nom_de_fichier\n");
    exit(0);
  }

  atexit(cleanup);
  srand(time(NULL));

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
  glutAddMenuEntry("Nouvelle CLUT", NouvelleClut);
  glutAddMenuEntry("Kmeans iterations", KmeansIterations);
  glutAddMenuEntry("Compresser", Compresser);
  glutAddMenuEntry("Decompresser", Decompresser);
  glutAddMenuEntry("Informations", Informations);
  glutAttachMenu(GLUT_LEFT_BUTTON);

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);

  glutMouseFunc(mouse);

  glutMainLoop();

  return 1;
}

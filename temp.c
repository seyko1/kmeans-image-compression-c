//  main..
if (argc == 3 && strcmp(argv[2], "--tests") == 0) {
  tests();
  return 0;
}

// tests
  void tests() {
  int i;
  clock_t t0, t1, dt;

  image = (image_t *) malloc(sizeof(image_t));
  
  for (i = 5; i <= 50; i++) {
    t0 = clock();

    clut = creerclut(image, i);
    appliqueriterations(KMEANS_ITER, image, &clut);
    
    t1 = clock();

    dt = t1 - t0;

    printf ("%d couleurs : %d ticks", i, (int) dt);
  }
}
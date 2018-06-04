#include <stdio.h>   // printf
#include <stdlib.h>   // atoi
#include <string.h>  // strlen
#include <unistd.h>  // getopt

#include "pngreader.h"
#include "tiffreader.h"
#include "emitpngs.h"
#include "emitdata.h"

extern char *optarg;
extern int optind, opterr, optopt;

#include <getopt.h>

void usage(const char *pn) {
  printf("Usage: %s -f filename[.pnm/.tif] -x 255 -y 255 -w 10 -h 10 -t/-m\n", pn);
  printf("f:input file\n");
  printf("x: tile x resolution\n");
  printf("y: tile y resolution\n");
  printf("w: num tiles x\n");
  printf("h: num tiles y\n");
  printf("t: emit textures\n");
  printf("m: emit meshes\n");
}

int main(int argc, char **argv) {
  char *optstring = "f:x:y:w:h:tm";
  int x, y, w, h;
  x = 255;
  y = 255;
  w = 10;
  h = 10;
  int opt;
  char *fn = "test.pnm";
  char isTiff = 'n';
  char tm = 't';
  int img_xres, img_yres, pdepth;
  uint32 *buffer;

  int numopts = 0;

  while ((opt = getopt(argc, argv, optstring)) != -1) {
    switch (opt) {
      case 'f':
        fn = optarg;
        numopts++;
        break;
      case 'x':
        x = atoi(optarg);
        numopts++;
        break;
      case 'y':
        y = atoi(optarg);
        numopts++;
        break;
      case 'w':
        w = atoi(optarg);
        numopts++;
        break;
      case 'h':
        h = atoi(optarg);
        numopts++;
        break;
      case 't':
        tm = 't';
        numopts++;
        break;
      case 'm':
        numopts++;
        tm = 'm';
        break;
    }
  }
  int fnl = strlen(fn);
  char *fnext = fn + fnl - 3;
  if (strcmp("png", fnext) == 0) {  // .png
     isTiff = 'n';
  } else if (strcmp("tif", fnext) == 0) {  // .tif
     isTiff = 'y';
  } else if (strcmp("iff", fnext) == 0) {  // .tiff
     isTiff = 'y';
  } else {
    printf("Image must be .png, or .tif (.tiff). %s\n", fn);
    usage(argv[0]);
    return(-1);
  }

  if (strcmp("test.pnm", fn) == 0 || numopts < 6) {
    usage(argv[0]);
    return(-1);
  }

  printf("Flags: fn: %s x: %d y: %d w: %d h: %d ",
         fn, x, y, w, h);
  if (isTiff == 'n') {
    printf("png ");
  } else {
    printf("tiff ");
  }
  if (tm == 'm') {
    printf("meshes.\n");
  } else {
    printf("textures.\n");
  }
  if (isTiff == 'n') {
    char swatch[256];
    for (int i = 0; i < 256; i++) {
       swatch[i] = 'a' + (i % 26);
    }
    swatch[0] = '.';
    swatch[255] = '#';

    uint32 *buffer = ReadPNG(fn, x, y, w, h, tm, &img_xres, &img_yres);
    if (tm == 'm') {
      int res = EmitData(img_xres, img_yres, buffer, x, y, w, h, 0, 4);
    } else {  // textures
      int res = EmitPNGs(img_xres, img_yres, buffer, x, y, w, h, 0, 4);
    }

    printf("Image is <%d,%d>\n", img_xres, img_yres);
    for (int y = 0; y < 60; y++) {
      for (int x = 0; x < 78; x++) {
         uint32 pixel = buffer[y * img_xres + x];
         unsigned char *rgb = (char *) &pixel;
         unsigned char red = rgb[0];
         unsigned char green = rgb[1];
         unsigned char blue = rgb[2];
         printf("%c", swatch[green]);
      }
      printf("\n");
    }
    printf("\n image corner.\n" );
  }
  if (isTiff == 'y') {
    uint32 *buffer = ReadTIFF(fn, x, y, w, h, tm, &img_xres, &img_yres, &pdepth);
    if (tm == 'm') {
      int res = EmitData(img_xres, img_yres, buffer, x, y, w, h, 1, pdepth);
    } else {  // textures
      int res = EmitPNGs(img_xres, img_yres, buffer, x, y, w, h, 1, pdepth);
    }
  }
}

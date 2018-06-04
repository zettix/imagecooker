#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <tiffio.h>

#include <sys/stat.h>  // mkdir
#include <sys/types.h>  // mkdir

#include <endian.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <byteswap.h>
#include "emitdata.h"

void FormatPath(int x, int y, char *buf, char *parentdir) {
  sprintf(buf, "%s/data_%d_%d.dat",parentdir, x ,y);
}

int EmitData(uint32 dataXres, uint dataYres, uint32 *raster, int tileXres, int tileYres, int tileXcount, int tileYcount, int isTiff, int pdepth ) {
    char parentdir[20];
    struct stat statbuf;
    int xtiles = dataXres / tileXres;
    int ytiles = dataYres / tileYres;
    if (tileXcount != -1) {
       xtiles = tileXcount;
    }
    if (tileYcount != -1) {
       ytiles = tileYcount;
    }
    printf("X tiles: %d", xtiles);
    printf("Y tiles: %d", ytiles);

    // we don't care, just interpolate.  
    float mmmy = ((float) dataYres / (float) ytiles);
    float mmmx = ((float) dataXres / (float) xtiles);
    char dataout[80];

    printf("mmmx : %f mmy: %f\n", mmmx, mmmy);
    for (int tY = 0; tY < ytiles; tY++) {
      ///////
      snprintf(parentdir, 19, "meshes/d_%d", tY);
      printf("Creating directory... %s " , parentdir);
      mode_t mode = S_IRWXU;
      int st = stat(parentdir, &statbuf);
      if (! S_ISDIR(statbuf.st_mode)) {
        mkdir(parentdir, mode);
      }
      ///////
      for (int tX = 0; tX < xtiles; tX++) {
        printf("Writing tile %d %d " , tX , tY);
        printf(" Writing resx %d resy %d" , tileXres , tileYres);
        printf(" Writing dataxr %u %u", dataXres , dataYres);
        FormatPath(tX, tY, dataout, parentdir);
        printf("Writing to: %s \n", dataout);
        FILE* fp = fopen(dataout, "wb");
        if (fp) {
          uint32 goo =  bswap_32(tileXres);
          fwrite(&goo, sizeof(int), 1, fp);
          goo =  bswap_32(tileYres);
          fwrite(&goo, sizeof(int), 1, fp);
          for (int mY = 0; mY < tileYres; mY++) {
              float yPos = (float) (tY * dataYres) / (float) ytiles +
                           (float) mY * mmmy / (float) (tileYres - 1);
            for (int mX = 0; mX < tileXres; mX++) {
              float xPos = (float) (tX * dataXres) / (float) xtiles +
                           (float) mX * mmmx / (float) (tileXres - 1);
              //printf("probing %3.2f %3.2f", xPos, yPos);
              if (xPos >= dataXres) {
               xPos = dataXres - 1.0;
              }
              if (yPos >= dataYres) {
               yPos = dataYres - 1.0;
              }
              if (isTiff) {
                if (pdepth == 3 || pdepth == 4) {
                    char *snack = (char *) raster;
                    int snackidx = pdepth * (((int) yPos * dataXres + (int) xPos));
                    unsigned char *pixel = &(snack[snackidx]);
                    unsigned char red = pixel[0];
                    unsigned char green = pixel[1];
                    unsigned char blue = pixel[2];
                    int16 irp = ((int) red + (int) green + (int) blue);
                    int16 goo2 = bswap_16(irp);
                    fwrite(&goo2,  sizeof(int16), 1, fp);
                 } else if (pdepth == 2) {
                    char *snack = (char *) raster;
                    int snackidx = 2 *( ((int) yPos * dataXres + (int) xPos));
                    uint16 *irpp = (uint16 *) &(snack[snackidx]);
                    uint16 irp = *irpp;
                    int16 goo2 = bswap_16(irp);
                    fwrite(&goo2,  sizeof(int16), 1, fp);
                 } else {
                     printf("unknown pixel depth: %d\n", pdepth);
                 }
              } else {  // png
                int idx = ((int) yPos * dataXres + (int) xPos);
                uint32 pixel =  raster[idx];
                unsigned char *rgb = (char *) &pixel;
                unsigned char red = rgb[0];
                unsigned char green = rgb[1];
                unsigned char blue = rgb[2];
                int16 irp = ((int) red + (int) green + (int) blue);
                int16 goo2 = bswap_16(irp);
                fwrite(&goo2,  sizeof(int16), 1, fp);
              }
            }
          }
          fclose(fp);
        } else {
          printf("Could not open: %s", dataout);
        }
      }
    }
}

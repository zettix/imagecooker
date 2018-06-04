#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>

#include <png.h>
#include <sys/stat.h>  // mkdir
#include <sys/types.h>  // mkdir

#include <endian.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <byteswap.h>

#include "emitpngs.h"

void FormatImage(int x, int y, char *buf, char *parentdir) {
  sprintf(buf, "%s/tex_%d_%d.png",parentdir, x ,y);
}

int EmitPNGs(uint32 dataXres, uint dataYres, uint32 *raster, int tileXres, int tileYres, int tileXcount, int tileYcount, int isTiff, int pdepth) {
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep * row_pointers;
    int row_bytes;
    FILE *fp;
    int xtiles = dataXres / tileXres;
    int ytiles = dataYres / tileYres;
    if (tileXcount != -1) {
       xtiles = tileXcount;
    }
    if (tileYcount != -1) {
       ytiles = tileYcount;
    }
    uint32 index;
    struct stat statbuf;
    int y;
    printf("X tiles: %d", xtiles);
    printf("Y tiles: %d", ytiles);

    // we don't care, just interpolate.  
    float mmmy = ((float) dataYres / (float) ytiles);
    float mmmx = ((float) dataXres / (float) xtiles);
    char dataout[80];
    char parentdir[20];

    printf("mmmx : %f mmy: %f\n", mmmx, mmmy);
    for (int tY = 0; tY < ytiles; tY++) {
      snprintf(parentdir, 19, "textures/i_%d", tY);
      printf("Creating directory... %s " , parentdir);
      mode_t mode = S_IRWXU;

      int st = stat(parentdir, &statbuf);
      if (! S_ISDIR(statbuf.st_mode)) {
        mkdir(parentdir, mode);
      }
      for (int tX = 0; tX < xtiles; tX++) {
        printf("Writing tile [%d, %d] " , tX , tY);
        printf(" res[%d,%d] " , tileXres , tileYres);
        printf(" all< %u, %u>", dataXres , dataYres);
        FormatImage(tX, tY, dataout, parentdir);
        int st = stat(dataout, &statbuf);
        if (st == 0) {
          printf("Skipping: %s \n", dataout);
          continue;
        }
        printf("Writing to: %s at %d \n", dataout, index);

        FILE* fp = fopen(dataout, "wb");
        if (!fp) {
          return(0);
        }
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                          (png_voidp) NULL, NULL, NULL);
        if (!png_ptr) return (0);
        png_init_io(png_ptr, fp);
        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) {
          png_destroy_write_struct( &png_ptr, (png_infopp) NULL);
          return (0);
        }
        png_set_IHDR(png_ptr, info_ptr, tileXres, tileYres,  8, PNG_COLOR_TYPE_RGB,
          PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
        row_bytes = png_get_rowbytes(png_ptr,info_ptr);
        row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * tileYres);
        for (y=0; y<tileYres; y++) {
          row_pointers[y] = (png_byte*) malloc(row_bytes);
        }
        png_write_info(png_ptr, info_ptr);

        ///////////////////////////////////////////////////////////////
        for (int mY = 0; mY < tileYres; mY++) {
          float yPos = (float) tY * (float) dataYres / (float) ytiles + (float) mY * mmmy / (float) (tileYres - 1);
          //png_byte* row = row_pointers[tileYres - 1 - mY];
          png_byte* row = row_pointers[mY];
          for (int mX = 0; mX < tileXres; mX++) {
            float xPos = (float) tX * (float) dataXres / (float) xtiles + (float) mX * mmmx / (float) (tileXres - 1);
            if (xPos >= dataXres) {
              xPos = dataXres - 1.0;
            }
            if (yPos >= dataYres) {
              yPos = dataYres - 1.0;
            }
            index = ((uint32) yPos * (uint32) dataXres + (uint32) xPos);
            unsigned char red = 0;
            unsigned char green = 0;
            unsigned char blue = 0;
            if (isTiff) {
              if (pdepth == 4) {
                uint32 sample;
                sample = raster[index];
                red = TIFFGetR(sample);
                green = TIFFGetG(sample);
                blue = TIFFGetB(sample);
              } else if (pdepth == 3 || pdepth == 2) {
                unsigned char *bp = (unsigned char *) raster;
                int idxxx = index * pdepth;
                red = bp[idxxx];
                green = bp[idxxx + 1];
                blue = 0;
                if (pdepth == 3) {
                  blue = bp[idxxx + 2];
                }
              }
            } else {
                uint32 pixel = raster[index];
                unsigned char *rgb = (char *) &pixel;
                red = rgb[0];
                green = rgb[1];
                blue = rgb[2];
            }
            png_byte* ptr = &(row[mX*3]);
            ptr[0] = red;
            ptr[1] = green;
            ptr[2] = blue;
          }
        }
        png_write_image(png_ptr, row_pointers);
        png_write_end(png_ptr, NULL);
        /* cleanup heap allocation */
        for (y=0; y<tileYres; y++)
          free(row_pointers[y]);
        free(row_pointers);
        fclose(fp);
        png_destroy_write_struct(&png_ptr, &info_ptr);
     } // xrow
   } // yrow
}

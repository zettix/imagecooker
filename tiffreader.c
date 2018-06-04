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

uint32 * ReadTIFF(char *filename, int tileXres, int tileYres, int tileXcount, int tileYcount, char isdem, int *img_xres, int *img_yres, int *pdepth) {
  uint32 w, h, bpp, spp, photo, i;
  uint32 *raster;
  TIFF* tif;
  size_t npixels;
  unsigned long linesize;
  int splen;
  int loadpercent;

  tif = TIFFOpen(filename, "r");
  if (tif) {
    // http://www.libtiff.org/libtiff.html
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
	  npixels = w * h;
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bpp);
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
    TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photo);
    printf("TIFFTAG_IMAGEWIDTH %d\n", w);
    printf("TIFFTAG_IMAGELENGTH %d\n", h);
    printf("TIFFTAG_BITSPERSAMPLE %d\n", bpp);
    printf("TIFFTAG_SAMPLESPERPIXEL %d\n", spp);
    linesize = TIFFScanlineSize(tif);
    printf("Scanline size: %lu\n", linesize);
    *img_xres = w;
    *img_yres = h;
    if (bpp == 8) {
        *pdepth = 4;
        printf("IMAGE Holy cow, actually loaded NASA TEX data!\n");
        printf("Allocate! %ld 32 bits\n", npixels);
        raster = (uint32*) malloc(npixels * sizeof(uint32));
       printf("Allocate! %ld\n", (int64) raster);
	     if (raster != NULL) {
	        TIFFReadRGBAImage(tif, w, h, raster, 0);
	     }
	     TIFFClose(tif);
        
    } else if (bpp == 16) {
        *pdepth = 2;
        printf("IMAGE Holy cow, actually loaded NASA DEM data!\n");
        //ReadDem(tif, tileXres, tileYres, tileXcount, tileYcount);
        loadpercent = h / 100;
        raster = (uint32 *) malloc(npixels * 2);
        for (i = 0; i < h; i++) {
          splen = TIFFReadScanline(tif, &raster[i * linesize], i, 0);
          if ((i % loadpercent) == 0) printf("Splen: [%d] %d\n", (i * 100) / h, splen);
        }
	      TIFFClose(tif);
    } else {
       printf("UNKNOWN BPP TIFF\n");
       return NULL;
    }
    return raster;
 }
 printf("NULL TIFF\n");
 return NULL;
}

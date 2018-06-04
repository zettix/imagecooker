#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <tiffio.h>
#include <png.h>
#include <sys/stat.h>  // mkdir
#include <sys/types.h>  // mkdir
#include <endian.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <byteswap.h>

uint32 * ReadPNG(char *filename, int tileXres, int tileYres, int tileXcount, int tileYcount, char isdem, int *img_xres, int *img_yres) {
	  uint32 w, h;
	  size_t npixels;
    uint32 *raster;
    png_bytep * row_pointers;
    int pixel_len = 3;

    png_structp png_ptr;
    png_infop info_ptr;
    int row_bytes, x, y, number_of_passes,interlace_type;
    FILE *fp;
    uint32 index;
    struct stat statbuf;
    printf("Reading PNG: %s\n", filename);
    fp = fopen(filename, "rb");
    if (!fp) {
      return(0);
    }
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                     (png_voidp) NULL, NULL, NULL);
    if (!png_ptr) return (0);
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
       png_destroy_read_struct( &png_ptr, (png_infopp) NULL, NULL);
       return (0);
    }
    png_init_io(png_ptr, fp);
    png_read_info(png_ptr, info_ptr);

    w = png_get_image_width(png_ptr, info_ptr);
    h = png_get_image_height(png_ptr, info_ptr);
    interlace_type = png_get_interlace_type(png_ptr, info_ptr);
    printf("Input image is %d x %d", w, h);
	  npixels = w * h;
    raster = (uint32*) malloc(npixels * sizeof(uint32));

    int bitdepth =  png_get_bit_depth(png_ptr, info_ptr);

    if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB_ALPHA) {
       printf(">>RGBA<<\n");
       pixel_len = 4;
    } else {
       printf(">>RGB<<\n");
    }
    *img_xres = w;
    *img_yres = h;
    if (interlace_type == PNG_INTERLACE_ADAM7)
       number_of_passes
           = png_set_interlace_handling(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    /* read file */
    row_bytes = png_get_rowbytes(png_ptr,info_ptr);
    int superw = w * 3;
    printf("size of row of bytes: %d\n vs %d", row_bytes, superw);

    row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * h);
    for (y=0; y<h; y++) {
       row_pointers[y] = (png_byte*) calloc(row_bytes, sizeof(png_byte));
    }
    png_read_image(png_ptr, row_pointers);
    fclose(fp);
    for (y=0; y<h; y++) {
      png_byte* row = (png_bytep) &(row_pointers[y]);
      for (x = 0; x<w; x++) {
        png_byte* ptr = &(row[x*pixel_len]);
        int idx = (w * y + x);
         unsigned char red   = row_pointers[y][x * pixel_len];
         unsigned char green = row_pointers[y][x * pixel_len + 1];
         unsigned char blue  = row_pointers[y][x * pixel_len + 2];
        unsigned char *pixel = (unsigned char *) &(raster[idx]);
        pixel[0] = red;
        pixel[1] = green;
        pixel[2] = blue;
        pixel[3] = 255;
      } // x
    }  // y
    for (y=0; y<h; y++) free(row_pointers[y]);
    free(row_pointers);
    png_destroy_read_struct( &png_ptr, &info_ptr, NULL );
    return raster;
}

#ifndef COM_ZETTIX_TIFF_READER__
#define COM_ZETTIX_TIFF_READER__
#include <tiffio.h>
uint32 * ReadTIFF(char *filename, int tileXres, int tileYres, int tileXcount, int tileYcount, char isdem, int *img_xres, int *img_yres, int *pdepth);
#endif

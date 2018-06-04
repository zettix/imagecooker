#ifndef COM_ZETTIX_PNG_READER__
#define COM_ZETTIX_PNG_READER__
#include <tiffio.h>
uint32 * ReadPNG(char *filename, int tileXres, int tileYres, int tileXcount, int tileYcount, char isdem, int *img_xres, int *img_yres);
#endif

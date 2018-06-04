#ifndef COM_ZETTIX_EMITPNGS_H__
#define COM_ZETTIX_EMITPNGS_H__
#include <tiffio.h>
int EmitPNGs(uint32 dataXres, uint dataYres, uint32 *raster, int tileXres, int tileYres, int tileXcount, int tileYcount, int isTiff, int pdepth);
#endif

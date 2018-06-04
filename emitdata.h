#ifndef COM_ZETTIX_EMIT_DATA_H__
#define COM_ZETTIX_EMIT_DATA_H__
#include <tiffio.h>
int EmitData(uint32 dataXres, uint dataYres, uint32 *raster, int tileXres, int tileYres, int tileXcount, int tileYcount, int isTiff, int pdepth);
#endif

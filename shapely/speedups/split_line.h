
#include <geos_c.h>

GEOSGeometry *GEOSGeom_splitLineStringDateline_r(GEOSContextHandle_t handle,
															const GEOSCoordSequence *poLS,
                              double dfDateLineOffset);
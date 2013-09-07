#define SWAP_DBL(a,b) do { double tmp = a; a = b; b = tmp; } while(0)

#include <math.h>
#include "split_line.h"

typedef struct
{
	  double x;
		double y;
} point;

GEOSGeometry *GEOSGeom_splitLineStringDateline_r(GEOSContextHandle_t handle,
																          const GEOSCoordSequence *poLS,
																			    double dfDateLineOffset)
{
    double dfLeftBorderX = 180 - dfDateLineOffset;
    double dfRightBorderX = -180 + dfDateLineOffset;
    double dfDiffSpace = 360 - dfDateLineOffset;

    unsigned int i, size;
    //OGRLineString* poNewLS = new OGRLineString();
    //poMulti->addGeometryDirectly(poNewLS);
		
		GEOSCoordSeq_getSize_r(handle, poLS, &size);

		// Allocate temp array
		point* tmp = (point*) calloc(sizeof(point), size);
		unsigned int t, j = 0;

		// Allocate temp array of GEOSGeometry
		GEOSGeometry** subs = (GEOSGeometry**) calloc(sizeof(GEOSGeometry*), size);
		unsigned int L = 0;

		for(i=0;i<size;i++) 
    {
        double dfX, dfX1, dfY1, dfX2, dfY2, tp;
        GEOSCoordSeq_getX_r(handle, poLS, i, &dfX);
				GEOSCoordSeq_getX_r(handle, poLS, i-1, &dfX1);
				GEOSCoordSeq_getY_r(handle, poLS, i, &dfY2);
				GEOSCoordSeq_getY_r(handle, poLS, i-1, &dfY1);
        if (i > 0 && fabs(dfX - dfX1) > dfDiffSpace)
        {
            dfX2 = dfX;

            double dfX3;
						GEOSCoordSeq_getX_r(handle, poLS, i+1, &dfX3);
						if (dfX1 > -180 && dfX1 < dfRightBorderX && dfX2 == 180 &&
                i+1 < size &&
                dfX3 > -180 && dfX3 < dfRightBorderX)
            {
								tmp[j].x = -180;
							  tmp[j].y = dfY2;
								j++;
								//poNewLS->addPoint(-180, dfY2);

                i++;

								GEOSCoordSeq_getX_r(handle, poLS, i, &dfX2);
								GEOSCoordSeq_getY_r(handle, poLS, i, &dfY2);
								tmp[j].x = dfX2;
							  tmp[j].y = dfY2;
								j++;
								//poNewLS->addPoint(dfX2, dfY2);
                continue;
            }
            else if (dfX1 > dfLeftBorderX && dfX1 < 180 && dfX2 == -180 &&
                     i+1 < size &&
                     dfX3 > dfLeftBorderX && dfX3 < 180)
            {
								tmp[j].x = 180;
							  tmp[j].y = dfY2;
								j++;
								//poNewLS->addPoint(180, dfY2);

                i++;
								GEOSCoordSeq_getX_r(handle, poLS, i, &dfX2);
								GEOSCoordSeq_getY_r(handle, poLS, i, &dfY2);

								tmp[j].x = dfX2;;
							  tmp[j].y = dfY2;
								j++;
								//poNewLS->addPoint(dfX2, dfY2);
								continue;
            }

            if (dfX1 < dfRightBorderX && dfX2 > dfLeftBorderX)
            {
                SWAP_DBL(dfX1, dfX2);
                SWAP_DBL(dfY1, dfY2);
            }
            if (dfX1 > dfLeftBorderX && dfX2 < dfRightBorderX)
                dfX2 += 360;

            if (dfX1 <= 180 && dfX2 >= 180 && dfX1 < dfX2)
            {
                double dfRatio = (180 - dfX1) / (dfX2 - dfX1);
                double dfY = dfRatio * dfY2 + (1 - dfRatio) * dfY1;
								GEOSCoordSeq_getX_r(handle, poLS, i-1, &tp);
								tmp[j].x = tp > dfLeftBorderX ? 180 : -180;
							  tmp[j].y = dfY;
								j++;
                //poNewLS->addPoint(dfX1 > dfLeftBorderX ? 180 : -180, dfY);
                // Copy into a new CoordSeq
								GEOSCoordSequence* cs = GEOSCoordSeq_create_r(handle, j, 2);
								for (t=0;t<j;t++)
								{
										GEOSCoordSeq_setX_r(handle, cs, t, tmp[t].x);
										GEOSCoordSeq_setY_r(handle, cs, t, tmp[t].y);
								}
								subs[L] = GEOSGeom_createLineString_r(handle, cs);
                L++;

								//poNewLS = new OGRLineString();
                j=0;
                //poMulti->addGeometryDirectly(poNewLS);
								GEOSCoordSeq_getX_r(handle, poLS, i-1, &tp);
								tmp[j].x = tp > dfLeftBorderX ? -180 : 180;
							  tmp[j].y = dfY;
								j++;
								//poNewLS->addPoint(dfX1 > dfLeftBorderX ? -180 : 180, dfY);
            }
            else
            {
								GEOSCoordSequence* cs = GEOSCoordSeq_create_r(handle, j, 2);
								for (t=0;t<j;t++)
								{
										GEOSCoordSeq_setX_r(handle, cs, t, tmp[t].x);
							      GEOSCoordSeq_setY_r(handle, cs, t, tmp[t].y);
								}
								subs[L] = GEOSGeom_createLineString_r(handle, cs);
                L++;
								//poNewLS = new OGRLineString();
								j = 0;
                //poMulti->addGeometryDirectly(poNewLS);
            }
        }
				GEOSCoordSeq_getY_r(handle, poLS, i, &tp);
				tmp[j].x = dfX;
				tmp[j].y = tp;
				j++;
				//poNewLS->addPoint(dfX, poLS->getY(i));
    }
		GEOSCoordSequence* cs = GEOSCoordSeq_create_r(handle, j, 2);
		for (t=0;t<j;t++)
		{
				GEOSCoordSeq_setX_r(handle, cs, t, tmp[t].x);
				GEOSCoordSeq_setY_r(handle, cs, t, tmp[t].y);
		}
		subs[L] = GEOSGeom_createLineString_r(handle, cs);
		L++;
		GEOSGeometry* coll = GEOSGeom_createCollection_r(handle, 5, subs, L);
		free(tmp);
		free(subs);
		return coll;
}
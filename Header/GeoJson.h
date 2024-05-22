
#include "../Header/ListInt.h"
#ifndef PROJETS2_GEOJSON_H
#define PROJETS2_GEOJSON_H

#endif //PROJETS2_GEOJSON_H

void CreateGeoJsonSimplePath(ListInt* Path, double** Coord);
double** CreateCoordTab(FILE* file, int Nb);
void CreateGeoJsonTravelPath(ListInt* Path, ListInt* Checkpoint, double** Coord);
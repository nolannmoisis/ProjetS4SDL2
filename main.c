#include "Header/Settings.h"
#include "Header/Graph.h"
#include "Header/ListInt.h"
#include "Header/MapPtr.h"
#include "Header/MaxFlow.h"
#include "Header/ShortestPath.h"
#include "Header/TreeInt.h"
#include "Header/Destination.h"
#include "Header/TSP.h"
#include "Header/GeoJson.h"
#include <SDL.h>
#include <stdbool.h>

int main(int argc, char* args []){
    srand(time(NULL));
    setvbuf(stdout, NULL, _IONBF, 0);
    char* filename = "../Tests/4_TSP_ACO/input1.txt";
    TSP_ACO(filename);
    return 0;
}

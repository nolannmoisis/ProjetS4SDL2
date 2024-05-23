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

int main(int argc, char* args []){
    srand(time(NULL));
    
    char* filename = "Tests/4_TSP_ACO/input1.txt";
    TSP_ACO(filename);

    return 0;
}

#pragma once

#include "Settings.h"
#include "Graph.h"
#include "ShortestPath.h"
#include "ListInt.h"

typedef struct Destination_s{
    Graph* graph;

    Path*** path;

    int nbDestination;

    ListInt* allDestination;
} Destination;

Destination* DestinationCreate(int nbDestination);

void DestinationDestroy(Destination* destination);

Destination* DestinationPathMatrix(char* filename, int nbDestination, int* destination);

void DestinationPrintList(ListInt* list);

void DestinationPrintFull(Destination* destination);

void DestinationPrint(Destination* destination);

void pathMatrix(char* filename);
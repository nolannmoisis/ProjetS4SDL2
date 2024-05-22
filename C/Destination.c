#include "../Header/Destination.h"

Destination* DestinationCreate(int nbDestination){
    Destination* destination = (Destination*)calloc(1, sizeof(Destination));

    destination->graph = Graph_create(nbDestination);

    destination->nbDestination = nbDestination;

    destination->path = (Path***)calloc(nbDestination, sizeof(Path**));
    for (int i = 0; i < nbDestination; i++){
        destination->path[i] = (Path**)calloc(nbDestination, sizeof(Path*));
        for (int j = 0; j < nbDestination; j++){
            destination->path[i][j] = Path_create(0);
        }
    }

    destination->allDestination = ListInt_create();

    return destination;
}

void DestinationDestroy(Destination* destination){
    for (int i = 0; i < destination->nbDestination; i++){
        for (int j = 0; j < destination->nbDestination; j++){
            Path_destroy(destination->path[i][j]);
        }
        free(destination->path[i]);
    }
    free(destination->path);

    Graph_destroy(destination->graph);

    ListInt_destroy(destination->allDestination);

    free(destination);
}

Destination* DestinationPathMatrix(char* filename, int nbDestination, int* destination){
    Graph* graph = Graph_load(filename);

    Destination* dest = DestinationCreate(nbDestination);

    for (int i = 0; i < nbDestination; i++){
        for (int j = i; j < nbDestination; j++){
            Path* path = Graph_shortestPath(graph, destination[i], destination[j]);
            Path* reversePath = PathReverse_copy(path);

            dest->path[i][j] = path;
            dest->path[j][i] = reversePath;

            ArcData data;
            data.weight = path->distance;

            Graph_setArc(dest->graph, i, j, &data);
            Graph_setArc(dest->graph, j, i, &data);

            printf(".");
            fflush(stdout);
        }
        printf("\n_");
        fflush(stdout);
    }

    Graph_destroy(graph);

    return dest;
}

void DestinationPrintList(ListInt* list){
    assert(list);

    ListIntNode *sentinel = &(list->sentinel);
    ListIntNode *curr = sentinel->next;
    printf("%d ", list->nodeCount);
    while (curr != sentinel->prev)
    {
        printf("%d ", curr->value);
        curr = curr->next;
    }
    if (curr != sentinel)
    {
        printf("%d", curr->value);
    }
    printf("\n");
}

void DestinationPrintFull(Destination* destination){
    for (int u = 0; u < destination->nbDestination; u++){
        for (ArcList* arc = Graph_getArcList(destination->graph, u); arc != NULL; arc = arc->next){
            ArcData* data = Graph_getArc(destination->graph, u, arc->target);
            printf("%d %d %.1f ", u, arc->target, data->weight);
            if(data->weight > 0.0f) DestinationPrintList(destination->path[u][arc->target]->list);
            else printf("\n");
        }
    }
}

void DestinationPrint(Destination* destination){
    for (int u = 0; u < destination->graph->size; u++){
        for (ArcList* arc = Graph_getArcList(destination->graph, u); arc != NULL; arc = arc->next){
            ArcData* data = Graph_getArc(destination->graph, u, arc->target);
            printf("%d %d %.1f\n", u, arc->target, data->weight);
        }
    }
}

void pathMatrix(char* filename){
    char fileGraphName[256];
    char fileInterName[256];

    FILE* file = fopen(filename, "r");
    if(file == NULL) return;

    #ifdef _MSC_VER
    fscanf(file, "%s", fileGraphName);
    fscanf(file, "%s", fileInterName);
    #else
    char tmp;
    fscanf(file, "%c", &tmp);
    fscanf(file, "%s\n", fileGraphName);
    fscanf(file, "%c", &tmp);
    fscanf(file, "%s", fileInterName);
    #endif

    int nbDestination = 0;
    fscanf(file, "%d", &nbDestination);

    int* destination = (int*)calloc(nbDestination, sizeof(int));

    for (int i = 0; i < nbDestination; i++){
        fscanf(file, "%d", &(destination[i]));
    }

    Destination* dest = DestinationPathMatrix(fileGraphName, nbDestination, destination);
    Graph_print(dest->graph);

    printf("\n----FullPrint----\n");
    DestinationPrintFull(dest);
    printf("-----------------\n");
    printf("\n----Print----\n");
    DestinationPrint(dest);
    printf("-------------\n\n");

    fclose(file);
    free(destination);
    DestinationDestroy(dest);
}
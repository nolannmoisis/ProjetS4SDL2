#include "../Header/Destination.h"

void DestinationWrite(Destination* destination, char* filename){
    FILE* file = fopen(filename, "w");
    if(file == NULL) return;

    int size = destination->nbDestination;

    fprintf(file, "%d %d", size, (size * size));
    for (int u = 0; u < size; u++){
        for (int v = 0; v < size; v++){
            fprintf(file, "\n%d %d %.1f %d", u, v, Graph_getArc(destination->graph, u, v)->weight, destination->path[u][v]->list->nodeCount);
            ListIntIter* iterList = ListIntIter_create(destination->path[u][v]->list);
            AssertNew(iterList);

            while (ListIntIter_isValid(iterList)){
                int actualNode = ListIntIter_get(iterList);
                fprintf(file, " %d", actualNode);
                ListIntIter_next(iterList);
            }

            ListIntIter_destroy(iterList);
        }
    }

    fprintf(file, "\n%d", destination->allDestination->nodeCount);
    ListIntIter* iterList = ListIntIter_create(destination->allDestination);
    AssertNew(iterList);

    while (ListIntIter_isValid(iterList)){
        int actualNode = ListIntIter_get(iterList);
        fprintf(file, " %d", actualNode);
        ListIntIter_next(iterList);
    }

    ListIntIter_destroy(iterList);

    fclose(file);
}

Destination* DestinationLoad(char* filename){
    FILE* file = fopen(filename, "r");
    if(file == NULL) return NULL;

    int size = 0;
    int nbArc = 0;
    int u = 0;
    int v = 0;
    int actualNode = 0;

    fscanf(file, "%d %d", &size, &nbArc);

    Destination* dest = DestinationCreate(size);

    dest->path = (Path***)calloc(size, sizeof(Path**));
    for (int i = 0; i < size; i++){
        dest->path[i] = (Path**)calloc(size, sizeof(Path*));
    }

    for (int i = 0; i <= nbArc; i++){
        ArcData data;
        fscanf(file, "%d %d %f %d", &u, &v, &data.weight, &dest->path[u][v]->list->nodeCount);
        dest->path[u][v]->distance = data.weight;
        Graph_setArc(dest->graph, u, v, &data);
        for (int j = 0; j < dest->path[u][v]->list->nodeCount; j++){
            fscanf(file, "%d", &actualNode);
            ListInt_insertLast(dest->path[u][v]->list, actualNode);
        }
    }

    return NULL;
}

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
        }
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

    char tmp;
    fscanf(file, "%c", &tmp);
    fscanf(file, "%s\n", fileGraphName);
    fscanf(file, "%c", &tmp);
    fscanf(file, "%s", fileInterName);

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
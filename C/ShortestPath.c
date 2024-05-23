#include "../Header/ShortestPath.h"

TasData* TasBinaire_create(int size){
    TasData* tas = (TasData*)calloc(size, sizeof(TasData));
    AssertNew(tas);

    #pragma omp parallel for
    for (int i = 0; i < size; i++){
        tas[i].idNode = -1;
        tas[i].distances = INFINITY;
    }

    return tas;
}

void TasBinaire_print(TasData *tas){
    assert(tas);
    int i = 0;
    while (tas[i].idNode != -1){
        printf("[%d|%.1f]-", tas[i].idNode, tas[i].distances);
        i++;
    }
    printf("\n");
}

void TasBinaire_Destroy(TasData *tas){
    assert(tas);
    free(tas);
}

int pereIndex(int index){
    if(index == 0) return -1;
    return ((index-1)/2);
}

int filsGaucheIndex(int index){
    return ((2*index)+1);
}

int filsDroitIndex(int index){
    return ((2*index)+2);
}

void TasBinaire_Permute(TasData *tas, int indexFirst, int indexSecond){
    assert(tas);
    TasData tmp;
    /*tmp.idNode = tas[indexFirst].idNode;
    tmp.distances = tas[indexFirst].distances;
    tas[indexFirst].idNode = tas[indexSecond].idNode;
    tas[indexFirst].distances = tas[indexSecond].distances;
    tas[indexSecond].idNode = tmp.idNode;
    tas[indexSecond].distances = tmp.distances;*/

    tmp = tas[indexFirst];
    tas[indexFirst] = tas[indexSecond];
    tas[indexSecond] = tmp;
}

void TasBinaireInsert(TasData *tas, int id, float distance, int size){
    assert(tas);
    int i = 0;
    while ((tas[i].idNode != -1) && (i < size)) {
        if(tas[i].idNode == id){
            tas[i].distances = distance;
            return;
        }
        i++;
    }

    if (i >= size){
        printf("Erreur : dépassement de la taille du tas.\n");
        return;
    }

    tas[i].idNode = id;
    tas[i].distances = distance;
    int fatherIndex = pereIndex(i);

    while ((fatherIndex != -1) && (tas[fatherIndex].distances > tas[i].distances)){
        TasBinaire_Permute(tas, fatherIndex, i);

        i = fatherIndex;
        fatherIndex = pereIndex(i);
    }
}

TasData TasBinairePop(TasData *tas){
    assert(tas);
    TasData racine = tas[0];

    int i = 0;
    while (tas[i].idNode != -1) i++;

    tas[0].idNode = tas[i-1].idNode;
    tas[0].distances = tas[i-1].distances;

    tas[i-1].idNode = -1;
    tas[i-1].distances = INFINITY;

    i = 0;
    int filsgauche = filsGaucheIndex(i);
    int filsdroit = filsDroitIndex(i);

    while (tas[filsgauche].idNode != -1){
        if(tas[filsdroit].idNode != -1){
            if(tas[filsdroit].distances > tas[filsgauche].distances){
                if(tas[filsgauche].distances < tas[i].distances){
                    TasBinaire_Permute(tas, filsgauche, i);

                    i = filsgauche;
                }
                else return racine;
            }
            else{
                if(tas[filsdroit].distances < tas[i].distances){
                    TasBinaire_Permute(tas, filsdroit, i);

                    i = filsdroit;
                }
                else return racine;
            }
        }
        else{
            if(tas[filsgauche].distances < tas[i].distances){
                TasBinaire_Permute(tas, filsgauche, i);

                return racine;
            }
            else return racine;
        }
        filsgauche = filsGaucheIndex(i);
        filsdroit = filsDroitIndex(i);
    }
    return racine;
}

TasData TasBinaire_GetIndex(TasData* tas, int index){
    assert(tas);
    int i = 0;
    TasData search;
    search.idNode = index;
    search.distances = INFINITY;
    while (tas[i].idNode != -1){
        if(tas[i].idNode == index){
            search.distances = tas[i].distances;
            return search;
        }
        i++;
    }
    return search;
}

bool TasBinaire_IsEmpty(TasData* tas){
    assert(tas);
    if(tas[0].idNode == -1) return true;
    return false;
}

void Graph_dijkstra(Graph *graph, int start, int end, int *predecessors, float *distances){
    assert(graph);
    assert(distances);
    assert(predecessors);

    int size = graph->size;

    #pragma omp parallel for
    for (int i = 0; i < size; i++){
        predecessors[i] = -1;
        distances[i] = INFINITY;
    }

    distances[start] = 0.0f;

    TasData* tas = TasBinaire_create(size);
    AssertNew(tas);
    TasBinaireInsert(tas, start, distances[start], size);

    while (!TasBinaire_IsEmpty(tas)){
        //TasBinaire_print(tas);
        TasData u = TasBinairePop(tas);

        if((u.idNode == end) || (u.distances == INFINITY)){
            TasBinaire_Destroy(tas);
            return;
        }

        for (ArcList* arc = Graph_getArcList(graph, u.idNode); arc != NULL; arc = arc->next){
            if ((u.distances + arc->data.weight) < distances[arc->target]){
                predecessors[arc->target] = u.idNode;
                distances[arc->target] = u.distances + arc->data.weight;
                TasBinaireInsert(tas, arc->target, distances[arc->target], size);
            }
        }
    }
    TasBinaire_Destroy(tas);
    return;
}

Path *Graph_dijkstraGetPath(int *predecessors, float *distances, int end)
{
    assert(predecessors);
    assert(distances);

    if(distances[end] == INFINITY) return NULL;
    Path *path = Path_create(end);

    path->distance = distances[end];

    while (predecessors[end] != -1){
        ListInt_insertFirst(path->list, predecessors[end]);
        end = predecessors[end];
    }

    return path;
}

Path *Graph_shortestPath(Graph *graph, int start, int end){
    assert(graph);

    int* predecessors = (int*)calloc(graph->size, sizeof(int));
    float* distances = (float*)calloc(graph->size, sizeof(float));

    Graph_dijkstra(graph, start, end, predecessors, distances);

    Path *path = Graph_dijkstraGetPath(predecessors, distances, end);

    free(predecessors);
    free(distances);

    return path;
}

Path *Path_create(int start)
{
    Path *path = (Path *)calloc(1, sizeof(Path));
    AssertNew(path);

    path->distance = 0.0f;
    path->list = ListInt_create();
    ListInt_insertLast(path->list, start);

    return path;
}

void Path_destroy(Path *path)
{
    if (path == NULL) return;

    ListInt_destroy(path->list);
    free(path);
}

Path *Path_copy(Path *path)
{
    assert(path && path->list);

    Path *newPath = (Path *)calloc(1, sizeof(Path));
    AssertNew(newPath);

    newPath->distance = path->distance;
    newPath->list = ListInt_copy(path->list);

    return newPath;
}

Path *PathReverse_copy(Path *path){
    assert(path && path->list);

    Path *newPath = (Path *)calloc(1, sizeof(Path));
    AssertNew(newPath);

    newPath->distance = path->distance;
    newPath->list = ListInt_Reverse(path->list);

    return newPath;
}

void Path_insertFirst(Path *path, int nodeID, float weight)
{
    assert(path && path->list);

    path->distance += weight;
    ListInt_insertFirst(path->list, nodeID);
}

void Path_insertLast(Path *path, int nodeID, float weight)
{
    assert(path && path->list);

    path->distance += weight;
    ListInt_insertLast(path->list, nodeID);
}

void Path_print(Path *path)
{
    if (path == NULL)
    {
        printf("path : NULL\n");
        return;
    }

    printf("path (distance = %f) : ", path->distance);
    ListInt_print(path->list);
}

void printLIst(ListInt* list){
    assert(list);

    ListIntNode *sentinel = &(list->sentinel);
    ListIntNode *curr = sentinel->next;
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

void Dijkstra(char* filename){
    char fileGraphName[256];
    char fileInterName[256];

    FILE* file = fopen(filename, "r");

    char tmp;
    fscanf(file, "%c", &tmp);
    fscanf(file, "%s\n", fileGraphName);
    fscanf(file, "%c", &tmp);
    fscanf(file, "%s", fileInterName);

    int start = 0;
    int end = 0;

    fscanf(file, "%d %d", &start, &end);

    fclose(file);

    Graph* graph = Graph_load(fileGraphName);

    printf("Fin load\n");

    Path* path = Graph_shortestPath(graph, start, end);

    printf("%.1f\n", path->distance);
    printf("%d\n", path->list->nodeCount);
    printLIst(path->list);


    FILE* fileInter = fopen(fileInterName, "r");
    int nb = 0;
    fscanf(fileInter,"%d",&nb);
    double** coord= CreateCoordTab(fileInter, nb);
    CreateGeoJsonTravelPath(path->list, path->list, coord);

    for(int i=0;i<nb;i++){
        free(coord[i]);
    }
    free(coord);

    fclose(fileInter);

    Path_destroy(path);

    Graph_destroy(graph);
}
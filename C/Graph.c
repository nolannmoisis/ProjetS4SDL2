#include "../Header/Graph.h"

Graph *Graph_create(int size)
{
    if (!size) return NULL;

#pragma region Graph Allocation
    Graph *graph = (Graph*)calloc(1, sizeof(Graph));
    graph->size = size;
    graph->nodes = (GraphNode*)calloc(size, sizeof(GraphNode));

    for (int i = 0; i < size; i++){
        GraphNode *node = &(graph->nodes[i]);

        node->arcList = NULL;
        node->arcCount = 0;

        graph->nodes[i].id = i;
    }
#pragma endregion

    return graph;
}

void Graph_destroy(Graph *graph)
{
    if (!graph) return;

    if (graph->nodes)
    {
        for (int i = 0; i < graph->size; i++)
        {
            GraphNode *node = &(graph->nodes[i]);
            ArcList *currArc = node->arcList;
            while (currArc != NULL)
            {
                ArcList *nextArc = currArc->next;
                free(currArc);
                currArc = nextArc;
            }
        }
        free(graph->nodes);
    }

    free(graph);
}

void Graph_setArc(Graph *graph, int u, int v, ArcData *data)
{
    if (!graph) return;
    if (!data) return;

    for (ArcList* arc = Graph_getArcList(graph, u); arc != NULL; arc = arc->next){
        if (arc->target == v){
            arc->data = *data;
            return;
        }
    }

    graph->nodes[u].arcCount++;

    ArcList *arcNew = (ArcList*)calloc(1, sizeof(ArcList));
    arcNew->data = *data;
    arcNew->next = Graph_getArcList(graph, u);
    arcNew->target = v;

    graph->nodes[u].arcList = arcNew;
}

void Graph_removeArc(Graph *graph, int u, int v)
{
    if (graph->nodes[u].arcCount == 0) return;
    if (graph->nodes[u].arcCount == 1){
        if(graph->nodes[u].arcList->target == v){
            free(graph->nodes[u].arcList);
            graph->nodes[u].arcCount--;
            graph->nodes[u].arcList = NULL;
            return;
        }
    }
    for (ArcList* arc = Graph_getArcList(graph, u); arc->next != NULL; arc = arc->next){
        if (arc->target == v){
            graph->nodes[u].arcCount--;
            graph->nodes[u].arcList = arc->next;
            free(arc);
            return;
        }
        if (arc->next->target == v){
            graph->nodes[u].arcCount--;
            arc->next = arc->next->next;
            return;
        }
    }
}

ArcData *Graph_getArc(Graph *graph, int u, int v)
{
    for (ArcList* arc = Graph_getArcList(graph, u); arc != NULL; arc = arc->next){
        if (arc->target == v){
            return &arc->data;
        }
    }
    return NULL;
}


Graph *Graph_load(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL){
        return NULL;
    }

    int size = 0;
    int nbArc = 0;
    int u = 0;
    int v = 0;
    ArcData* data = (ArcData*)calloc(1, sizeof(ArcData));

    fscanf(file, "%d %d", &size, &nbArc);

    Graph *graph = Graph_create(size);

    for (int i = 0; i <= nbArc; i++){
        fscanf(file, "%d %d %f", &u, &v, &data->weight);
        Graph_setArc(graph, u, v, data);
    }

    free(data);
    fclose(file);
    return graph;
}

void Graph_print(Graph *graph)
{
    assert(graph);

    printf("size : %d\n", graph->size);
    for (int i = 0; i < graph->size; i++){
        printf("[%d]->", graph->nodes[i].id);
        for (ArcList *arc = Graph_getArcList(graph, i); arc != NULL; arc = arc->next)
        {
            printf("%d:%.2f->", arc->target, arc->data.weight);

        }
        printf("NULL\n");
    }
}

bool Graph_isDirected(Graph *graph){
    assert(graph);

    int size = Graph_size(graph);
    ArcData* data = NULL;

    for (int i = 0; i < size; i++){
        int u = graph->nodes[i].id;
        for (ArcList* arc = Graph_getArcList(graph, u); arc != NULL; arc = arc->next){
            int v = arc->target;
            data = Graph_getArc(graph, v, u);
            if(data == NULL) return true;
        }
    }
    return false;
}

void Graph_degreeTab(Graph *graph, int *degreeTab){
    assert(graph);

    int size = Graph_size(graph);

    for (int actualNode = 0; actualNode < size; actualNode++){
        degreeTab[actualNode] = Graph_getArcCount(graph, actualNode);
    }
}

bool isCompleted(int *tab, int size){
    for (int i = 0; i < size; i++){
        if(tab[i] == -1) return false;
    }
    return true;
}

void availableColor(Graph *graph, int* colorTab, int id){
    int color = 0;

    while (colorTab[id] == -1){
        int ok = 1;
        for (ArcList *arc = Graph_getArcList(graph, id); arc != NULL; arc = arc->next){
            if (color == colorTab[arc->target]){
                color++;
                ok = 0;
                break;
            }
        }
        if (ok) colorTab[id] = color;
    }
}

int *Graph_dSatur(Graph *graph, int *colorCount){
    assert(graph);

    *colorCount = 0;

#pragma region Properties
    int size = Graph_size(graph);

    int *degreeTab = (int*)calloc(size, sizeof(int));
    Graph_degreeTab(graph, degreeTab);

    int *colorTab = (int*)calloc(size, sizeof(int));
    for (int i = 0; i < size; i++) colorTab[i] = -1;

    int *satTab = (int*)calloc(size, sizeof(int));
#pragma endregion

    while (!isCompleted(colorTab, size)){
        int maxSat = -1;
        int maxId = -1;

        for (int i = 0; i < size; i++){
            if(colorTab[i] == -1){
                if ((maxSat == satTab[i]) && (degreeTab[maxId] < degreeTab[i])) maxId = i;

                else if ((maxSat < satTab[i])){
                    maxSat = satTab[i];
                    maxId = i;
                }
            }
        }

        availableColor(graph, colorTab, maxId);

        if (*colorCount < colorTab[maxId]){
            *colorCount = colorTab[maxId];
        }
    }

    (*colorCount)++;

    free(degreeTab);
    free(satTab);

    return colorTab;
}

int Graph_maxDegree(Graph *graph){
    assert(graph);

    int max = graph->nodes[0].arcCount;

    for (int i = 1; i < graph->size; i++){
        if (max < graph->nodes[i].arcCount) max = graph->nodes[i].arcCount;
    }

    return max;
}

bool Graph_isColorationValid(Graph *graph, int *colors){
    assert(graph);

    for (int id = 0; id < graph->size; id++){
        for (ArcList *arc = Graph_getArcList(graph, id); arc != NULL; arc = arc->next){
            if (colors[id] == colors[arc->target]) return false;
        }
    }

    return true;
}
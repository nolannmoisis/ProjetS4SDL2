#include "../Header/MaxFlow.h"

Graph *Graph_maxFlow(Graph *capacities, int s, int t){
    Graph* residual = Graph_initResidual(capacities);
    ListInt* file = Graph_findImprovingPath(residual, s, t);

    if (file == NULL) {
        return NULL;
    }

    while((file != NULL) && (!(ListInt_isEmpty(file)))){
        float score = Graph_getImprovingPathScore(residual, file);
        Graph_updateResidual(residual, file, score);
        ListInt_destroy(file);
        file = Graph_findImprovingPath(residual, s, t);
    }
    
    capacities = Graph_createFlowFromResidual(residual, capacities);

    return capacities;
}

Graph *Graph_initResidual(Graph *capacities){
    Graph* residualGraph = capacities;
    int size = Graph_size(residualGraph);

    ArcData* data = (ArcData*)calloc(1, sizeof(ArcData));
    data->weight = 0;

    for (int u = 0; u < size; u++){
        for (ArcList *arc = Graph_getArcList(residualGraph, u); arc != NULL; arc = arc->next){
            if(arc->data.weight > 0) Graph_setArc(residualGraph, arc->target, u, data);
        }
    }

    free(data);

    return residualGraph;
}

ListInt *Graph_findImprovingPath(Graph *residual, int s, int t){
    int size = Graph_size(residual);
    int* pred = (int*)calloc(size, sizeof(int));
    for (int i = 0; i < size; i++){
        pred[i] = -1;
    }
    
    ListInt* file = ListInt_create();
    ListInt_enqueue(file, s);

    while ((!ListInt_isEmpty(file)) && (pred[t] == -1)){
        int u = ListInt_dequeue(file);
        
        for (ArcList *arc = Graph_getArcList(residual, u); arc != NULL; arc = arc->next){
            int v = arc->target;
            if ((pred[v] == -1) && (arc->data.weight > 0)){
                pred[v] = u;
                if(v == t) break;
                ListInt_enqueue(file, v);
            }
        }
    }

    ListInt* path = ListInt_create();
    if (pred[t] != -1) {
        for (int v = t; v != s; v = pred[v]) {
            ListInt_insertFirst(path, v);
        }
        ListInt_insertFirst(path, s);
    }
    ListInt_destroy(file);
    free(pred);
    if(ListInt_isEmpty(path)) return NULL;
    return path;
}

float Graph_getImprovingPathScore(Graph *residual, ListInt *path){
    float improvingPathScore = INFINITY;

    ListIntNode *sentinel = &(path->sentinel);
    ListIntNode *node = sentinel->next;
    while (node->next != sentinel){
        int u = node->value;
        int v = node->next->value;

        ArcData* uv = Graph_getArc(residual, u, v);

        if(improvingPathScore > uv->weight) improvingPathScore = uv->weight;

        node = node->next;
    }
    return improvingPathScore;
}

void Graph_updateResidual(Graph *residual, ListInt *path, float score){
    ListIntNode *sentinel = &(path->sentinel);
    ListIntNode *node = sentinel->next;
    while (node->next != sentinel){
        int u = node->value;
        int v = node->next->value;

        ArcData* uv = Graph_getArc(residual, u, v);
        uv->weight -= score;
        Graph_setArc(residual, u, v, uv);

        ArcData* vu = Graph_getArc(residual, v, u);
        vu->weight += score;
        Graph_setArc(residual, v, u, vu);

        node = node->next;
    }
}

Graph *Graph_createFlowFromResidual(Graph *residual, Graph *capacities){
    int size = Graph_size(capacities);
    ArcData* dataUV = (ArcData*)calloc(1, sizeof(ArcData));

    for (int u = 0; u < size; u++){
        for (ArcList *arc = Graph_getArcList(capacities, u); arc != NULL; arc = arc->next){
            dataUV = Graph_getArc(residual, arc->target, u);
            Graph_setArc(capacities, u, arc->target, dataUV);
        }
    }
    
    return capacities;
}
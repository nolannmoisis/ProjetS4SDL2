#include "../Header/TSP.h"
#include <SDL.h>
#include <stdbool.h>

int argmin(Path* path, Graph* graph, int start){
    float minDistance = INFINITY;
    int min = 0;
    for (int v = 0; v < graph->size; v++){
        if((v != start) && (!ListInt_isIn(path->list, v))){
            ArcData* data = Graph_getArc(graph, start, v);
            if(minDistance > data->weight){
                minDistance = data->weight;
                min = v;
            }
        }
    }
    return min;
}

Path* Graph_tspFromHeuristic(Graph* graph, int station){
    Path* tourne = Path_create(station);
    int prev = station;

    for (int i = 1; i < graph->size; i++){
        int next = argmin(tourne, graph, prev);
        ArcData* data = Graph_getArc(graph, prev, next);
        tourne->distance += data->weight;
        ListInt_insertLast(tourne->list, next);
        prev = next;
    }

    ArcData* data = Graph_getArc(graph, prev, station);
    tourne->distance += data->weight;
    ListInt_insertLast(tourne->list, station);

    return tourne;
}

Graph* Graph_PheromoneCreate(Graph* graph){
    Graph* pheromone = Graph_create(graph->size);

    ArcData init;
    init.weight = 1.0f;

    for (int u = 0; u < graph->size; u++){
        for (ArcList* arc = Graph_getArcList(graph, u); arc != NULL; arc = arc->next){
            Graph_setArc(pheromone, u, arc->target, &init);
        }
    }

    return pheromone;
}

Path* Graph_tspFromACO(Graph* graph, int station, int iterationCount, int antCount, float alpha, float beta, float rho, float q){
    Path* bestTourne = (Path *)calloc(1, sizeof(Path));
    AssertNew(bestTourne);
    Graph* pheromone = Graph_PheromoneCreate(graph);
    AssertNew(pheromone);

    Path** tourne = (Path**)calloc(antCount, sizeof(Path*));

    for (int i = 0; i < iterationCount; i++){
#pragma omp parallel for
        for (int j = 0; j < antCount; j++){
            tourne[j] = Graph_acoConstructPath(graph, pheromone, station, alpha, beta);
        }
        Graph_acoPheromoneGlobalUpdate(pheromone, rho);
        for (int j = 0; j < antCount; j++){
            if((bestTourne->list == NULL) || (tourne[j]->distance < bestTourne->distance)){
                Path_destroy(bestTourne);
                bestTourne = Path_copy(tourne[j]);
            }
            Graph_acoPheromoneUpdatePath(pheromone, tourne[j], q);
            Path_destroy(tourne[j]);
        }
    }

    Graph_destroy(pheromone);
    free(tourne);

    return bestTourne;
}

float* Graph_acoGetProbabilities(Graph* graph, Graph* pheromones, int station, bool* explored, float alpha, float beta){
    float* proba = (float*)calloc(graph->size, sizeof(float));
    float sumTauxByDist = 0.0f;
    int count = 0;
    for (ArcList* arc = Graph_getArcList(pheromones, station); arc != NULL; arc = arc->next){
        if(!explored[arc->target]){
            ArcData* distance = Graph_getArc(graph, station, arc->target);
            proba[arc->target] = powf(arc->data.weight, alpha) * powf(1/(distance->weight), beta);
            sumTauxByDist += proba[arc->target];
            count++;
        }
    }

    for (ArcList* arc = Graph_getArcList(pheromones, station); arc != NULL; arc = arc->next){
        if(!explored[arc->target]){
            if(sumTauxByDist < 1e-15) proba[arc->target] = 1.0f/(count);
            else proba[arc->target] = proba[arc->target]/sumTauxByDist;
        }
    }

    /*float sommeProba = 0.0f;
    for (int i = 0; i < graph->size; i++){
        sommeProba += proba[i];
    }
    assert((sommeProba > 0.9f) && (sommeProba < 1.1f));*/

    return proba;
}

Path* Graph_acoConstructPath(Graph* graph, Graph* pheromones, int station, float alpha, float beta){
    Path* tourne = Path_create(station);

    bool* explored = (bool*)calloc(graph->size, sizeof(bool));
    explored[station] = true;

    int prev = station;
    int next = -1;

    for (int i = 1; i < graph->size; i++){
        float* proba = Graph_acoGetProbabilities(graph, pheromones, prev, explored, alpha, beta);
        next = -1;

        //unsigned int r = ((rand() ^ (rand() << 15)) & 0x7FFFFFFF);

        float probaRandom = (float)rand()/RAND_MAX;
        float totalProba = 0.0f;
        int idNext = -1;

        for (ArcList* arc = Graph_getArcList(graph, prev); arc != NULL; arc = arc->next){
            if(!explored[arc->target]){
                if (idNext == -1){
                    idNext = arc->target;
                }
                totalProba += proba[arc->target];
                if(totalProba > probaRandom){
                    next = arc->target;
                    break;
                }
            }
        }

        if(next == -1) next = idNext;

        explored[next] = true;
        ArcData* data = Graph_getArc(graph, prev, next);
        tourne->distance += data->weight;
        ListInt_insertLast(tourne->list, next);
        prev = next;

        free(proba);
    }

    ArcData* data = Graph_getArc(graph, prev, station);
    tourne->distance += data->weight;
    ListInt_insertLast(tourne->list, station);

    free(explored);

    return tourne;
}

void Graph_acoPheromoneUpdatePath(Graph* pheromones, Path* path, float q){
    int start = ListInt_dequeue(path->list);
    int actual = start;
    int next = 0;

    while (!ListInt_isEmpty(path->list)){
        next = ListInt_dequeue(path->list);

        ArcData* actualTauxUV = Graph_getArc(pheromones, actual, next);
        actualTauxUV->weight += (q/(path->distance));

        if(next == start) break;

        actual = next;
    }

    return;
}

void Graph_acoPheromoneGlobalUpdate(Graph* pheromones, float rho){
    for (int u = 0; u < pheromones->size; u++){
        for (ArcList* arc = Graph_getArcList(pheromones, u); arc->next != NULL; arc = arc->next){
            ArcData* actualPheromoneUV = Graph_getArc(pheromones, u, arc->target);
            actualPheromoneUV->weight = ((1-rho) * actualPheromoneUV->weight);
        }
    }

    return;
}

ListInt* pathAllCheckpoint(Destination* dest, Path* tourne){
    int start = ListInt_popFirst(tourne->list);
    int bin = 0;
    ListInt* list = ListInt_create();
    bool first = true;
    while (!ListInt_isEmpty(tourne->list)){
        int next = ListInt_popFirst(tourne->list);
        if(first == true){
            ListInt_concatenate(list, dest->path[start][next]->list);
            first = false;
        }
        else {
            bin = ListInt_popFirst(dest->path[start][next]->list);
            ListInt_concatenate(list, dest->path[start][next]->list);
        }
        start = next;
    }

    bin += 1;

    return list;
}

void TSP_Heuristic(char* filename){
    char fileGraphName[256];
    char fileInterName[256];

    FILE* file = fopen(filename, "r");
    if(file == NULL) return;

    fscanf(file, "%s", fileGraphName);
    fscanf(file, "%s", fileInterName);

    int nbDestination = 0;
    fscanf(file, "%d", &nbDestination);

    int* destination = (int*)calloc(nbDestination, sizeof(int));

    for (int i = 0; i < nbDestination; i++){
        fscanf(file, "%d", &(destination[i]));
    }

    Destination* dest = DestinationPathMatrix(fileGraphName, nbDestination, destination);

    for (int i = 0; i < nbDestination; i++){
        ListInt_insertFirst(dest->allDestination, destination[i]);
    }

    Path* tourne = Graph_tspFromHeuristic(dest->graph, 0);

    printf("%.1f %d\n", tourne->distance, tourne->list->nodeCount);
    DestinationPrintList(tourne->list);

    FILE* fileInter = fopen(fileInterName, "r");

    int nb = 0;
    fscanf(fileInter,"%d",&nb);

    double** coord = CreateCoordTab(fileInter, nb);

    ListInt* list = pathAllCheckpoint(dest, tourne);

    CreateGeoJsonTravelPath(list, dest->allDestination, coord);

    ListInt_destroy(list);
    for(int i=0;i<nb;i++){
        free(coord[i]);
    }
    free(coord);
    DestinationDestroy(dest);
    free(destination);
    fclose(fileInter);
    fclose(file);
    Path_destroy(tourne);
}

void TSP_ACO(char* filename){
    char fileGraphName[256];
    char fileInterName[256];

    FILE* file = fopen(filename, "r");
    if(file == NULL) return;

    fscanf(file, "%s", fileGraphName);
    fscanf(file, "%s", fileInterName);

    int nbDestination = 0;
    fscanf(file, "%d", &nbDestination);

    int* destination = (int*)calloc(nbDestination, sizeof(int));

    for (int i = 0; i < nbDestination; i++){
        fscanf(file, "%d", &(destination[i]));
    }

    Destination* dest = DestinationPathMatrix(fileGraphName, nbDestination, destination);

    for (int i = 0; i < nbDestination; i++){
        ListInt_insertFirst(dest->allDestination, destination[i]);
    }

    Path* tourne = Graph_tspFromACO(dest->graph, 0, 1000, 100, 2.0f, 3.0f, 0.1f, 2.0f);

    printf("%.1f %d\n", tourne->distance, tourne->list->nodeCount);
    DestinationPrintList(tourne->list);

    FILE* fileInter = fopen(fileInterName, "r");

    int nb = 0;
    fscanf(fileInter,"%d",&nb);

    double** coord = CreateCoordTab(fileInter, nb);

    ListInt* list = pathAllCheckpoint(dest, tourne);
    double minLat=1024;
    double maxLat=-1024;
    double minLong=1024;
    double maxLong=-1024;

    for(int i=0;i<nb;i++){
        if(coord[i][0]<minLong)minLong=coord[i][0];
        if(coord[i][0]>maxLong)maxLong=coord[i][0];
        if(coord[i][1]<minLat)minLat=coord[i][1];
        if(coord[i][1]>maxLat)maxLat=coord[i][1];
    }
    printf("minLat:%lf\nmaxLat:%lf\nminLong:%lf\nmaxLong:%lf\n",minLat,maxLat,minLong,maxLong);
    double RLat=maxLat-minLat;
    double RLong=maxLong-minLong;
    printf("RatioLat:%lf\nRatioLong:%lf\n",RLat,RLong);

    SDL_Init(SDL_INIT_VIDEO);
    int width = 1200;
    int height = 800;
    SDL_Window* window = SDL_CreateWindow("Blank Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    SDL_Texture * texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                              SDL_TEXTUREACCESS_TARGET, width, height);
    if (texture == NULL) {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    SDL_SetRenderTarget(renderer, texture);

    int adjust=width;
    int addX=0;
    if(height<adjust){
        adjust=height;
        addX=(width-height)/2;
    }
    for(int i=0;i<nb;i++){
        double x = ((coord[i][1]-minLat)*adjust)/RLat;
        double y = ((coord[i][0]-minLong)*adjust)/RLong;
        //printf("X:%lf,Y:%lf\n",x,y);
        //if(i<nb/3)SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        //else if(i<(nb/3)*2)SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        //else SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawPointF(renderer,(float)y+addX,(float)adjust-x);
    }
    SDL_SetRenderTarget(renderer, NULL);
    SDL_Rect dst = {0, 0, width, height};
    //SDL_RenderCopy(renderer, texture, NULL, &dst);

    bool running = true;
    SDL_Event event;
    ListIntNode* tmpNode = list->sentinel.next;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN || event.type == SDL_MOUSEBUTTONDOWN) {
                running = false;
                break;
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, texture, NULL, &dst);

        for(int i=0;i<list->nodeCount;i++) {
            if (tmpNode != &list->sentinel && tmpNode->next != &list->sentinel) {
                double xA = ((coord[tmpNode->value][1] - minLat) * adjust) / RLat;
                double yA = ((coord[tmpNode->value][0] - minLong) * adjust) / RLong;
                double xB = ((coord[tmpNode->next->value][1] - minLat) * adjust) / RLat;
                double yB = ((coord[tmpNode->next->value][0] - minLong) * adjust) / RLong;
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderDrawLineF(renderer, (float) yA + addX, (float) adjust - xA, (float) yB + addX,(float) adjust - xB);
            }
            tmpNode = tmpNode->next;
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    CreateGeoJsonTravelPath(list, dest->allDestination, coord);

    ListInt_destroy(list);
    for(int i=0;i<nb;i++){
        free(coord[i]);
    }
    free(coord);
    DestinationDestroy(dest);
    free(destination);
    fclose(fileInter);
    fclose(file);
    Path_destroy(tourne);
}
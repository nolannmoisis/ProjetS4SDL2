#include "../Header/TSP.h"

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

Graph* Graph_PheromoneCreatePath(Graph* graph, Path* tourne){
    Graph* pheromone = Graph_create(graph->size);

    ArcData init;
    init.weight = 1.0f;

    for (int u = 0; u < graph->size; u++){
        for (ArcList* arc = Graph_getArcList(graph, u); arc != NULL; arc = arc->next){
            Graph_setArc(pheromone, u, arc->target, &init);
        }
    }

    int start = ListInt_dequeue(tourne->list);
    int actual = start;
    int next = 0;

    while (!ListInt_isEmpty(tourne->list)){
        next = ListInt_dequeue(tourne->list);

        ArcData* actualTauxUV = Graph_getArc(pheromone, actual, next);
        actualTauxUV->weight += 1.0f;

        if(next == start) break;

        actual = next;
    }

    return pheromone;
}

Path* Graph_tspFromACOWithGlouton(Graph* graph, int station, int iterationCount, int antCount, float alpha, float beta, float rho, float q){
    Path* bestTourne = (Path *)calloc(1, sizeof(Path));
    AssertNew(bestTourne);

    Path* tourneGlouton = Graph_tspFromHeuristic(graph, station);

    printf("%.1f %d\n", tourneGlouton->distance, tourneGlouton->list->nodeCount);
    DestinationPrintList(tourneGlouton->list);

    Graph* pheromone = Graph_PheromoneCreatePath(graph, tourneGlouton);
    AssertNew(pheromone);

    Path_destroy(tourneGlouton);

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

Path* Graph_tspFromACOWithGloutonWithSDL(Graph* graph, int station, int iterationCount, int antCount, float alpha, float beta, float rho, float q,SDL_Renderer* renderer,double** coord,double minLat,double minLong,double RLat,double RLong,int adjust,int addX,Destination* dest,SDL_Texture* texture,SDL_Rect dst){
    Path* bestTourne = (Path *)calloc(1, sizeof(Path));
    AssertNew(bestTourne);

    Path* tourneGlouton = Graph_tspFromHeuristic(graph, station);

    printf("%.1f %d\n", tourneGlouton->distance, tourneGlouton->list->nodeCount);
    DestinationPrintList(tourneGlouton->list);

    Graph* pheromone = Graph_PheromoneCreatePath(graph, tourneGlouton);
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
            ListInt* List = pathAllCheckpoint(dest, tourne[j]);
            ListInt* listCopy = ListInt_copy(List);
            int actual = ListInt_popFirst(listCopy);
            int next = 0;
            if (j % (int)(antCount / 2) == 0) {
                while (!ListInt_isEmpty(listCopy)) {
                    next = ListInt_popFirst(listCopy);
                    double xA = ((coord[actual][1] - minLat) * adjust) / RLat;
                    double yA = ((coord[actual][0] - minLong) * adjust) / RLong;
                    double xB = ((coord[next][1] - minLat) * adjust) / RLat;
                    double yB = ((coord[next][0] - minLong) * adjust) / RLong;
                    SDL_SetRenderDrawColor(renderer, 255, 0 , 0, 1);
                    SDL_RenderDrawLineF(renderer, (float) yA + addX, (float)adjust - xA, (float)yB + addX,(float)adjust - xB);
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    if (List->nodeCount < 1) break;

                    actual = next;
                }
            }
            SDL_RenderPresent(renderer);
            fflush(stdout);
            ListInt_destroy(listCopy);
            ListInt_destroy(List);
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

    return proba;
}

int argminACO(int station, Graph* graph, bool* explored){
    int minIndex = 0;
    float minDist = INFINITY;

    for (int u = 0; u < graph->size; u++){
        if(!explored[u]){
            ArcData* arc = Graph_getArc(graph, station, u);
            if(minDist > arc->weight){
                minIndex = u;
                minDist = arc->weight;
            }
        }
    }
    
    return minIndex;
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

        float probaRandom = (float)rand()/RAND_MAX;
        float totalProba = 0.0f;

        for (ArcList* arc = Graph_getArcList(graph, prev); arc != NULL; arc = arc->next){
            if(!explored[arc->target]){
                totalProba += proba[arc->target];
                if(totalProba >= probaRandom){
                    next = arc->target;
                    break;
                }
            }
        }

        if(next == -1){
            printf("next == -1\n");
            next = argminACO(prev, graph, explored);
        }

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

        //if(next == start) break;

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
    if(!tourne || !dest){
        printf("Zeubi");
        return NULL;
    }
    int start = ListInt_popFirst(tourne->list);
    int bin = 0;
    ListInt* list = ListInt_create();
    bool first = true;
    ListIntIter* listIter = ListIntIter_create(tourne->list);
    while (ListIntIter_isValid(listIter)){
        int next = ListIntIter_get(listIter);
        ListInt* destCopyList = ListInt_copy(dest->path[start][next]->list);
        if(first == true){
            ListInt_concatenate(list, destCopyList);
            first = false;
        }
        else {
            bin = ListInt_popFirst(destCopyList);
            ListInt_concatenate(list, destCopyList);
        }
        start = next;
        ListIntIter_next(listIter);
        ListInt_destroy(destCopyList);
    }
    ListIntIter_destroy(listIter);
    bin += 1;

    return list;
}

void TSP_Heuristic(char* filename){
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

    for (int i = 0; i < nbDestination; i++){
        ListInt_insertFirst(dest->allDestination, destination[i]);
    }

    char* fileRegister = "pathMatrixRegister.txt";

    DestinationWrite(dest, fileRegister);
    
    //Path* tourne = Graph_tspFromACO(dest->graph, 0, 1000, 100, 2.0f, 3.0f, 0.1f, 2.0f);
    FILE* fileInter = fopen(fileInterName, "r");

    int nb = 0;
    fscanf(fileInter,"%d",&nb);

    double** coord = CreateCoordTab(fileInter, nb);

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
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);

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
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    bool running = true;
    SDL_Event event;

    Path* tourne = Graph_tspFromACOWithGloutonWithSDL(dest->graph, 0, 1000, 100, 2.0f, 3.0f, 0.1f, 2.0f, renderer,coord,minLat,minLong,RLat,RLong,adjust,addX,dest,texture,dst);
    //Path* tourne = Graph_tspFromACOWithGlouton(dest->graph, 0, 1000, 100, 2.0f, 3.0f, 0.1f, 2.0f);

    printf("%.1f %d\n", tourne->distance, tourne->list->nodeCount);
    DestinationPrintList(tourne->list);
    ListInt* list = pathAllCheckpoint(dest, tourne);

    ListIntNode* tmpNode = list->sentinel.next;

    for(int i=0;i<list->nodeCount;i++) {
        if (tmpNode != &list->sentinel && tmpNode->next != &list->sentinel) {
            double xA = ((coord[tmpNode->value][1] - minLat) * adjust) / RLat;
            double yA = ((coord[tmpNode->value][0] - minLong) * adjust) / RLong;
            double xB = ((coord[tmpNode->next->value][1] - minLat) * adjust) / RLat;
            double yB = ((coord[tmpNode->next->value][0] - minLong) * adjust) / RLong;
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderDrawLineF(renderer, (float) yA + addX, (float) adjust - xA, (float) yB + addX,(float) adjust - xB);
        }
        tmpNode = tmpNode->next;
    }

    SDL_RenderPresent(renderer);

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN || event.type == SDL_MOUSEBUTTONDOWN) {
                running = false;
                break;
            }
        }
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
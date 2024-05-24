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
    setvbuf(stdout, NULL, _IONBF, 0);

    char* filenameConfig = "Config.txt";
    FILE* fileConfig = fopen(filenameConfig, "r");
    if(fileConfig == NULL){
        printf("ERROR: File creation\n");
        return EXIT_FAILURE;
    }
    int TSP_Choose = 0;
    int Instance_Choose = 0;
    fscanf(fileConfig, "%d %d", &TSP_Choose, &Instance_Choose);

    fclose(fileConfig);

    char* input = NULL;

    switch (Instance_Choose){
        case 1:
            printf("Instance : input1\n");
            input = "Tests/4_TSP_ACO/input1.txt";
            break;
        case 2:
            printf("Instance : input2\n");
            input = "Tests/4_TSP_ACO/input2.txt";
            break;
        case 3:
            printf("Instance : input3\n");
            input = "Tests/4_TSP_ACO/input3.txt";
            break;
        case 4:
            printf("Instance : input4\n");
            input = "Tests/4_TSP_ACO/input4.txt";
            break;
        case 5:
            printf("Instance : inputGrandeInstance\n");
            input = "Tests/5_Grande_instance/input.txt";
            break;
        default:
            printf("ERROR: Instance entré n'esxiste pas !\n");
            printf("Instance : input1, se lancera automatiquement\n");
            input = "Tests/4_TSP_ACO/input1.txt";
            break;
    }

    switch (TSP_Choose)
    {
        case 1:
            printf("Lancement de l'Heuristic\n");
            TSP_HeuristicByName(input);
            break;
        case 2:
            printf("Lancement de l'ACO\n");
            TSP_ACOByName(input);
            break;
        case 3:
            printf("Lancement de l'ACO avec Heuristic\n");
            TSP_ACOWithGloutonByName(input);
            break;
        case 4:
            printf("Lancement de l'ACO avec Heuristic et SDL\n");
            TSP_ACOWithGloutonWithSDLByName(input);
            break;
        case 5:
            printf("Lancement de l'enregistrement du path matrice\n");
            pathMatrixRegister(input);
            break;
        case 6:
            printf("Lancement de L'ACO avec Heuristic et SDL avec un path matrice enregistré\n");
            TSP_ACOWithGloutonWithSDLWithChargingByName(input);
            break;
        default:
            printf("ERROR: TSP entré n'esxiste pas !\n");
            printf("Lancement automatique de l'ACO\n");
            TSP_ACOByName(input);
            break;
    }

    return EXIT_SUCCESS;
}
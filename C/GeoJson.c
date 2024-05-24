
#include "../Header/GeoJson.h"

void CreateGeoJsonSimplePath(ListInt* Path, double** Coord){
    FILE *GJs;
    double start1=Coord[Path->sentinel.next->value][0];
    double start2=Coord[Path->sentinel.next->value][1];
    double end1=Coord[Path->sentinel.prev->value][0];
    double end2=Coord[Path->sentinel.prev->value][1];
    GJs = fopen("./SimplePath.geojson", "w");
    fprintf(GJs,"%s%lf%s%lf%s%lf%s%lf%s","{\n"
           "    \"type\": \"FeatureCollection\",\n"
           "    \"features\": [\n"
           "        {\n"
           "            \"type\": \"Feature\",\n"
           "            \"properties\": {\n"
           "                \"_umap_options\": {\n"
           "                    \"color\": \"#2196F3\",\n"
           "                    \"weight\": 6,\n"
           "                    \"iconClass\": \"Drop\",\n"
           "                    \"showLabel\": null\n"
           "                },\n"
           "                \"name\": \"Start\"\n"
           "            },            \"geometry\": {\n"
           "                \"type\": \"Point\",\n"
           "                \"coordinates\": [",start1,",",start2,"]\n"
           "            }\n"
           "        },\n"
           "        {\n"
           "            \"type\": \"Feature\",\n"
           "            \"properties\": {\n"
           "                \"_umap_options\": {\n"
           "                    \"color\": \"#F44336\",\n"
           "                    \"weight\": 6,\n"
           "                    \"iconClass\": \"Drop\",\n"
           "                    \"showLabel\": null\n"
           "                },\n"
           "                \"name\": \"End\"\n"
           "            },            \"geometry\": {\n"
           "                \"type\": \"Point\",\n"
           "                \"coordinates\": [",end1,",",end2,"]\n"
           "            }\n"
           "        },\n"
           "        {\n"
           "            \"type\": \"Feature\",\n"
           "            \"properties\": {\n"
           "                \"_umap_options\": {\n"
           "                    \"color\": \"#9C27B0\",\n"
           "                    \"weight\": 6,\n"
           "                    \"showLabel\": null\n"
           "                },\n"
           "                \"name\": \"Path\"\n"
           "            },            \"geometry\": {\n"
           "                \"type\": \"LineString\",\n"
           "                \"coordinates\": [\n"
    );
    ListInt* Clone = Path;
    int tmp=ListInt_popFirst(Clone);
    fprintf(GJs,"%s%lf%s%lf%s","[",Coord[tmp][0],",",Coord[tmp][1],"]");
    int Count=Path->nodeCount;
    for(int i=0;i<Count;i++){
        tmp=ListInt_popFirst(Clone);
        fprintf(GJs,"%s%lf%s%lf%s",", [",Coord[tmp][0],",",Coord[tmp][1],"]");
    }
    fprintf(GJs,"%s","\t\t\t\t]\n\t\t\t}\n\t\t}\n\t]\n}");
    fclose(GJs);
}

void CreateGeoJsonTravelPath(ListInt* Path, ListInt* Checkpoint, double** Coord){
    FILE *GJs;
    GJs = fopen("../TravelPath.geojson", "w");
    fprintf(GJs,"%s",
                     "{\n"
                     "    \"type\": \"FeatureCollection\",\n"
                     "    \"features\": [\n"
                     "        {\n"
    );
    ListIntNode* Node=Checkpoint->sentinel.next;
    while(Node!=&Checkpoint->sentinel){
        fprintf(GJs, "%s%lf%s%lf%s",
                "            \"type\": \"Feature\",\n"
                "            \"properties\": {\n"
                "                \"_umap_options\": {\n"
                "                    \"color\": \"#1AEBD2\",\n"
                "                    \"weight\": 6,\n"
                "                    \"iconClass\": \"Drop\",\n"
                "                    \"showLabel\": null\n"
                "                },\n"
                "                \"name\": \"Start\"\n"
                "            },            \"geometry\": {\n"
                "                \"type\": \"Point\",\n"
                "                \"coordinates\": [", Coord[Node->value][0], ",", Coord[Node->value][1], "]\n"
                "            }\n"
                "        },\n"
                "        {\n"
        );
        Node=Node->next;
    }
    fprintf(GJs,"%s",
            "            \"type\": \"Feature\",\n"
            "            \"properties\": {\n"
            "                \"_umap_options\": {\n"
            "                    \"color\": \"#0AF761\",\n"
            "                    \"weight\": 6,\n"
            "                    \"showLabel\": null\n"
            "                },\n"
            "                \"name\": \"Path\"\n"
            "            },            \"geometry\": {\n"
            "                \"type\": \"LineString\",\n"
            "                \"coordinates\": [\n");
    ListInt* Clone = Path;
    int tmp=ListInt_popFirst(Clone);
    fprintf(GJs,"%s%lf%s%lf%s","[",Coord[tmp][0],",",Coord[tmp][1],"]");
    int Count=Path->nodeCount;
    for(int i=0;i<Count;i++){
        tmp=ListInt_popFirst(Clone);
        fprintf(GJs,"%s%lf%s%lf%s",", [",Coord[tmp][0],",",Coord[tmp][1],"]");
    }
    fprintf(GJs,"%s","\t\t\t\t]\n\t\t\t}\n\t\t}\n\t]\n}");
    fclose(GJs);
}


double** CreateCoordTab(FILE* file, int Nb){
    double** Coord = calloc(Nb,sizeof(double));
    double a=0;
    double b=0;
    for(int i=0;i<Nb;i++){
        Coord[i]= calloc(2,sizeof(double));
        fscanf(file,"%lf",&a);
        fscanf(file,"%lf",&b);
        Coord[i][0]=a;
        Coord[i][1]=b;
    }

    return Coord;
}


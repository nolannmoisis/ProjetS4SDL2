#pragma once

#include "Settings.h"
#include "Graph.h"
#include "ListInt.h"
#include "GeoJson.h"
//#include "Destination.h"

/// @brief Structure repr sentant un chemin dans un graphe.
typedef struct Path_s
{
    /// @brief Liste des sommets du chemin.
    /// Le premier  l ment de la liste est le sommet et d part
    /// et le dernier  l ment le sommet d'arriv e.
    ListInt *list;

    /// @brief Longueur du chemin.
    /// Autrement dit la somme des poids des arcs qui composent le chemin.
    float distance;
} Path;

typedef struct TasData_s
{
    int idNode;
    float distances;
} TasData;

TasData* TasBinaire_create(int size);

void TasBinaire_print(TasData *tas);

void TasBinaire_Destroy(TasData *tas);

void TasBinaire_Permute(TasData *tas, int indexFirst, int indexSecond);

int pereIndex(int index);

int filsGaucheIndex(int index);

int filsDroitIndex(int index);

void TasBinaireInsert(TasData *tas, int id, float distance, int size);

TasData TasBinairePop(TasData *tas);

TasData TasBinaire_GetIndex(TasData* tas, int index);

bool TasBinaire_IsEmpty(TasData* tas);

/// @brief Cr e un nouveau chemin.
/// @param start le sommet de d part du chemin.
/// @return Le chemin cr  .
Path *Path_create(int start);

/// @brief D truit un chemin cr   avec Path_create().
/// @param path le chemin   d truire.
void Path_destroy(Path *path);

/// @brief Copie un chemin.
/// Il s'agit d'une copie profonde, tous les noeuds de la liste sont r allou s.
/// @param path le chemin.
/// @return Une copie profonde du chemin.
Path *Path_copy(Path *path);

Path *PathReverse_copy(Path *path);

/// @brief Insert un sommet au d but d'un chemin.
/// @param path le chemin.
/// @param nodeID l'identifiant du noeud   ajouter.
/// @param weight le poids le l'arc ajout .
void Path_insertFirst(Path *path, int nodeID, float weight);

/// @brief Insert un sommet   la fin d'un chemin.
/// @param path le chemin.
/// @param nodeID l'identifiant du noeud   ajouter.
/// @param weight le poids le l'arc ajout .
void Path_insertLast(Path *path, int nodeID, float weight);

/// @brief Affiche un chemin.
/// @param path le chemin.
void Path_print(Path *path);

/// @brief Renvoie un plus court chemin entre deux sommets d'un graphe.
/// Si aucun chemin n'existe, renvoie NULL.
/// Cette fonction suit l'algorithme de Dijkstra.
/// Elle a une complexit  en O(n^2) o  n d signe le nombre de noeuds du graphe.
///
/// @param graph le graphe.
/// @param start l'identifiant du sommet de d part.
/// @param end l'identifiant du sommet d'arriv e.
/// @return Un plus court chemin en cas d'existance, NULL sinon.
Path *Graph_shortestPath(Graph *graph, int start, int end);

/// @brief Effectue l'algorithme de Dijkstra.
/// Si end >= 0, cette fonction calcule un plus court chemin entre les noeuds
/// start et end.
/// Le chemin doit cependant  tre reconstruit   partir des tableaux
/// predecessors et distances en utilisant la fonction Graph_dijkstraGetPath().
/// Si end < 0, l'algorithme calcule un plus court chemin pour chaque noeud
/// accessible depuis start.
///
/// @param graph le graphe.
/// @param start l'identifiant du sommet de d part.
/// @param end l'identifiant du sommet d'arriv e ou un entier < 0 pour calculer
///     tous les chemins.
/// @param predecessors tableau p alablement allou  dont la taille est  gale au
///     nombre de noeuds du graphe. Apr s l'appel   la fonction, il contient le
///     pr d cesseur de chaque noeud.
/// @param distances tableau p alablement allou  dont la taille est  gale au
///     nombre de noeuds du graphe. Apr s l'appel   la fonction, il contient
///     pour chaque noeud sa distance avec le noeud start.
void Graph_dijkstra(Graph *graph, int start, int end, int *predecessors, float *distances);

/// @brief Reconstruit un chemin   partir de la sortie de l'algorithme de
/// Dijkstra.
///
/// @param predecessors tableau des pr decesseurs.
/// @param distances tableau des distances.
/// @param end noeud d'arriv e.
/// @return Le plus court chemin arrivant au noeud end s'il existe, NULL sinon.
Path *Graph_dijkstraGetPath(int *predecessors, float *distances, int end);

void printLIst(ListInt* list);

void Dijkstra(char* filename);
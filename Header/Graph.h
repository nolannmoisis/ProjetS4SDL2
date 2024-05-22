#pragma once

#include "Settings.h"

typedef struct Graph Graph;
typedef struct GraphNode GraphNode;
typedef struct ArcList ArcList;
typedef struct ArcData ArcData;

struct ArcData
{
    float weight;
};

/// @brief Structure représentant un graphe orienté.
struct Graph
{
    /// @brief Tableau contenant les noeuds du graphe.
    GraphNode* nodes;

    /// @brief Nombre de noeuds du graphe.
    int size;
};

/// @brief Structure représentant un noeud d'un graphe.
struct GraphNode
{
    /// @brief Identifiant du noeud.
    /// Compris entre 0 et la taille du graphe moins 1.
    int id;

    /// @brief Degré sortant du noeud.
    int arcCount;

    /// @brief Liste des arcs sortants du noeud.
    ArcList* arcList;
};

/// @brief Structure représentant une liste simplement chaînée des arcs sortants d'un noeud.
struct ArcList
{
    /// @brief Pointeur vers l'élément suivant de la liste.
    /// Vaut NULL s'il s'agit du dernier élément.
    ArcList* next;

    int target;

    ArcData data;
};

/// @brief Crée un nouveau graphe.
/// Le nombre de noeuds doit être défini au moment de la création et ne peut
/// plus être modifié par la suite.
/// @param size Le nombre de noeuds du graphe.
/// @return Le graphe créé.
Graph *Graph_create(int size);

/// @brief Détruit un graphe créé avec Graph_create().
/// @param graph le graphe.
void Graph_destroy(Graph *graph);

/// @brief Crée un nouveau graphe à partir du graphe stocké dans le fichier filename.
/// @param filenamne Chemin vers le fichier texte contenant le graphe
/// @return Le graphe créé.
Graph *Graph_load(char *filename);

/// @brief Affiche le graphe.
/// @param graph Le graphe 
void Graph_print(Graph *graph);

/// @brief Renvoie le nombre de noeuds d'un graphe.
/// @param graph le graphe.
/// @return Le nombre de noeuds du graphe.
INLINE int Graph_size(Graph *graph)
{
    assert(graph);
    return graph->size;
}

/// @brief Retourne le nombre de voisins du noeud u
/// @param graphe Le graphe
/// @param u L'id du noeud
/// @return Le degré sortant de u.
INLINE int Graph_getArcCount(Graph *graph, int u)
{
    assert(graph);
    assert(0 <= u && u < graph->size);
    return graph->nodes[u].arcCount;
}

/// @brief Retourne la liste d'arcs reliant u à ses voisins
/// @param graphe Le graphe
/// @param u L'id du noeud
/// @return La liste d'arcs reliant u à ses voisins
INLINE ArcList *Graph_getArcList(Graph *graph, int u)
{
    assert(graph);
    assert(0 <= u && u < graph->size);
    return graph->nodes[u].arcList;
}

/// @brief Retourne un pointeur sur l'arc entre les sommets u et v,
/// NULL s'il n'existe pas. Ne supprime pas l'arc.
/// @param graphe Le graphe
/// @param u L'id du noeud source
/// @param v L'id du noeud destination
/// @return un pointeur sur l'arc
ArcData *Graph_getArc(Graph *graph, int u, int v);

/// @brief Ajoute un nouvel arc entre les sommets u et v
/// @param graphe Le graphe
/// @param u L'id du noeud source
/// @param v L'id du noeud destination
/// @param ArcData Un pointeur sur la valeur de l'arc
void Graph_setArc(Graph *graph, int u, int v, ArcData *data);

/// @brief Supprime l'arc entre les sommets u et v s'il existe
/// @param graphe Le graphe
/// @param u L'id du noeud source
/// @param v L'id du noeud destination
void Graph_removeArc(Graph *graph, int u, int v);

/// @brief Dit si l'arbre est orienté ou non
/// @param graph Le graphe
/// @return si orienté false sinon true
bool Graph_isDirected(Graph *graph);

/// @brief 
/// @param graph 
/// @param degreeTab 
void Graph_degreeTab(Graph *graph, int *degreeTab);

/// @brief 
/// @param graph 
/// @param colorCount 
/// @return 
int *Graph_dSatur(Graph *graph, int *colorCount);

/// @brief 
/// @param graph 
/// @return 
int Graph_maxDegree(Graph *graph);

/// @brief 
/// @param graph 
/// @param colors 
/// @return 
bool Graph_isColorationValid(Graph *graph, int *colors);
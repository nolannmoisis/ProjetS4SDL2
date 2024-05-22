#pragma once

#include "Settings.h"

typedef struct MapPtrNode MapPtrNode;
struct MapPtrNode
{
    MapPtrNode *parent;
    MapPtrNode *leftChild;
    MapPtrNode *rightChild;
    int height;

    char *key;
    void *value;
};

typedef struct MapPtr
{
    MapPtrNode *root;
    int size;
} MapPtr;

typedef struct MapPtrIter
{
    MapPtr *dict;
    MapPtrNode *curr;
    bool first;
} MapPtrIter;

/// @brief Crée un dictionnaire vide.
/// 
/// @return Le dictionnaire créé.
MapPtr *MapPtr_create();

/// @brief Détruit un dictionnaire.
/// Les clés sont automatiquement libérées mais les données associées ne le sont pas.
/// L'utilisateur doit donc parcourir le dictionnaire pour les libérer avant d'appeler cette méthode.
/// 
/// @param self le dictionnaire.
void MapPtr_destroy(MapPtr *self);

/// @brief Détruit un dictionnaire.
/// Les clés sont automatiquement libérées et les données associées sont libérées
/// en utilisant la fonction passée en argument.
/// 
/// @param self le dictionnaire.
/// @param freeValue la fonction utilisée pour libérer les valeurs.
void MapPtr_destroyMapAndValues(MapPtr *self, void (*freeValue)(void *));

/// @brief Renvoie la taille d'un dictionnaire.
/// 
/// @param self le dictionnaire.
/// @return La taille du dictionnaire.
int MapPtr_size(MapPtr *self);

/// @brief Indique si un dictionnaire contient une clé.
/// 
/// @param self le dictionnaire.
/// @param key la clé recherchée.
/// @return true si le dictionnaire contient la clé, false sinon.
bool MapPtr_containsKey(MapPtr *self, const char *key);

/// @brief Renvoie la valeur associée à une clé dans le dictionnaire
/// ou NULL si la clé n'est pas présente.
/// 
/// @param self le dictionnaire.
/// @param key la clé.
/// @return la valeur associée à la clé.
void *MapPtr_get(MapPtr *self, char *key);

/// @brief Ajoute un couple clé/valeur dans un dictionnaire si la clé n'est pas présente,
/// sinon modifie la valeur associée à une clé.
/// La chaîne de caractères représentant la clé est copiée dans le dictionnaire.
/// 
/// @param self le dictionnaire.
/// @param key la clé.
/// @param value la valeur associée à la clé.
/// @return Si la clé était déjà présente dans le dictionnaire, renvoie la valeur précédente
/// (pour que l'utilisateur puisse libérer la mémoire), sinon renvoie NULL.
void *MapPtr_insert(MapPtr *self, char *key, void *value);

/// @brief Supprime une clé dans un dictionnaire.
/// Cette fonction est sans effet si la clé n'est pas présente.
/// 
/// @param self le dictionnaire.
/// @param key la clé.
/// @return Si la clé est présente dans le dictionnaire, renvoie la valeur associée
/// (pour que l'utilisateur puisse libérer la mémoire), sinon renvoie NULL.
void *MapPtr_remove(MapPtr *self, char *key);

//--------------------------------------------------------------------------------------------------
// Itérateur sur un dictionnaire de pointeurs

/// @brief Crée un itérateur sur un dictionnaire.
/// Si le dictionnaire n'est pas vide, l'itérateur pointe vers le premier élément.
/// 
/// @param map le dictionnaire à parcourir.
/// @return Un itérateur sur le dictionnaire.
MapPtrIter *MapPtrIter_create(MapPtr *map);

/// @brief Détruit un itérateur sur un dictionnaire.
/// 
/// @param self l'itérateur.
void MapPtrIter_destroy(MapPtrIter *self);


/// @brief Renvoie la clé de l'élément courant de l'itérateur.
/// L'utilisateur doit vérifier que l'itérateur est valide avant d'appeler cette méthode.
/// 
/// @param self l'itérateur.
/// @return La clé de l'élément courant de l'itérateur.
const char *MapPtrIter_getKey(MapPtrIter *self);

/// @brief Renvoie la valeur de l'élément courant de l'itérateur.
/// L'utilisateur doit vérifier que l'itérateur est valide avant d'appeler cette méthode.
/// 
/// @param self l'itérateur.
/// @return La valeur de l'élément courant de l'itérateur.
void *MapPtrIter_getValue(MapPtrIter *self);

/// @brief Fait avancer l'itérateur sur vers l'élément suivant.
/// L'utilisateur doit vérifier que l'itérateur est valide avant d'appeler cette méthode.
/// 
/// @param self l'itérateur.
void MapPtrIter_next(MapPtrIter *self);

/// @brief Indique si un itérateur pointe sur un élément valide.
/// 
/// @param self l'itérateur.
/// @return true si l'itérateur pointe sur un élément valide, false sinon.
bool MapPtrIter_isValid(MapPtrIter *self);

void MapPtr_print(MapPtr *self);
void MapPtr_printTree(MapPtr *self);
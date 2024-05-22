#pragma once

#include "Settings.h"
#include "ListInt.h"

/// @brief Structure représentant un arbre d'entiers.
/// Chaque noeud peut avec un nombre quelconque d'enfants.
typedef struct TreeInt
{
    /// @brief Pointeur vers le premier enfants du noeud.
    struct TreeInt* firstChild;

    /// @brief Pointeur vers le noeud frère suivant.
    struct TreeInt* nextSibling;

    /// @brief Valeur du noeud.
    int value;
} TreeInt;

/// @brief Crée un arbre avec uniquement un noeud.
/// @param value la valeur du noeud (la racine de l'arbre).
/// @return L'arbre créé.
TreeInt* TreeInt_create(int value);

/// @brief Détruit récursivement tous les noeuds d'un arbre donné par sa racine.
/// @param tree (la racine de) l'arbre.
void TreeInt_destroy(TreeInt* tree);

/// @brief Ajoute un enfant à un noeud d'un arbre.
/// @param node le noeud.
/// @param child l'enfant à ajouter. Il peut s'agir d'un arbre complet.
void TreeInt_addChild(TreeInt* node, TreeInt* child);

/// @brief Recherche un noeud dans un arbre.
/// @param node la racine de l'arbre.
/// @param value la valeur recherchée.
/// @return Un noeud ayant la valeur recherchée ou NULL si aucun noeud ne
/// possède cette valeur.
TreeInt* TreeInt_get(TreeInt* node, int value);

/// @brief Renvoie la liste des valeurs des noeuds d'un arbre.
/// @param tree l'arbre.
/// @return La liste des valeurs des noeuds de l'arbre.
ListInt* TreeInt_values(TreeInt* tree);

/// @brief Renvoie la liste des valeurs des feuilles d'un arbre.
/// @param tree l'arbre.
/// @return La liste des valeurs des feuilles de l'arbre.
ListInt* TreeInt_leaves(TreeInt* tree);

/// @brief Affiche un arbre
/// @param arbre l'arbre.
void TreeInt_print(TreeInt* arbre);

#include "../Header/TreeInt.h"

TreeInt *TreeInt_create(int value)
{
    TreeInt *node = (TreeInt *)calloc(1, sizeof(TreeInt));
    AssertNew(node);
    node->value = value;
    return node;
}

void TreeInt_destroy(TreeInt *tree)
{
    if (!tree) return;
    TreeInt_destroy(tree->firstChild);
    TreeInt_destroy(tree->nextSibling);
    free(tree);
}

void TreeInt_addChild(TreeInt *node, TreeInt *child)
{
    if ((!node) || (!child)) return;
    child->nextSibling = node->firstChild;
    node->firstChild = child;
}

TreeInt *TreeInt_get(TreeInt *node, int value)
{
    if ((!node) || (node->value == value)) return node;

    TreeInt *res = TreeInt_get(node->firstChild, value);
    if (res) return res;

    res = TreeInt_get(node->nextSibling, value);
    if (res) return res;

    return NULL;
}

void TreeInt_valuesRec(TreeInt *currNode, ListInt *list)
{
    if (!currNode) return;

    ListInt_insertLast(list, currNode->value);
    TreeInt_valuesRec(currNode->firstChild, list);
    TreeInt_valuesRec(currNode->nextSibling, list);
}

ListInt *TreeInt_values(TreeInt *tree)
{
    assert(tree);

    ListInt *list = ListInt_create();
    TreeInt_valuesRec(tree, list);
    return list;
}

void TreeInt_leavesRec(TreeInt *currNode, ListInt *list)
{
    if (!currNode) return;

    if (currNode->firstChild)
    {
        TreeInt_leavesRec(currNode->firstChild, list);
    }
    else
    {
        ListInt_insertLast(list, currNode->value);
    }
    TreeInt_leavesRec(currNode->nextSibling, list);
}

ListInt *TreeInt_leaves(TreeInt *tree)
{
    assert(tree);

    ListInt *list = ListInt_create();
    TreeInt_leavesRec(tree, list);
    return list;
}

int numberLen(int value)
{
    int res = 1;
    while (value >= 10)
    {
        value /= 10;
        res++;
    }
    return res;
}

int maxNumberLen(TreeInt *node)
{
    if (!node) return 0;
    int score = numberLen(node->value);
    int score_ = maxNumberLen(node->firstChild);
    if (score < score_) score = score_;
    score_ = maxNumberLen(node->nextSibling);
    if (score < score_) score = score_;
    return score;
}

int print(TreeInt *noeud, int level, int ligne, char **map, int maxLen)
{
    if (!noeud) return 0;
    int gauche = print(noeud->firstChild, level + 1, ligne, map, maxLen);
    char buffer[1024];
    int len = numberLen(noeud->value);
    int i = 0;
    for (i = 0; i < maxLen - len; i++) buffer[i] = '-';
    if ((noeud->firstChild) && (noeud->firstChild->nextSibling)) sprintf(buffer + i, "[%d]+", noeud->value);
    else sprintf(buffer + i, "[%d] ", noeud->value);
    for (int i = 0; i < maxLen + 3; i++) map[ligne][level * (maxLen + 3) + i] = buffer[i];
    int nbLignes = (gauche > 0) ? gauche : 1;
    int ligne_ = ligne;
    if ((noeud->firstChild == NULL) || (noeud->firstChild->nextSibling == NULL))
    {
        if (noeud->firstChild) map[ligne][level * (maxLen + 3) + maxLen + 2] = '-';
        return nbLignes;
    }
    TreeInt *current = noeud->firstChild->nextSibling;
    while (current)
    {
        for (int j = ligne_ + 1; j <= ligne + nbLignes; j++)
        {
            map[j][level * (maxLen + 3) + maxLen + 2] = '|';
        }
        map[ligne + nbLignes + 1][level * (maxLen + 3) + maxLen + 2] = '+';
        ligne_ = ligne + nbLignes + 1;
        nbLignes += print(current, level + 1, ligne + nbLignes + 1, map, maxLen) + 1;
        current = current->nextSibling;
    }
    return nbLignes;
}

void TreeInt_print(TreeInt *arbre)
{
    char **map = (char **)calloc(200, sizeof(char *));
    for (int i = 0; i < 200; i++)
    {
        map[i] = (char *)calloc(1024, sizeof(char));
        for (int j = 0; j < 1023; j++) map[i][j] = ' ';
    }
    int maxLen = maxNumberLen(arbre);
    print(arbre, 0, 0, map, maxLen);
    int fin = 0;
    for (int i = 0; i < 200; i++)
    {
        if (fin)
        {
            free(map[i]);
            continue;
        }
        fin = 1;
        for (int j = 0; j < 1023; j++)
        {
            if (map[i][j] != ' ')
            {
                fin = 0;
                break;
            }
        }
        if (!fin)
        {
            int lastCar = 0;
            for (int j = 0; j < 1023; j++)
            {
                if (map[i][j] != ' ')
                {
                    lastCar = j;
                }
            }
            map[i][lastCar + 1] = 0;
            printf("%s\n", map[i]);
        }
        free(map[i]);
    }
    free(map);
}
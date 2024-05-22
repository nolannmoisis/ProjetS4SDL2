#include "../Header/MapPtr.h"

bool MapPtr_find(MapPtr *self, const char *key, MapPtrNode **res);
void MapPtr_balance(MapPtr *self, MapPtrNode *node);

MapPtrNode *MapPtrNode_create(char *key, void *value)
{
    MapPtrNode *node = (MapPtrNode *)calloc(1, sizeof(MapPtrNode));
    AssertNew(node);

    node->key = strdup(key);
    node->value = value;

    return node;
}

void MapPtrNode_destroy(MapPtrNode *self)
{
    if (!self) return;
    free(self->key);
    free(self);
}

void MapPtrNode_destroyRec(MapPtrNode *self)
{
    if (!self) return;
    MapPtrNode_destroyRec(self->leftChild);
    MapPtrNode_destroyRec(self->rightChild);
    MapPtrNode_destroy(self);
}

void MapPtrNode_setLeft(MapPtrNode *self, MapPtrNode *left)
{
    self->leftChild = left;
    if (left != NULL)
        left->parent = self;
}

void MapPtrNode_setRight(MapPtrNode *self, MapPtrNode *right)
{
    self->rightChild = right;
    if (right != NULL)
        right->parent = self;
}

void MapPtrNode_update(MapPtrNode *self)
{
    assert(self);
    int hr = (self->rightChild) ? self->rightChild->height : -1;
    int hl = (self->leftChild) ? self->leftChild->height : -1;
    self->height = (hr > hl) ? hr + 1 : hl + 1;
}

int MapPtrNode_getBalance(MapPtrNode *self)
{
    if (self == NULL) return 0;

    int hr = (self->rightChild) ? self->rightChild->height : -1;
    int hl = (self->leftChild) ? self->leftChild->height : -1;
    return hr - hl;
}

MapPtrNode *MapPtrNode_maximum(MapPtrNode *self)
{
    assert(self != NULL);

    while (self->rightChild != NULL)
    {
        self = self->rightChild;
    }

    return self;
}

MapPtr *MapPtr_create()
{
    MapPtr *self = (MapPtr *)calloc(1, sizeof(MapPtr));
    AssertNew(self);

    return self;
}

void MapPtr_destroy(MapPtr *self)
{
    if (!self) return;

    MapPtrNode_destroyRec(self->root);
    free(self);
}

void MapPtr_destroyMapAndValues(MapPtr *self, void(*freeValue)(void *))
{
    MapPtrIter *it = NULL;
    for (it = MapPtrIter_create(self);
         MapPtrIter_isValid(it);
         MapPtrIter_next(it))
    {
        freeValue(MapPtrIter_getValue(it));
    }
    MapPtrIter_destroy(it);
    MapPtr_destroy(self);
}

int MapPtr_size(MapPtr *self)
{
    assert(self);
    return self->size;
}

void MapPtrNode_print(MapPtrNode *self)
{
    if (!self) return;
    MapPtrNode_print(self->leftChild);
    printf("  \"%s\": %p,\n", self->key, self->value);
    MapPtrNode_print(self->rightChild);
}

void MapPtr_print(MapPtr *self)
{
    printf("size = %d\n", self->size);
    MapPtrNode_print(self->root);
}

void MapPtrNode_printTree(MapPtrNode *self, int depth, int isLeft)
{
    if (!self) return;

    MapPtrNode_printTree(self->rightChild, depth + 1, false);

    for (int i = 0; i < depth - 1; i++) printf("    ");
    if (depth > 0)
    {
        if (isLeft) printf("  \\-");
        else printf("  /-");
    }
    printf("[%s]\n", self->key);

    MapPtrNode_printTree(self->leftChild, depth + 1, true);
}

void MapPtr_printTree(MapPtr *self)
{
    MapPtrNode_printTree(self->root, 0, false);
}

void *MapPtr_insert(MapPtr *self, char *key, void *value)
{
    if (self->root == NULL)
    {
        self->root = MapPtrNode_create(key, value);
        self->size = 1;

        return NULL;
    }

    MapPtrNode *node = NULL;
    if (MapPtr_find(self, key, &node))
    {
        // La donnée est déjà présente dans l'arbre
        void *prevValue = node->value;
        node->value = value;

        return prevValue;
    }

    // Crée le nouveau noeud
    MapPtrNode *newNode = MapPtrNode_create(key, value);
    if (strcmp(key, node->key) < 0)
    {
        MapPtrNode_setLeft(node, newNode);
    }
    else
    {
        MapPtrNode_setRight(node, newNode);
    }
    self->size++;

    // Equilibre l'arbre à partir du parent du nouveau noeud
    MapPtr_balance(self, node);

    return NULL;
}

bool MapPtr_find(MapPtr *self, const char *key, MapPtrNode **res)
{
    if (self->root == NULL)
    {
        *res = NULL;
        return false;
    }

    MapPtrNode *node = self->root;
    while (true)
    {
        int cmp = strcmp(key, node->key);
        if (cmp == 0)
        {
            *res = node;
            return true;
        }
        else if (cmp < 0)
        {
            if (node->leftChild == NULL)
            {
                *res = node;
                return false;
            }
            node = node->leftChild;
        }
        else
        {
            if (node->rightChild == NULL)
            {
                *res = node;
                return false;
            }
            node = node->rightChild;
        }
    }
}


bool MapPtr_containsKey(MapPtr *self, const char *key)
{
    MapPtrNode *node = NULL;
    return MapPtr_find(self, key, &node);
}

void *MapPtr_get(MapPtr *self, char *key)
{
    MapPtrNode *res = NULL;
    if (MapPtr_find(self, key, &res))
    {
        return res->value;
    }
    return NULL;
}

void MapPtr_replaceChild(
    MapPtr *self, MapPtrNode *parent,
    MapPtrNode *curChild, MapPtrNode *newChild)
{
    if (parent == NULL)
    {
        self->root = newChild;
    }
    else
    {
        if (parent->leftChild == curChild)
            parent->leftChild = newChild;
        else if (parent->rightChild == curChild)
            parent->rightChild = newChild;
        else
            assert(false);
    }

    if (newChild)
        newChild->parent = parent;
}

void MapPtr_rotateLeft(MapPtr *self, MapPtrNode *node)
{
    assert(node && node->rightChild);

    MapPtrNode *parent = node->parent;
    MapPtrNode *rightChild = node->rightChild;

    MapPtrNode_setRight(node, rightChild->leftChild);
    MapPtrNode_setLeft(rightChild, node);
    MapPtr_replaceChild(self, parent, node, rightChild);

    // Met à jour les hauteurs
    MapPtrNode_update(node);
    MapPtrNode_update(rightChild);
}

void MapPtr_rotateRight(MapPtr *self, MapPtrNode *node)
{
    assert(node && node->leftChild);

    MapPtrNode *parent = node->parent;
    MapPtrNode *leftChild = node->leftChild;

    MapPtrNode_setLeft(node, leftChild->rightChild);
    MapPtrNode_setRight(leftChild, node);
    MapPtr_replaceChild(self, parent, node, leftChild);

    // Met à jour les hauteurs
    MapPtrNode_update(node);
    MapPtrNode_update(leftChild);
}

void MapPtr_balance(MapPtr *self, MapPtrNode *node)
{
    // Remonte éventuellement jusqu'à la racine pour rééquilibrer l'arbre
    while (node != NULL)
    {
        MapPtrNode_update(node);
        int balance = MapPtrNode_getBalance(node);

        MapPtrNode *parent = node->parent;

        if (balance == 2)
        {
            int rightBalance = MapPtrNode_getBalance(node->rightChild);
            if (rightBalance == -1)
            {
                MapPtr_rotateRight(self, node->rightChild);
            }
            MapPtr_rotateLeft(self, node);
        }
        else if (balance == -2)
        {
            int leftBalance = MapPtrNode_getBalance(node->leftChild);
            if (leftBalance == 1)
            {
                MapPtr_rotateLeft(self, node->leftChild);
            }
            MapPtr_rotateRight(self, node);
        }

        node = parent;
    }
}

void *MapPtr_remove(MapPtr *self, char *key)
{
    if (self->root == NULL)
    {
        return NULL;
    }

    MapPtrNode *node = NULL;
    if (MapPtr_find(self, key, &node) == false)
    {
        return NULL;
    }
    assert(node != NULL);
    void *value = node->value;

    MapPtrNode *start = NULL;
    if (node->leftChild == NULL)
    {
        // Remplacement par le fils droit
        MapPtr_replaceChild(self, node->parent, node, node->rightChild);
        start = node->parent;
        MapPtrNode_destroy(node);
    }
    else if (node->rightChild == NULL)
    {
        // Remplacement par le fils gauche
        MapPtr_replaceChild(self, node->parent, node, node->leftChild);
        start = node->parent;
        MapPtrNode_destroy(node);
    }
    else
    {
        // Le noeud a deux fils
        // On l'échange avec sa valeur immédiatement inférieure (qui n'a plus de fils droit)
        MapPtrNode *maxLeft = MapPtrNode_maximum(node->leftChild);

        node->key = maxLeft->key;
        node->value = maxLeft->value;

        // Puis on le supprime comme précédemment
        MapPtr_replaceChild(self, maxLeft->parent, maxLeft, maxLeft->leftChild);
        start = maxLeft->parent;
        MapPtrNode_destroy(maxLeft);
    }
    self->size--;

    // Equilibre l'arbre à partir du parent du noeud supprimé
    MapPtr_balance(self, start);

    return value;
}


MapPtrIter *MapPtrIter_create(MapPtr *map)
{
    MapPtrIter *self = (MapPtrIter *)calloc(1, sizeof(MapPtrIter));
    AssertNew(self);


    MapPtrNode *curr = NULL;
    if (map->root)
    {
        curr = map->root;
        while (curr->leftChild)
        {
            curr = curr->leftChild;
        }
    }

    self->curr = curr;
    self->dict = map;

    return self;
}

void MapPtrIter_destroy(MapPtrIter *self)
{
    if (!self) return;
    free(self);
}

const char *MapPtrIter_getKey(MapPtrIter *self)
{
    assert(self && self->curr);
    return self->curr->key;
}

void *MapPtrIter_getValue(MapPtrIter *self)
{
    assert(self && self->curr);
    return self->curr->value;
}

void MapPtrIter_next(MapPtrIter *self)
{
    if (self->curr == NULL) return;

    MapPtrNode *curr = self->curr;
    MapPtrNode *next = NULL;

    if (curr->rightChild != NULL)
    {
        // On cherche la valeur suivante en descendant dans l'arbre
        next = curr->rightChild;
        while (next->leftChild)
            next = next->leftChild;
    }
    else
    {
        // On cherche la valeur suivante en remontant dans l'arbre
        next = curr->parent;
        while (next)
        {
            if (curr == next->leftChild)
                break;

            curr = next;
            next = next->parent;
        }
    }

    self->curr = next;
}

bool MapPtrIter_isValid(MapPtrIter *self)
{
    assert(self);
    return self->curr != NULL;
}

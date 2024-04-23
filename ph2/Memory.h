#include "DataStructure.h" // Include your header file here

void updateParent(struct TreeNode *node);

struct TreeNode *memoryRoot;

void createTree(struct TreeNode *node) {
    printf("intitate Tree");
    node = (struct TreeNode *)malloc(sizeof(struct TreeNode));
    node->i = 0;
    node->j = 1023;
    node->size = 1024;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->allocated = false;
}

struct TreeNode *splitTree(struct TreeNode *node) {
    if (node->left == NULL && node->right == NULL) {
        node->left = (struct TreeNode *)malloc(sizeof(struct TreeNode));
        node->right = (struct TreeNode *)malloc(sizeof(struct TreeNode));
        node->left->parent = node;
        node->left->i = node->i;
        node->left->j = (node->i + node->j) / 2;
        node->left->size = node->size / 2;
        node->left->left = NULL;
        node->left->right = NULL;
        node->left->allocated = false;

        node->right->parent = node;
        node->right->i = (node->i + node->j) / 2 + 1;
        node->right->j = node->j;
        node->right->size = node->size / 2;

        node->right->left = NULL;
        node->right->right = NULL;
        node->right->allocated = false;
    }
    return node;
}

void findSmallestRec(struct TreeNode ** smallestNode, struct TreeNode *node, int size) {
    if (node == NULL || node->allocated) return;

    if (node->left == NULL && node->right == NULL) {
        if(node->size >= size ) {
            if(!(*smallestNode)) {
                *smallestNode = node;
            } else if (node->size < (*smallestNode)->size) {
                *smallestNode = node;
            }
        }
        return;
    }

    findSmallestRec(smallestNode, node->left, size);
    findSmallestRec(smallestNode, node->right, size);
}

bool allocateMemory(struct PData *process) {
    printf("start node %d %d\n", process->id, process->memorySize);
    // allocate memory
    struct TreeNode * smallestNode = NULL;
    findSmallestRec(&smallestNode, memoryRoot, process->memorySize);
    if (smallestNode == NULL) {
        printf("no smallest node %d\n", process->id);
        return false;
    } else {
        printf("found smallest node %d\n", process->id);
    }

    if ((smallestNode->size / 2) >= process->memorySize) {
        printf("splitting smallest node %d\n", process->id);
        splitTree(smallestNode);

        struct TreeNode *node = smallestNode->left;
        while (node->size / 2 >= process->memorySize) {
            splitTree(node);
            node = node->left;
        }
        node->allocated = true;
        process->memoryStart = node->i;
        process->memoryEnd = node->j;
        process->memoryNode = node;
    } else {
        printf("using smallest node %d\n", process->id);
        smallestNode->allocated = true;
        process->memoryStart = smallestNode->i;
        process->memoryEnd = smallestNode->j;
        process->memoryNode = smallestNode;
        updateParent(smallestNode);
    }

    printf("Memory Allocated for process %d from %d to %d\n", process->id, process->memoryStart, process->memoryEnd);

    return true;
}

void mergeNodes(struct TreeNode *parent) {
    while (parent && !parent->left->allocated && !parent->right->allocated) {
        free(parent->left);
        free(parent->right);
        parent->left = NULL;
        parent->right = NULL;
        parent->allocated = false;
        parent = parent->parent;
    }
}

void deallocate(struct PData *process) {
    printf("Memory freed for process %d from %d to %d\n", process->id, process->memoryStart, process->memoryEnd);
    // deallocate memory
    struct TreeNode *memoryNode = process->memoryNode;
    memoryNode->allocated = false;
    // merge nodes
    mergeNodes(memoryNode->parent);
}

void updateParent(struct TreeNode *node) {
    while (node->parent != NULL) {
        if (node->parent->left->allocated && node->parent->right->allocated) {
            node->parent->allocated = true;
        } else {
            node->parent->allocated = false;
        }
        node = node->parent;
    }
}

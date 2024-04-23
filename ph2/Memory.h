#include "DataStructure.h" // Include your header file here

#define THRESHOLD 8

void updateParent(struct TreeNode *node);

struct TreeNode *memoryRoot;

void createTree() {
    memoryRoot = (struct TreeNode *)malloc(sizeof(struct TreeNode));
    memoryRoot->i = 0;
    memoryRoot->j = 1023;
    memoryRoot->size = 1024;
    memoryRoot->left = NULL;
    memoryRoot->right = NULL;
    memoryRoot->parent = NULL;
    memoryRoot->allocated = false;
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

struct TreeNode *findSmallestRec(struct TreeNode *node, int size)
{
    if (node == NULL || node->allocated)
        return NULL;

    if (node->left == NULL && node->right == NULL)
    {
        if (node->size >= size)
        {

            return node;
        }
        return NULL;
    }

    struct TreeNode *leftSmallest = findSmallestRec(node->left, size);
    struct TreeNode *rightSmallest = findSmallestRec(node->right, size);

    if (leftSmallest == NULL)
    {
        return rightSmallest;
    }
    else if (rightSmallest == NULL)
    {
        return leftSmallest;
    }
    else
    {
        return leftSmallest->size < rightSmallest->size ? leftSmallest : rightSmallest;
    }
}

struct TreeNode * findByMemoryRange(int i, int j, struct TreeNode *node) {
    if (node == NULL) {
        return NULL;
    }
    if (node->i == i && node->j == j) {
        return node;
    }
    struct TreeNode *left = findByMemoryRange(i, j, node->left);
    struct TreeNode *right = findByMemoryRange(i, j, node->right);
    return left ? left : right;

}

bool allocateMemory(struct PData *process)
{
    printf("start node %d %d\n", process->id, process->memorySize);
    // allocate memory
    struct TreeNode *smallestNode = NULL;
    smallestNode = findSmallestRec(memoryRoot, process->memorySize);
    if (smallestNode == NULL)
    {
        printf("no smallest node %d\n", process->id);
        return false;
    }
    else
    {
        printf("found smallest node %d\n", process->id);
    }

    if ((smallestNode->size / 2) >= process->memorySize && (smallestNode->size / 2) >= THRESHOLD)
    {
        printf("splitting smallest node %d\n", process->id);
        splitTree(smallestNode);

        struct TreeNode *node = smallestNode->left;
        while (node->size / 2 >= process->memorySize && node->size / 2 >= THRESHOLD)
        {
            splitTree(node);
            node = node->left;
        }
        node->allocated = true;
        process->memoryStart = node->i;
        process->memoryEnd = node->j;
    }
    else
    {
        printf("using smallest node %d\n", process->id);
        smallestNode->allocated = true;
        process->memoryStart = smallestNode->i;
        process->memoryEnd = smallestNode->j;
        updateParent(smallestNode);
    }

    printf("Memory Allocated for process %d from %d to %d\n", process->id, process->memoryStart, process->memoryEnd);

    return true;
}

void printMemoryRec(struct TreeNode *node) {
    if (node == NULL) {
        return;
    }
    printMemoryRec(node->left);
    printf("Node %d %d %d %d\n", node->i, node->j, node->size, node->allocated);
    printMemoryRec(node->right);
}

void printMemory() {
    printf("Printing Memory\n");
    printMemoryRec(memoryRoot);
}



void mergeNodes(struct TreeNode *parent) {
    while (parent != NULL && !parent->left->allocated && !parent->right->allocated) {
        printf("Merging nodes\n");
        free(parent->left);
        free(parent->right);
        parent->left = NULL;
        parent->right = NULL;
        parent->allocated = false;
        parent = parent->parent;
    }
}

void deallocateMemory(struct PData *process) {
    printf("Memory freed for process %d from %d to %d\n", process->id, process->memoryStart, process->memoryEnd);
    // deallocate memory
    struct TreeNode *memoryNode = findByMemoryRange(process->memoryStart, process->memoryEnd, memoryRoot);
    printf("Omar \n");
    if(memoryNode != NULL) {
     
    memoryNode->allocated = false;
    updateParent(memoryNode);
    // merge nodes
    printf("Before Merging nodes\n");
    mergeNodes(memoryNode->parent);
       
    } else {
        printMemory();
    }
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

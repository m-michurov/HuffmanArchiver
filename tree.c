#include "tree.h"

bool isLeaf(
        CHNode *node)
{
    if (node == NULL)
        return 0;

    return node->child[left] || node->child[right] ? false : true;
}


unsigned int treeHeight(
        CHNode * root)
{
    if (root == NULL)
        return 0;

    unsigned int l = treeHeight(root->child[left]);
    unsigned int r = treeHeight(root->child[right]);

    return (l > r ? l : r) + 1;
}


int treeCount(
        CHNode * root)
{
    if (root == NULL)
        return 1;

    return treeCount(root->child[left]) + treeCount(root->child[right]);
}


void makeLeavesArray(
        CHNode * node,
        unsigned char * arr)
{
    if (node == NULL)
        return;

    static unsigned int count = 0;

    if (isLeaf(node)) {
        arr[count++] = node->c;
        return;
    }

    makeLeavesArray(node->child[left], arr);
    makeLeavesArray(node->child[right], arr);

}

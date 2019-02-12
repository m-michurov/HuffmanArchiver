#include "tree.h"

bool isLeaf(
        CHNode *node)
{
    return node->left || node->right ? false : true;
}


unsigned int treeHeight(
        CHNode * root)
{
    if (root == NULL)
        return 0;

    unsigned int l = treeHeight(root->left);
    unsigned int r = treeHeight(root->right);

    return (l > r ? l : r) + 1;
}


int treeCount(
        CHNode * root)
{
    if (root == NULL)
        return 1;

    return treeCount(root->left) + treeCount(root->right);
}


void makeLeavesArray(
        CHNode * node,
        unsigned char * arr)
{

    static unsigned int count = 0;

    if (isLeaf(node)) {
        arr[count++] = node->c;
        return;
    }

    makeLeavesArray(node->left, arr);
    makeLeavesArray(node->right, arr);

}

/*----------------------------------------------------------------*
 * https://www.geeksforgeeks.org/priority-queue-using-linked-list *
 *----------------------------------------------------------------*/

#include "pqueue.h"

TREE_NODE * newCHNode(
        unsigned int freq,
        unsigned char c,
        TREE_NODE * l,
        TREE_NODE * r)
{
    TREE_NODE* new_node = (TREE_NODE*)malloc(sizeof(TREE_NODE));

    new_node->child[left] = l;
    new_node->child[right] = r;

    new_node->c = c;
    new_node->quantity = freq;

    return new_node;
}

QUEUE_NODE * newQNode(
        TREE_NODE * data)
{
    QUEUE_NODE * new_node = (QUEUE_NODE *)malloc(sizeof(QUEUE_NODE));

    new_node->char_data = data;
    new_node->next = NULL;

    return new_node;
}

inline bool isLeaf(
        TREE_NODE * node)
{
    if (node == NULL)
        return 0;

    return node->child[left] || node->child[right] ? false : true;
}

void freeTree(
        TREE_NODE * root)
{
    if (root == NULL)
        return;

    freeTree(root->child[left]);
    freeTree(root->child[right]);

    free(root);
}

inline TREE_NODE * peek(
        QUEUE_NODE ** head)
{
    return (*head)->char_data;
}

inline void pop(
        QUEUE_NODE ** head)
{
    QUEUE_NODE * temp = * head;
    (*head) = (*head)->next;

    free(temp);
}

inline int isEmpty(
        QUEUE_NODE ** head)
{
    return (* head) == NULL;
}

void push(
        QUEUE_NODE ** head,
        TREE_NODE * data)
{
    if (isEmpty(head)) {
        *head = newQNode(data);
        return;
    }

    QUEUE_NODE * start = (*head);

    // Create new Node
    QUEUE_NODE* temp = newQNode(data);

    // Special Case: The head of list has lesser
    // priority than new char_node. So insert new
    // char_node before head char_node and change head char_node.
    if ((*head)->char_data->quantity > data->quantity) {

        // Insert New Node before head
        temp->next = *head;
        (*head) = temp;
    }
    else {
        // Traverse the list and find a
        // position to insert new char_node
        while (start->next != NULL &&
               start->next->char_data->quantity < data->quantity) {
            start = start->next;
        }

        // Either at the ends of the list
        // or at required position
        temp->next = start->next;
        start->next = temp;
    }
}

int queueLength(
        QUEUE_NODE ** head)
{
    QUEUE_NODE * start = *head;
    int len = 0;

    if (isEmpty(&start))
        return 0;

    while (start) {
        len++;
        start = start->next;
    }

    return len;
}

inline TREE_NODE * get(
        QUEUE_NODE ** head)
{
    TREE_NODE * temp = peek(head);
    pop(head);

    return temp;
}

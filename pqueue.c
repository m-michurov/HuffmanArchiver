/*----------------------------------------------------------------*
 * https://www.geeksforgeeks.org/priority-queue-using-linked-list *
 *----------------------------------------------------------------*/

#include "pqueue.h"

TREE_NODE * NewTreeNode(
        unsigned char c,
        TREE_NODE * l,
        TREE_NODE * r)
{
    TREE_NODE* new_node = (TREE_NODE*)malloc(sizeof(TREE_NODE));

    new_node->child[left] = l;
    new_node->child[right] = r;

    new_node->c = c;

    return new_node;
}

QUEUE_NODE * NewQueueNode(
        TREE_NODE * data,
        unsigned int priority)
{
    QUEUE_NODE * new_node = (QUEUE_NODE *)malloc(sizeof(QUEUE_NODE));

    new_node->char_data = data;
    new_node->priority = priority;
    new_node->next = NULL;

    return new_node;
}

inline bool IsLeaf(
        TREE_NODE * node)
{
    if (node == NULL)
        return 0;

    return node->child[left] || node->child[right] ? false : true;
}

void DestroyTree(
        TREE_NODE * root)
{
    if (root == NULL)
        return;

    DestroyTree(root->child[left]);
    DestroyTree(root->child[right]);

    free(root);
}

inline TREE_NODE * Peek(
        QUEUE_NODE ** head)
{
    return (*head)->char_data;
}

inline void Pop(
        QUEUE_NODE ** head)
{
    QUEUE_NODE * temp = * head;
    (*head) = (*head)->next;

    free(temp);
}

inline int QueueIsEmpty(
        QUEUE_NODE ** head)
{
    return (* head) == NULL;
}

void QueuePush(
        QUEUE_NODE ** head,
        TREE_NODE * data,
        unsigned int priority)
{
    if (QueueIsEmpty(head)) {
        *head = NewQueueNode(data, priority);
        return;
    }

    QUEUE_NODE * start = (*head);

    // Create new Node
    QUEUE_NODE * temp = NewQueueNode(data, priority);

    // Special Case: The head of list has lesser
    // priority than new char_node. So insert new
    // char_node before head char_node and change head char_node.
    if ((*head)->priority > priority) {

        // Insert New Node before head
        temp->next = *head;
        (*head) = temp;
    }
    else {
        // Traverse the list and find a
        // position to insert new char_node
        while (start->next != NULL &&
               start->next->priority < priority) {
            start = start->next;
        }

        // Either at the ends of the list
        // or at required position
        temp->next = start->next;
        start->next = temp;
    }
}

int QueueLength(
        QUEUE_NODE ** head)
{
    QUEUE_NODE * start = *head;
    int len = 0;

    if (QueueIsEmpty(&start))
        return 0;

    while (start) {
        len++;
        start = start->next;
    }

    return len;
}

inline TREE_NODE * QueueGet(
        QUEUE_NODE ** head)
{
    TREE_NODE * temp = Peek(head);

    Pop(head);

    return temp;
}

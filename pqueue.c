/*----------------------------------------------------------------*
 * https://www.geeksforgeeks.org/priority-queue-using-linked-list *
 *----------------------------------------------------------------*/

#include "pqueue.h"
#include "definitions.h"

CHNode * newCHNode(
        unsigned int freq,
        unsigned char c,
        CHNode * l,
        CHNode * r)
{
    CHNode* new_node = (CHNode*)malloc(sizeof(CHNode));

    new_node->child[left] = l;
    new_node->child[right] = r;

    new_node->c = c;
    new_node->freq = freq;

    return new_node;
}

QNode * newQNode(
        CHNode * data)
{
    QNode* new_node = (QNode*)malloc(sizeof(QNode));

    new_node->char_data = data;
    new_node->next = NULL;

    return new_node;
}

inline bool isLeaf(
        CHNode *node)
{
    if (node == NULL)
        return 0;

    return node->child[left] || node->child[right] ? false : true;
}

void freeTree(
        CHNode * root)
{
    if (root == NULL)
        return;

    freeTree(root->child[left]);
    freeTree(root->child[right]);

    free(root);
}

inline CHNode * peek(
        QNode** head)
{
    return (*head)->char_data;
}

inline void pop(
        QNode ** head)
{
    QNode* temp = *head;
    (*head) = (*head)->next;
    free(temp);
}

inline int isEmpty(
        QNode ** head)
{
    return (*head) == NULL;
}

void push(
        QNode ** head,
        CHNode * data)
{
    if (isEmpty(head)) {
        *head = newQNode(data);
        return;
    }

    QNode* start = (*head);

    // Create new Node
    QNode* temp = newQNode(data);

    // Special Case: The head of list has lesser
    // priority than new char_node. So insert new
    // char_node before head char_node and change head char_node.
    if ((*head)->char_data->freq > data->freq) {

        // Insert New Node before head
        temp->next = *head;
        (*head) = temp;
    }
    else {
        // Traverse the list and find a
        // position to insert new char_node
        while (start->next != NULL &&
               start->next->char_data->freq < data->freq) {
            start = start->next;
        }

        // Either at the ends of the list
        // or at required position
        temp->next = start->next;
        start->next = temp;
    }
}

int queueLength(
        QNode ** head)
{
    QNode * start = *head;
    int len = 0;

    if (isEmpty(&start))
        return 0;

    while (start) {
        len++;
        start = start->next;
    }

    return len;
}

inline CHNode * get(
        QNode ** head)
{
    CHNode * temp = peek(head);
    pop(head);

    return temp;
}

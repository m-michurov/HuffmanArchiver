#include "pqueue.h"

CHNode* newCHNode(
        int freq,
        unsigned char c,
        CHNode * left,
        CHNode * right)
{
    CHNode* data = (CHNode*)malloc(sizeof(CHNode));

    data->left = left;
    data->right = right;

    data->c = c;
    data->freq = freq;

    return data;
}


QNode* newQNode(
        CHNode * data)
{
    QNode* temp = (QNode*)malloc(sizeof(QNode));

    temp->char_data = data;

    temp->next = NULL;

    return temp;
}


CHNode * peek(
        QNode** head)
{
    return (*head)->char_data;
}


void pop(
        QNode** head)
{
    QNode* temp = *head;
    (*head) = (*head)->next;
    free(temp);
}


int isEmpty(QNode** head)
{
    return (*head) == NULL;
}


void push(
        QNode** head,
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
        QNode **head)
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

CHNode * get(
        QNode** head)
{
    CHNode * temp = peek(head);
    pop(head);

    return temp;
}

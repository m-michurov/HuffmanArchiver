#pragma once

#ifndef HUFFMANARCHIVER_PQUEUE_H
#define HUFFMANARCHIVER_PQUEUE_H

#include <malloc.h>
#include <stdlib.h>

#include "definitions.h"

CHNode* newCHNode(
        int freq,
        unsigned char c,
        CHNode * left,
        CHNode * right);

// Function to Create A New Node
QNode* newQNode(
        CHNode * data);

// Return the value at head
CHNode * peek(
        QNode** head);

// Removes the element with the
// highest priority form the list
void pop(
        QNode** head);

// Function to check is list is empty
int isEmpty(
        QNode** head);

// Function to push according to priority
void push(
        QNode** head, CHNode * data);

// Function to get number of nodes in queue
int queueLength(
        QNode **head);

// Return element with the highest
// priority and remove it from the queue
CHNode * get(
        QNode** head);


#endif
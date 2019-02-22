#pragma once

#ifndef HUFFMANARCHIVER_PQUEUE_H
#define HUFFMANARCHIVER_PQUEUE_H

#include <malloc.h>
#include <stdlib.h>

#include "definitions.h"

// struct types TREE_NODE and QUEUE_NODE are defined in "definitions.h"


TREE_NODE * NewTreeNode(
        unsigned int quantity,
        unsigned char c,
        TREE_NODE * l,
        TREE_NODE * r);


QUEUE_NODE * NewQueueNode(
        TREE_NODE * data);


bool IsLeaf(
        TREE_NODE * node);


void DestroyTree(
        TREE_NODE * root);


TREE_NODE * Peek(
        QUEUE_NODE ** head);


void Pop(
        QUEUE_NODE ** head);


int QueueIsEmpty(
        QUEUE_NODE ** head);


void QueuePush(
        QUEUE_NODE ** head,
        TREE_NODE * data);


int QueueLength(
        QUEUE_NODE ** head);


TREE_NODE * QueueGet(
        QUEUE_NODE ** head);

#endif
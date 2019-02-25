#pragma once

#ifndef HUFFMANARCHIVER_PQUEUE_H
#define HUFFMANARCHIVER_PQUEUE_H

#include <malloc.h>
#include <stdlib.h>

#include "definitions.h"

typedef struct st_QueueNode QUEUE_NODE;
typedef struct st_TreeNode TREE_NODE;

// A binary tree node
struct st_TreeNode
{
    TREE_NODE * child[2];
    unsigned char c;
};

// A queue node
struct st_QueueNode
{
    TREE_NODE * char_data;

    unsigned int priority;

    QUEUE_NODE * next;
};


TREE_NODE * NewTreeNode(
        unsigned char c,
        TREE_NODE * l,
        TREE_NODE * r);


QUEUE_NODE * NewQueueNode(
        TREE_NODE * data,
        unsigned int priority);


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
        TREE_NODE * data,
        unsigned int priority);


int QueueLength(
        QUEUE_NODE ** head);


TREE_NODE * QueueGet(
        QUEUE_NODE ** head);

#endif
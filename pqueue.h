#pragma once

#ifndef HUFFMANARCHIVER_PQUEUE_H
#define HUFFMANARCHIVER_PQUEUE_H

#include <malloc.h>
#include <stdlib.h>

#include "definitions.h"

// struct types are defined in "definitions.h"

TREE_NODE * newCHNode(
        unsigned int freq,
        unsigned char c,
        TREE_NODE * l,
        TREE_NODE * r);


QUEUE_NODE * newQNode(
        TREE_NODE * data);


bool isLeaf(
        TREE_NODE * node);


void freeTree(
        TREE_NODE * root);


TREE_NODE * peek(
        QUEUE_NODE ** head);


void pop(
        QUEUE_NODE ** head);


int isEmpty(
        QUEUE_NODE ** head);


void push(
        QUEUE_NODE ** head, TREE_NODE * data);


int queueLength(
        QUEUE_NODE ** head);


TREE_NODE * get(
        QUEUE_NODE ** head);

#endif
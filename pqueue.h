#pragma once

#ifndef HUFFMANARCHIVER_PQUEUE_H
#define HUFFMANARCHIVER_PQUEUE_H

#include <malloc.h>
#include <stdlib.h>

#include "definitions.h"

// struct types are defined in "definitions.h"

CHNode * newCHNode(
        unsigned int freq,
        unsigned char c,
        CHNode * l,
        CHNode * r);


QNode * newQNode(
        CHNode * data);


bool isLeaf(
        CHNode * node);


void freeTree(
        CHNode * root);


CHNode * peek(
        QNode ** head);


void pop(
        QNode ** head);


int isEmpty(
        QNode ** head);


void push(
        QNode ** head, CHNode * data);


int queueLength(
        QNode ** head);


CHNode * get(
        QNode ** head);

#endif
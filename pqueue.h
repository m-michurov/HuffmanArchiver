#pragma once

#ifndef HUFFMANARCHIVER_PQUEUE_H
#define HUFFMANARCHIVER_PQUEUE_H

#include <malloc.h>
#include <stdlib.h>

#include "definitions.h"

CHNode * newCHNode(
        unsigned int freq,
        unsigned char c,
        CHNode * l,
        CHNode * r);


QNode * newQNode(
        CHNode * data);


extern inline CHNode * peek(
        QNode ** head);


void pop(
        QNode ** head);


extern inline int isEmpty(
        QNode ** head);


void push(
        QNode ** head, CHNode * data);


int queueLength(
        QNode ** head);


extern inline CHNode * get(
        QNode ** head);

#endif
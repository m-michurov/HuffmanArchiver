#pragma once

#ifndef HUFFMANARCHIVER_PQUEUE_H
#define HUFFMANARCHIVER_PQUEUE_H

#include <malloc.h>
#include <stdlib.h>

#include "definitions.h"

CHNode * newCHNode(
        int freq,
        unsigned char c,
        CHNode * left,
        CHNode * right);


QNode * newQNode(
        CHNode * data);


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
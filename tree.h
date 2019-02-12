#pragma once

#ifndef HUFFMANARCHIVER_TREE_H
#define HUFFMANARCHIVER_TREE_H

#include <stdlib.h>

#include "definitions.h"

bool isLeaf(
        CHNode * node);


unsigned int treeHeight(
        CHNode * root);

int treeCount(
        CHNode * root);

void makeLeavesArray(
        CHNode * node,
        unsigned char * arr);

#endif

#pragma once

#ifndef HUFFMANARCHIVER_CODING_H
#define HUFFMANARCHIVER_CODING_H

#include <stdio.h>
#include <string.h>

#include "definitions.h"
#include "pqueue.h"

// struct types are defined in "definitions.h"


void encode(
        FILE *in,
        FILE *out,
        bool skip);


void decode(
        FILE * in,
        FILE * out);

#endif

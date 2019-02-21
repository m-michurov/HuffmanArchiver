#pragma once

#ifndef HUFFMANARCHIVER_CODING_H
#define HUFFMANARCHIVER_CODING_H

#include <string.h>

#include "definitions.h"
#include "pqueue.h"
#include "binio.h"

void encode(
        FILE *fin,
        FILE *fout,
        bool skip);


void decode(
        FILE * fin,
        FILE * fout);

#endif
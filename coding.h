#pragma once

#ifndef HUFFMANARCHIVER_CODING_H
#define HUFFMANARCHIVER_CODING_H

#include <string.h>

#include "definitions.h"
#include "pqueue.h"
#include "binio.h"

// Boolean variable "skip" is used for special cases
// when data representing operation mode is stored
// in first three bytes of input file
void EncodeFile(
        char * in_file,
        char * out_file,
        bool skip_three);

// Boolean variable "skip" is used for special cases
// when data representing operation mode is stored
// in first three bytes of input file
void DecodeFile(
        char * in_file,
        char * out_file,
        bool skip_three);

#endif
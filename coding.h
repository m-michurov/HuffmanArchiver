#pragma once

#ifndef HUFFMANARCHIVER_CODING_H
#define HUFFMANARCHIVER_CODING_H

#include <string.h>

#include "definitions.h"
#include "pqueue.h"
#include "binio.h"

// A struct to store character code and it's length
typedef struct st_Code
{
    unsigned char code[MAX_BYTE_COUNT + 1];

    size_t len;
} Code;

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
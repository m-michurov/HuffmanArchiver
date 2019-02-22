#pragma once

#ifndef HUFFMANARCHIVER_BINIO_H
#define HUFFMANARCHIVER_BINIO_H

#include <stdio.h>
#include <string.h>

#include "pqueue.h"
#include "definitions.h"

typedef struct st_BitBuff
{
    size_t string_pos;
    size_t byte_pos;

    FILE * file;
    unsigned char * string;
} IO_BUFF;


void BitWrite(
        IO_BUFF * out,
        int bit);


int BitRead(
        IO_BUFF *in);


void ByteWrite(
        IO_BUFF * out,
        unsigned char byte);


unsigned char ByteRead(
        IO_BUFF * in);


void EndWrite(
        IO_BUFF * out);


void NextByte(
        IO_BUFF * buff);

// Boolean variable "mode" should be
// either WRITE or READ (defined in "definitions.h")
IO_BUFF * InitBinaryIO(
        FILE * file,
        bool mode);

#endif

#pragma once

#ifndef HUFFMANARCHIVER_BINIO_H
#define HUFFMANARCHIVER_BINIO_H

#include <stdio.h>
#include <string.h>

#include "pqueue.h"
#include "definitions.h"

typedef struct st_BitBuff
{
    unsigned char string[BLOCK_SIZE];

    size_t string_pos;
    size_t byte_pos;
    FILE * file;
} IO_BUFF;


void __write_bit(
        IO_BUFF * out,
        int bit);


int __read_bit(
        IO_BUFF * in);


void __write_byte(
        IO_BUFF * out,
        unsigned char byte);


unsigned char __read_byte(
        IO_BUFF * in);


void __write_end(
        IO_BUFF * out);


void __next(
        IO_BUFF * buff);


IO_BUFF * __io_stream_init(
        FILE * file,
        bool mode); // mode is either WRITE or READ (defined in "definitions.h")

#endif

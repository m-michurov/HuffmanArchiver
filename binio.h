#pragma once

#ifndef HUFFMANARCHIVER_BINIO_H
#define HUFFMANARCHIVER_BINIO_H

#include <stdio.h>
#include <string.h>

#include "pqueue.h"
#include "definitions.h"

typedef struct st_BitBuff
{
    unsigned char * string;

    size_t string_pos;
    size_t byte_pos;
    FILE * file;
} IO_BUFF;


void write_bit(
        IO_BUFF *out,
        int bit);


int read_bit(
        IO_BUFF *in);


void write_byte(
        IO_BUFF *out,
        unsigned char byte);


unsigned char read_byte(
        IO_BUFF *in);


void write_end(
        IO_BUFF *out);


void next(
        IO_BUFF *buff);


IO_BUFF * io_stream_init(
        FILE *file,
        bool mode); // mode is either WRITE or READ (defined in "definitions.h")

#endif

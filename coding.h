#pragma once

#ifndef HUFFMANARCHIVER_CODING_H
#define HUFFMANARCHIVER_CODING_H

#include <stdio.h>

#include "definitions.h"
#include "pqueue.h"

// struct types are defined in "definitions.h"


static inline int write_bit(
        FILE * out,
        int bit,
        bool last);


static inline int read_bit(
        FILE * in,
        bool last);


static inline int write_byte(
        FILE * out,
        unsigned char byte);


static inline char read_byte(
        FILE * in);


static CHNode * read_tree(
        FILE * in);


static void write_tree(
        FILE *out,
        CHNode *node,
        bool first);


static Code * make_code_from_string(
        const char *str_code,
        int code_len);


static void build_codes(
        CHNode *node,
        unsigned int pos,
        Code **table);


unsigned char * count_freqs(
        FILE * in,
        unsigned int * len);


static void init_encode(
        FILE *in,
        FILE *out,
        QNode **pq,
        Code **codes);


static CHNode * init_decode(
        FILE * in,
        CHNode * tree,
        unsigned int * len);


static inline void encode_string(
        unsigned char *string,
        unsigned int string_len,
        Code **codes,
        FILE *out,
        bool last);


void encode(
        FILE *in,
        FILE *out,
        bool skip);


void decode(
        FILE * in,
        FILE * out);

#endif

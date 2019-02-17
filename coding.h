#pragma once

#ifndef HUFFMANARCHIVER_CODING_H
#define HUFFMANARCHIVER_CODING_H

static inline int write_bit(
        FILE * out,
        int bit,
        bool last);

static inline int read_bit(
        FILE * in,
        bool last);

void encode_string(
        unsigned char *string,
        unsigned int string_len,
        Code **codes,
        FILE *out,
        bool last);

static CHNode * read_tree(
        FILE * in,
        unsigned char * leaves,
        unsigned int max);

static void write_tree(
        FILE *out,
        CHNode *node,
        bool first);

static void write_header(
        CHNode *node,
        FILE *out,
        unsigned short char_count,
        unsigned int len);

static Code * make_code(
        const char * str_code,
        int code_len);

static void build_codes(
        CHNode *node,
        unsigned int pos,
        Code **table);

void init_encode(
        FILE *in,
        FILE *out,
        QNode **pq,
        Code **codes);


CHNode * init_decode(
        FILE * in,
        CHNode * tree,
        unsigned int * len);


void encode(
        FILE *in,
        FILE *out,
        bool skip);


void decode(
        FILE * in,
        FILE * out);

#endif

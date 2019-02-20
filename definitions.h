#pragma once

#ifndef HUFFMANARCHIVER_DEFINITIONS_H
#define HUFFMANARCHIVER_DEFINITIONS_H

#include <stdio.h>

#define BLOCK_SIZE 256 // 2^8 do not change

#define INPUT_FILE_ERROR {puts("Error: cannot access input file"); exit(EXIT_SUCCESS);};
#define MEM_ERROR {puts("Error: unable to allocate memory"); exit(EXIT_SUCCESS);};
#define SIZE_ERROR {puts("Error: input file too big"); exit(EXIT_SUCCESS);};
#define OUTPUT_FILE_ERROR {puts("Error: cannot access output file"); exit(EXIT_SUCCESS);};
#define CMD_ARGUMENTS_ERROR {puts("Error: invalid combination of parameters. Usage:\nNAME\n\t" \
                                    "HuffmanArchiver - Huffman-encode/decode files\nSYNOPSIS\n\tHuffmanArchiver " \
                                    "{-e|-d} <inputfilename> <outputfilename>\nDESCRIPTION\n\tEncode/decode " \
                                    "INPUTFILENAME to OUTPUTFILENAME\n\n\tOptions:\n\t\t-e\tencode\n\t\t-d\t" \
                                    "decode\n\t\t"); exit(EXIT_SUCCESS);};

#define true 1
#define false 0

#define right 1
#define left 0

typedef unsigned int bool;

typedef struct st_QNode QNode;
typedef struct st_CHnode CHNode;

// a binary tree node
struct st_CHnode {
    CHNode * child[2];
    unsigned int freq;
    unsigned char c;
};

// a queue node
struct st_QNode {
    CHNode * char_data;

    QNode * next;
};

typedef struct st_Code {
    char code[BLOCK_SIZE + 1];
    unsigned short len;
} Code;

typedef struct st_BitBuff {
    char string[BLOCK_SIZE];
    int string_pos;
    int byte_pos;
    FILE * file;
} IO_BUFF;

#endif

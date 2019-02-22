#pragma once

#ifndef HUFFMANARCHIVER_DEFINITIONS_H
#define HUFFMANARCHIVER_DEFINITIONS_H

#include <stdio.h>

#define INPUT_FILE_ERROR {puts("Error: cannot access input file"); exit(EXIT_SUCCESS);};
#define MEM_ERROR {puts("Error: unable to allocate memory"); exit(EXIT_SUCCESS);};
#define SIZE_ERROR {puts("Error: input file too big"); exit(EXIT_SUCCESS);};
#define OUTPUT_FILE_ERROR {puts("Error: cannot access output file"); exit(EXIT_SUCCESS);};
#define CMD_ARGUMENTS_ERROR {puts("Error: invalid combination of parameters. Usage:\nNAME\n\t" \
                                    "HuffmanArchiver - Huffman-encode/decode files\nSYNOPSIS\n\tHuffmanArchiver " \
                                    "{-e|-d} <inputfilename> <outputfilename>\nDESCRIPTION\n\tEncode/decode " \
                                    "INPUTFILENAME to OUTPUTFILENAME\n\n\tOptions:\n\t\t-e\tencode\n\t\t-d\t" \
                                    "decode\n\t\t"); exit(EXIT_SUCCESS);};

#define BLOCK_SIZE 256
#define MAX_BYTE_COUNT 256 // 2^8 do not change

#define true 1
#define false 0

#define WRITE 256
#define READ 0

#define right 1
#define left 0

typedef unsigned int bool;

typedef struct st_QueueNode QUEUE_NODE;
typedef struct st_TreeNode TREE_NODE;

// a binary tree node
struct st_TreeNode
{
    TREE_NODE * child[2];
    size_t quantity;
    unsigned char c;
};

// a queue node
struct st_QueueNode
{
    TREE_NODE * char_data;

    QUEUE_NODE * next;
};

// a struct to store Huffman code and it's length
typedef struct st_Code
{
    unsigned char code[BLOCK_SIZE + 1];

    size_t len;
} Code;

#endif

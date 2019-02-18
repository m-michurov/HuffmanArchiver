#pragma once

#ifndef HUFFMANARCHIVER_DEFINITIONS_H
#define HUFFMANARCHIVER_DEFINITIONS_H

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

typedef struct st_qnode QNode;
typedef struct st_chnode CHNode;

// a binary tree node
struct st_chnode {
    CHNode * child[2];
    unsigned int freq;
    unsigned char c;
};

// a queue node
struct st_qnode {
    CHNode * char_data;

    QNode * next;
};

typedef struct st_code {
    long long int code;
    unsigned char len;
} Code;

#endif

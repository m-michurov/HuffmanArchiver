//
// Created by Мичуров on 10.02.2019.
//

#ifndef HUFFMANARCHIVER_DEFINITIONS_H
#define HUFFMANARCHIVER_DEFINITIONS_H

#define BLOCK_SIZE 256

#define INPUT_FILE_ERROR {puts("Error: cannot access input file"); exit(EXIT_SUCCESS);};
#define MEM_ERROR {puts("Error: unable to allocate memory"); exit(EXIT_SUCCESS);};
#define SIZE_ERROR {puts("Error: input file too big"); exit(EXIT_SUCCESS);};
#define OUTPUT_FILE_ERROR {puts("Error: cannot access output file"); exit(EXIT_SUCCESS);};
#define CMD_ARGUMENTS_ERROR {puts("Error: invalid combination of parameters. Usage:\nNAME\n\tarc - Huffman-encode/decode files\nSYNOPSIS\n\tarc {-e|-d} <inputfilename> <outputfilename>\nDESCRIPTION\n\tEncode/decode INPUTFILENAME to TARGETFILENAME\n\n\tOptions:\n\t\t-e\tencode\n\t\t-d\tdecode\n\t\t"); exit(EXIT_SUCCESS);};

#define true 1
#define false 0

#define right 1
#define left 0

typedef unsigned int bool;

typedef struct st_chnode CHNode;

struct st_chnode {
    CHNode * child[2];
    unsigned int freq;
    unsigned char c;
};

typedef struct st_code {
    long long int code;
    unsigned char len;
} Code;


typedef struct st_qnode QNode;

struct st_qnode {
    CHNode * char_data;

    QNode * next;

};

#endif

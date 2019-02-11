//
// Created by Мичуров on 10.02.2019.
//

#ifndef HUFFMANARCHIVER_DEFINITIONS_H
#define HUFFMANARCHIVER_DEFINITIONS_H

#define BLOCK_SIZE 1024

#define INPUT_FILE_ERROR {puts("Error: cannot access input file"); exit(EXIT_SUCCESS);};
#define MEM_ERROR {puts("Error: unable to allocate memory"); exit(EXIT_SUCCESS);};
#define SIZE_ERROR {puts("Error: input file too big"); exit(EXIT_SUCCESS);};
#define ACCESS_ERROR {puts("Error: function call with invalid parameters"); exit(EXIT_SUCCESS);};
#define OUTPUT_FILE_ERROR {puts("Error: cannot access output file"); exit(EXIT_SUCCESS);};


#define true 1
#define false 0

typedef unsigned int bool;

typedef struct st_chnode CHNode;

struct st_chnode {
    CHNode *left, *right;
    int freq;
    unsigned char c;
};


typedef struct st_qnode QNode;

struct st_qnode {
    CHNode * char_data;

    QNode * next;

};

#endif

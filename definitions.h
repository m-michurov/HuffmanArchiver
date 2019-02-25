#pragma once

#ifndef HUFFMANARCHIVER_DEFINITIONS_H
#define HUFFMANARCHIVER_DEFINITIONS_H

#include <stdio.h>

#define INPUT_FILE_ERROR    {puts("Error: cannot access input file");   exit(EXIT_SUCCESS);};
#define MEM_ERROR           {puts("Error: unable to allocate memory");  exit(EXIT_SUCCESS);};
#define SIZE_ERROR          {puts("Error: input file too big");         exit(EXIT_SUCCESS);};
#define OUTPUT_FILE_ERROR   {puts("Error: cannot access output file");  exit(EXIT_SUCCESS);};
#define CMD_ARGUMENTS_ERROR {puts("Error: invalid combination of parameters. Usage:\nNAME\n\t" \
                                  "HuffmanArchiver - Encode/decode files\nSYNOPSIS\n\tHuffman" \
                                  "Archiver {-e|-d} <inputfilename> <outputfilename>\n" \
                                  "DESCRIPTION\n\tEncode/Decode INPUTFILENAME to " \
                                  "OUTPUTFILENAME\n\n\tOptions:\n\t\t-e\tEncode file\n\t\t-d\t" \
                                  "Decode file\n\t\t"); exit(EXIT_SUCCESS);};

#define MAX_BYTE_COUNT  256 // 2^8 do not change
#define BLOCK_SIZE      MAX_BYTE_COUNT * MAX_BYTE_COUNT

typedef unsigned int bool;

#define true    1
#define false   0

#define right   1
#define left    0

#endif

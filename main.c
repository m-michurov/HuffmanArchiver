#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <limits.h>

#include "definitions.h"
#include "coding.h"
#include <limits.h>

int main(
        int argc,
        char * argv[])
{
    char buff;

    if (argc == 4) {
        if (!strcmp(argv[1], "-e")) {
            EncodeFile(argv[argc - 2], argv[argc - 1], false);
        }
        else if (!strcmp(argv[1], "-d")) {
            DecodeFile(argv[argc - 2], argv[argc - 1], false);
        }
        else CMD_ARGUMENTS_ERROR;
    }
    else if (argc == 1) {
        FILE * in = fopen("in.txt", "rb");

        if (in == NULL) INPUT_FILE_ERROR;

        fread(&buff, 1, 1, in);

        fclose(in);

        if (buff == 'c') {
            EncodeFile("in.txt", "out.txt", true);
        }
        else if (buff == 'd') {
            DecodeFile("in.txt", "out.txt", true);
        }
        else CMD_ARGUMENTS_ERROR;
    }
    else CMD_ARGUMENTS_ERROR;

    return 0;
}
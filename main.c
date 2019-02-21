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
        char *argv[])
{
    if (argc == 4) {

        FILE * in = fopen(argv[argc - 2], "rb"),
             * out = fopen(argv[argc - 1], "wb");

        if (in == NULL) INPUT_FILE_ERROR;
        if (out == NULL) OUTPUT_FILE_ERROR;

        if (!strcmp(argv[1], "-e")) {
            encode(in, out, false);
        }
        else if (!strcmp(argv[1], "-d")) {
            decode(in, out);
        }
        else CMD_ARGUMENTS_ERROR;

        fclose(in);
        fclose(out);
    }
    else if (argc == 1) {
        FILE * in = fopen("in.txt", "rb"),
             * out = fopen("out.txt", "wb");

        if (in == NULL) INPUT_FILE_ERROR;
        if (out == NULL) OUTPUT_FILE_ERROR;

        char buff;

        fread(&buff, 1, 1, in);

        fseek(in, 3, 0);

        if (buff == 'c') {
            encode(in, out, true);
        }
        else if (buff == 'd') {
            decode(in, out);
        }
        else CMD_ARGUMENTS_ERROR;

        fclose(in);
        fclose(out);
    }
    else CMD_ARGUMENTS_ERROR;

    return 0;
}
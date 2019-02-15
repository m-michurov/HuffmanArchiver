/*---------------------------------------------------------------*
 * https://www.geeksforgeeks.org/priority-queue-using-linked-list*
 *---------------------------------------------------------------*/


#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <limits.h>

#include "definitions.h"
#include "coding.h"

int main(
        int argc,
        char *argv[])
{
    if (argc != 4) CMD_ARGUMENTS_ERROR;

    FILE *in = fopen(argv[argc - 2], "rb"),
         *out = fopen(argv[argc - 1], "wb");

    if (in == NULL) INPUT_FILE_ERROR;
    if (out == NULL) OUTPUT_FILE_ERROR;

    if (!strcmp(argv[1], "-e"))
        encode(in, out);

    else if (!strcmp(argv[1], "-d"))
        decode(in, out);

    else CMD_ARGUMENTS_ERROR;

    fclose(in);
    fclose(out);

    return 0;
}
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
        void)
{
    FILE *in = fopen("in.txt", "rb"),
         *out = fopen("out.txt", "wb");

    if (in == NULL) INPUT_FILE_ERROR;
    if (out == NULL) OUTPUT_FILE_ERROR;

    QNode *pq = 0;

    Code *codes[256] = { 0 };

    init_encode(in, out, &pq, codes);

    //printf("\n%u %u %u %u\n", queueLength(&pq), peek(&pq)->freq, treeHeight(pq->char_data), treeCount(pq->char_data));

    encode(in, out, codes);

    fclose(in);
    fclose(out);

    FILE *_in = fopen("out.txt", "rb"),
         *_out = fopen("new.txt", "wb");

    if (_in == NULL) INPUT_FILE_ERROR;
    if (_out == NULL) OUTPUT_FILE_ERROR;

    CHNode *tree = 0;

    unsigned int len;

    tree = init_decode(_in, tree, &len);

    decode(_in, _out, tree, len);

    fclose(_in);
    fclose(_out);

    return 0;
}
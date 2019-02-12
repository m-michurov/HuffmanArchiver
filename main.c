/*---------------------------------------------------------------*
 * https://www.geeksforgeeks.org/priority-queue-using-linked-list*
 *---------------------------------------------------------------*/


#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <limits.h>

#include "definitions.h"
#include "tree.h"
#include "pqueue.h"


int write_bit(
        FILE * out,
        int bit,
        bool last)
{
    static char buff = 0;
    static int pos = 0;

    buff += (bit & 0x01) << (7 - pos++);

    if (pos > 7 || last)
        fputc(buff, out), buff = 0, pos = 0;

    return pos;
}

int read_bit(
        FILE * in,
        bool last)
{
    static char buff;
    static int pos = 8;

    if (pos > 7)
        buff = (char)fgetc(in), pos = 0;

    if (last)
        pos = 7;

    return (buff >> (7 - pos++))  & 0x01;
}

void make_everything_nice(
        unsigned char *string,
        unsigned int string_len,
        Code ** codes,
        FILE * out,
        bool last)
{
    static long long int code;

    static   int code_len = 0,
                        pos = 0;

    static int rest;

    while (string_len > 0) {
        code = codes[*string]->code;
        code_len = codes[*(string++)]->len;

        for (pos = code_len - 1 ; pos >= 0; pos--)
            rest = write_bit(out, (int)(code >> pos) & 0x01, false);

        string_len--;

    }

    if (rest && last)
        write_bit(out, 0, true);;
}

CHNode * read_tree(
        FILE * in,
        unsigned char * leaves,
        unsigned int max)
{
    static unsigned int leaves_count = 0;

    static int bit;

    CHNode * node;

    bit = read_bit(in, false);

    if (bit == 0) {
        node = newCHNode(0, leaves[leaves_count++], 0, 0);
    } else {
        node = newCHNode(0, 0, 0, 0);
        node->left = read_tree(in, leaves, max);
        node->right = read_tree(in, leaves, max);
    }

    return node;
}

void write_tree(
        FILE *out,
        CHNode *node,
        bool first)
{
    static int rest;

    if (isLeaf(node)) {
        rest = write_bit(out, 0, false);
    }
    else {
        rest = write_bit(out, 1, false);

        write_tree(out, node->left, false);
        write_tree(out, node->right, false);
    }

    if (first && rest)
        write_bit(out, 0, true);
}

void write_header(
        CHNode *node,
        FILE *out,
        unsigned char char_count,
        unsigned int len)
{
    printf("\n>>%u\n", char_count);
    unsigned char * leaves = (unsigned char *)malloc(char_count);

    if (leaves == NULL) MEM_ERROR;

    makeLeavesArray(node, leaves);

    fwrite(&len, sizeof(int), 1, out);

    printf("\nwritten len %u\n", len);

    fwrite(&char_count, 1, 1, out);

    printf("\nwritten char count %u\n", char_count);

    fwrite(leaves, 1, char_count, out);

    for (int t = 0; t < char_count; t++)
        putchar(leaves[t]);

    puts("");

    write_tree(out, node, true);
}

Code * make_code(
        const char * str_code,
        int code_len)
{
    Code * new_code = malloc(sizeof(Code));

    if (new_code == NULL) MEM_ERROR;

    new_code->code = 0;
    new_code->len = (unsigned char)code_len;

    for (int y = 0; y < code_len ; y++)
        new_code->code += ((str_code[y] - '0') << ( code_len - y - 1));
    return new_code;
}

void build_codes(
        CHNode *node,
        unsigned int pos,
        unsigned char * buff,
        Code **table)
{

    if (isLeaf(node)) {
        if (pos == 0)
            buff[pos++] = '0';

        buff[pos] = 0;

        table[node->c] = make_code((char *)buff, pos);

        return;
    }
    buff[pos] = '0';
    build_codes(node->left, pos + 1, buff, table);

    buff[pos] = '1';
    build_codes(node->right, pos + 1, buff, table);

}

void init_encode(
        FILE *in,
        FILE *out,
        QNode **pq,
        Code **codes)
{
    unsigned char
            input_buff[BLOCK_SIZE],
            char_freqs[UCHAR_MAX + 1],
            scale = 0;

    unsigned int
            max = UCHAR_MAX,
            k = 0,
            freqs[256] = { 0 },
            len = 0,
            pos = 0,
            CHAR_COUNT = 0;

    unsigned long long total = 0;

    for ( ;(len = fread(input_buff, 1, BLOCK_SIZE, in)); pos = 0, total += len)
        while (pos < len)
            freqs[(int) input_buff[pos++]]++;

    rewind(in);

    if (total > UINT_MAX)
    SIZE_ERROR;

    len = (unsigned int) total;


    for (k = 0; k <= UCHAR_MAX; k++)
        if (freqs[k] > max)
            max = freqs[k];


    for (k = 0; k <= UCHAR_MAX; k++) {
        scale = (unsigned char)(freqs[k] / ((double)max / (double)UCHAR_MAX));
        if (scale == 0 && freqs[k] != 0)
            char_freqs[k] = 1;
        else
            char_freqs[k] = scale;
    }

    for (int i = 0; i < 256; i++)
        if (char_freqs[i])
            push(pq, newCHNode(char_freqs[i], (unsigned char)i, 0, 0)), CHAR_COUNT++;


    CHNode * a, * b;

    while (queueLength(pq) > 1) {
        a = get(pq);
        b = get(pq);
        push(pq, newCHNode(a->freq + b->freq, 0, a, b));
    }

    write_header((*pq)->char_data, out, (unsigned char)CHAR_COUNT, len);

    build_codes((*pq)->char_data, 0, (unsigned char *)malloc(64), codes);
    /*
    for (int y = 0; y < 256; y++)
        if (codes[y]) {
            printf("%c : ", y);

            for (int u = 0; u < codes[y]->len; u++)
                printf("%d", (int)(codes[y]->code >> ( codes[y]->len - u - 1)) & 0x01);
            puts("");
        }
    */
}


CHNode * init_decode(
        FILE * in,
        CHNode * tree,
        unsigned int * len)
{
    fread(len, sizeof(int), 1, in);

    //printf("\nread len %u\n", *len);

    Code *codes[256] = { 0 };

    unsigned char leaves_count;

    fread(&leaves_count, 1, 1, in);

    //printf("\nread char count %u\n", leaves_count);

    unsigned char * leaves = (unsigned char *)malloc(leaves_count);

    if (leaves == NULL) MEM_ERROR;

    fread(leaves, 1, leaves_count, in);

    /*
    for (int t = 0; t < leaves_count; t++)
        putchar(leaves[t]);
    */
    //puts("");

    tree = read_tree(in, leaves, leaves_count);

    if (treeCount(tree) % 8)
        read_bit(in, true);

    build_codes(tree, 0, (unsigned char *)malloc(64), codes);

    /*
    for (int y = 0; y < 256; y++)
        if (codes[y]) {
            printf("%c : ", y);

            for (int u = 0; u < codes[y]->len; u++)
                printf("%d", (int)(codes[y]->code >> ( codes[y]->len - u - 1)) & 0x01);
            puts("");
        }
    */

    return tree;

}


void encode(
        FILE *in,
        FILE *out,
        Code ** codes)
{
    unsigned char input_buff[8];
    unsigned int len = 0;

    while ((len = fread(input_buff, 1, 8, in)) == 8)
        make_everything_nice(input_buff, len, codes, out, false);

    make_everything_nice(input_buff, len, codes, out, true);
}


void decode(
        FILE * in,
        FILE * out,
        CHNode *t,
        unsigned int len)
{
    CHNode *n = t;
    static int bit;

    if (isLeaf(t))
        while (len > 0)
            fputc(n->c, out), len--;

    else
        while (len > 0) {
            bit = read_bit(in, false);

            if (bit)
                n = n->right;
            else
                n = n->left;

            if (isLeaf(n))
                fputc(n->c, out), n = t, len--;
        }
}


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
#include <stdio.h>

#include "definitions.h"
#include "coding.h"
#include "tree.h"
#include "pqueue.h"

static inline int write_bit(
        FILE * out,
        int bit,
        bool last)
{
    static char buff = 0;
    static int pos = 0;

    //printf(" %d", pos);

    buff += (bit & 0x01) << (7 - pos++);

    if (pos > 7 || last) {
        fputc(buff, out);
        //printf("%d ", buff);
        buff = 0;
        pos = 0;
    }

    return pos;
}

static inline int read_bit(
        FILE * in,
        bool last)
{
    static char buff;
    static int pos = 8;

    if (pos > 7) {
        buff = (char) fgetc(in);
        //printf("%d ", buff);
        pos = 0;
    }

    if (last && (pos != 0)) {
        buff = (char) fgetc(in);
        //printf("%d ", buff);
        pos = 0;
        return 0;
    }

    //printf(" %d", pos);

    return (buff >> (7 - pos++))  & 0x01;
}

void encode_string(
        unsigned char *string,
        unsigned int string_len,
        Code **codes,
        FILE *out,
        bool last)
{
    static long long int code;

    static int code_len = 0,
               pos = 0;

    static int rest;

    while (string_len > 0) {
        code = codes[*string]->code;
        code_len = codes[*(string++)]->len;

        for (pos = code_len - 1 ; pos >= 0; pos--) {
            rest = write_bit(out, (int) ((code >> pos) & 0x01), false);
            //printf("(%d)", (int) ((code >> pos) & 0x01));
        }

        string_len--;
    }

    if (rest && last)
        write_bit(out, 0, true);;
}

// max - ???
static CHNode * read_tree(
        FILE * in,
        unsigned char * leaves,
        unsigned int max)
{
    static unsigned int leaves_count = 0;

    static int bit;

    CHNode * node;

    bit = read_bit(in, false);

    //printf("%d", bit);

    if (bit == 0) {
        node = newCHNode(0, leaves[leaves_count++], 0, 0);
    } else {
        node = newCHNode(0, 0, 0, 0);
        node->child[left] = read_tree(in, leaves, max);
        node->child[right] = read_tree(in, leaves, max);
    }

    return node;
}

static void write_tree(
        FILE *out,
        CHNode *node,
        bool first) {
    static int rest;

    if (isLeaf(node)) {
        rest = write_bit(out, 0, false);
        //printf("%d", 0);
    } else {
        rest = write_bit(out, 1, false);
        //printf("%d", 1);
        write_tree(out, node->child[left], false);
        write_tree(out, node->child[right], false);
    }

    if (first && rest)
        write_bit(out, 0, true);
}

static void write_header(
        CHNode *node,
        FILE *out,
        unsigned short char_count,
        unsigned int len)
{
    unsigned char * leaves = (unsigned char *)malloc(char_count);

    if (leaves == NULL) MEM_ERROR;

    makeLeavesArray(node, leaves);

    fwrite(&len, sizeof(int), 1, out);

    fwrite(&char_count, sizeof(short), 1, out);

    fwrite(leaves, 1, char_count, out);

    write_tree(out, node, true);
/*
    for (int t = 0; t < char_count; t++)
        putchar(leaves[t]);

    printf("\n>>%u\n", char_count);

    printf("\nwritten len %u\n", len);

    printf("\nwritten char count %u\n", char_count);

    puts("");
    */
}

static Code * make_code(
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

static void build_codes(
        CHNode *node,
        unsigned int pos,
        Code **table)
{
    if (node == NULL)
        return;

    static char buff[BLOCK_SIZE / 4];

    if (isLeaf(node)) {
        buff[pos] = 0;

        table[node->c] = make_code((char *)buff, pos);

        return;
    }

    buff[pos] = '0';
    build_codes(node->child[left], pos + 1, table);

    buff[pos] = '1';
    build_codes(node->child[right], pos + 1, table);

}

void init_encode(
        FILE *in,
        FILE *out,
        QNode **pq,
        Code **codes)
{
    unsigned char
            input_buff[BLOCK_SIZE],
            char_freqs[BLOCK_SIZE],
            scale = 0;

    unsigned int
            max = UCHAR_MAX,
            k = 0,
            freqs[BLOCK_SIZE] = { 0 },
            len = 0,
            pos = 0;

    unsigned short char_count = 0;

    unsigned long long total = 0;
    printf("%d\n", len);

    for ( ;(len = fread(input_buff, 1, BLOCK_SIZE, in)) > 0; pos = 0, total += len) {
        //printf("%d\n", len);
        while (pos < len)
            freqs[(int) input_buff[pos++]]++;
    }

    if (total > UINT_MAX)
        SIZE_ERROR;

    len = (unsigned int) total;

    for (k = 0; k < BLOCK_SIZE; k++)
        if (freqs[k] > max)
            max = freqs[k];


    for (k = 0; k < BLOCK_SIZE; k++) {
        scale = (unsigned char)(freqs[k] / ((double)max / (double)UCHAR_MAX));
        if (scale == 0 && freqs[k] != 0)
            char_freqs[k] = 1;
        else
            char_freqs[k] = scale;
    }

    for (int i = 0; i < BLOCK_SIZE; i++)
        if (char_freqs[i])
            push(pq, newCHNode(char_freqs[i], (unsigned char)i, 0, 0)), char_count++;

    if (!char_count)
        return;

    CHNode * a, * b;

    while (queueLength(pq) > 1) {
        a = get(pq);
        b = get(pq);
        push(pq, newCHNode(a->freq + b->freq, 0, a, b));
    }

    write_header((*pq)->char_data, out, char_count, len);

    build_codes((*pq)->char_data, 0, codes);

    printf("counted char count %d\ntree nodes count %d\nheader takes %d byes\n", char_count, treeCount((*pq)->char_data),
           sizeof(int) + sizeof(short) + char_count + (treeCount((*pq)->char_data) + 7) / 8);

    for (int y = 0; y < BLOCK_SIZE; y++)
        if (codes[y]) {
            printf("%c : ", y);

            for (int u = 0; u < codes[y]->len; u++)
                printf("%d", (int)(codes[y]->code >> ( codes[y]->len - u - 1)) & 0x01);
            puts("");
        }

}


CHNode * init_decode(
        FILE * in,
        CHNode * tree,
        unsigned int * len)
{
    //Code *codes[256] = { 0 };

    unsigned short leaves_count;

    if (!fread(len, sizeof(int), 1, in)) {
        *len = 0;
        return NULL;
    }

    if (!fread(&leaves_count, sizeof(short), 1, in))
        return NULL;

    unsigned char * leaves = (unsigned char *)malloc(leaves_count);

    if (leaves == NULL) MEM_ERROR;

    if (!fread(leaves, 1, leaves_count, in))
        return NULL;

    tree = read_tree(in, leaves, leaves_count);

    read_bit(in, true);

    //puts("");
/*
    build_codes(tree, 0, codes);

    printf("\n read len %u\n", *len);

    printf("\n read char count %u\n", leaves_count);

    for (int t = 0; t < leaves_count; t++)
        putchar(leaves[t]);

    puts("");


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
        bool skip)
{
    QNode *pq = 0;

    Code *codes[BLOCK_SIZE] = { 0 };

    unsigned char input_buff[BLOCK_SIZE];
    unsigned int len = 0;

    init_encode(in, out, &pq, codes);

    rewind(in);

    if (skip) {
        fseek(in, 3, 0);
        //while (fgetc(in) == '\r');
    }

    //puts("");

    while ((len = fread(input_buff, 1, BLOCK_SIZE, in)) == BLOCK_SIZE)
        encode_string(input_buff, len, codes, out, false);

    encode_string(input_buff, len, codes, out, true);
}


void decode(
        FILE * in,
        FILE * out)
{
    unsigned int len = 0;

    CHNode *tree = 0,
           *n = 0;

    tree = init_decode(in, tree, &len);

    if (tree == NULL)
        return;
    n = tree;

    int bit;

    //printf("%d", len);

    if (isLeaf(tree)) {
        while (len > 0)
            fputc(n->c, out), len--;
    }
    else {
        while (len > 0) {
            bit = read_bit(in, false);
            //printf("(%d)", bit);

            if (bit)
                n = n->child[right];
            else
                n = n->child[left];

            if (isLeaf(n))
                fputc(n->c, out), n = tree, len--;
        }
    }
}

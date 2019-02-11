/*----------------------------------------------------*
 *    https://rosettacode.org/wiki/Huffman_coding#C   *
 *----------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <limits.h>

#include "definitions.h"

#define true 1
#define false 0

typedef unsigned int bool;


typedef struct char_node {
    struct CHNode *left, *right;
    int freq;
    unsigned char c;
} CHNode;

bool isLeaf(CHNode * n) {
    return ((n->left != NULL) || (n->right != NULL)) ? false : true;
}


unsigned int make_everything_nice(
        unsigned const char *string,
        unsigned int string_len,
        unsigned char ** huffman_code,
        FILE * out,
        bool last)
{
    static unsigned char buff = 0,
            * code = 0,
            a = 0;

    static unsigned int code_len = 0,
            buff_pos = 0,
            code_pos = 0,
            count = 0;

    while (string_len) {
        a = code_pos < code_len ? a : *(string++);
        code_pos = code_pos < code_len ? code_pos : 0;

        code = huffman_code[a];
        code_len = strlen((char *)code);

        for (/*code_pos = buff_pos > 7 ? code_pos : 0*/; code_pos < code_len && buff_pos < 8; code_pos++, buff_pos++) {
            buff += (code[code_pos] - '0') << (7 - buff_pos);

            //putchar('0' + ((buff >> (7 - buff_pos)) & 0x01));

            /*for (int k = 0; k < buff_pos; k++)
                putchar('_');
            printf("bp: %d cp: %d c: %d\n", buff_pos, code_pos, code[code_pos] - '0');
            */
        }

        if (buff_pos > 7) {
            fputc(buff, out);
            buff = 0, buff_pos = 0;
            //puts("");
            //printf("%d\n", count++);
        }

        if (code_pos == code_len)
            string_len--;

    }

    if (last) {
        fputc(buff, out);
        //printf("last byte: '%c' : %d\n", buff, buff);
    }

/*
    if (buff_pos < 8)
        for ( ; buff_pos < 8; buff_pos++)
            buff += 0 << (7 - buff_pos);
    else
*/
    return 0;
}


CHNode *new_node(int freq, unsigned char c, CHNode *a, CHNode *b, CHNode pool[256])
{
    static int n_nodes = 0;
    CHNode *n = pool + n_nodes++;

    if (freq)
        n->c = c, n->freq = freq;
    else {
        n->left = a, n->right = b;
        n->freq = a->freq + b->freq;
    }
    return n;
}


void qinsert(CHNode *n, CHNode ** queue, int * qend)
{
    int j, i = (*qend)++;
    while ((j = i / 2)) {
        if (queue[j]->freq <= n->freq) break;
        queue[i] = queue[j], i = j;
    }
    queue[i] = n;
}


CHNode *qremove(CHNode ** queue, int * qend)
{
    int i, l;
    CHNode *n = queue[i = 1];

    if ((*qend) < 2) return 0;
    (*qend)--;
    while ((l = i * 2) < *qend) {
        if (l + 1 < (*qend) && queue[l + 1]->freq < queue[l]->freq) l++;
        queue[i] = queue[l], i = l;
    }
    queue[i] = queue[*qend];

    printf(">>%d\n",* qend);

    return n;
}


void build_code(CHNode *n, unsigned char *s, int len, unsigned char ** code_table)
{
    static unsigned char buf[1024];
    static unsigned char *out = buf;
    if (isLeaf(n)) {
        if (!len) {
            s[len] = '0';
            s[len+1] = 0;
        } else
            s[len] = 0;

        strcpy((char *)out, (char *)s);
        code_table[n->c] = out;
        out += len + 1;
        return;
    }

    s[len] = '0';
    build_code(n->left,  s, len + 1, code_table);

    s[len] = '1';
    build_code(n->right, s, len + 1, code_table);
}


void init(FILE * in, FILE * out, CHNode ** queue, unsigned char ** code_table)
{
    int i, qend = 1;
    unsigned char c[64],
            input_buff[BLOCK_SIZE],
            char_freqs[UCHAR_MAX + 1],
            scale = 0;

    CHNode pool[256] = {{0}};

    unsigned int max = UCHAR_MAX,
            k = 0,
            freqs[256] = { 0 },
            len = 0,
            pos = 0;

    unsigned long long total = 0;

    while ((len = fread(input_buff, 1, BLOCK_SIZE, in))) {
        while (pos < len)
            freqs[(int) input_buff[pos++]]++;

        pos = 0;
        total += len;
    }

    rewind(in);

    if (total > UINT_MAX) {
        puts("input file too big");
        exit(EXIT_SUCCESS);
    } else
        len = (unsigned int) total;

    fwrite(&len, sizeof(int), 1, out);

    for (k = 0; k <= UCHAR_MAX; k++) {
        if (freqs[k] > max)
            max = freqs[k];
    }

    for (k = 0; k <= UCHAR_MAX; k++) {
        scale = (unsigned char)(freqs[k] / ((double)max / (double)UCHAR_MAX));
        if (scale == 0 && freqs[k] != 0)
            char_freqs[k] = 1;
        else
            char_freqs[k] = scale;

        if (char_freqs[k])
            printf("%u : %u\n", k, char_freqs[k]);
    }

    fwrite(char_freqs, 1, UCHAR_MAX + 1, out);

    for (i = 0; i < 256; i++)
        if (char_freqs[i]) {
            qinsert(new_node(char_freqs[i], (unsigned char) i, 0, 0, pool), queue, &qend);
            printf("%d\n", qend);
        }

    printf("%d\n", qend);

    while (qend > 2) {
        printf(">BEFORE>%d\n", qend);
        qinsert(new_node(0, 0, qremove(queue, &qend), qremove(queue, &qend), pool), queue, &qend);
        printf(">AFTER>%d\n", qend);
    }

    build_code(queue[1], c, 0, code_table);
}

void init_decode(FILE * in, CHNode ** queue, unsigned int * len, unsigned char ** code_table)
{
    int i, qend = 1;
    unsigned char freqs[UCHAR_MAX +1];
    unsigned char c[64];

    CHNode pool[256] = {{0}};


    fread(len, sizeof(int), 1, in);

    printf("\n%u\n", *len);

    fread(freqs, 1, UCHAR_MAX + 1, in);

    for (i = 0; i < 256; i++)
        if (freqs[i])
            qinsert(new_node(freqs[i], (unsigned char) i, 0, 0, pool), queue, &qend);

    while (qend > 2)
        qinsert(new_node(0, 0, qremove(queue, &qend), qremove(queue, &qend), pool), queue, &qend);

    build_code(queue[1], c, 0, code_table);
}


void encode(FILE *in, FILE *out, unsigned char ** huffman_code)
{
    unsigned char input_buff[8];
    unsigned int len = 0;

    while ((len = fread(input_buff, 1, 8, in)) == 8)
        make_everything_nice(input_buff, len, huffman_code, out, false);

    make_everything_nice(input_buff, len, huffman_code, out, true);
    /*while (*s) {
        strcpy((char *)out, (char *)huffman_code[*s]);
        out += strlen((char *)huffman_code[*s++]);
    }*/
}


void decode(FILE * in, FILE * out, CHNode *t, unsigned int len)
{
    CHNode *n = t;
    unsigned char buff;

    //printf("decoded ");

    unsigned int buff_pos = 0;

    while (len > 0) {
        buff = (unsigned char)fgetc(in);

        for (buff_pos = 0 ; buff_pos < 8 && len; buff_pos++) {

            putchar('0' + ((buff >> (7 - buff_pos)) & 0x01));

            printf(" char_node n->c : %u n->right : %u n->left : %u\n", n->c, n->right != NULL, n->left != NULL);

            if ((buff >> (7 - buff_pos)) & 0x01) {
                n = n->right ? n->right : n;
            } else {
                n = n->left ? n->left : n;
            }
            printf(" char_node n->c : %u n->right : %u n->left : %u\n", n->c, n->right ? 1 : 0, n->left ? 1 : 0);

            if (isLeaf(n)) {
                printf("leaf\n");

                putchar(n->c);

                //printf("\ndecoded ");

                fputc(n->c, out), n = t;

                len--;
            }
        }
    }
/*
    while (*s) {
        if (*s++ == '0') n = n->left;
        else n = n->right;

        if (n->c)
            putc((char)n->c, out), n = t;
    }

    //putc('\n', out);
    if (t != n) printf("garbage input\n");
    */
}

int __main(void)
{
    FILE * it = fopen("in.txt", "wb");
    unsigned char u = 0;
    for (u = 0; u < 255; u++)
        fwrite(&u, 1, 1, it);
    fwrite(&u, 1, 1, it);
    fclose(it);
    FILE * in = fopen("in.txt", "rb"), * out = fopen("out.txt", "wb");

    if (in == NULL) INPUT_FILE_ERROR;
    if (out == NULL) OUTPUT_FILE_ERROR;

    int i = 0;
    //unsigned char buf[4096];

    unsigned char * code_table[256] = { 0 };

    CHNode * qqq[256], **queue = qqq - 1;

    init(in, out, queue, code_table);

    //for (i = 0; i < strlen((char *)str); i++)
    //    printf("'%c': %u\n", str[i], str[i]);
/*
    for (i = 0; i < 256; i++)
        if (code_table[i])
            printf("'%c': %u : %s\n", i, i, code_table[i]);
*/
    printf("encoded: ");
    encode(in, out, code_table);


    printf("\ndecoded: ");
    //decode(buf, queue[1], out);

    fclose(out);
    fclose(in);

    in = fopen("out.txt", "rb");
    out = fopen("new.txt", "wb");

    unsigned char * _code_table[256] = { 0 };

    unsigned int len;

    CHNode * _qqq[256], **_queue = _qqq - 1;

    init_decode(in, _queue, &len, _code_table);

    for (i = 0; i < 256; i++)
        if (_code_table[i])
            printf("'%c': %u : %s\n", i, i, _code_table[i]);

    decode(in, out, _queue[1], len);


    return 0;
}
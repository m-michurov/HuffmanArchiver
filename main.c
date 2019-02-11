/*---------------------------------------------------------------*
 * https://www.geeksforgeeks.org/priority-queue-using-linked-list*
 *---------------------------------------------------------------*/


#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <limits.h>

#include "definitions.h"
#include "pqueue.h"

bool isLeaf(CHNode *node) {
    return node->left || node->right ? false : true;
}

void make_everything_nice(
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
            code_pos = 0;

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
}

void build_codes(CHNode *node, unsigned int pos, unsigned char **code_table, unsigned char * buff) {
    //static unsigned char buff[64];
    static unsigned char *c;

    if (isLeaf(node)) {
        buff[pos] = 0;

        c = malloc(pos);
        strcpy((char *)c, (char *)buff);
        code_table[node->c] = c;
        return;
    }

    buff[pos] = '0';
    build_codes(node->left, pos + 1, code_table, buff);

    buff[pos] = '1';
    build_codes(node->right, pos + 1, code_table, buff);

}

void init(FILE * in, FILE * out, QNode** pq, unsigned char ** code_table)
{
    unsigned char
            input_buff[BLOCK_SIZE],
            char_freqs[UCHAR_MAX + 1],
            scale = 0;

    unsigned int max = UCHAR_MAX,
            k = 0,
            freqs[256] = { 0 },
            len = 0,
            pos = 0;

    unsigned long long total = 0;

    for ( ;(len = fread(input_buff, 1, BLOCK_SIZE, in)); pos = 0, total += len)
        while (pos < len)
            freqs[(int) input_buff[pos++]]++;

    rewind(in);

    if (total > UINT_MAX)
        SIZE_ERROR;

    len = (unsigned int) total;

    fwrite(&len, sizeof(int), 1, out);

    for (k = 0; k <= UCHAR_MAX; k++)
        if (freqs[k] > max)
            max = freqs[k];


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

    for (int i = 0; i < 256; i++)
        if (char_freqs[i])
            push(pq, newCHNode(char_freqs[i], (unsigned char)i, 0, 0));


    CHNode * a, * b;

    while (queueLength(pq) > 1) {
        a = get(pq)/*, printf("node a freq : %d\n", a->freq)*/;
        b = get(pq)/*, printf("node b freq : %d\n", b->freq)*/;
        push(pq, newCHNode(a->freq + b->freq, 0, a, b))/*, printf(">>%d\n", queueLength(pq))*/;
    }

    build_codes((*pq)->char_data, 0, code_table, (unsigned char *)malloc(64));

    for (int y = 0; y < 256; y++)
        if (code_table[y])
            printf("%c : %s\n", y, code_table[y]);

}


void init_decode(FILE * in, QNode ** pq, unsigned int * len, unsigned char ** code_table)
{
    unsigned char char_freqs[UCHAR_MAX +1];


    fread(len, sizeof(int), 1, in);

    printf("\n%u\n", *len);

    fread(char_freqs, 1, UCHAR_MAX + 1, in);

    for (int i = 0; i < 256; i++)
        if (char_freqs[i])
            push(pq, newCHNode(char_freqs[i], (unsigned char)i, 0, 0));


    CHNode * a, * b;

    while (queueLength(pq) > 1) {
        a = get(pq)/*, printf("node a freq : %d\n", a->freq)*/;
        b = get(pq)/*, printf("node b freq : %d\n", b->freq)*/;
        push(pq, newCHNode(a->freq + b->freq, 0, a, b))/*, printf(">>%d\n", queueLength(pq))*/;
    }

    build_codes((*pq)->char_data, 0, code_table, (unsigned char *)malloc(64));

    for (int y = 0; y < 256; y++)
        if (code_table[y])
            printf("%c : %s\n", y, code_table[y]);
}


void encode(FILE *in, FILE *out, unsigned char ** code_table)
{
    unsigned char input_buff[8];
    unsigned int len = 0;

    while ((len = fread(input_buff, 1, 8, in)) == 8)
        make_everything_nice(input_buff, len, code_table, out, false);

    make_everything_nice(input_buff, len, code_table, out, true);
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

            //putchar('0' + ((buff >> (7 - buff_pos)) & 0x01));

            //printf(" char_node n->c : %u n->right : %u n->left : %u\n", n->c, n->right != NULL, n->left != NULL);

            if ((buff >> (7 - buff_pos)) & 0x01)
                n = n->right;
            else
                n = n->left;

            //printf(" char_node n->c : %u n->right : %u n->left : %u\n", n->c, n->right ? 1 : 0, n->left ? 1 : 0);

            if (isLeaf(n)) {
                //printf("leaf\n");

                //putchar(n->c);

                //printf("\n decoded ");

                fputc(n->c, out), n = t;

                len--;
            }
        }
    }
}


unsigned int treeHeight(CHNode * root) {
    if (root == NULL)
        return 0;

    unsigned int l = treeHeight(root->left);
    unsigned int r = treeHeight(root->right);

    return (l > r ? l : r) + 1;
}


// Driver code
int main()
{

    char t = 0;

    FILE *ch = fopen("in.txt", "rb");

    if (ch == NULL) INPUT_FILE_ERROR;


    t = (char)getc(ch);
    getc(ch);

    rewind(ch);

    fclose(ch);


    if (t == 'c') {

        FILE *in = fopen("in.txt", "rb"),
                *out = fopen("out.txt", "wb");
        if (in == NULL) INPUT_FILE_ERROR;
        if (out == NULL) OUTPUT_FILE_ERROR;

        unsigned char *code_table[256] = {0};
        QNode *pq = 0;

        //fseek(in, 2, 0);

        init(in, out, &pq, code_table);

        //fseek(in, 2, 0);

        printf("%u %u %u\n", queueLength(&pq), peek(&pq)->freq, treeHeight(pq->char_data));

        encode(in, out, code_table);

        fclose(in);
        fclose(out);
    }

    if (t == 'd') {

        FILE *_in = fopen("in.txt", "rb"),
                *_out = fopen("out.txt", "wb");

        if (_in == NULL) INPUT_FILE_ERROR;
        if (_out == NULL) OUTPUT_FILE_ERROR;

        fseek(_in, 2, 0);

        QNode *_pq = 0;

        unsigned char *_code_table[256] = {0};

        unsigned int len;

        init_decode(_in,
                    &_pq, &len, _code_table);

        decode(_in, _out, _pq
                ->char_data, len);

        fclose(_in);
        fclose(_out);
    }
    return 0;
}
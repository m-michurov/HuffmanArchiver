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
        Code ** codes,
        FILE * out,
        bool last)
{
    static unsigned char buff = 0,
            a = 0;

    static long long int code;

    static unsigned int code_len = 0,
            buff_pos = 0,
            code_pos = 0;

    while (string_len) {
        a = code_pos < code_len ? a : *(string++);
        code_pos = code_pos < code_len ? code_pos : 0;

        code = codes[a]->code;
        code_len = codes[a]->len;

        for ( ; code_pos < code_len && buff_pos < 8; code_pos++, buff_pos++) {
            buff += (code[code_pos] - '0') << (7 - buff_pos);
        }

        if (buff_pos > 7)
            fputc(buff, out), buff = 0, buff_pos = 0;

        if (code_pos == code_len)
            string_len--;

    }

    if (last) {
        fputc(buff, out);
    }
}

CHNode * read_tree(FILE * in, unsigned char * leaves, unsigned int max) {
    static unsigned char buff;
    static unsigned int buff_pos = 8,
                        leaves_count = 0;

    if (leaves_count == max)
        return NULL;

    int bit;

    CHNode * node;

    if (buff_pos > 7) {
        buff = (unsigned char) fgetc(in), buff_pos = 0;
        for (int y = 7; y >= 0; y--)
            printf("%d", (buff >> y) & 0x01);

        puts("");
    }


    bit = (buff >> (7 - buff_pos++)) & 0x01;

    if (bit == 0) {
        //printf("%d", bit);
        node = newCHNode(0, leaves[leaves_count++], 0, 0);
        //putchar(leaves[leaves_count - 1]);
    } else {
        node = newCHNode(0, 0, 0, 0);
        node->left = read_tree(in, leaves, max);
        node->right = read_tree(in, leaves, max);
    }

    return node;
}

void write_tree(CHNode *node, FILE *out, bool first) {

    static unsigned char buff = 0;
    static unsigned int buff_pos = 0;

    if (isLeaf(node)) {
        buff_pos++;

        //printf("0");

        if (buff_pos > 7) {
            fputc(buff, out);

            for (int y = 7; y >= 0; y--)
                printf("%d", (buff >> y) & 0x01);

            puts("");

            buff_pos = 0, buff = 0;
        }
    } else {
        buff += (1 << (7 - buff_pos++));

        //printf("1");

        if (buff_pos > 7) {
            fputc(buff, out);

            for (int y = 7; y >= 0; y--)
                printf("%d", (buff >> y) & 0x01);

            puts("");

            buff_pos = 0, buff = 0;
        }

        write_tree(node->left, out, false);
        write_tree(node->right, out, false);
    }

    if (first && buff_pos) {
        fputc(buff, out);
        for (int y = 7; y >= 0; y--)
            printf("%d", (buff >> y) & 0x01);

        puts("");
    }
}

void mk_leaves_array(CHNode *node, unsigned char * arr) {

    static unsigned int count = 0;

    if (isLeaf(node)) {
        arr[count] = node->c;
        count++;
        putchar(node->c);
        return;
    }

    mk_leaves_array(node->left, arr);
    mk_leaves_array(node->right, arr);

}

void write_header(CHNode *node, FILE *out, unsigned char char_count, unsigned int len) {
    printf("\n>>%u\n", char_count);
    unsigned char * leaves = (unsigned char *)malloc(char_count);

    mk_leaves_array(node, leaves);

    fwrite(&len, sizeof(int), 1, out);

    printf("\nwritten len %u\n", len);

    fwrite(&char_count, 1, 1, out);

    printf("\nwritten char count %u\n", char_count);

    fwrite(leaves, 1, char_count, out);

    for (int t = 0; t < char_count; t++)
        putchar(leaves[t]);

    puts("");

    write_tree(node, out, true);
}

Code * make_code(const char * str_code, int code_len) {
    Code * new_code = malloc(sizeof(Code));

    new_code->code = 0;
    new_code->len = (unsigned char)code_len;

    for (int y = 0; y < code_len ; y++)
        new_code->code += ((str_code[y] - '0') << ( code_len - y - 1));
    return new_code;
}

void build_codes(CHNode *node, unsigned int pos, unsigned char **code_table, unsigned char * buff, Code **table) {
    //static unsigned char buff[64];
    static unsigned char *c;

    if (isLeaf(node)) {
        if (pos == 0)
            buff[pos++] = '0';

        buff[pos] = 0;

        c = malloc(pos);
        strcpy((char *)c, (char *)buff);
        code_table[node->c] = c;

        table[node->c] = make_code((char *)buff, pos);

        return;
    }
/*
    if (isLeaf(node)) {
        buff[pos] = 0;

        c = malloc(pos);
        strcpy((char *)c, (char *)buff);
        code_table[node->c] = c;
        return;
    }

*/
    buff[pos] = '0';
    build_codes(node->left, pos + 1, code_table, buff, table);

    buff[pos] = '1';
    build_codes(node->right, pos + 1, code_table, buff, table);

}

void init(FILE * in, FILE * out, QNode** pq, unsigned char ** code_table, Code ** codes)
{
    unsigned char
            input_buff[BLOCK_SIZE],
            char_freqs[UCHAR_MAX + 1],
            scale = 0;

    unsigned int max = UCHAR_MAX,
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

        //if (char_freqs[k])
        //   printf("%c : %u : %u\n", k, k, char_freqs[k]);
    }

    //fwrite(char_freqs, 1, UCHAR_MAX + 1, out);

    for (int i = 0; i < 256; i++)
        if (char_freqs[i])
            push(pq, newCHNode(char_freqs[i], (unsigned char)i, 0, 0)), CHAR_COUNT++;


    CHNode * a, * b;

    while (queueLength(pq) > 1) {
        a = get(pq)/*, printf("node a freq : %d\n", a->freq)*/;
        b = get(pq)/*, printf("node b freq : %d\n", b->freq)*/;
        push(pq, newCHNode(a->freq + b->freq, 0, a, b))/*, printf(">>%d\n", queueLength(pq))*/;
    }

    write_header((*pq)->char_data, out, (unsigned char)CHAR_COUNT, len);

    build_codes((*pq)->char_data, 0, code_table, (unsigned char *)malloc(64), codes);

    for (int y = 0; y < 256; y++)
        if (code_table[y])
            printf("+ %c : %s \n", y, code_table[y]);

    for (int y = 0; y < 256; y++)
        if (codes[y]) {
            printf("%c : ", y);

            for (int u = 0; u < codes[y]->len; u++)
                printf("%d", (int)(codes[y]->code >> ( codes[y]->len - u - 1)) & 0x01);
            puts("");
        }

}


CHNode * init_decode(FILE * in, CHNode * tree, unsigned int * len, unsigned char ** code_table)
{
    fread(len, sizeof(int), 1, in);

    printf("\nread len %u\n", *len);

    Code *codes[256] = { 0 };

    unsigned char leaves_count;

    fread(&leaves_count, 1, 1, in);

    printf("\nread char count %u\n", leaves_count);

    unsigned char * leaves = (unsigned char *)malloc(leaves_count);

    fread(leaves, 1, leaves_count, in);

    for (int t = 0; t < leaves_count; t++)
        putchar(leaves[t]);

    puts("");

    tree = read_tree(in, leaves, leaves_count);

    //printf(" char_node n->c : %u n->right : %u n->left : %u\n", tree->c, tree->right != NULL, tree->left != NULL);

    build_codes(tree, 0, code_table, (unsigned char *)malloc(64), codes);

    //printf(" char_node n->c : %u n->right : %u n->left : %u\n", tree->c, tree->right != NULL, tree->left != NULL);
    /*
    unsigned char char_freqs[UCHAR_MAX +1];


    fread(len, sizeof(int), 1, in);

    printf("\n%u\n", *len);

    fread(char_freqs, 1, UCHAR_MAX + 1, in);

    for (int i = 0; i < 256; i++)
        if (char_freqs[i])
          push(pq, newCHNode(char_freqs[i], (unsigned char)i, 0, 0));


    CHNode * a, * b;

    while (queueLength(pq) > 1) {
        a = get(pq);
        b = get(pq);
        push(pq, newCHNode(a->freq + b->freq, 0, a, b));
    }

    build_codes((*pq)->char_data, 0, code_table, (unsigned char *)malloc(64));

    */
/*
    for (int y = 0; y < 256; y++)
        if (code_table[y])
            printf("+ %c : %s\n", y, code_table[y]);
*/
    for (int y = 0; y < 256; y++)
        if (codes[y]) {
            printf("%c : ", y);

            for (int u = 0; u < codes[y]->len; u++)
                printf("%d", (int)(codes[y]->code >> ( codes[y]->len - u - 1)) & 0x01);
            puts("");
        }

    return tree;

}


void encode(FILE *in, FILE *out, unsigned char ** code_table, Code ** codes)
{
    unsigned char input_buff[8];
    unsigned int len = 0;

    while ((len = fread(input_buff, 1, 8, in)) == 8)
        make_everything_nice(input_buff, len, code_table, codes, out, false);

    make_everything_nice(input_buff, len, code_table, codes, out, true);
}


void decode(FILE * in, FILE * out, CHNode *t, unsigned int len)
{
    CHNode *n = t;
    unsigned char buff;

    //printf("decoded ");

    unsigned int buff_pos = 0;

    if (isLeaf(t)) {
        while (len > 0) {

            fputc(n->c, out);

            len--;

        }
    } else {

        while (len > 0) {
            buff = (unsigned char) fgetc(in);

            for (buff_pos = 0; buff_pos < 8 && len; buff_pos++) {

                //putchar('0' + ((buff >> (7 - buff_pos)) & 0x01));

                printf(" char_node n->c : %u n->right : %u n->left : %u\n", n->c, n->right != NULL, n->left != NULL);

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
}


unsigned int treeHeight(CHNode * root) {
    if (root == NULL)
        return 0;

    unsigned int l = treeHeight(root->left);
    unsigned int r = treeHeight(root->right);

    return (l > r ? l : r) + 1;
}

int treeCount(CHNode * root) {
    static int nodes = 0;

    if (root == NULL)
        return 0;

    nodes++;

    treeCount(root->left);
    treeCount(root->right);

    return nodes;
}


// Driver code
int main() {
    FILE *in = fopen("in.txt", "rb"),
            *out = fopen("out.txt", "wb");
    if (in == NULL) INPUT_FILE_ERROR;
    if (out == NULL) OUTPUT_FILE_ERROR;

    unsigned char *code_table[256] = {0};
    QNode *pq = 0;

    Code *codes[256] = { 0 };

    //fseek(in, 2, 0);

    init(in, out, &pq, code_table, codes);

    //fseek(in, 2, 0);

    printf("\n%u %u %u %u\n", queueLength(&pq), peek(&pq)->freq, treeHeight(pq->char_data), treeCount(pq->char_data));

    //encode(in, out, code_table, codes);

    fclose(in);
    fclose(out);

    FILE *_in = fopen("out.txt", "rb"),
            *_out = fopen("new.txt", "wb");

    if (_in == NULL) INPUT_FILE_ERROR;
    if (_out == NULL) OUTPUT_FILE_ERROR;

    //fseek(_in, 2, 0);

    CHNode *tree = 0;

    unsigned char *_code_table[256] = {0};

    unsigned int len;

    tree = init_decode(_in, tree, &len, _code_table);

    //printf(" char_node n->c : %u n->right : %u n->left : %u\n", tree->c, tree->right != NULL, tree->left != NULL);

    //decode(_in, _out, tree, len);

    fclose(_in);
    fclose(_out);

    return 0;
}
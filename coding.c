#include "coding.h"

static inline int write_bit(
        FILE * out,
        int bit,
        bool last)
{
    static char buff = 0;
    static int pos = 0;

    buff += (bit & 0x01) << (7 - pos++);

    if (pos > 7 || last) {
        fputc(buff, out);
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
        pos = 0;
    }

    if (last && (pos != 0)) {
        buff = (char) fgetc(in);
        pos = 0;
        return 0;
    }

    return (buff >> (7 - pos++))  & 0x01;
}

static inline int write_byte(
        FILE * out,
        unsigned char byte)
{
    int pos = 0;

    for (int k = 7; k >= 0; k--)
        pos = write_bit(out, (byte >> k) & 0x01, false);

    return pos;
}

static inline char read_byte(
        FILE * in)
{
    unsigned char byte = 0;

    for (int k = 7; k >= 0; k--)
        byte += (read_bit(in, false) << k);

    return byte;
}

static void write_tree(
        FILE * out,
        CHNode * node,
        bool first) {
    static int rest;

    if (isLeaf(node)) {
        rest = write_bit(out, 0, false);

        rest = write_byte(out, node->c);
    } else {
        rest = write_bit(out, 1, false);

        write_tree(out, node->child[left], false);
        write_tree(out, node->child[right], false);
    }

    if (first && rest)
        write_bit(out, 0, true);
}

static CHNode * read_tree(
        FILE * in)
{
    static int bit = 0;

    static unsigned char byte = 0;

    CHNode * node;

    bit = read_bit(in, false);

    if (bit == 0) {
        byte = (unsigned char)read_byte(in);
        node = newCHNode(0, byte, 0, 0);
    } else {
        node = newCHNode(0, 0, 0, 0);
        node->child[left] = read_tree(in);
        node->child[right] = read_tree(in);
    }

    return node;
}

static Code * make_code_from_string(
        const char *str_code,
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
        CHNode * node,
        unsigned int pos,
        Code ** table)
{
    if (node == NULL)
        return;

    static char buff[BLOCK_SIZE / 4];

    if (isLeaf(node)) {
        buff[pos] = 0;

        table[node->c] = make_code_from_string((char *) buff, pos);

        return;
    }

    buff[pos] = '0';
    build_codes(node->child[left], pos + 1, table);

    buff[pos] = '1';
    build_codes(node->child[right], pos + 1, table);
}

static inline void encode_string(
        unsigned char *string,
        unsigned int string_len,
        Code ** codes,
        FILE * out,
        bool last)
{
    static long long int code;

    static int
            code_len = 0,
            pos = 0,
            rest = 0;

    while (string_len > 0) {
        code = codes[(int) *string]->code;
        code_len = codes[(int) *(string++)]->len;

        for (pos = code_len - 1 ; pos >= 0; pos--)
            rest = write_bit(out, (int) ((code >> pos) & 0x01), false);

        string_len--;
    }

    if (rest && last)
        write_bit(out, 0, true);;
}

unsigned char * count_freqs(
        FILE * in,
        unsigned int * len)
{
    unsigned char
            input_buff[BLOCK_SIZE],
          * char_freqs = (unsigned char *)malloc(BLOCK_SIZE),
            scale = 0;

    if (char_freqs == NULL) MEM_ERROR;

    unsigned int
            max = UCHAR_MAX,
            k = 0,
            freqs[BLOCK_SIZE] = { 0 },
            pos = 0;

    unsigned long long total = 0;

    for ( ;(*len = fread(input_buff, 1, BLOCK_SIZE, in)) > 0; pos = 0, total += *len)
        while (pos < *len)
            freqs[(int) input_buff[pos++]]++;


    if (total > UINT_MAX) SIZE_ERROR;

    *len = (unsigned int) total;

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

    return char_freqs;
}

static void init_encode(
        FILE *in,
        FILE *out,
        QNode **pq,
        Code **codes)
{
    unsigned char * char_freqs;
    unsigned int len = 0;

    CHNode * a, * b;

    char_freqs = count_freqs(in, &len);

    for (int i = 0; i < BLOCK_SIZE; i++)
        if (char_freqs[i])
            push(pq, newCHNode(char_freqs[i], (unsigned char)i, 0, 0));

    if (*pq == NULL)
        return;

    while (queueLength(pq) > 1) {
        a = get(pq);
        b = get(pq);
        push(pq, newCHNode(a->freq + b->freq, 0, a, b));
    }

    fwrite(&len, sizeof(int), 1, out);

    write_tree(out, (*pq)->char_data, true);

    build_codes((*pq)->char_data, 0, codes);

    freeTree((*pq)->char_data);

    free(*pq);
}

static CHNode * init_decode(
        FILE * in,
        CHNode * tree,
        unsigned int * len)
{
    if (!fread(len, sizeof(int), 1, in)) {
        *len = 0;
        return NULL;
    }

    tree = read_tree(in);

    read_bit(in, true);

    return tree;
}

void encode(
        FILE *in,
        FILE *out,
        bool skip)
{
    QNode *pq = 0;

    Code *code_table[BLOCK_SIZE] = { 0 };

    unsigned char input_buff[BLOCK_SIZE];
    unsigned int len = 0;

    init_encode(in, out, &pq, code_table);

    rewind(in);

    if (skip)
        fseek(in, 3, 0);

    while ((len = fread(input_buff, 1, BLOCK_SIZE, in)))
        encode_string(input_buff, len, code_table, out, false);

    encode_string(input_buff, len, code_table, out, true);

    for (int k = 0; k < BLOCK_SIZE; k++)
        if (code_table[k])
            free(code_table[k]);
}

void decode(
        FILE * in,
        FILE * out)
{
    unsigned int len = 0;
    int bit;

    CHNode *tree = 0,
           *n = 0;

    tree = init_decode(in, tree, &len);

    if (tree == NULL)
        return;

    n = tree;

    if (isLeaf(tree)) {
        while (len > 0)
            fputc(n->c, out), len--;
    }
    else {
        while (len > 0) {
            bit = read_bit(in, false);

            if (bit)
                n = n->child[right];
            else
                n = n->child[left];

            if (isLeaf(n))
                fputc(n->c, out), n = tree, len--;
        }
    }

    freeTree(tree);
}

#include "coding.h"

static inline void __write_bit(
        IO_BUFF * out,
        int bit)
{
    out->string[out->string_pos] += (bit & 0x01) << (7 - out->byte_pos++);
    //printf("%d|%d(%d) ", out->string_pos, out->byte_pos-1, bit);

    if (out->byte_pos > 7) {
        out->byte_pos = 0;

        //printf("--%d--", out->string[out->string_pos]);

        if (out->string_pos == BLOCK_SIZE - 1) {
            fwrite(out->string, 1, BLOCK_SIZE, out->file);
            out->string_pos = 0;
            memset(out->string, 0, BLOCK_SIZE);
            return;
        }
        out->string_pos++;
    }
}

static inline int __read_bit(
        IO_BUFF * in)
{
    if (in->byte_pos > 7) {
        in->string_pos++;
        in->byte_pos = 0;
    }

    if (in->string_pos == BLOCK_SIZE) {
        fread(in->string, 1, BLOCK_SIZE, in->file);
        in->string_pos = 0;
        in->byte_pos = 0;
    }

    return ((in->string[in->string_pos]) >> (7 - in->byte_pos++))  & 0x01;
}

static inline void __write_byte(
        IO_BUFF * out,
        unsigned char byte)
{
    for (int k = 7; k >= 0; k--)
        __write_bit(out, (byte >> k) & 0x01);
}
static inline char __read_byte(
        IO_BUFF * in)
{
    unsigned char byte = 0;

    for (int k = 7; k >= 0; k--)
        byte += (__read_bit(in) << k);

    return byte;
}

static inline void __write_end(
        IO_BUFF * out)
{
    fwrite(out->string, 1, out->string_pos + (size_t)(out->byte_pos ? 1 : 0), out->file);
}

static inline void __next(
        IO_BUFF * buff)
{
    buff->string_pos += buff->byte_pos ? 1 : 0;
    buff->byte_pos = 0;
}

static void __write_tree(
        IO_BUFF * out,
        CHNode * node)
{
    if (isLeaf(node)) {
        __write_bit(out, 0);

        __write_byte(out, node->c);
    } else {
        __write_bit(out, 1);

        __write_tree(out, node->child[left]);
        __write_tree(out, node->child[right]);
    }
}
static CHNode * __read_tree(
        IO_BUFF * in)
{
    int bit = 0;

    unsigned char byte = 0;

    CHNode * node;

    bit = __read_bit(in);

    if (bit == 0) {
        byte = (unsigned char)__read_byte(in);
        node = newCHNode(0, byte, 0, 0);
    } else {
        node = newCHNode(0, 0, 0, 0);
        node->child[left] = __read_tree(in);
        node->child[right] = __read_tree(in);
    }

    return node;
}

static void build_codes(
        CHNode * node,
        unsigned int pos,
        char * buff,
        Code ** table)
{
    if (node == NULL)
        return;

    if (isLeaf(node)) {
        buff[pos] = 0;

        table[node->c] = (Code *)malloc(sizeof(Code));

        if (table[node->c] == NULL) MEM_ERROR;

        strcpy(table[node->c]->code, buff);
        table[node->c]->len = (short)pos;

        return;
    }

    buff[pos] = '0';
    build_codes(node->child[left], pos + 1, buff, table);

    buff[pos] = '1';
    build_codes(node->child[right], pos + 1, buff, table);
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
        Code **codes,
        IO_BUFF * stream)
{
    unsigned char * char_freqs;
    char * buff = (char *)malloc(BLOCK_SIZE + 1);
    unsigned int len = 0;

    CHNode * a, * b;

    if (buff == NULL) MEM_ERROR;

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

    __write_tree(stream, (*pq)->char_data);
    __next(stream);

    build_codes((*pq)->char_data, 0, buff, codes);

    freeTree((*pq)->char_data);
    free(*pq);
}

static CHNode * init_decode(
        IO_BUFF * in,
        CHNode * tree,
        unsigned int * len)
{
    if (!fread(len, sizeof(int), 1, in->file)) {
        *len = 0;
        return NULL;
    }

    tree = __read_tree(in);

    __next(in);

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
    unsigned int len = 0,
    string_pos = 0;
    char * code;

    int code_len = 0,
            pos = 0;

    IO_BUFF * buff = (IO_BUFF *)malloc(sizeof(IO_BUFF));

    memset(buff->string, 0, BLOCK_SIZE);
    buff->file = out;
    buff->string_pos = 0;
    buff->byte_pos = 0;

    init_encode(in, out, &pq, code_table, buff);

    rewind(in);

    if (skip)
        fseek(in, 3, 0);

    while ((len = fread(input_buff, 1, BLOCK_SIZE, in))) {
        string_pos = 0;

        while (len > 0) {
            code = code_table[input_buff[string_pos]]->code;
            code_len = code_table[input_buff[string_pos++]]->len;

            for (pos = 0; pos < code_len; pos++)
                __write_bit(buff, code[pos] - '0');

            len--;
        }
    }

    __write_end(buff);

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

    IO_BUFF * buff = (IO_BUFF *)malloc(sizeof(IO_BUFF));

    memset(buff->string, 0, BLOCK_SIZE);
    buff->file = in;
    buff->string_pos = BLOCK_SIZE;
    buff->byte_pos = 0;

    tree = init_decode(buff, tree, &len);

    if (tree == NULL)
        return;

    n = tree;

    while (len > 0) {
        if (isLeaf(n)) {
            fputc(n->c, out);
            n = tree;
            len--;
        } else {
            bit = __read_bit(buff);

            if (bit)
                n = n->child[right];
            else
                n = n->child[left];
        }
    }

    freeTree(tree);
}

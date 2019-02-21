#include "coding.h"

static void __write_tree(
        IO_BUFF * out,
        TREE_NODE * node)
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
static TREE_NODE * __read_tree(
        IO_BUFF * in)
{
    int bit = 0;

    unsigned char byte = 0;

    TREE_NODE * node;

    bit = __read_bit(in);

    if (bit == 0) {
        byte = __read_byte(in);
        node = newCHNode(0, byte, 0, 0);
    } else {
        node = newCHNode(0, 0, 0, 0);
        node->child[left] = __read_tree(in);
        node->child[right] = __read_tree(in);
    }

    return node;
}

static void build_codes(
        TREE_NODE * node,
        size_t pos,
        unsigned char * buff,
        Code ** table)
{
    if (node == NULL)
        return;

    if (isLeaf(node)) {
        buff[pos] = 0;

        table[node->c] = (Code *)malloc(sizeof(Code));

        if (table[node->c] == NULL) MEM_ERROR;

        strcpy((char *)table[node->c]->code, (char *)buff);
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
    unsigned char input_buff[BLOCK_SIZE],
                * char_freqs = (unsigned char *)malloc(BLOCK_SIZE),
                  scale = 0;

    unsigned int max = UCHAR_MAX,
                 k = 0,
                 freqs[BLOCK_SIZE] = { 0 },
                 pos = 0;

    unsigned long long total = 0;

    if (char_freqs == NULL) MEM_ERROR;

    while ((*len = fread(input_buff, 1, BLOCK_SIZE, in))) {
        while (pos < *len)
            freqs[(int) input_buff[pos++]]++;

        pos = 0;
        total += *len;
    }

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

static TREE_NODE * build_tree(
        FILE * in,
        unsigned int * len)
{
    unsigned char * char_freqs;
    unsigned int input_file_len = 0;

    TREE_NODE * a,
              * b,
              * tree;

    QUEUE_NODE * pq = 0;
    QUEUE_NODE ** queue = &pq;

    char_freqs = count_freqs(in, &input_file_len);

    *len = input_file_len;

    for (int i = 0; i < BLOCK_SIZE; i++)
        if (char_freqs[i])
            push(queue, newCHNode(char_freqs[i], (unsigned char)i, 0, 0));

    if (*queue == NULL)
        return NULL;

    while (queueLength(queue) > 1) {
        a = get(queue);
        b = get(queue);
        push(queue, newCHNode(a->quantity + b->quantity, 0, a, b));
    }

    tree = (*queue)->char_data;

    free(*queue);

    return tree;
}

void encode(
        FILE * fin,
        FILE * fout,
        bool skip)
{
    unsigned char input_buff[BLOCK_SIZE];
    unsigned char * code;

    size_t buff_len = 0,
           buff_pos = 0,
           code_len = 0,
           pos = 0;

    unsigned int input_file_len = 0;

    Code * code_table[BLOCK_SIZE] = { 0 };

    IO_BUFF * out = __io_stream_init(fout, WRITE);

    TREE_NODE * tree = build_tree(fin, &input_file_len);

    if (tree == NULL)
        return;

    fwrite(&input_file_len, sizeof(int), 1, fout);

    build_codes(tree, 0, input_buff, code_table);

    __write_tree(out, tree);
    __next(out);

    freeTree(tree);

    rewind(fin);

    if (skip)
        fseek(fin, 3, 0);

    while ((buff_len = fread(input_buff, 1, BLOCK_SIZE, fin))) {
        buff_pos = 0;

        while (buff_len > 0) {
            code = code_table[input_buff[buff_pos]]->code;
            code_len = code_table[input_buff[buff_pos++]]->len;

            for (pos = 0; pos < code_len; pos++)
                __write_bit(out, code[pos] - '0');

            buff_len--;
        }
    }

    __write_end(out);

    free(out);

    for (int k = 0; k < BLOCK_SIZE; k++)
        if (code_table[k])
            free(code_table[k]);
}

void decode(
        FILE * fin,
        FILE * fout)
{
    unsigned int len = 0;
    int bit;

    TREE_NODE * tree = 0,
              * n = 0;

    IO_BUFF * out = __io_stream_init(fin, READ);

    fread(&len, sizeof(int), 1, out->file);

    if (!len)
        return;

    tree = __read_tree(out);
    __next(out);

    if (tree == NULL)
        return;

    n = tree;

    while (len > 0) {
        if (isLeaf(n)) {
            fputc(n->c, fout);
            n = tree;
            len--;
        } else {
            bit = __read_bit(out);

            if (bit)
                n = n->child[right];
            else
                n = n->child[left];
        }
    }

    freeTree(tree);
}

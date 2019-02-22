#include "coding.h"

static void write_tree(
        IO_BUFF * out,
        TREE_NODE * node)
{
    if (isLeaf(node)) {
        write_bit(out, 0);

        write_byte(out, node->c);
    } else {
        write_bit(out, 1);

        write_tree(out, node->child[left]);
        write_tree(out, node->child[right]);
    }
}

static TREE_NODE * read_tree(
        IO_BUFF * in)
{
    int bit = 0;

    unsigned char byte = 0;

    TREE_NODE * node;

    bit = read_bit(in);

    if (bit == 0) {
        byte = read_byte(in);

        node = newCHNode(0, byte, 0, 0);
    } else {
        node = newCHNode(0, 0, 0, 0);

        node->child[left] = read_tree(in);
        node->child[right] = read_tree(in);
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
                * char_freqs = (unsigned char *)malloc(MAX_BYTE_COUNT),
                  scaled = 0;

    unsigned int max = UCHAR_MAX,
                 k = 0,
                 freqs[MAX_BYTE_COUNT] = { 0 },
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

    for (k = 0; k < MAX_BYTE_COUNT; k++)
        if (freqs[k] > max)
            max = freqs[k];


    for (k = 0; k < MAX_BYTE_COUNT; k++) {
        scaled = (unsigned char)(freqs[k] / ((double)max / (double)UCHAR_MAX));

        if (scaled == 0 && freqs[k] != 0)
            char_freqs[k] = 1;
        else
            char_freqs[k] = scaled;
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

    for (int i = 0; i < MAX_BYTE_COUNT; i++)
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
    free(char_freqs);

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

    Code * code_table[MAX_BYTE_COUNT] = { 0 };

    IO_BUFF * out = io_stream_init(fout, WRITE);

    TREE_NODE * tree = build_tree(fin, &input_file_len);

    if (tree == NULL)
        return;

    fwrite(&input_file_len, sizeof(int), 1, fout);

    build_codes(tree, 0, input_buff, code_table);

    write_tree(out, tree);
    next(out);

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
                write_bit(out, code[pos] - '0');

            buff_len--;
        }
    }

    write_end(out);

    free(out);

    for (int k = 0; k < MAX_BYTE_COUNT; k++)
        if (code_table[k])
            free(code_table[k]);
}

void decode(
        FILE * fin,
        FILE * fout)
{
    unsigned char output_buff[BLOCK_SIZE];
    unsigned int len = 0,
                 output_pos = 0;
    int bit;

    TREE_NODE * tree = 0,
              * n = 0;

    IO_BUFF * out = io_stream_init(fin, READ);

    fread(&len, sizeof(int), 1, out->file);

    if (!len)
        return;

    tree = read_tree(out);
    next(out);

    if (tree == NULL)
        return;

    n = tree;

    while (len > 0) {
        if (isLeaf(n)) {
            output_buff[output_pos++] = n->c;

            if (output_pos == BLOCK_SIZE) {
                fwrite(output_buff, 1, BLOCK_SIZE, fout);
                output_pos = 0;
            }

            n = tree;
            len--;
        } else {
            bit = read_bit(out);

            if (bit)
                n = n->child[right];
            else
                n = n->child[left];
        }
    }

    fwrite(output_buff, 1, output_pos, fout);
    freeTree(tree);
}

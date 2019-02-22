#include "coding.h"

// Recursively traverse the tree and write tree structure to file
static void write_tree(
        IO_BUFF * out,
        TREE_NODE * node)
{
    if (IsLeaf(node)) {
        BitWrite(out, 0);

        ByteWrite(out, node->c);
    } else {
        BitWrite(out, 1);

        write_tree(out, node->child[left]);
        write_tree(out, node->child[right]);
    }
}

// Recursively recreate the tree structure from file
static TREE_NODE * read_tree(
        IO_BUFF * in)
{
    int bit = 0;

    unsigned char byte = 0;

    TREE_NODE * node;

    bit = BitRead(in);

    if (bit == 0) {
        byte = ByteRead(in);

        node = NewTreeNode(0, byte, 0, 0);
    } else {
        node = NewTreeNode(0, 0, 0, 0);

        node->child[left] = read_tree(in);
        node->child[right] = read_tree(in);
    }

    return node;
}

// Recursively traverse the tree and build character codes
static void build_codes(
        TREE_NODE * node,
        Code ** table,
        size_t pos,
        unsigned char * buff)
{
    if (node == NULL)
        return;

    if (IsLeaf(node)) {
        buff[pos] = 0;

        table[node->c] = (Code *)malloc(sizeof(Code));

        if (table[node->c] == NULL) MEM_ERROR;

        strcpy((char *)table[node->c]->code, (char *)buff);
        table[node->c]->len = (short)pos;

        return;
    }

    buff[pos] = '0';
    build_codes(node->child[left], table, pos + 1, buff);

    buff[pos] = '1';
    build_codes(node->child[right], table, pos + 1, buff);
}

// Count character quantities and scale each to fit 0 - 255 range
// to avoid integer overflow when building the tree
static unsigned char * count_freqs(
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

// Build the tree using priority queue (algorithm taken directly
// from https://en.wikipedia.org/wiki/Huffman_coding)
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
            QueuePush(queue, NewTreeNode(char_freqs[i], (unsigned char) i, 0, 0));

    if (*queue == NULL)
        return NULL;

    while (QueueLength(queue) > 1) {
        a = QueueGet(queue);
        b = QueueGet(queue);
        QueuePush(queue, NewTreeNode(a->quantity + b->quantity, 0, a, b));
    }

    tree = (*queue)->char_data;

    free(*queue);
    free(char_freqs);

    return tree;
}
void EncodeFile(
        char *in_file,
        char *out_file,
        bool skip_three)
{
    FILE * fin = fopen(in_file, "rb"),
         * fout = fopen(out_file, "wb");

    if (fin == NULL) INPUT_FILE_ERROR;
    if (fout == NULL) OUTPUT_FILE_ERROR;

    unsigned char input_buff[BLOCK_SIZE];
    unsigned char * code;

    size_t buff_len = 0,
           buff_pos = 0,
           code_len = 0,
           pos = 0;

    unsigned int input_file_len = 0,
                 completed_len = 0;

    Code * code_table[MAX_BYTE_COUNT] = { 0 };

    if (skip_three)
        fseek(fin, 3, 0);

    IO_BUFF * out = InitBinaryIO(fout, WRITE);

    TREE_NODE * tree = build_tree(fin, &input_file_len);

    if (tree == NULL)
        return;

    fwrite(&input_file_len, sizeof(int), 1, fout);

    build_codes(tree, code_table, 0, input_buff);

    write_tree(out, tree);
    NextByte(out);

    DestroyTree(tree);

    if (skip_three)
        fseek(fin, 3, 0);
    else
        fseek(fin, 0, 0);

    while ((buff_len = fread(input_buff, 1, BLOCK_SIZE, fin))) {
        completed_len += buff_len;
        buff_pos = 0;

        while (buff_len > 0) {
            code = code_table[input_buff[buff_pos]]->code;
            code_len = code_table[input_buff[buff_pos++]]->len;

            for (pos = 0; pos < code_len; pos++)
                BitWrite(out, code[pos] - '0');

            buff_len--;
        }

        if (!skip_three)
            printf("\r%.2lf %%", (double)completed_len / input_file_len * 100);
    }

    EndWrite(out);

    free(out);

    fclose(fin);
    fclose(fout);

    for (int k = 0; k < MAX_BYTE_COUNT; k++)
        if (code_table[k])
            free(code_table[k]);
}

void DecodeFile(
        char *in_file,
        char *out_file,
        bool skip_three)
{
    FILE * fin = fopen(in_file, "rb"),
         * fout = fopen(out_file, "wb");

    if (fin == NULL) INPUT_FILE_ERROR;
    if (fout == NULL) OUTPUT_FILE_ERROR;

    unsigned char output_buff[BLOCK_SIZE];
    unsigned int len = 0,
                 output_pos = 0,
                 stored_len = 0,
                 completed_len = 0;
    int bit;

    TREE_NODE * tree = 0,
              * n = 0;

    if (skip_three)
        fseek(fin, 3, 0);

    IO_BUFF * out = InitBinaryIO(fin, READ);

    fread(&len, sizeof(int), 1, out->file);

    if (!len)
        return;

    stored_len = len;

    tree = read_tree(out);
    NextByte(out);

    if (tree == NULL)
        return;

    n = tree;

    while (len > 0) {
        if (IsLeaf(n)) {
            output_buff[output_pos++] = n->c;

            if (output_pos == BLOCK_SIZE) {
                fwrite(output_buff, 1, BLOCK_SIZE, fout);

                completed_len += output_pos;

                if (!skip_three)
                    printf("\r%.2lf %%", (double)completed_len / stored_len * 100);

                output_pos = 0;
            }

            n = tree;
            len--;
        } else {
            bit = BitRead(out);

            if (bit)
                n = n->child[right];
            else
                n = n->child[left];
        }
    }

    DestroyTree(tree);

    fwrite(output_buff, 1, output_pos, fout);

    if (!skip_three)
        printf("\r%.2lf %%", 100.00);

    fclose(fin);
    fclose(fout);
}

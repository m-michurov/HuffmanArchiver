#include "coding.h"

// Recursively traverse the tree and write tree structure to file and build codes
static void traverse(
        IO_BUFF * out,
        TREE_NODE * node,
        Code ** table,
        size_t pos,
        unsigned char * buff)
{
    if (IsLeaf(node)) {
        BitWrite(out, 0);
        ByteWrite(out, node->c);

        buff[pos] = 0;

        table[node->c] = (Code *) malloc(sizeof(Code));
        if (table[node->c] == NULL) MEM_ERROR;

        strcpy((char *) table[node->c]->code, (char *) buff);
        table[node->c]->len = (short) pos;

        return;
    }

    BitWrite(out, 1);

    buff[pos] = '0';
    traverse(out, node->child[left], table, pos + 1, buff);

    buff[pos] = '1';
    traverse(out, node->child[right], table, pos + 1, buff);
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

        node = NewTreeNode(byte, NULL, NULL);
    } else {
        node = NewTreeNode('\0', NULL, NULL);

        node->child[left] = read_tree(in);
        node->child[right] = read_tree(in);
    }

    return node;
}

// Build the tree using priority queue (algorithm taken directly
// from https://en.wikipedia.org/wiki/Huffman_coding)
static TREE_NODE * build_tree(
        FILE * in,
        unsigned int * len)
{
    unsigned char input_buff[BLOCK_SIZE];

    unsigned int freqs[MAX_BYTE_COUNT],
                 read = 0,
                 new_priority = 0,
                 pos = 0;

    unsigned long long total = 0;

    TREE_NODE * a,
              * b,
              * tree;

    QUEUE_NODE * pq = 0,
               ** queue = &pq;

    while ((read = fread(input_buff, 1, BLOCK_SIZE, in))) {
        while (pos < read)
            freqs[(int) input_buff[pos++]]++;

        pos = 0;
        total += read;
    }

    if (total > UINT_MAX) SIZE_ERROR;

    *len = (unsigned int)total;

    for (int i = 0; i < MAX_BYTE_COUNT; i++)
        if (freqs[i])
            QueuePush(queue, NewTreeNode((unsigned char) i, 0, 0), freqs[i]);

    if (*queue == NULL)
        return NULL;

    while (QueueLength(queue) > 1) {
        new_priority = (*queue)->priority;
        a = QueueGet(queue);
        new_priority += (*queue)->priority;
        b = QueueGet(queue);
        QueuePush(queue, NewTreeNode(0, a, b), new_priority);
    }

    tree = (*queue)->char_data;

    free(*queue);

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

    fwrite(&input_file_len, sizeof(int), 1, fout);

    if (tree)
        traverse(out, tree, code_table, 0, input_buff);

    DestroyTree(tree);

    NextByte(out);

    if (skip_three)
        fseek(fin, 3, 0);
    else {
        fseek(fin, 0, 0);
        printf("Encoding %s\n", in_file);
    }

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

    free(out->string);
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

    IO_BUFF * in = InitBinaryIO(fin, READ);

    if (skip_three)
        fseek(fin, 3, 0);
    else
        printf("Decoding %s\n", in_file);

    fread(&len, sizeof(int), 1, in->file);

    if (len) {
        stored_len = len;
        tree = read_tree(in);
    }

    NextByte(in);

    n = tree;

    while (len > 0) {
        if (IsLeaf(n)) {
            output_buff[output_pos++] = n->c;

            if (output_pos == BLOCK_SIZE) {
                fwrite(output_buff, 1, BLOCK_SIZE, fout);

                completed_len += output_pos;
                output_pos = 0;

                if (!skip_three)
                    printf("\r%.2lf %%", (double)completed_len / stored_len * 100);
            }

            n = tree;
            len--;
        } else {
            bit = BitRead(in);

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

    free(in->string);
    free(in);

    fclose(fin);
    fclose(fout);
}
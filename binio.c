#include "binio.h"

inline void BitWrite(
        IO_BUFF * out,
        int bit)
{
    out->string[out->string_pos] += (bit & 0x01) << (7 - out->byte_pos++);

    if (out->byte_pos > 7) {
        out->byte_pos = 0;

        if (out->string_pos == BLOCK_SIZE - 1) {
            fwrite(out->string, 1, BLOCK_SIZE, out->file);
            out->string_pos = 0;

            memset(out->string, 0, BLOCK_SIZE);
            return;
        }
        out->string_pos++;
    }
}

inline int BitRead(
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

inline void ByteWrite(
        IO_BUFF *out,
        unsigned char byte)
{
    for (int k = 7; k >= 0; k--)
        BitWrite(out, (byte >> k) & 0x01);
}

inline unsigned char ByteRead(
        IO_BUFF * in)
{
    unsigned char byte = 0;

    for (int k = 7; k >= 0; k--)
        byte += (BitRead(in) << k);

    return byte;
}

inline void EndWrite(
        IO_BUFF * out)
{
    fwrite(out->string, 1, out->string_pos + (size_t)(out->byte_pos ? 1 : 0), out->file);
}

inline void NextByte(
        IO_BUFF *buff)
{
    buff->string_pos += buff->byte_pos ? 1 : 0;
    buff->byte_pos = 0;
}

IO_BUFF * InitBinaryIO(
        FILE * file,
        bool mode)
{
    IO_BUFF * new_buff = (IO_BUFF *)malloc(sizeof(IO_BUFF));

    if (new_buff == NULL) MEM_ERROR;

    new_buff->string = (unsigned char *)malloc(BLOCK_SIZE);

    memset(new_buff->string, 0, BLOCK_SIZE);

    new_buff->file = file;
    new_buff->string_pos = mode;
    new_buff->byte_pos = 0;

    return new_buff;
}

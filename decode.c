#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "decode.h"
#include "types.h"
#include "common.h"

#define BMP_HEADER_SIZE 54
#define BYTE_BITS 8

static Status read_lsb_bytes(DecodeInfo *decInfo, char *buffer, int count)
{
    char image_buffer[8];

    for (int i = 0; i < count; i++)
    {
        if (fread(image_buffer, 1, 8, decInfo->fptr_src_image) != 8)
            return e_failure;

        if (decode_byte_to_lsb(&buffer[i], image_buffer) != e_success)
            return e_failure;
    }

    return e_success;
}

Status decode_byte_to_lsb(char *data, const char *image_buffer)
{
    unsigned char value = 0;

    if (data == NULL || image_buffer == NULL)
        return e_failure;

    for (int i = 0; i < 8; i++)
        value |= (unsigned char)(image_buffer[i] & 1) << i;

    *data = (char)value;
    return e_success;
}

static unsigned int decode_32_bits(const char *buffer)
{
    unsigned int value = 0;

    for (int i = 0; i < 32; i++)
        value |= (unsigned int)(buffer[i] & 1) << i;

    return value;
}

uint get_decode_image_size_for_bmp(FILE *fptr_image)
{
    unsigned char header[4];

    if (fseek(fptr_image, 18, SEEK_SET) != 0 ||fread(header, 1, 4, fptr_image) != 4)
    {
        return 0;
    }

    return (uint)header[0] | ((uint)header[1] << 8) | ((uint)header[2] << 16) |((uint)header[3] << 24);
}

uint get_decode_file_size(FILE *fptr)
{
    long current_position;
    long file_size;
    current_position = ftell(fptr);
    if (fseek(fptr, 0, SEEK_END) != 0)
    {
        return 0;
    }

    file_size = ftell(fptr);
    fseek(fptr, current_position, SEEK_SET);
    return file_size > 0 ? (uint)file_size : 0;
}

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (argv == NULL || decInfo == NULL)
    {
        return e_failure;
    }
    if (strcmp(argv[1], "-d") != 0)
    {
        return e_failure;
    }
    if (argv[2] == NULL || argv[3] == NULL)
    {
        return e_failure;
    }
    if (strstr(argv[2], ".bmp") == NULL)
    {
        return e_failure;
    }
    decInfo->src_image_fname = argv[2];
    decInfo->secret_fname = argv[3];
    return e_success;
}

Status open_decode_files(DecodeInfo *decInfo)
{
    decInfo->fptr_src_image = fopen(decInfo->src_image_fname, "rb");

    if (decInfo->fptr_src_image == NULL)
    {
        perror("ERROR: Unable to open stego image\n");
        return e_failure;
    }

    return e_success;
}

Status check_decode_capacity(DecodeInfo *decInfo)
{
    long file_size;
    file_size = get_decode_file_size(decInfo->fptr_src_image);
    if (file_size <= BMP_HEADER_SIZE)
    {
        return e_failure;
    }
    decInfo->image_capacity = (uint)(file_size - BMP_HEADER_SIZE);
    return e_success;
}

Status copy_decode_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    unsigned char buffer[BMP_HEADER_SIZE];
    rewind(fptr_src_image);
    if (fread(buffer, 1, BMP_HEADER_SIZE, fptr_src_image) != BMP_HEADER_SIZE)
    {
        return e_failure;
    }
    if (fwrite(buffer, 1, BMP_HEADER_SIZE, fptr_dest_image) != BMP_HEADER_SIZE)
    {
        return e_failure;
    }

    return e_success;
}

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    char *decoded_magic;
    size_t magic_length;
    Status status;

    if (magic_string == NULL)
    {
        return e_failure;
    }

    magic_length = strlen(magic_string);
    decoded_magic = calloc(magic_length + 1, sizeof(char));

    if (decoded_magic == NULL)
    {
        return e_failure;
    }

    status = read_lsb_bytes(decInfo, decoded_magic, (int)magic_length);

    if (status == e_success &&
        strcmp(decoded_magic, magic_string) != 0)
    {
        status = e_failure;
    }

    free(decoded_magic);
    return status;
}

Status decode_secret_file_extn_size(int size, DecodeInfo *decInfo)
{
    char image_buffer[32];

    if (size <= 0 || size >= (int)sizeof(decInfo->extn_secret_file))
    {
        return e_failure;
    }

    if (fread(image_buffer, 1, 32, decInfo->fptr_src_image) != 32)
    {
        return e_failure;
    }

    if (decode_size_to_lsb(size, image_buffer) != e_success)
    {
        return e_failure;
    }

    return e_success;
}

Status decode_secret_file_extn(const char *file_extn, DecodeInfo *decInfo)
{
    char image_buffer[8];
    int extn_size;

    if (file_extn == NULL || decInfo == NULL)
        return e_failure;

    extn_size = (int)strlen(file_extn);

    if (extn_size <= 0 ||extn_size >= (int)sizeof(decInfo->extn_secret_file))
        return e_failure;

    for (int i = 0; i < extn_size; i++)
    {
        if (fread(image_buffer, 1, 8,decInfo->fptr_src_image) != 8)
            return e_failure;

        if (decode_byte_to_lsb(&decInfo->extn_secret_file[i], image_buffer) != e_success)
            return e_failure;
    }

    decInfo->extn_secret_file[extn_size] = '\0';
    return e_success;
}

Status decode_secret_file_size(long file_size, DecodeInfo *decInfo)
{
    char image_buffer[32];
    unsigned int decoded_size;
    long remaining_bytes;

    if (decInfo == NULL || decInfo->fptr_src_image == NULL)
        return e_failure;

    if (fread(image_buffer, 1, 32,
              decInfo->fptr_src_image) != 32)
        return e_failure;

    decoded_size = decode_32_bits(image_buffer);

    remaining_bytes = file_size - ftell(decInfo->fptr_src_image);

    /*
     * Every secret byte uses 8 image bytes.
     */
    if ((long)decoded_size > remaining_bytes / 8)
        return e_failure;

    decInfo->size_secret_file = (long)decoded_size;
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char image_buffer[8];
    char data;

    decInfo->fptr_secret = fopen(decInfo->secret_fname, "wb");

    if (decInfo->fptr_secret == NULL)
        return e_failure;

    for (long i = 0; i < decInfo->size_secret_file; i++)
    {
        if (fread(image_buffer, 1, 8,
                  decInfo->fptr_src_image) != 8)
        {
            fclose(decInfo->fptr_secret);
            return e_failure;
        }

        if (decode_byte_to_lsb(&data, image_buffer) != e_success)
        {
            fclose(decInfo->fptr_secret);
            return e_failure;
        }

        if (fwrite(&data, 1, 1, decInfo->fptr_secret) != 1)
        {
            fclose(decInfo->fptr_secret);
            return e_failure;
        }
    }

    fclose(decInfo->fptr_secret);
    decInfo->fptr_secret = NULL;

    return e_success;
}

Status copy_remaining_decode_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    unsigned char buffer[1024];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fptr_src)) > 0)
    {
        if (fwrite(buffer, 1, bytes_read, fptr_dest) != bytes_read)
        {
            return e_failure;
        }
    }

    return ferror(fptr_src) ? e_failure : e_success;
}

Status decode_size_to_lsb(int size, char *imageBuffer)
{
    int decoded_size = 0;

    for (int i = 0; i < 32; i++)
    {
        decoded_size |= (imageBuffer[i] & 1) << i;
    }

    return decoded_size == size ? e_success : e_failure;
}

Status do_decoding(DecodeInfo *decInfo)
{
    char image_buffer[8];
    char extn_size_buffer[32];
    unsigned int extn_size;
    long image_size;

    if (decInfo == NULL)
        return e_failure;

    if (open_decode_files(decInfo) != e_success)         
        return e_failure;  

    image_size = (long)get_decode_file_size(decInfo->fptr_src_image);

    if (fseek(decInfo->fptr_src_image,
              BMP_HEADER_SIZE, SEEK_SET) != 0)
        return e_failure;

    if (decode_magic_string(MAGIC_STRING, decInfo) != e_success)
        return e_failure;

    if (fread(extn_size_buffer, 1, 32,
              decInfo->fptr_src_image) != 32)
        return e_failure;

    extn_size = 0;

    for (int i = 0; i < 32; i++)
        extn_size |= (unsigned int)(extn_size_buffer[i] & 1) << i;

    if (extn_size == 0 ||
        extn_size >= sizeof(decInfo->extn_secret_file))
        return e_failure;

    for (unsigned int i = 0; i < extn_size; i++)
    {
        if (fread(image_buffer, 1, 8,
                  decInfo->fptr_src_image) != 8)
            return e_failure;

        if (decode_byte_to_lsb(&decInfo->extn_secret_file[i],
                               image_buffer) != e_success)
            return e_failure;
    }

    decInfo->extn_secret_file[extn_size] = '\0';

    if (decode_secret_file_size(image_size, decInfo) != e_success)
        return e_failure;

    if (decode_secret_file_data(decInfo) != e_success)
        return e_failure;

    fclose(decInfo->fptr_src_image);
    decInfo->fptr_src_image = NULL;

    return e_success;
}

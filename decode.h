#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "common.h"
#include "types.h"

typedef struct _DecodeInfo
{
    char *src_image_fname;
    FILE *fptr_src_image;
    uint image_capacity;

    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[5];
    char secret_data[100];
    long size_secret_file;

    char *stego_image_fname;
    FILE *fptr_stego_image;
} DecodeInfo;

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
Status do_decoding(DecodeInfo *decInfo);Status open_decode_files(DecodeInfo *decInfo);
Status check_decode_capacity(DecodeInfo *decInfo);

uint get_decode_image_size_for_bmp(FILE *fptr_image);
uint get_decode_file_size(FILE *fptr);

Status copy_decode_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);
Status decode_secret_file_extn_size(int size, DecodeInfo *decInfo);
Status decode_secret_file_extn(const char *file_extn, DecodeInfo *decInfo);
Status decode_secret_file_size(long file_size, DecodeInfo *decInfo);
Status decode_secret_file_data(DecodeInfo *decInfo);
Status decode_byte_to_lsb(char *data, const char *image_buffer);
Status decode_size_to_lsb(int size, char *imageBuffer);
Status copy_remaining_decode_img_data(FILE *fptr_src, FILE *fptr_dest);

#endif

#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
#include"common.h"
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
 
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    long current_position = ftell(fptr);
    if (current_position < 0)
        return 0;

    fseek(fptr, 0, SEEK_END);
    uint size = (uint)ftell(fptr);
    fseek(fptr, current_position, SEEK_SET);
    return size;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // step 1 -> check argv[2] is having .bmp or not
        //yes -> store the file name into encInfo -> src_image_fname = argv[2]
        //no - > return e_failure

    // step 2 -> check argv[3] is having extn there or not
        // yes - > store the file name into encInfo -> secret_fname = argv[3]
        // no -> return e_failure
    
    // step 3 -> check argv[4] is NULL or not
        //yes -> check argv[4] is having .bmp or not
                //yes -> store the file name into encInfo -> stego_image_fname = argv[4]
                //no - > return e_failure
        
        //no -> store default name the encInfo -> stego_image_fname = "stego.bmp"

    // return e_success;
    char *str = strrchr(argv[2], '.');
    if(str != NULL && strcmp(str, ".bmp")==0)
    {
        encInfo -> src_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }

    str = strrchr(argv[3], '.');
    if(str != NULL && strcmp(str, ".txt")==0)
    {
        encInfo->secret_fname = argv[3];
        strcpy(encInfo->extn_secret_file, str);

    }
    else
    {
        return e_failure;
    }
    if (argv[4] != NULL)
    {
        str = strrchr(argv[4], '.');
        if (str == NULL || strcmp(str, ".bmp") != 0)
            return e_failure;
        encInfo->stego_image_fname = argv[4];
    }
    else
    {
        encInfo->stego_image_fname = "stego.bmp";
    }
    return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "[ERROR]  Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "[ERROR]  Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "[ERROR]  Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    //step 1 -> encInfo -> image_capacity = call get_image_size_for_bmp(encInfo -> fptr_src_image)

    //step 2 -> encInfo -> size_secret_file = call get_file_size(encInfo -> fptr_secret)

    //step 3 -> check encInfo -> image_capacity > 16 + 32 + (strlen(encInfo -> extn_secret_file) * 8) + 32 + (encInfo -> size_secret_file * 8)
    // Calculate required bits: magic (16) + extn_size (32) + extn_chars (extn_len*8) + file_size (32) + file_data (file_size*8)


    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    uint required_capacity = 16 + 32 + (strlen(encInfo->extn_secret_file) * 8) + 32 + (encInfo->size_secret_file * 8);
    
    if(encInfo->image_capacity >= required_capacity)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    // step 1 -> rewind source file pointer

    // step 2 -> read 54 bytes from source file

    // step 3 -> write 54 bytes to dest file

    // return e_success
    
    rewind(fptr_src_image);
    char buffer[54];
    if (fread(buffer, sizeof(char), 54, fptr_src_image) != 54 ||
        fwrite(buffer, sizeof(char), 54, fptr_dest_image) != 54)
        return e_failure;
    if (ftell(fptr_dest_image) == 54)
        return e_success;
    return e_failure;
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    // char buffer[8];

    //step 1 -> read 8 bytes from source file

    //step 2 -> call encode_byte_to_lsb(magic_string[i], buffer)

    //step 3 -> write the buffer into dest file

    // repeat this for strlen(magic_string) times from step 1
    char buffer[8];
    int size=strlen(magic_string);
    for( int i=0;i<size;i++)
    {
        fread(buffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_string[i], buffer);
        fwrite(buffer,8,1,encInfo->fptr_stego_image);
    }
    if(ftell(encInfo->fptr_src_image)!=ftell(encInfo->fptr_stego_image))
    {
        return e_failure;
    }
    return e_success;
}
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    // char buffer[32];

    //step 1 -> read 32 bytes from source file

    //step 2 -> call encode_size_to_lsb(size, buffer)

    //step 3 -> write the buffer into dest file
    char buffer[32];
    fread(buffer,1,32,encInfo->fptr_src_image);
    encode_size_to_lsb(size, buffer);
    fwrite(buffer,1,32,encInfo->fptr_stego_image);

    if(ftell(encInfo->fptr_src_image)!=ftell(encInfo->fptr_stego_image))
        return e_failure;
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    // char buffer[8];

    //step 1 -> read 8 bytes from source file

    //step 2 -> call encode_byte_to_lsb(file_extn[i], buffer)

    //step 3 -> write the buffer into dest file

    // repeat this for strlen(file_extn) times from step 1 
    char buffer[8];
    int size=strlen(file_extn);
    for( int i=0;i<size;i++)
    {
        fread(buffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i], buffer);
        fwrite(buffer,8,1,encInfo->fptr_stego_image);
    }

    if(ftell(encInfo->fptr_src_image)!=ftell(encInfo->fptr_stego_image))
        return e_failure;
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    // char buffer[32];

    //step 1 -> read 32 bytes from source file

    //step 2 -> call encode_size_to_lsb(file_size, buffer)

    //step 3 -> write the buffer into dest file
    char buffer[32];
    fread(buffer,sizeof(char),32,encInfo->fptr_src_image);
    encode_size_to_lsb(file_size, buffer);
    fwrite(buffer,sizeof(char),32,encInfo->fptr_stego_image);
    if(ftell(encInfo->fptr_stego_image)==ftell(encInfo->fptr_src_image))
        return e_success;
    return e_failure;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    //step 1 -> read secret data from file and store it into encInfo -> secret_data

    // char buffer[8];

    //step 2 -> read 8 bytes from source file

    //step 3 -> call encode_byte_to_lsb(encInfo -> secret_data[i], buffer)

    //step 4 -> write the buffer into dest file

    // repeat this for strlen(encInfo -> size_secret_file) times from step 2

    char buffer[8];
    int len = (int)encInfo->size_secret_file;
    if (len > (int)sizeof(encInfo->secret_data))
        return e_failure;
    fread(encInfo->secret_data,sizeof(char),len,encInfo -> fptr_secret);
    for(int i=0;i<len;i++)
    {
        fread(buffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(encInfo -> secret_data[i], buffer);
        fwrite(buffer,8,1,encInfo->fptr_stego_image);
    }
    if(ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_stego_image))
        return e_success;
    return e_failure;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    // write logic for copy remaining data

    // return e_success
    char ch;
    while(fread(&ch,1,1,fptr_src)==1)
    {
        fwrite(&ch,1,1,fptr_dest);
    }
    if(ftell(fptr_src)==ftell(fptr_dest))
        return e_success;
    return e_failure;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    // write logic to encode the char
    for(int i=0;i<8;i++)
    {
        int bit=data&1;                             // get
        image_buffer[i]=image_buffer[i]&0XFE;       // clear
        image_buffer[i]=image_buffer[i]|bit;        // set
        data=data>>1;                               // right shift
    }

    return e_success;
}

Status encode_size_to_lsb(int size, char *imageBuffer)
{
    for (int bit_index = 0; bit_index < 32; bit_index++)
    {
        imageBuffer[bit_index] = (imageBuffer[bit_index] & 0xFE) |
                                 ((size >> bit_index) & 1);
    }
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    //step 1 -> check open_files(encInfo) is returning e_success or not
            //no -> print error return e_failure
            //yes -> print success and goto next step

    //step 2 -> check check_capacity(encInfo) is returning e_success or not
            //no -> print error return e_failure
            //yes -> print success and goto next step

    //step 3 -> call copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image)is returning e_success or not
            //no -> print error return e_failure
            //yes -> print success and goto next step

    //step 4 -> call encode_magic_string(MAGIC_STRING, encInfo)

    //step 5 -> call encode_secret_file_extn_size(strlen(encInfo -> extn_secret_file), encInfo)

    //step 6 -> call encode_secret_file_extn(encInfo -> extn_secret_file, encInfo)

    //step 7 -> call encode_secret_file_size(encInfo ->  size_secret_file, encInfo)

    //step 8 -> call encode_secret_file_data(encInfo)

    //step 9 -> copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image)is returning e_success or not
            //no -> print error return e_failure
            //yes -> print success


    // return e_success

    if(open_files(encInfo) == e_success)
    {
        printf("[SUCCESS] Open file successfully\n");
    }
    else
    {
        printf("[ERROR]   Failed to Open file\n");
        return e_failure;
    }
    if(check_capacity(encInfo)==e_success)
    {
        printf("[SUCCESS] check capacity successfully done\n");
    }
    else
    {
        printf("[ERROR]   Failed to check capacity \n");
        return e_failure;
    }
    if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image)==e_success)
    {
        printf("[SUCCESS] successfully copied bmp header\n");
    }
    else
    {
        printf("[ERROR]   Failed to copy bmp header\n");
        return e_failure;
    }
    if (encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
        printf("[ERROR]   Failed to encode magic string.\n");
        return e_failure;
    }
    else
    {
        printf("[SUCCESS] Magic string encoded successfully.\n");
    }
    if (encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo) == e_failure)
    {
        printf("[ERROR]   Failed to encode secret file extension size.\n");
        return e_failure;
    }
    else
    {
        printf("[SUCCESS] Secret file extension size encoded successfully.\n");
    }
    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure)
    {
        printf("[ERROR]   Failed to encode secret file extension.\n");
        return e_failure;
    }
    else
    {
        printf("[SUCCESS] Secret file extension encoded successfully.\n");
    }
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
    {
        printf("[ERROR]   Failed to encode secret file size.\n");
        return e_failure;
    }
    else
    {
        printf("[SUCCESS] Secret file size encoded successfully.\n");
    }
    if (encode_secret_file_data(encInfo) == e_failure)
    {
        printf("[ERROR]   Failed to encode secret file data.\n");
        return e_failure;
    }
    else
    {
        printf("[SUCCESS] Secret file data encoded successfully.\n");
    }
    if(copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image)==e_success)
    {
        printf("[SUCCESS] successfully copied remaining image data\n");
    }
    else
    {
        printf("[ERROR]   Failed to  copy remaining image data\n");
        return e_failure;
    }
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);
    return e_success;

}
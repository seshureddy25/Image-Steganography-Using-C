#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
#include "common.h"
#include"decode.h"
OperationType check_operation_type(char *);

int main(int argc, char *argv[]) // .aout -e <source_file> <secret_data_file> <optional>
{
    //step 1 -> check check_operation_type(argv[1]) is returning e_encode or not
        // yes -> EncodeInfo encInfo;
            // check read_and_validate_encode_args(argv, &encInfo) is returning e_success or e_failure
                // failure -> print error msg and stop
                // success -> print success msg 
                            // check do_encoding(&encInfo) is returning e_success or e_failure
                                    // failure -> print error msg and stop
                                    // success -> print success msg and stop

    
    if(argc < 4 || (strcmp(argv[1], "-e") == 0 && argc > 5))
    {
        printf("Usage: %s -e <src.bmp> <secret.txt> [stego.bmp]\n", argv[0]);
        printf("       %s -d <stego.bmp> [output_file]\n", argv[0]);
        printf("       %s -d <stego.bmp> <output_file>\n", argv[0]);
        return 0;
    }

       if(check_operation_type(argv[1]) == e_encode)
    {
        printf("1. Encoding\n");
        EncodeInfo encInfo;
        if(read_and_validate_encode_args(argv, &encInfo) != e_failure)
        {
            printf("[SUCCESS] successfully validated\n");
            if(do_encoding(&encInfo) == e_success)
            {
                printf("[SUCCESS] Encoded successfully\n");
                return e_success;
            }
            else
            {
                return e_failure;
            }
        }
        else
        {
            printf("[ERROR]   Encoding is failed\n");
        }
    }
    else if(check_operation_type(argv[1]) == e_decode)
    {
        printf("1. Decoding\n");
        DecodeInfo decInfo;
        if(read_and_validate_decode_args(argv, &decInfo) != e_failure)
        {
            printf("[SUCCESS] successfully validated\n");
            if(do_decoding(&decInfo) == e_success)
            {
                printf("[SUCCESS] Decoded successfully\n");
                return e_success;
            }
            else
            {
                printf("[ERROR]   Decoding is failed\n");
                return e_failure;
            }
        }
        else
        {
            printf("[ERROR]   Decoding is failed\n");
        }
    }
    else
    {
        printf("[ERROR]   Unsupported operation type\n");
        return 0;
    }

    return 0;

}

OperationType check_operation_type(char *symbol)
{
    //step 1 -> check symbol is -e or not
        // yes -> return e_encode

    // step 2 -> check symbol is -d or not
        // yes -> return e_decode
    

    // return e_unsupported

    if(strcmp(symbol,"-e")==0)
    {
        return e_encode;
    }
    else if(strcmp(symbol,"-d")==0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}

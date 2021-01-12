#include <openssl/blowfish.h>
#include <openssl/engine.h>

#define BUFSIZE 1024
/**
 * source code form : https://github.com/kulkarniamit/openssl-evp-demo/blob/master/openssl_evp_demo.c
 */
void blowfish_encrypt(unsigned char *data, int data_len, char* key)
{
    BF_KEY bfkey;
    BF_set_key(&bfkey, strlen(key)+1, (unsigned char*) key);

    unsigned char ivec[8];
    memset(ivec, 0, 8);

    unsigned char *out = (unsigned char*) malloc(1024);
    int num = 0;
    BF_cfb64_encrypt(data, out, data_len, &bfkey, ivec, &num, BF_ENCRYPT);

    memcpy(data, out, data_len);
    free(out);
}

void blowfish_decrypt(unsigned char *data, int data_len, char* key)
{
    BF_KEY bfkey;
    BF_set_key(&bfkey, strlen(key)+1, (unsigned char*) key);

    unsigned char ivec[8];
    memset(ivec, 0, 8);

    unsigned char *out = (unsigned char*) malloc(1024);
    int num = 0;
    BF_cfb64_encrypt(data, out, data_len, &bfkey, ivec, &num, BF_DECRYPT);

    memcpy(data, out, data_len);
    free(out);
}

int encrypt_file(char *input, char *output, char *password){
    FILE *f_input, *f_enc;
    unsigned char in_buf[BUFSIZE];
    int num_bytes_read = 0;
    f_input = fopen(input, "rb");
    f_enc = fopen(output, "wb");
    while(1) {
        num_bytes_read = fread(in_buf, sizeof(unsigned char), BUFSIZE, f_input);
        blowfish_encrypt(in_buf, num_bytes_read, password);
        fwrite(in_buf, sizeof(unsigned char), strlen((char*)in_buf), f_enc);
        if (num_bytes_read < BUFSIZE) {
            /* Reached End of file */
            break;
        }
    }

    fclose(f_input);
    fclose(f_enc);

    return 1;
}
int decrypt_file(char *input, char *output, char *password){
    FILE *f_input, *f_enc;
    unsigned char in_buf[BUFSIZE];
    int num_bytes_read = 0;
    f_input = fopen(input, "rb");
    f_enc = fopen(output, "wb");
    while(1) {
        num_bytes_read = fread(in_buf, sizeof(unsigned char), BUFSIZE, f_input);
        blowfish_decrypt(in_buf, num_bytes_read, password);
        fwrite(in_buf, sizeof(unsigned char), strlen((char*)in_buf), f_enc);
        if (num_bytes_read < BUFSIZE) {
            /* Reached End of file */
            break;
        }
    }

    fclose(f_input);
    fclose(f_enc);

    return 1;
}
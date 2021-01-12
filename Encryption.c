#include <openssl/blowfish.h>
#include <openssl/engine.h>

#define BUFSIZE 1024
/**
 * this function will encrypt and replace a chunk of bits (data) using a key (password)
 * @param data
 * @param data_len
 * @param key
 */
void blowfish_encrypt(unsigned char *data, int data_len, char* key)
{
    BF_KEY bfkey;
    BF_set_key(&bfkey, strlen(key)+1, (unsigned char*) key); //inititate the Blowfish Key with the password typed by user

    unsigned char ivec[8]; //create the inititale vector that will be used to encrypt the first cipher block
    memset(ivec, 0, 8);

    unsigned char *out = (unsigned char*) malloc(1024); //create a temporary string that will hold the value of the encrypted data
    int num = 0;
    BF_cfb64_encrypt(data, out, data_len, &bfkey, ivec, &num, BF_ENCRYPT); //call the openssl blowfish encryption function

    memcpy(data, out, data_len); //copy the content into the data variable (passed by reference which will replace it's content
    free(out);//clear the temporary string
}

/**
 * this function will encrypt and replace a chunk of bits (data) using a key (password)
 * @param data
 * @param data_len
 * @param key
 */
void blowfish_decrypt(unsigned char *data, int data_len, char* key)
{
    BF_KEY bfkey;  //bcrypt key
    BF_set_key(&bfkey, strlen(key)+1, (unsigned char*) key);

    unsigned char ivec[8];
    memset(ivec, 0, 8);

    unsigned char *out = (unsigned char*) malloc(1024);
    int num = 0;
    BF_cfb64_encrypt(data, out, data_len, &bfkey, ivec, &num, BF_DECRYPT); //call the blowfish encryption function with the decryptoion flag in order to decrypt the data

    memcpy(data, out, data_len);
    free(out);
}

int encrypt_file(char *input, char *output, char *password){
    FILE *f_input, *f_enc; //create two file pointer that point to an input and output file
    unsigned char in_buf[BUFSIZE]; // create a chunk holder with 1024 bit size
    int num_bytes_read = 0;
    f_input = fopen(input, "rb"); //open the input file in read binary mode
    f_enc = fopen(output, "wb"); //open the output file in a write binary mode
    /**
     * Keep looping over the file till reach the end
     * and encrypt each block (chunk with size 1024) and write it to the output file
     */
    while(1) {
        num_bytes_read = fread(in_buf, sizeof(unsigned char), BUFSIZE, f_input); //read BUFSIZE from the f_input file and write it in the chunk buffer
        blowfish_encrypt(in_buf, num_bytes_read, password); //encrypt the chunk using the previously defined encryption function
        fwrite(in_buf, sizeof(unsigned char), strlen((char*)in_buf), f_enc); //write the chunk into the output file
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
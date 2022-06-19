/**
 * @file unseal_file.c
 * @author WangFengwei Email: 110.visual@gmail.com
 * @brief unseal a file
 * @created 2011-06-20
 * @modified
 */

#include "common.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <openssl/aes.h>

/**
 * Create an 256 bit key and IV using the supplied key_data. salt can be added for taste.
 * Fills in the encryption and decryption ctx objects and returns 0 on success
 **/
int aes_init(unsigned char *key_data, int key_data_len, unsigned char *salt, EVP_CIPHER_CTX *d_ctx)
{
    int i, nrounds = 5;
    unsigned char key[32], iv[32];
  
    /*
    * Gen key & IV for AES 256 CBC mode. A SHA1 digest is used to hash the supplied key material.
    * nrounds is the number of times the we hash the material. More rounds are more secure but
    * slower.
    */
    i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha1(), salt, key_data, key_data_len, nrounds, key, iv);
    if (i != 32) {
        printf("Key size is %d bits - should be 256 bits\n", i);
        return -1;
    }

    EVP_CIPHER_CTX_init(d_ctx);
    EVP_DecryptInit_ex(d_ctx, EVP_aes_256_cbc(), NULL, key, iv);

    return 0;
}


/*
 * Decrypt *len bytes of ciphertext
 */
unsigned char *aes_decrypt(EVP_CIPHER_CTX *e, unsigned char *ciphertext, int *len) {
    /* plaintext will always be equal to or lesser than length of ciphertext*/
    int p_len = *len, f_len = 0;
    unsigned char *plaintext = malloc(p_len);
    
    EVP_DecryptInit_ex(e, NULL, NULL, NULL, NULL);
    EVP_DecryptUpdate(e, plaintext, &p_len, ciphertext, *len);
    EVP_DecryptFinal_ex(e, plaintext+p_len, &f_len);

    *len = p_len + f_len;
    return plaintext;
}

void usage(char *pch_name) {
	printf("Usage: %s source destination\n", pch_name);
	printf("eg: %s plaintext_file cipertext_file\n", pch_name);
}

int main(int argc, char **argv) {

#define BUF_LEN	(1024*1024)
#define KEY_SIZE 64
	TSS_RESULT result;
	TSS_HCONTEXT hContext;
	TSS_HKEY hSRK, hKey;
	TSS_HPOLICY hPolicy;
	TSS_HTPM hTPM;
	TSS_HENCDATA hEncData;
	UINT32 u32EncDataLen, u32UnsealedDataLen;
	BYTE *pEncData, *rgbUnsealedData;
	FILE *fpIn = NULL, *fpOut = NULL;
	int len, size;
	char *pBufIn = NULL, *pBufOut = NULL;
	unsigned int salt[] = {12345, 54321};
	EVP_CIPHER_CTX de;
	TSS_UUID UUID_K1 =  {0, 0, 0, 0, 0, {8, 0, 0, 0, 0, 1}} ;

	if (argc < 3) {
		usage(argv[0]);
		return 0;
	}

	result = connect_load_all(&hContext, &hSRK, &hTPM);
	if (TSS_SUCCESS != result) {
		printf("connect_load_all failed\n");
		return result;
	}

	result = Tspi_Context_CreateObject(hContext, 
							TSS_OBJECT_TYPE_ENCDATA, 
							TSS_ENCDATA_SEAL, 
							&hEncData);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Context_CreateObject", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	result = set_secret(hContext, hEncData, &hPolicy);
	if (TSS_SUCCESS != result) {
		printf("set_secret failed\n");
		Tspi_Context_Close(hContext);
		return result;
	}

	result = Tspi_Context_LoadKeyByUUID(hContext, 
						TSS_PS_TYPE_SYSTEM, 
						UUID_K1, 
						&hKey);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Context_LoadKeyByUUID", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	result = set_popup_secret(hContext, 
					hKey, 
					TSS_POLICY_USAGE, 
					"Input K1's Pin\n", 
					0);
	if (TSS_SUCCESS != result) {
		print_error("set_popup_secret", result);
		Tspi_Context_Close(hContext);
		return result;
	}
	
	/*result = Tspi_GetPolicyObject(hKey, TSS_POLICY_USAGE, &hPolicy);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_GetPolicyObject", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	result = Tspi_Policy_SetSecret(hPolicy, 
						TESTSUITE_KEY_SECRET_MODE, 
						TESTSUITE_KEY_SECRET_LEN, 
						TESTSUITE_KEY_SECRET);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Policy_SetSecret", result);
		Tspi_Context_Close(hContext);
		return result;
	}*/

	fpIn = fopen(argv[1], "rb");

	len = fread(&u32EncDataLen, 1, sizeof(UINT32), fpIn);
	if (sizeof(UINT32) != len) {
		printf("fread u32EncDataLen failed\n");
		Tspi_Context_Close(hContext);
		fclose(fpIn);
		return -1;
	}

	pEncData = malloc(u32EncDataLen);
	if (!pEncData) {
		printf("No Memory\n");
		Tspi_Context_Close(hContext);
		fclose(fpIn);
		return -1;
	}

	len = fread(pEncData, 1, u32EncDataLen, fpIn);
	if (len != u32EncDataLen) {
		printf("fread pEncData failed\n");
		Tspi_Context_Close(hContext);
		fclose(fpIn);
		free(pEncData);
		return -1;
	}

	result = Tspi_SetAttribData(hEncData, 
						TSS_TSPATTRIB_ENCDATA_BLOB, 
						TSS_TSPATTRIB_ENCDATABLOB_BLOB, 
						u32EncDataLen, 
						pEncData);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_SetAttribData", result);
		Tspi_Context_Close(hContext);
		fclose(fpIn);
		free(pEncData);
		return result;
	}

	//free(pEncData);
    printf("1\n");
	result = Tspi_Data_Unseal(hEncData, 
						hKey, 
						&u32UnsealedDataLen, 
						&rgbUnsealedData);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Data_Unseal", result);
		Tspi_Context_Close(hContext);
		fclose(fpIn);
		return result;
	}
	
	printf("2\n");
	len = fread(&size, 1, sizeof(int), fpIn);
	if (len != sizeof(int)) {
		printf("fread error");
		Tspi_Context_Close(hContext);
		fclose(fpIn);
		return -1;
	}

	if (size > BUF_LEN) {
		printf("file is more than 1MB\n");
		Tspi_Context_Close(hContext);
		fclose(fpIn);
		return -1;
	}

	pBufIn = malloc(size);
	if (!pBufIn) {
		printf("No Memory\n");
		Tspi_Context_Close(hContext);
		fclose(fpIn);
		return -1;
	}

	len = fread(pBufIn, 1, size, fpIn);
	if (len != size) {
		printf("fread failed\n");
		Tspi_Context_Close(hContext);
		fclose(fpIn);
		free(pBufIn);
		return -1;
	}

	if (aes_init(rgbUnsealedData, u32UnsealedDataLen, (unsigned char*)&salt, &de)) {
		printf("aes_init failed\n");
		Tspi_Context_Close(hContext);
		fclose(fpIn);
		free(pBufIn);
		return -1;
	}

	fclose(fpIn);
	free(pEncData);
    
    pBufOut = aes_decrypt(&de, pBufIn, &size);
	
    free(pBufIn);
	
    fpOut = fopen(argv[2], "wb");
	if (!fpOut) {
		printf("open file: %s failed\n", argv[2]);
		Tspi_Context_Close(hContext);
		free(pBufOut);
		return -1;
	}
    len = fwrite(pBufOut, 1, size, fpOut);
	if (len != size) {
		printf("fwrite failed\n");
		Tspi_Context_Close(hContext);
		free(pBufOut);
		fclose(fpOut);
		return -1;
	}
    
    Tspi_Context_Close(hContext);
    fclose(fpOut);
	free(pBufOut);

	return 0;
}
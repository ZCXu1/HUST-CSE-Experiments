/**
 * @file unseal.c
 * @author WangFengwei Email: 110.visual@gmail.com
 * @brief Unseal data
 * @created 2011-05-08
 * @modified
 */

#include "common.h"
#include "hex-out.h"
#include <stdlib.h>

int main(int argc, char **argv)
{
	char version;
	
	version = parseArgs(argc, argv);
	if (version) {
		main_v1_1();
	} else {
		print_wrongVersion();
	}
}

int main_v1_1(void)
{
	TSS_RESULT result;
	TSS_HKEY hSRK;
	TSS_HTPM hTPM;
	TSS_HCONTEXT hContext;
	TSS_HENCDATA hEncData;
	TSS_HPOLICY hEncPolicy;
	UINT32 ulEncDataLen, ulDataUnsealedLen;
	BYTE *rgbDataUnsealed, *dataBuf;
	int i, exitCode;
	FILE *fEncData = NULL;
	char *file_name = "sealedData";
	BYTE *rgbDataToSeal = "0123456789ABCDEF0123456789ABCDEF";
	UINT32 ulDataToSealLen = 32;

	// load Context, SRK, TPM
	result = connect_load_all(&hContext, &hSRK, &hTPM);
	if (result) {
		printf("connect_load_all failed\n");
		return result;
	}
	
	// Create Encrypted Data Object
	result = Tspi_Context_CreateObject(hContext,
				TSS_OBJECT_TYPE_ENCDATA,
				TSS_ENCDATA_SEAL,
				&hEncData);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Context_CreateObject", result);
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		exit(result);
	}

	result = set_secret(hContext, hEncData, &hEncPolicy);
	if (result) {
		printf("set_secret failed\n");
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		exit(result);
	}

	fEncData = fopen(file_name, "rb");
	if (!fEncData) {
		printf("open %s failed\n", file_name);
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		exit(-1);
	}

	// Get file size
	fseek(fEncData, 0L, SEEK_END);
	ulEncDataLen = ftell(fEncData);
	
	fseek(fEncData, 0L, SEEK_SET);
	dataBuf = malloc(ulEncDataLen);
	if (fread(dataBuf, 1, ulEncDataLen, fEncData) != ulEncDataLen) {
		printf("fread failed\n");
		free(dataBuf);
		fclose(fEncData);
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		exit(-1);
	}

	fclose(fEncData);

	printf("Sealed data:\n");
	print_hex(dataBuf, ulEncDataLen);

	result = Tspi_SetAttribData(hEncData,
				TSS_TSPATTRIB_ENCDATA_BLOB,
				TSS_TSPATTRIB_ENCDATABLOB_BLOB,
				ulEncDataLen,
				dataBuf);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_SetAttribData", result);
		free(dataBuf);
		dataBuf = NULL;
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		exit(result);
	}

	free(dataBuf);
	dataBuf = NULL;

	result = Tspi_Data_Unseal(hEncData,
				hSRK,
				&ulDataUnsealedLen,
				&rgbDataUnsealed);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Data_Unseal", result);
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		exit(result);
	}

	printf("Unsealed Data:\n");
	print_hex(rgbDataUnsealed, ulDataUnsealedLen);
	
	if (ulDataUnsealedLen != ulDataToSealLen ||
			memcmp(rgbDataToSeal, rgbDataUnsealed, ulDataToSealLen)) {
		printf("\nFailed\n");
	} else {
		printf("\nSuccess\n");
	}

	Tspi_Context_FreeMemory(hContext, NULL);
	Tspi_Context_Close(hContext);
	
	return 0;
	
}

/**
 * @file extend.c
 * @author WangFengwei Email: 110.visual@gmail.com
 * @brief Extend pcr15
 * @created 2011-05-08
 * @modified
 */

#include "common.h"

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
	TSS_HCONTEXT hContext;
	TSS_HTPM hTPM;
	BYTE pcrValue[] = "01234567890123456789";
	UINT32 ulPcrValLen = 20;
	BYTE *newPcrValue;
	UINT32 ulnewPcrValLen;

		// Create Context
	result = Tspi_Context_Create(&hContext);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Context_Create", result);
		exit(result);
	}

		// Connect Context
	result = Tspi_Context_Connect(hContext, get_server(GLOBALSERVER));
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Context_Connect", result);
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		exit(result);
	}

		// Get TPM Object
	result = Tspi_Context_GetTpmObject(hContext, &hTPM);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Context_GetTpmObject", result);
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		exit(result);
	}
	
	printf("ulPcrValLen:%d\n", ulPcrValLen);

		// Call PcrExtend
	result = Tspi_TPM_PcrExtend(hTPM, 15, ulPcrValLen, pcrValue,
				NULL, &ulnewPcrValLen, &newPcrValue);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_TPM_PcrExtend", result);
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		exit(result);
	}

	printf("\nSuccess\n");
	Tspi_Context_FreeMemory(hContext, NULL);
	Tspi_Context_Close(hContext);
	return 0;
}

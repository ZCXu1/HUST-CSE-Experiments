/**
 * @file seal.c
 * @author WangFengwei Email: 110.visual@gmail.com
 * @brief Seal data
 * @created 2011-05-08
 * @modified
 */

#include "common.h"
#include "hex-out.h"

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
	TSS_RESULT	result;
	TSS_HKEY hSRK;
	TSS_HCONTEXT	hContext;
	TSS_HTPM	hTPM;
	TSS_HENCDATA	hEncData;
	TSS_HPOLICY	hEncPolicy;
	TSS_HPCRS	hPcrs;
	UINT32 subCap, subCapLength, ulPcrValueLength, ulEncDataLength;
	UINT32 pulRespDataLength, dataLength = 32;
	UINT32 numPcrs;
	BYTE *pNumPcrs, *rgbDataToSeal;
	BYTE *rgbPcrValue, *rgbEncData;
	int i, exitCode;
	FILE *fEncData = NULL;
	char *file_name = "sealedData";

	rgbDataToSeal = "0123456789ABCDEF0123456789ABCDEF";

#define DEBUG
#ifdef DEBUG
#define SET_ANCHOR(x) printf(x "\n")
#else
#define SET_ANCHOR(x) do { }while(0)
#endif // #ifdef DEBUG

	SET_ANCHOR("1");

	// load Context, SRK, and TPM
	if ((result = connect_load_all(&hContext, &hSRK, &hTPM))) {
		return result;
	}

	SET_ANCHOR("2");

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

	SET_ANCHOR("3");

	if ((result = set_secret(hContext, hEncData, &hEncPolicy))) {
		printf("set_secret failed\n");
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		exit(result);
	}

	SET_ANCHOR("4");

	// Create PCRs Object
	result = Tspi_Context_CreateObject(hContext,
				TSS_OBJECT_TYPE_PCRS,
				0,
				&hPcrs);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Context_CreateObject", result);
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		exit(result);
	}

	SET_ANCHOR("5");

	subCap = TSS_TPMCAP_PROP_PCR;
	subCapLength = sizeof(UINT32);

	result = Tspi_TPM_GetCapability(hTPM,
				TSS_TPMCAP_PROPERTY,
				subCapLength, (BYTE *)&subCap,
				&pulRespDataLength,
				&pNumPcrs);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_TPM_GetCapability", result);
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		exit(result);
	}

	SET_ANCHOR("6");

	numPcrs = *(UINT32 *)pNumPcrs;

	for (i = 0; i < numPcrs; i++) {
		// In Physical TPM not all pcrs are available, so we choose pcr 15
		if (i != 15) {
			continue;
		}
		result = Tspi_TPM_PcrRead(hTPM, i, &ulPcrValueLength, &rgbPcrValue);
		if (TSS_SUCCESS != result) {
			print_error("Tspi_TPM_PcrRead", result);
			Tspi_Context_FreeMemory(hContext, NULL);
			Tspi_Context_Close(hContext);
			exit(result);
		}

		result = Tspi_PcrComposite_SetPcrValue(hPcrs, i, ulPcrValueLength, 
					rgbPcrValue);
		if (TSS_SUCCESS != result) {
			print_error("Tspi_PcrComposite_SetPcrValue", result);
			Tspi_Context_FreeMemory(hContext, NULL);
			Tspi_Context_Close(hContext);
			exit(result);
		}
	}

	SET_ANCHOR("7");

	// Seal Data
	result = Tspi_Data_Seal(hEncData,
				hSRK,
				dataLength,
				rgbDataToSeal,
				hPcrs);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Data_Seal", result);
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		exit(result);
	}

	result = Tspi_GetAttribData(hEncData,
				TSS_TSPATTRIB_ENCDATA_BLOB,
				TSS_TSPATTRIB_ENCDATABLOB_BLOB,
				&ulEncDataLength,
				&rgbEncData);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_GetAttribData", result);
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		exit(result);
	}

	printf("EncDataBlob:\n");
	print_hex(rgbEncData, ulEncDataLength);

	fEncData = fopen(file_name, "w+b");
	if (NULL == fEncData) {
		printf("fopen %s failed\n", file_name);
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		exit(-1);
	}

	if (fwrite(rgbEncData, 1, ulEncDataLength, fEncData) != ulEncDataLength) {
		printf("fwrite failed\n");
		fclose(fEncData);
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		exit(-1);
	}

	fclose(fEncData);

	printf("\nSuccess\n");

	Tspi_Context_FreeMemory(hContext, NULL);
	Tspi_Context_Close(hContext);
	//exit(exitCode);

	return 0;
}

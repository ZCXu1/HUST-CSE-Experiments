/**
 * @file platform_dst.c
 * @author WangFengwei Email: 110.visual@gmail.com
 * @brief Migrate a key from platform_src to platform_dst
 * @created 2011-06-04
 * @modified
 */

#define PUB_SRK		"srk.pub"
#define MIG_BLOB	"mig.blob"

#include "common.h"
#include <stdio.h>
#include <string.h>

void
usage(char *name)
{
	printf("Usage: %s -g|-m\n", name);
	printf("-g: Generate SRK pubkey and save it to file %s\n", PUB_SRK);
	printf("-m: Migrate key from file %s\n", MIG_BLOB);
}

int
gen_pubSrk(void)
{
	TSS_RESULT result;
	TSS_HCONTEXT hContext;
	TSS_HKEY hSRK;
	TSS_HPOLICY hPolicy;
	TSS_HTPM hTPM;
	UINT32 u32PubSrkLen;
	BYTE *prgbPubSrk;
	FILE *fp;
	size_t len; 

	// Create Context
	result = Tspi_Context_Create(&hContext);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Context_Create", result);
		return result;
	}

	// Connect Context
	result = Tspi_Context_Connect(hContext, NULL);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Context_Connect", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	// Get TPM Object
	result = Tspi_Context_GetTpmObject(hContext, &hTPM);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Context_GetTpmObject", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	// Set TPM Usage Secret
	result = Tspi_GetPolicyObject(hTPM, TSS_POLICY_USAGE, &hPolicy);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_GetPolicyObject", result);
		Tspi_Context_Close(hContext);
		return result;
	}
	result = Tspi_Policy_SetSecret(hPolicy,
					TESTSUITE_OWNER_SECRET_MODE, 
					TESTSUITE_OWNER_SECRET_LEN, 
					TESTSUITE_OWNER_SECRET);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Policy_SetSecret", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	// Load SRK
	result = Tspi_Context_LoadKeyByUUID(hContext, 
					TSS_PS_TYPE_SYSTEM, 
					SRK_UUID, 
					&hSRK);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Context_LoadKeyByUUID", result);
		Tspi_Context_Close(hContext);
		return result;
	}

#ifndef TESTSUITE_NOAUTH_SRK
	// Set SRK Usage Secret
	result = Tspi_GetPolicyObject(hSRK, TSS_POLICY_USAGE, &hPolicy);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_GetPolicyObject", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	result = Tspi_Policy_SetSecret(hPolicy, 
					TESTSUITE_SRK_SECRET_MODE, 
					TESTSUITE_SRK_SECRET_LEN, 
					TESTSUITE_SRK_SECRET);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Policy_SetSecret", result);
		Tspi_Context_Close(hContext);
		return result;
	}
#endif // #ifndef TESTSUITE_NOAUTH_SRK

	// Get SRK Pub Key
	result = Tspi_TPM_OwnerGetSRKPubKey(hTPM, &u32PubSrkLen, &prgbPubSrk);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_TPM_OwnerGetSRKPubKey", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	fp = fopen(PUB_SRK, "wb");
	if (!fp) {
		fprintf(stderr, "open file: %s failed\n", PUB_SRK);
		Tspi_Context_Close(hContext);
		return -1;
	}

	len = fwrite(&u32PubSrkLen, sizeof(u32PubSrkLen), 1, fp);
	if (1 != len) {
		fprintf(stderr, "fwrite u32PubSrkLen faild\n");
		fclose(fp);
		Tspi_Context_Close(hContext);
		return -1;
	}

	len = fwrite(prgbPubSrk, u32PubSrkLen, 1, fp);
	if (1 != len) {
		fprintf(stderr, "fwrite prgPubSrk faild\n");
		fclose(fp);
		Tspi_Context_Close(hContext);
		return -1;
	}

	fclose(fp);

	Tspi_Context_Close(hContext);
	
	printf("Generating Pub Key Success\n");
	return 0;
}

int 
mig_key(void)
{
	TSS_RESULT result;
	TSS_HCONTEXT hContext;
	TSS_HKEY hSRK;
	TSS_HPOLICY hPolicy;
	TSS_HKEY hNewMigKey;
	FILE *fp;
	size_t len;
	UINT32 u32RandomLen;
	BYTE *pRandom;
	UINT32 u32MigBlobLen;
	BYTE *pMigBlob;

	// Create Context
	result = Tspi_Context_Create(&hContext);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Context_Create", result);
		return result;
	}

	// Connect Context
	result = Tspi_Context_Connect(hContext, NULL);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Context_Connect", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	// Load SRK
	result = Tspi_Context_LoadKeyByUUID(hContext, 
					TSS_PS_TYPE_SYSTEM, 
					SRK_UUID, 
					&hSRK);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Context_LoadKeyByUUID", result);
		Tspi_Context_Close(hContext);
		return result;
	}

#ifndef TESTSUITE_NOAUTH_SRK
	// Set SRK Usage Secret
	result = Tspi_GetPolicyObject(hSRK, TSS_POLICY_USAGE, &hPolicy);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_GetPolicyObject", result);
		Tspi_Context_Close(hContext);
		return result;
	}
	
	result = Tspi_Policy_SetSecret(hPolicy, 
				TESTSUITE_SRK_SECRET_MODE, 
				TESTSUITE_SRK_SECRET_LEN, 
				TESTSUITE_SRK_SECRET);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Policy_SetSecret", result);
		Tspi_Context_Close(hContext);
		return result;
	}
#endif // #ifndef TESTSUITE_NOAUTH_SRK

	// Create New Migrated Key Object
	result = Tspi_Context_CreateObject(hContext, 
					TSS_OBJECT_TYPE_RSAKEY, 
					TSS_KEY_TYPE_SIGNING | 
					TSS_KEY_SIZE_2048 | 
					TSS_KEY_NO_AUTHORIZATION | 
					TSS_KEY_MIGRATABLE, 
					&hNewMigKey);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Context_CreateObject", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	// Load Migration Blob
	fp = fopen(MIG_BLOB, "rb");	
	if (!fp) {
		fprintf(stderr, "open file: %s failed\n", MIG_BLOB);
		Tspi_Context_Close(hContext);
		return -1;
	}

	len = fread(&u32RandomLen, sizeof(UINT32), 1, fp);
	if (1 != len) {
		fprintf(stderr, "fread u32RandomLen failed\n");
		fclose(fp);
		Tspi_Context_Close(hContext);
		return -1;
	}

	pRandom = malloc(u32RandomLen);
	if (!pRandom) {
		perror("No Memory\n");
		fclose(fp);
		Tspi_Context_Close(hContext);
		return -1;
	}

	len = fread(pRandom, u32RandomLen, 1, fp);
	if (1 != len) {
		fprintf(stderr, "fread pRandom failed");
		free(pRandom);
		fclose(fp);
		Tspi_Context_Close(hContext);
		return -1;
	}

	len = fread(&u32MigBlobLen, sizeof(UINT32), 1, fp);
	if (1 != len) {
		fprintf(stderr, "fread u32MigBlob failed\n");
		fclose(fp);
		free(pRandom);
		Tspi_Context_Close(hContext);
		return -1;
	}

	pMigBlob = malloc(u32MigBlobLen);
	if (!pMigBlob) {
		perror("No Memory\n");
		fclose(fp);
		free(pRandom);
		Tspi_Context_Close(hContext);
		return -1;
	}

	len = fread(pMigBlob, u32MigBlobLen, 1, fp);
	if (1 != len) {
		fprintf(stderr, "fread pMigBlob\n");
		fclose(fp);
		free(pRandom);
		free(pMigBlob);
		Tspi_Context_Close(hContext);
		return -1;
	}

	fclose(fp);

	// TODO Convert Migration Blob
	// 	完成以下代码，参考Tspi_Key_ConvertMigrationBlob
	result = Tspi_Key_ConvertMigrationBlob(hNewMigKey, 
					hSRK, 
					u32RandomLen, 
					pRandom, 
					u32MigBlobLen, 
					pMigBlob);
	free(pRandom);
	free(pMigBlob);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Key_ConvertMigrationBlob", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	// Load the New Migrated Key
	result = Tspi_Key_LoadKey(hNewMigKey, hSRK);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Key_Load", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	// verify that the new migrated key is valid
	result = sign_and_verify(hContext, hNewMigKey);
	if (TSS_SUCCESS != result) {
		print_error("sign_and_verify", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	Tspi_Context_Close(hContext);

	printf("Migration Success\n");
	return 0;

}

int
main(int argc, char **argv)
{
	if (argc < 2) {
		usage(argv[0]);
		return 0;
	}

	if (!strcmp("-g", argv[1])) {
		return gen_pubSrk();
	} else if (!strcmp("-m", argv[1])) {
		return mig_key();
	} else {
		usage(argv[0]);
		return 0;
	}
}
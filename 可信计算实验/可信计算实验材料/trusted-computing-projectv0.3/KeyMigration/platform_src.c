/**
 * @file platform_src.c
 * @author WangFengwei Email: 110.visual@gmail.com
 * @brief Migrate a key from platform_src to platform_dst
 * @created 2011-06-04
 * @modified
 */

#define PUB_SRK		"srk.pub"
#define MIG_BLOB	"mig.blob"

#include "common.h"

int
main(int argc, char **argv)
{
	TSS_RESULT result;
	TSS_HCONTEXT hContext;
	TSS_HKEY hSRK;
	TSS_HTPM hTPM;
	TSS_HKEY hMigStorageKey;
	TSS_HPOLICY hTpmUsagePolicy;
	TSS_HPOLICY hPolicy;
	UINT32 u32PubSrkLen;
	BYTE *pPubSrk;
	TSS_HKEY hMigratableKey;
	UINT32 u32MigTicketLen;
	BYTE *prgbMigTicket;
	UINT32 u32RandomLen;
	BYTE *prgbRandom;
	UINT32 u32MigBlobLen;
	BYTE *prgbMigBlob;
	//TSS_HKEY hNewMigrateKey;
	FILE *fp;
	size_t len;
		TSS_UUID uuid_mig = {0, 0, 0, 0, 0, {8, 0, 0, 0, 0, 9}};
	
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
	result = Tspi_GetPolicyObject(hTPM, TSS_POLICY_USAGE, &hTpmUsagePolicy);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_GetPolicyObject", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	result = Tspi_Policy_SetSecret(hTpmUsagePolicy, 
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
	// Set SRK Usage Policy
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
	/*result = Tspi_TPM_OwnerGetSRKPubKey(hTPM, &u32PubSrkLen, &pPubSrk);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_TPM_OwnerGetSRKPubKey", result);
		Tspi_Context_Close(hContext);
		return result;
	}*/
	

/*	Tspi_Context_Close(hContext);
	printf("OK\n");
	return 0;*/

	// Create Migration Storage Key Object
	result = Tspi_Context_CreateObject(hContext, 
					TSS_OBJECT_TYPE_RSAKEY,
					TSS_KEY_TYPE_STORAGE | 
					TSS_KEY_SIZE_2048 |
					TSS_KEY_VOLATILE | 
					TSS_KEY_NO_AUTHORIZATION | 
					TSS_KEY_NOT_MIGRATABLE, 
					&hMigStorageKey);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Context_CreateObject", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	// Set Encryption scheme
	result = Tspi_SetAttribUint32(hMigStorageKey,
					TSS_TSPATTRIB_KEY_INFO, 
					TSS_TSPATTRIB_KEYINFO_ENCSCHEME, 
					TSS_ES_RSAESOAEP_SHA1_MGF1);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_SetAttribUint32", result);
		Tspi_Context_Close(hContext);
		return result;
	}

		// Load SRK Pub Key from file
	fp = fopen(PUB_SRK, "rb");
	if (!fp) {
		fprintf(stderr, "open file: %s failed\n", PUB_SRK);
		Tspi_Context_Close(hContext);
		return -1;
	}

	len = fread(&u32PubSrkLen, sizeof(UINT32), 1, fp);
	if (1 != len) {
		fprintf(stderr, "fread u32PubSrkLen failed\n");
		fclose(fp);
		Tspi_Context_Close(hContext);
		return -1;
	}

	pPubSrk = malloc(u32PubSrkLen);
	if (!pPubSrk) {
		perror("No Memory");
		fclose(fp);
		Tspi_Context_Close(hContext);
		return -1;
	}

	len = fread(pPubSrk, u32PubSrkLen, 1, fp);
	if (1 != len) {
		fprintf(stderr, "fread pPubSrk failed\n");
		fclose(fp);
		free(pPubSrk);
		Tspi_Context_Close(hContext);
		return -1;
	}

	fclose(fp);

	result = Tspi_SetAttribData(hMigStorageKey, 
					TSS_TSPATTRIB_KEY_BLOB, 
					TSS_TSPATTRIB_KEYBLOB_PUBLIC_KEY, 
					u32PubSrkLen, 
					pPubSrk);
	free(pPubSrk);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_SetAttribData", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	// Create Migratable Key Object
	/*result = Tspi_Context_CreateObject(hContext, 
					TSS_OBJECT_TYPE_RSAKEY, 
					TSS_KEY_TYPE_SIGNING | 
					TSS_KEY_SIZE_2048 | 
					TSS_KEY_NO_AUTHORIZATION | 
					TSS_KEY_MIGRATABLE, 
					&hMigratableKey);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Context_CreateObject", result);
		Tspi_Context_Close(hContext);
		return result;
	}
	
	result = Tspi_Context_CreateObject(hContext, 
					TSS_OBJECT_TYPE_POLICY, 
					TSS_POLICY_MIGRATION, 
					&hPolicy);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Context_CreateObject", result);
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
	}

	result = Tspi_Policy_AssignToObject(hPolicy, hMigratableKey);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Policy_AssignToObject", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	result = Tspi_Key_CreateKey(hMigratableKey, hSRK, 0);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Key_CreateKey", result);
		Tspi_Context_Close(hContext);
		return result;
	}*/

	result = Tspi_Context_LoadKeyByUUID(hContext, 
					TSS_PS_TYPE_SYSTEM, 
					uuid_mig, 
					&hMigratableKey);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Context_LoadKeyByUUID", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	result = set_popup_secret(hContext, 
					hMigratableKey, 
					TSS_POLICY_MIGRATION, 
					"Input Migration Key's Pin\n", 
					0);
	if (TSS_SUCCESS != result) {
		print_error("set_popup_secret", result);
		Tspi_Context_Close(hContext);
		return result;
	}
	// Authorize Migrate Ticket
	result = Tspi_TPM_AuthorizeMigrationTicket(hTPM, 
					hMigStorageKey, 
					TSS_MS_MIGRATE, 
					&u32MigTicketLen, 
					&prgbMigTicket);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_TPM_AuthorizeMigrationticket", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	// Create Migration Blob
	result = Tspi_Key_CreateMigrationBlob(hMigratableKey, 
					hSRK, 
					u32MigTicketLen, 
					prgbMigTicket, 
					&u32RandomLen, 
					&prgbRandom, 
					&u32MigBlobLen, 
					&prgbMigBlob);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Key_CreateMigrationBlob", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	// Save Migration Blob to File
	fp = fopen(MIG_BLOB, "wb");
	if (!fp) {
		fprintf(stderr, "fopen file:%s failed\n", MIG_BLOB);
		Tspi_Context_Close(hContext);
		return -1;
	}

	len = fwrite(&u32RandomLen, sizeof(UINT32), 1, fp);
	if (1 != len) {
		fprintf(stderr, "fwrite u32RandomLen failed\n");
		fclose(fp);
		Tspi_Context_Close(hContext);
		return -1;
	}

	len = fwrite(prgbRandom, u32RandomLen, 1, fp);
	if (1 != len) {
		fprintf(stderr, "fwrite prgbRandom failed\n");
		fclose(fp);
		Tspi_Context_Close(hContext);
		return -1;
	}

	len = fwrite(&u32MigBlobLen, sizeof(UINT32), 1, fp);
	if (1 != len) {
		fprintf(stderr, "fwrite u32MigBlobLen failed\n");
		fclose(fp);
		Tspi_Context_Close(hContext);
		return -1;
	}

	len = fwrite(prgbMigBlob, u32MigBlobLen, 1, fp);
	if (1 != len) {
		fprintf(stderr, "fwrite prgbMigBlob failed\n");
		fclose(fp);
		Tspi_Context_Close(hContext);
		return -1;
	}

	fclose(fp);

	// Create New Migrated Key Object
	/*result = Tspi_Context_CreateObject(hContext, 
					TSS_OBJECT_TYPE_RSAKEY, 
					TSS_KEY_TYPE_SIGNING | 
					TSS_KEY_SIZE_2048 | 
					TSS_KEY_NO_AUTHORIZATION | 
					TSS_KEY_MIGRATABLE, 
					&hNewMigrateKey);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Context_CreateObject", result);
		Tspi_Context_Close(hContext);
		return result;
	}*/

	// Convert Migration Blob
	/*result = Tspi_Key_ConvertMigrationBlob(hNewMigrateKey, 
					hSRK, 
					u32RandomLen, 
					prgbRandom, 
					u32MigBlobLen, 
					prgbMigBlob);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Key_ConvertMigrationBlob", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	// Load the New Migrated Key
	result = Tspi_Key_LoadKey(hNewMigrateKey, hSRK);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Key_LoadKey", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	// verify that New Migrated Key is valid
	result = sign_and_verify(hContext, hNewMigrateKey);
	if (TSS_SUCCESS != result) {
		print_error("sign_and_verify", result);
		Tspi_Context_Close(hContext);
		return result;
	}*/

	Tspi_Context_Close(hContext);
	printf("OK\n");
	return 0;
}

/*
  功能：从系统永久存储区Load密钥到TPM中。
  流程：	1. load SRK 并验证其使用权限。
	2. load SRK 的子密钥K1，并验证其使用权限。
	3. load K1的子密钥K2，并验证其使用权限。
	4. 重新验证SRK的使用权限(每次使用之前都需验证)。
	5. load SRK 的子密钥K3，并验证其使用权限。
	6. load K3的子密钥K4，并验证其使用权限。
  运行： ./xxx -v 1.2
*/

#include "common.h"

TSS_UUID UUID_K1 =  {0, 0, 0, 0, 0, {8, 0, 0, 0, 0, 1}} ;
TSS_UUID UUID_K2 = {0, 0, 0, 0, 0, {8, 0, 0, 0, 0, 2}} ;
TSS_UUID UUID_K3 = {0, 0, 0, 0, 0, {8, 0, 0, 0, 0, 3}} ;
TSS_UUID UUID_K4 = {0, 0, 0, 0, 0, {8, 0, 0, 0, 0, 4}} ;

char *key_structure = "SRK\n"
		      "|__UserK1(Storage key, unmigratable)\n"
		      "|  |__UserK2(Signing key, unmigratable)\n"
		      "|\n"
                      "|__UserK3(Storage key, migratable)\n"
		      "   |__UserK4(Bind key, migratable)\n";

int main(int argc, char **argv)
{
	char		version;

	version = parseArgs( argc, argv );
	if (version == TESTSUITE_TEST_TSS_1_2)
		main_v1_2(version);
	//else if (version)
		//main_v1_1();
	else
		print_wrongVersion();
}


main_v1_2(char version){

	char		*nameOfFunction = "Load KEY";
	TSS_HCONTEXT	hContext;
	TSS_HTPM	hTPM;
	TSS_FLAG	initFlags;
	TSS_HKEY 	hKey1, hKey2, hKey3, hKey4;
	TSS_HKEY	hSRK;
	TSS_RESULT	result;
	TSS_UUID	uuid;
	TSS_HPOLICY	srkUsagePolicy, keyUsagePolicy, keyMigPolicy, hKeyPolicy;

	print_begin_test(nameOfFunction);
	printf("KEY structure:\n%s\n", key_structure);

	//Create Context
	result = Tspi_Context_Create(&hContext);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_Connect", result);
		print_error_exit(nameOfFunction, err_string(result));
		exit(result);
	}
	//Connect Context
	result = Tspi_Context_Connect(hContext, get_server(GLOBALSERVER));
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_Connect", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_Close(hContext);
		exit(result);
	}
		
	//Load Key By UUID
	result = Tspi_Context_LoadKeyByUUID(hContext,
				TSS_PS_TYPE_SYSTEM, SRK_UUID, &hSRK);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_LoadKeyByUUID", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_Close(hContext);
		exit(result);
	}
	printf("Load SRK sucessed!\n");
#ifndef TESTSUITE_NOAUTH_SRK
		//Get Policy Object
	result = Tspi_GetPolicyObject(hSRK, TSS_POLICY_USAGE, &srkUsagePolicy);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_GetPolicyObject", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_Close(hContext);
		exit(result);
	}
		//Set Secret
	result = Tspi_Policy_SetSecret(srkUsagePolicy, TESTSUITE_SRK_SECRET_MODE,
			TESTSUITE_SRK_SECRET_LEN, TESTSUITE_SRK_SECRET);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Policy_SetSecret", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_Close(hContext);
		exit(result);
	}
#endif
	// -------------------------------- load k1 ---------------------------------
	printf("Loading K1...\n");
	result = Tspi_Context_GetKeyByUUID(hContext,
					    TSS_PS_TYPE_SYSTEM, UUID_K1,
					    &hKey1);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_LoadKeyByUUID", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		exit(result);
	}
	
	//验证使用测略
	result = Tspi_Key_LoadKey(hKey1, hSRK);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Key_LoadKey", result);
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		exit(result);
	}
	printf("Load UserK1 sucessed!\n");

	// -------------------------------- load k2 ---------------------------------
	//Load Key By UUID   --K2
	printf("Loading K2...\n");
	result = Tspi_Context_GetKeyByUUID( hContext, TSS_PS_TYPE_SYSTEM,
						UUID_K2,
						&hKey2 );
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_LoadKeyByUUID", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		exit(result);
	}
	//验证使用测略	
	result = set_popup_secret(hContext, hKey1, TSS_POLICY_USAGE, "Input K1's pin", 0);
	if (TSS_SUCCESS != result) {
		print_error("set_popup_secret", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	result = Tspi_Key_LoadKey(hKey2, hKey1);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Key_LoadKey", result);
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		exit(result);
	}
	printf("Load UserK2 sucessed!\n");

	// 给SRK授权
	#ifndef TESTSUITE_NOAUTH_SRK
	//Get Policy Object
	result = Tspi_GetPolicyObject(hSRK, TSS_POLICY_USAGE, &srkUsagePolicy);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_GetPolicyObject", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_Close(hContext);
		exit(result);
	}
	//Set Secret
	result = Tspi_Policy_SetSecret(srkUsagePolicy, TESTSUITE_SRK_SECRET_MODE,
			TESTSUITE_SRK_SECRET_LEN, TESTSUITE_SRK_SECRET);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Policy_SetSecret", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_Close(hContext);
		exit(result);
	}
	#endif

	// -------------------------------- load k3 ---------------------------------
	printf("Loading K3...\n");
	result = Tspi_Context_LoadKeyByUUID(hContext,
					    TSS_PS_TYPE_SYSTEM, UUID_K3,
					    &hKey3);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_GetKeyByUUID", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		exit(result);
	}
	
	printf("Load UserK3 sucessed!\n");

	// -------------------------------- load k4 ---------------------------------
	printf("Loading K4...\n");
	// TODO:
	//      这部分代码由同学们自己完成
	//

	
	printf("Load UserK4 sucessed!\n");

	//验证使用测略
	result = set_popup_secret(hContext, hKey3, TSS_POLICY_USAGE, "Input K3's pin", 0);
	if (TSS_SUCCESS != result) {
		print_error("set_popup_secret", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	result = Tspi_Key_LoadKey(hKey4, hKey3);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Key_LoadKey", result);
		Tspi_Context_FreeMemory(hContext, NULL);
		Tspi_Context_Close(hContext);
		exit(result);
	}
	printf("Load UserK4 sucessed!\n");
	
	print_success(nameOfFunction, result);
	print_end_test(nameOfFunction);
	Tspi_Context_FreeMemory(hContext, NULL);
	Tspi_Context_Close(hContext);
	exit(0);	
}

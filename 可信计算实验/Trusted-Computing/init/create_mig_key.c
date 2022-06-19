/**
 * @file create_mig_key.c
 * @author WangFengwei Email: 110.visual@gmail.com
 * @brief create a migratable key
 * @created 2011-06-15
 * @modified
 */

#include "common.h"
#include <stdio.h>

TSS_RESULT 
my_create_key(TSS_HKEY hParent, TSS_HKEY *phKey)
{
	TSS_RESULT result;

	result = Tspi_Key_CreateKey(*phKey, hParent, 0);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Key_CreateKey", result);
		return result;
	}

	return result;
}

TSS_RESULT 
create_keyObj(TSS_HCONTEXT hContext, 
			TSS_FLAG initFlags, 
			TSS_HKEY hParent, 
			TSS_HKEY *phKey, 
			const char *pUsage, 
			const char *pMig)
{
	TSS_RESULT result;
	TSS_HPOLICY hPolicy;

	// Create Key Object
	result = Tspi_Context_CreateObject(hContext, 
					TSS_OBJECT_TYPE_RSAKEY, 
					initFlags, 
					phKey);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Context_CreateObject", result);
		return result;
	}

	// Set Usage Authorization
	if (initFlags & TSS_KEY_AUTHORIZATION) {
		result = set_popup_secret(hContext, 
						*phKey, 
						TSS_POLICY_USAGE, 
						pUsage, 
						1);
		if (TSS_SUCCESS != result) {
			print_error("set_popup_secret", result);
			return result;
		}
	}

	// Set Migration Authorization
	if (initFlags & TSS_KEY_MIGRATABLE) {
		result = set_popup_secret(hContext, 
						*phKey, 
						TSS_POLICY_MIGRATION, 
						pMig, 
						1);
		if (TSS_SUCCESS != result) {
			print_error("set_popup_secret", result);
			return result;
		}
		/*result = Tspi_Context_CreateObject(hContext, 
						TSS_OBJECT_TYPE_POLICY, 
						TSS_POLICY_MIGRATION, 
						&hPolicy);
		if (TSS_SUCCESS != result) {
			print_error("Tspi_Context_CreateObject", result);
			return result;
		}

		fprintf(stderr, "%s\n", pMig);

		result = Tspi_Policy_SetSecret(hPolicy, 
						TSS_SECRET_MODE_POPUP, 
						0, 
						NULL);
		if (TSS_SUCCESS != result) {
			print_error("Tspi_Policy_Setsecret", result);
			return result;
		}

		result = Tspi_Policy_AssignToObject(hPolicy, *phKey);
		if (TSS_SUCCESS != result) {
			print_error("Tspi_Policy_AssignToObject", result);
			return result;
		}*/

		
	}

	return result;
}

TSS_RESULT 
createAndRegist_key(
		TSS_HCONTEXT hContext, 
		TSS_HKEY hParentKey, 
		const TSS_UUID *pUuid, 
		TSS_FLAG initFlags, 
		TSS_HKEY *phKey, 
		const char *pUsage, 
		const char *pMig)
{
	TSS_RESULT result;
	TSS_HKEY hTempKey;
	
	// Create Key Object and Initialize Policies
	result = create_keyObj(hContext, 
			initFlags, 
			hParentKey, 
			phKey, 
			pUsage, 
			pMig);
	if (TSS_SUCCESS != result) {
		print_error("create_keyObj", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	result = my_create_key(hParentKey, phKey);
	if (TSS_SUCCESS != result) {
		print_error("create_key", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	result = Tspi_Context_UnregisterKey(hContext, 
				TSS_PS_TYPE_SYSTEM, 
				*pUuid, 
				&hTempKey);
	if (TSS_SUCCESS != result && TSS_E_PS_KEY_NOTFOUND != TSS_ERROR_CODE(result)) {
		print_error("Tspi_Context_UnregisterKey", result);
		Tspi_Context_Close(hContext);
		return result;
	}

    if (TSS_SUCCESS == result) {
	    result = Tspi_Context_CloseObject(hContext, hTempKey);
	    if (TSS_SUCCESS != result) {
		    print_error("Tspi_Context_CloseObject", result);
		    Tspi_Context_Close(hContext);
		    return result;
	    }
    }
	
	result = Tspi_Context_RegisterKey(hContext, 
					*phKey, 
					TSS_PS_TYPE_SYSTEM, 
					*pUuid, 
					TSS_PS_TYPE_SYSTEM, 
					SRK_UUID);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Context_RegisterKey", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	return result;
}

int 
main(int argc, char **argv)
{
	TSS_RESULT result;
	TSS_HCONTEXT hContext;
	TSS_HKEY hSRK;
	TSS_HKEY hKey;
	TSS_HPOLICY hPolicy;
	TSS_FLAG initFlags = TSS_KEY_TYPE_SIGNING | 
						TSS_KEY_SIZE_2048 | 
						TSS_KEY_NO_AUTHORIZATION | 
						TSS_KEY_MIGRATABLE;
	TSS_UUID uuid_mig = {0, 0, 0, 0, 0, {8, 0, 0, 0, 0, 9}};
	const char *pMig = "Please input Migratable key's migration secret";
	const char *pUsage = "Please input Migratable key's usage secret";

	// Create Context
	result = Tspi_Context_Create(&hContext);
	if (TSS_SUCCESS != result) {
		print_error("Tspi_Context_Create", result);
		return result;
	}

	// Connect Context
	result = Tspi_Context_Connect(hContext, get_server(GLOBALSERVER));
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
#endif // #ifdef TESTSUITE_NOAUTH_SRK

	// Create and Regist a Migratable Key
	result = createAndRegist_key(hContext, 
					hSRK, 
					&uuid_mig, 
					initFlags, 
					&hKey, 
					pUsage, 
					pMig);
	if (TSS_SUCCESS != result) {
		print_error("createAndRegist_key", result);
		Tspi_Context_Close(hContext);
		return result;
	}

	Tspi_Context_Close(hContext);
	
	printf("success\n");

	return 0;
}

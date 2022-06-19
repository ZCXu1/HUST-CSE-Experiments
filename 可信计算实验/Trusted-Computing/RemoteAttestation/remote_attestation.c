/*
 *
 *   Copyright (C) International Business Machines  Corp., 2005
 *
 *   This program is free software;  you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY;  without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 *   the GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program;  if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/*
 * NAME
 *	Tspi_TPM_CreateIdentity.c
 *
 * DESCRIPTION
 *	This test will attempt to act as a PrivacyCA and create a TPM
 *	identity.
 *
 * ALGORITHM
 *	Setup:
 *		Create Context, load the SRK and get a TPM handle
 *		Generate an openssl RSA key to represent the CA's key
 *		Create the identity key's object
 *		Call Tspi_TPM_CollateIdentityRequest
 *		Decrypt the symmetric and asymmetric blobs
 *		Verify the identity binding
 *		Create a fake credential and encrypt it
 *		Call Tspi_TPM_ActivateIdentity
 *	Test:
 *		Compare the returned credential to the one we passed in
 *
 *	Cleanup:
 *		Free memory associated with the context
 *		Print error/success message
 *
 * USAGE:	First parameter is --options
 *			-v or --version
 *		Second Parameter is the version of the test case to be run.
 *		This test case is currently only implemented for 1.1
 *
 * HISTORY
 *	Kent Yoder, kyoder@users.sf.net
 *
 * RESTRICTIONS
 *	None.
 */

 //#define UNUSED

//TEMP
#include "mydebug.h"
int currlevel = DINFO;
//TEMP

#include <limits.h>

#include "common.h"

#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/x509.h>

#include "remote_attestation.h"

#define CA_KEY_SIZE_BITS 2048

#define CERT_VERIFY_BYTE 0x5a
#define UUID_BYTE 0x5a

//#define CA_CODE
#define CHALLENGER_CODE
//#define UNUSED

#define RA_REQ_PCR 123

BYTE sizeOfSeclect[2] = "\x00\x02";
//if select PCR in PCR 0~15 BYTE sizeOfSeclect[2] = "\x00\x02";(SRTM)
//if select PCR in PCR 16~23 BYTE sizeOfSeclect[2] = "\x00\x03";(DRTM)

BYTE pcrSelect[3] = "\x00\x53\x00"; //example is PCR 0~9 12 14  FF5300
//First byte is in PCR 0~7, lowest bit is PCR 0 and the highest bit is PCR 7
//Second byte is in PCR 8~15, lowest bit is PCR 8 and the highest bit is PCR 15
//Third byte is in PCR 16~23, lowest bit is PCR 16 and the highest bit is PCR 23
//pcrSelect[] size is based on the sizeOfSeclect;

TCPA_ALGORITHM_ID symAlg = TCPA_ALG_3DES;
TSS_ALGORITHM_ID tssSymAlg = TSS_ALG_3DES;

/* substitute this for TPM_IDENTITY_CREDENTIAL in the TPM docs */
struct trousers_ca_tpm_identity_credential
{
	/* This should be set up according to the TPM 1.1b spec section
	 * 9.5.5.  For now, just use it to verify we got the right stuff
	 * back from the activate identity call. */
	X509 cert;
} ca_cred;

/* helper struct for passing stuff from function to function */
struct ca_blobs
{
	UINT32 asymBlobSize, symBlobSize;
	BYTE *asymBlob;
	BYTE *symBlob;
};

const char *fn = "Remote Attestation";

TSS_RESULT
ca_setup_key(TSS_HCONTEXT hContext, TSS_HKEY *hCAKey, RSA **rsa, int padding)
{
	TSS_RESULT result;
	UINT32 encScheme, size_n;
	BYTE n[2048];

	switch (padding) {
		case RSA_PKCS1_PADDING:
			encScheme = TSS_ES_RSAESPKCSV15;
			break;
		case RSA_PKCS1_OAEP_PADDING:
			encScheme = TSS_ES_RSAESOAEP_SHA1_MGF1;
			break;
		case RSA_NO_PADDING:
			encScheme = TSS_ES_NONE;
			break;
		default:
			return TSS_E_INTERNAL_ERROR;
			break;
	}

		//Create CA Key Object
	result = Tspi_Context_CreateObject(hContext,
					   TSS_OBJECT_TYPE_RSAKEY,
					   TSS_KEY_TYPE_LEGACY|TSS_KEY_SIZE_2048,
					   hCAKey);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_CreateObject", result);
		return result;
	}

		// generate a software key to represent the CA's key
	if ((*rsa = RSA_generate_key(CA_KEY_SIZE_BITS, 65537, NULL, NULL)) == NULL) {
		ERR_print_errors_fp(stdout);
		return result;
	}

		// get the pub CA key
	if ((size_n = BN_bn2bin((*rsa)->n, n)) <= 0) {
		fprintf(stderr, "BN_bn2bin failed\n");
		ERR_print_errors_fp(stdout);
		RSA_free(*rsa);
                return 254; // ?
        }

	result = set_public_modulus(hContext, *hCAKey, size_n, n);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_SetAttribData ", result);
		RSA_free(*rsa);
		return result;
	}

		// set the CA key's algorithm
	result = Tspi_SetAttribUint32(*hCAKey, TSS_TSPATTRIB_KEY_INFO,
				      TSS_TSPATTRIB_KEYINFO_ALGORITHM,
				      TSS_ALG_RSA);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_SetAttribUint32", result);
		RSA_free(*rsa);
		return result;
	}

		// set the CA key's number of primes
	result = Tspi_SetAttribUint32(*hCAKey, TSS_TSPATTRIB_RSAKEY_INFO,
				      TSS_TSPATTRIB_KEYINFO_RSA_PRIMES,
				      2);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_SetAttribUint32", result);
		RSA_free(*rsa);
		return result;
	}

		// set the CA key's encryption scheme
	result = Tspi_SetAttribUint32(*hCAKey, TSS_TSPATTRIB_KEY_INFO,
				      TSS_TSPATTRIB_KEYINFO_ENCSCHEME,
				      encScheme);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_SetAttribUint32", result);
		RSA_free(*rsa);
		return result;
	}

	return TSS_SUCCESS;
}

/* The identity binding is the following:
 * Sign_AIK(SHA1(label || TCPA_PUBKEY(CApub)))
 */
TSS_RESULT
ca_verify_identity_binding(TSS_HCONTEXT hContext, TSS_HTPM hTPM,
			   TSS_HKEY hCAKey, TSS_HKEY hIdentityKey,
			   TCPA_IDENTITY_PROOF *proof)
{
	TSS_RESULT result;
	BYTE blob[2048];
	UINT16 offset;
	TCPA_KEY key;
	TCPA_DIGEST digest, chosenId;
	UINT32 keyBlobSize, subCap = TSS_TPMCAP_VERSION, versionSize;
	BYTE *keyBlob;
	TSS_HHASH hHash;
	BYTE *versionBlob;

	if ((result = Tspi_Context_CreateObject(hContext,
						TSS_OBJECT_TYPE_HASH,
						TSS_HASH_SHA1, &hHash))) {
		print_error("Tspi_Context_CreateObject", result);
		return result;
	}

	if ((result = Tspi_GetAttribData(hCAKey, TSS_TSPATTRIB_KEY_BLOB,
					 TSS_TSPATTRIB_KEYBLOB_BLOB,
					 &keyBlobSize, &keyBlob))) {
		print_error("Tspi_GetAttribData", result);
		return result;
	}

	offset = 0;
	if ((result = TestSuite_UnloadBlob_KEY(&offset, keyBlob, &key))) {
		print_error("TestSuite_UnloadBlob_KEY", result);
		return result;
	}

	Tspi_Context_FreeMemory(hContext, keyBlob);

	/* make the chosen id hash */
	offset = 0;
	TestSuite_LoadBlob(&offset, proof->labelSize, blob, proof->labelArea);
	TestSuite_LoadBlob_KEY_PARMS(&offset, blob, &key.algorithmParms);
	TestSuite_LoadBlob_STORE_PUBKEY(&offset, blob, &key.pubKey);

	if ((result = TestSuite_Hash(TSS_HASH_SHA1, offset, blob,
				 (BYTE *)&chosenId.digest)))
		return result;

	if ((result = Tspi_TPM_GetCapability(hTPM, TSS_TPMCAP_VERSION,
					     0, NULL, &versionSize,
					     &versionBlob))) {
		print_error("Tspi_TPM_GetCapability", result);
		return result;
	}
#ifndef TPM_ORD_MakeIdentity
#define TPM_ORD_MakeIdentity 121
#endif

	offset = 0;
	TestSuite_LoadBlob(&offset, versionSize, blob, versionBlob);
	TestSuite_LoadBlob_UINT32(&offset, TPM_ORD_MakeIdentity, blob);
	TestSuite_LoadBlob(&offset, 20, blob, (BYTE *)&chosenId.digest);
	TestSuite_LoadBlob_PUBKEY(&offset, blob, &proof->identityKey);

	free(key.algorithmParms.parms);
	free(key.pubKey.key);
	free(key.encData);
	free(key.PCRInfo);

	if ((result = TestSuite_Hash(TSS_HASH_SHA1, offset, blob,
				 (BYTE *)&digest.digest)))
		return result;

	if ((result = Tspi_Hash_SetHashValue(hHash, 20, (BYTE *)&digest.digest))) {
		print_error("Tspi_Hash_SetHashValue", result);
		return result;
	}

	return Tspi_Hash_VerifySignature(hHash, hIdentityKey,
					 proof->identityBindingSize,
					 proof->identityBinding);
}

TSS_RESULT
ca_create_credential(TSS_HCONTEXT hContext, TSS_HTPM hTPM,
		     TSS_HKEY hIdentityKey, struct ca_blobs *b)
{
	TCPA_SYM_CA_ATTESTATION symAttestation;
	TCPA_ASYM_CA_CONTENTS asymContents;
	BYTE blob[2048], blob2[2048];
	BYTE *identityBlob;
	UINT32 identityBlobSize, blobSize = 2048, blob2Size = 2048;
	TCPA_KEY idKey;
	UINT16 offset;
	TSS_HKEY hPubEK;
	TSS_HENCDATA hEncData;
	TSS_RESULT result;
	BYTE *pubEK;
	UINT32 pubEKSize, tmpblobsize = 0x2000, i;
	BYTE tmpblob[0x2000];

	if ((result = Tspi_TPM_GetPubEndorsementKey(hTPM, TRUE, NULL,
						    &hPubEK))) {
		print_error("Tspi_TPM_GetPubEndorsementKey", result);
		return result;
	}

	if ((result = Tspi_Context_CreateObject(hContext,
						TSS_OBJECT_TYPE_ENCDATA,
						TSS_ENCDATA_BIND,
						&hEncData))) {
		print_error("Tspi_Context_CreateObject", result);
		return result;
	}

	/* fill out the asym contents structure */
	asymContents.sessionKey.algId = symAlg;
	asymContents.sessionKey.encScheme = TCPA_ES_NONE;

	switch (asymContents.sessionKey.algId) {
		case TCPA_ALG_AES:
			asymContents.sessionKey.size = 128/8;
			break;
		case TCPA_ALG_DES:
			asymContents.sessionKey.size = 64/8;
			break;
		case TCPA_ALG_3DES:
			asymContents.sessionKey.size = 192/8;
			break;
		default:
			print_error("Invalid symmetric algorithm!", -1);
			return TSS_E_INTERNAL_ERROR;
	}

	if ((result = Tspi_TPM_GetRandom(hTPM, asymContents.sessionKey.size,
					 &asymContents.sessionKey.data))) {
		print_error("Tspi_TPM_GetRandom", result);
		return result;
	}

	/* TPM 1.1b spec pg. 277 step 4 says "digest of the public key of
	 * idKey"...  I'm assuming that this is a TCPA_PUBKEY */
	if ((result = Tspi_GetAttribData(hIdentityKey,
					 TSS_TSPATTRIB_KEY_BLOB,
					 TSS_TSPATTRIB_KEYBLOB_BLOB,
					 &identityBlobSize,
					 &identityBlob))) {
		print_error("Tspi_GetAttribData", result);
		Tspi_Context_FreeMemory(hContext,
					asymContents.sessionKey.data);
		return result;
	}

	offset = 0;
	if ((result = TestSuite_UnloadBlob_KEY(&offset, identityBlob, &idKey))) {
		print_error("TestSuite_UnloadBlob_KEY", result);
		return result;
	}

	offset = 0;
	TestSuite_LoadBlob_KEY_PARMS(&offset, blob, &idKey.algorithmParms);
	TestSuite_LoadBlob_STORE_PUBKEY(&offset, blob, &idKey.pubKey);
	TestSuite_Hash(TSS_HASH_SHA1, offset, blob,
		   (BYTE *)&asymContents.idDigest.digest);

	/* create the TCPA_SYM_CA_ATTESTATION structure */
	symAttestation.algorithm.algorithmID = symAlg;
	symAttestation.algorithm.encScheme = TCPA_ES_NONE;
	symAttestation.algorithm.sigScheme = 0;
	symAttestation.algorithm.parmSize = 0;
	symAttestation.algorithm.parms = NULL;

	/* set the credential to something known before encrypting it
	 * so that we can compare the credential that the TSS returns to
	 * something other than all 0's */
	memset(&ca_cred, CERT_VERIFY_BYTE, sizeof(struct trousers_ca_tpm_identity_credential));

	/* encrypt the credential w/ the sym key */
	if ((result = TestSuite_SymEncrypt(symAlg, TCPA_ES_NONE, asymContents.sessionKey.data, NULL,
					   (BYTE *)&ca_cred,
					   sizeof(struct trousers_ca_tpm_identity_credential),
					   blob2, &blob2Size))) {
		print_error("TestSuite_SymEncrypt", result);
		Tspi_Context_FreeMemory(hContext, asymContents.sessionKey.data);
		return result;
	}

	/* blob2 now contains the encrypted credential, which is part of the TCPA_SYM_CA_ATTESTATION
	 * structure that we'll pass into ActivateIdentity. */
	symAttestation.credential = blob2;
	symAttestation.credSize = blob2Size;
	offset = 0;
	TestSuite_LoadBlob_SYM_CA_ATTESTATION(&offset, blob, &symAttestation);

	/* blob now contains the serialized TCPA_SYM_CA_ATTESTATION struct */
	if ((b->symBlob = malloc(offset)) == NULL) {
		fprintf(stderr, "malloc failed.");
		Tspi_Context_FreeMemory(hContext, asymContents.sessionKey.data);
		return TSS_E_OUTOFMEMORY;
	}

	memcpy(b->symBlob, blob, offset);
	b->symBlobSize = offset;

	/* encrypt the TCPA_ASYM_CA_CONTENTS blob with the TPM's PubEK */
	offset = 0;
	TestSuite_LoadBlob_ASYM_CA_CONTENTS(&offset, blob, &asymContents);

	/* free the malloc'd structure member now that its in the blob */
	Tspi_Context_FreeMemory(hContext, asymContents.sessionKey.data);
	if ((result = Tspi_GetAttribData(hPubEK, TSS_TSPATTRIB_RSAKEY_INFO,
					TSS_TSPATTRIB_KEYINFO_RSA_MODULUS,
					&pubEKSize, &pubEK))) {
		free(b->symBlob);
		b->symBlob = NULL;
		b->symBlobSize = 0;
		return result;
	}

	/* encrypt using the TPM's custom OAEP padding parameter */
	if ((result = TestSuite_TPM_RSA_Encrypt(blob, offset, tmpblob,
						&tmpblobsize, pubEK,
						pubEKSize))) {
		Tspi_Context_FreeMemory(hContext, pubEK);
		free(b->symBlob);
		b->symBlob = NULL;
		b->symBlobSize = 0;
		return result;
	}

	Tspi_Context_FreeMemory(hContext, pubEK);

	if ((b->asymBlob = malloc(tmpblobsize)) == NULL) {
		free(b->symBlob);
		b->symBlob = NULL;
		b->symBlobSize = 0;
		return TSS_E_OUTOFMEMORY;
	}

	memcpy(b->asymBlob, tmpblob, tmpblobsize);
	b->asymBlobSize = tmpblobsize;

	return TSS_SUCCESS;
}

//#ifdef CHALLENGER_CODE
/////////////////////////////////////////////////
// send message to dest
/////////////////////////////////////////////////
/*int SendMessage(char* serverIP, char* clientIP, int CommTYPE, int data_length, BYTE * buffer){
	printf("client %s send message to %s\n", clientIP, serverIP);
	return 0;
}
int ReceiveMessage(char* serverIP, char* clientIP, int CommTYPE, UINT16 * data_length, BYTE * buffer){
	printf("client %s send message to %s\n", clientIP, serverIP);
	return 0;
}*/

//#endif

int remote_attestation(BYTE * nonce_from_client, int sizeofclientnonce,
			BYTE **receive_msg, int * receive_msg_size,
			remote_verifier_data** remote_data, int * remote_data_size) {

	TSS_HCONTEXT		hContext;
	TSS_HTPM		hTPM;
	TSS_FLAG		initFlags;
	TSS_HKEY		hSRK, hIdentKey, hCAKey; //TPP:  hIdentKey = AIK I need??  But I need only the public key--what's this one?
	TSS_HPOLICY		hTPMPolicy, hIdPolicy;
	TSS_RESULT		result;
	RSA			*rsa = NULL;
	BYTE			*rgbIdentityLabelData = NULL, *identityReqBlob;
	BYTE			*labelString = "My Identity Label";
	UINT32			labelLen = strlen(labelString) + 1;
	UINT32			ulIdentityReqBlobLen, utilityBlobSize, credLen;
	UINT16			offset;
	TCPA_IDENTITY_REQ	identityReq;
	TCPA_IDENTITY_PROOF	identityProof;
	TCPA_SYMMETRIC_KEY	symKey;
	TCPA_ALGORITHM_ID	algID;
	TCPA_SYM_CA_ATTESTATION symAttestation;
	BYTE			utilityBlob[USHRT_MAX], *cred;
	int			padding = RSA_PKCS1_OAEP_PADDING, tmp;
	struct ca_blobs		b;
	TSS_HPCRS	hPcrComposite;
	TSS_VALIDATION validationData;
	BYTE *tmp_nonce;
	BYTE *nonce;
	UINT16 nonce_size = 20;
	BYTE *getnonce;
	UINT16 getnonce_size = 20;
	int i = 0, j = 0;
	char 	serverIP[16] = {0};
	char 	clientIP[16] = {0};
	unsigned long pcrCompositeSize = 0;
	unsigned char *pcrComposite; //TPP:  this is the pcrComposite I need?
	unsigned long signedPCRSize = 0;
	unsigned char* signedPCR; // TPP:  this is the signedPCR I need?
	BYTE		*pcrValue1;
	UINT32		pcrSize1;
	unsigned long keyPubSize;
//	BYTE *identityBlob; //no use
//	UINT32 identityBlobSize;
//	TCPA_KEY idKey;
	unsigned char keyPub[256];
	int pcrsizeOfSeclect = sizeOfSeclect[1]&0x03;
	TPM_PCRVALUE pcrValue[24];
	int nr_select_PCR = 0;// = 12;
	int selectpcrSize = 0;
	BYTE *responds_msg;
	int responds_msg_size = 0;
	BYTE  *rc, *pcr_tpm; //[29];//="\x00\x02\x00\x01\x0\x0\x0\x14\x8F\xB2\xCB\xE2\x37\x09\x2C\x0BA\xA8\xF8\x16\xB4\x7F\x06\xD6\x50\x2B\x65\x83\x76";
	unsigned long sizeof_rc = 0;

	BYTE *keyData;
	UINT32 keyDataSize;
	TCPA_KEY keyContainer;
	unsigned char tpmVersion[4];//out


	int result1 = 0;
//	unsigned char tpmVersion[4] = "\x1\x1\x0\0"; //TPP:  need this, right?
//	TSS_HKEY	hIdentKey2;
	TSS_UUID	uuid0;
	memset(&uuid0, UUID_BYTE, sizeof(TSS_UUID));


	initFlags	= TSS_KEY_TYPE_IDENTITY | TSS_KEY_SIZE_2048  |
			TSS_KEY_VOLATILE | TSS_KEY_NO_AUTHORIZATION |
			TSS_KEY_NOT_MIGRATABLE;

	MYDBGL(DINFO,"Beginning remote_attestation()\n");

	OpenSSL_add_all_algorithms();
	ERR_load_crypto_strings();

		//Create Context
	result = connect_load_all(&hContext, &hSRK, &hTPM);
	if (result != TSS_SUCCESS) {
		print_error("connect_load_all", result);
		exit(result);
	}

	//#error "it"
#ifdef UNUSED
		//Insert the owner auth into the TPM's policy
		//#error "valid"
	result = Tspi_GetPolicyObject(hTPM, TSS_POLICY_USAGE, &hTPMPolicy);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_GetPolicyObject", result);
		exit(result);
	}

	result = Tspi_Policy_SetSecret(hTPMPolicy, TESTSUITE_OWNER_SECRET_MODE,
				       TESTSUITE_OWNER_SECRET_LEN, TESTSUITE_OWNER_SECRET);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Policy_SetSecret", result);
		exit(result);
	}

		//Create Identity Key Object
	result = Tspi_Context_CreateObject(hContext,
					   TSS_OBJECT_TYPE_RSAKEY,
					   initFlags, &hIdentKey);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_CreateObject", result);
		Tspi_Context_Close(hContext);
		exit(result);
	}

		//Insert the auth into the identity key's policy
	result = Tspi_GetPolicyObject(hIdentKey, TSS_POLICY_USAGE, &hIdPolicy);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_GetPolicyObject", result);
		exit(result);
	}

	result = Tspi_Policy_SetSecret(hIdPolicy, TESTSUITE_KEY_SECRET_MODE,
				       TESTSUITE_KEY_SECRET_LEN, TESTSUITE_KEY_SECRET);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Policy_SetSecret", result);
		exit(result);
	}
#endif

#ifdef CA_CODE
/************************************************************************
 *CA code:create CA's key then send to server
 ************************************************************************/
	result = ca_setup_key(hContext, &hCAKey, &rsa, padding);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_CreateObject", result);
		Tspi_Context_Close(hContext);
		exit(result);
	}
#endif

#ifdef UNUSED
	rgbIdentityLabelData = TestSuite_Native_To_UNICODE(labelString, &labelLen);
	if (rgbIdentityLabelData == NULL) {
		fprintf(stderr, "TestSuite_Native_To_UNICODE failed\n");
		Tspi_Context_Close(hContext);
		RSA_free(rsa);
                exit(result);
	}

	result = Tspi_TPM_CollateIdentityRequest(hTPM, hSRK, hCAKey, labelLen,
						 rgbIdentityLabelData,
						 hIdentKey, tssSymAlg,
						 &ulIdentityReqBlobLen,
						 &identityReqBlob);
	if (result != TSS_SUCCESS){
		print_error("Tspi_TPM_CollateIdentityRequest", result);
		Tspi_Context_Close(hContext);
		RSA_free(rsa);
		exit(result);
	}


/************************************************************************
 *CA code:Decrypt the symmetric and asymmetric blobs
 ************************************************************************/
	/* decrypt the TCPA_IDENTITY_REQ blobs */
	offset = 0;
	if ((result = TestSuite_UnloadBlob_IDENTITY_REQ(&offset,
						    identityReqBlob,
						    &identityReq))) {
		print_error("TestSuite_UnloadBlob_IDENTITY_REQ", result);
		Tspi_Context_Close(hContext);
		RSA_free(rsa);
		exit(result);
	}

	if ((tmp = RSA_private_decrypt(identityReq.asymSize,
				       identityReq.asymBlob,
				       utilityBlob,
				       rsa, padding)) <= 0) {
		print_error("RSA_private_decrypt", result);
		Tspi_Context_Close(hContext);
		RSA_free(rsa);
		exit(result);
	}

	offset = 0;
	if ((result = TestSuite_UnloadBlob_SYMMETRIC_KEY(&offset, utilityBlob,
						     &symKey))) {
		print_error("TestSuite_UnloadBlob_SYMMETRIC_KEY", result);
		Tspi_Context_Close(hContext);
		RSA_free(rsa);
		exit(result);
	}

	switch (symKey.algId) {
		case TCPA_ALG_DES:
			algID = TSS_ALG_DES;
			break;
		case TCPA_ALG_3DES:
			algID = TSS_ALG_3DES;
			break;
		case TCPA_ALG_AES:
			algID = TSS_ALG_AES;
			break;
		default:
			fprintf(stderr, "symmetric blob encrypted with an "
				"unknown cipher\n");
			Tspi_Context_Close(hContext);
			RSA_free(rsa);
			exit(result);
			break;
	}

	utilityBlobSize = sizeof(utilityBlob);
	if ((result = TestSuite_SymDecrypt(algID, symKey.encScheme, symKey.data, NULL,
				       identityReq.symBlob, identityReq.symSize, utilityBlob,
				       &utilityBlobSize))) {
		print_error("TestSuite_SymDecrypt", result);
		Tspi_Context_Close(hContext);
		RSA_free(rsa);
		exit(result);
	}

	offset = 0;
	if ((result = TestSuite_UnloadBlob_IDENTITY_PROOF(&offset, utilityBlob,
						      &identityProof))) {
		print_error("TestSuite_UnloadBlob_IDENTITY_PROOF", result);
		Tspi_Context_Close(hContext);
		RSA_free(rsa);
		exit(result);
	}

	/* verify the identity binding */
	if ((result = ca_verify_identity_binding(hContext, hTPM, hCAKey,
						 hIdentKey, &identityProof))) {
		if (TSS_ERROR_CODE(result) == TSS_E_FAIL)
			fprintf(stderr, "Identity Binding signature doesn't "
				"match!\n");
		print_error(fn, result);
		Tspi_Context_Close(hContext);
		RSA_free(rsa);
		exit(result);
	}


	/* binding is verified, load the identity key into the TPM */
	if ((result = Tspi_Key_LoadKey(hIdentKey, hSRK))) {
		print_error("Tspi_Key_LoadKey", result);
		Tspi_Context_Close(hContext);
		RSA_free(rsa);
		exit(result);
	}

#endif

#ifdef CA_CODE
/************************************************************************
 *CA code:Create a fake credential and encrypt it.
 ************************************************************************/
	/* verify the EK's credentials in the identity proof structure */
	/* XXX Doesn't yet exist. In 1.2 they should be in NVDATA somewhere */

	if ((result = ca_create_credential(hContext, hTPM, hIdentKey, &b))) {
		print_error("ca_create_credential", result);
		Tspi_Context_Close(hContext);
		RSA_free(rsa);
		exit(result);
	}

	/* activate the TPM identity, receiving back the decrypted
	 * credential */
	result = Tspi_TPM_ActivateIdentity(hTPM, hIdentKey, b.asymBlobSize,
					   b.asymBlob, b.symBlobSize,
					   b.symBlob, &credLen, &cred);

	if (result) {
		print_error("Tspi_TPM_ActivateIdentity", result);
		Tspi_Context_Close(hContext);
		RSA_free(rsa);
		exit(result);
	}

#endif

#ifdef UNUSED
if ((result = Tspi_Context_UnregisterKey(hContext,
					 TSS_PS_TYPE_SYSTEM,
					 uuid0, &hIdentKey)) &&
	(TSS_ERROR_CODE(result) != TSS_E_PS_KEY_NOTFOUND)) {
	print_error("Tspi_Context_UnregisterKey", result);
	return result;
}

if ((result = Tspi_Context_RegisterKey(hContext, hIdentKey,
					   TSS_PS_TYPE_SYSTEM,
					   uuid0,
					   TSS_PS_TYPE_SYSTEM,
					   SRK_UUID))) {
	print_error("Tspi_Context_RegisterKey", result);
	return result;
}


/************************************************************************
 * Challenger code:Generate a nonce then send the remote attestation request to server
 ************************************************************************/
 	tmp_nonce = malloc(nonce_size * sizeof(BYTE));

	if ((result = Tspi_TPM_GetRandom(hTPM, nonce_size,
					 &tmp_nonce))) {
		print_error("Tspi_TPM_GetRandom", result);
		return result;
	}
	nonce = malloc(nonce_size * sizeof(BYTE));
	memcpy(nonce, tmp_nonce, nonce_size);
	printf("##The content of the nonce is:##\n");
	print_hex(nonce, nonce_size);
	printf("##The content of the nonce is over##\n");

	if ((result = SendMessage(serverIP, clientIP,
					RA_REQ_PCR, nonce_size, nonce))) {
		print_error("Tspi_TPM_GetRandom", result);
		return result;
	}
#endif
{
//	TSS_HKEY	hIdentKey2;

	result = Tspi_Context_LoadKeyByUUID(hContext,
						TSS_PS_TYPE_SYSTEM,
						uuid0, &hIdentKey);

	getnonce_size = (UINT16)sizeofclientnonce;
	getnonce = nonce_from_client;
	if (currlevel >= DVERBOSE) {
	  print_hex(getnonce,getnonce_size);
	}
//	memcpy(getnonce, nonce, getnonce_size);
		//Create object for the hPcrComposite Key
	result = Tspi_Context_CreateObject(hContext,
			TSS_OBJECT_TYPE_PCRS, 0,
			&hPcrComposite);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_CreateObject", result);
		Tspi_Context_CloseObject(hContext, hIdentKey);
		Tspi_Context_Close(hContext);
		exit(result);
	}

	for (i = 0; i < pcrsizeOfSeclect* 8; i++) {
		/* is PCR number i selected ? */
		if (pcrSelect[i >> 3] & (1 << (i & 7))) {

				//SelectPcrIndex
			result = Tspi_PcrComposite_SelectPcrIndex(hPcrComposite, i);
			if (result != TSS_SUCCESS) {
				print_error("Tspi_PcrComposite_SelectPcrIndex", result);
				Tspi_Context_CloseObject(hContext, hIdentKey);
				Tspi_Context_CloseObject(hContext, hPcrComposite);
				Tspi_Context_Close(hContext);
				exit(result);
			}
		}
	}

	for (i = 0, j = 0; i < pcrsizeOfSeclect* 8; i++) {
		/* is PCR number i selected ? */
		if (pcrSelect[i >> 3] & (1 << (i & 7))) {
//			j++;
			result = Tspi_TPM_PcrRead(hTPM, i, &pcrSize1, &pcrValue1);
			if (result != TSS_SUCCESS) {
				print_error("Tspi_TPM_PcrRead", result);
				Tspi_Context_Close(hContext);
				exit(result);
			}
			memcpy(&pcrValue[j++].digest, pcrValue1, sizeof(TPM_PCRVALUE));
			if (1/*currlevel >= DVERBOSE*/) {
			  MYDBGLF(DINFO,"##The content of the pcr%d Value is:##\n", i);
			  print_hex(pcrValue[j-1].digest, pcrSize1);
			  MYDBGLF(DINFO,"##The content of the pcr%d Value is over##\n", i);
			}

		}
	}

	nr_select_PCR = j;
	selectpcrSize = sizeof(TPM_PCRVALUE) * nr_select_PCR;
	MYDBGLF(DINFO,"selectpcrSize: %d  nr_select_PCR: %d\n",selectpcrSize,nr_select_PCR);

	//  TPP
	//  So is this the basic code I need for the TPM quote?  Execute this, and validationData is my result that I put into the signature?
	//  Of course I assume I will need a lot of the things earlier in the file before it will run.
	//

	validationData.ulExternalDataLength=getnonce_size;
	validationData.rgbExternalData = malloc(validationData.ulExternalDataLength);
	memcpy(validationData.rgbExternalData,getnonce, validationData.ulExternalDataLength);


		//Call TPM Quote
		printf("Tspi_TPM_Quote...\n");
		sleep(3);
	result = Tspi_TPM_Quote(hTPM, hIdentKey, hPcrComposite, &validationData);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_TPM_Quote", result);
		Tspi_Context_Close(hContext);
		exit(result);
	}

//*****************************************************************************************************************************//
//********************************************  get the publickey of AIK ******************************************************//
//*****************************************************************************************************************************//
	if ((result = Tspi_GetAttribData(hIdentKey, TSS_TSPATTRIB_KEY_BLOB,
			       TSS_TSPATTRIB_KEYBLOB_BLOB, &keyDataSize, &keyData))) {
		print_error("Tspi_GetAttribData", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_Close(hContext);
		Tspi_Context_CloseObject(hContext, hIdentKey);
		Tspi_Context_CloseObject(hContext, hPcrComposite);
		return result;
	}

    UINT64 offset64 = 0;
	memset(&keyContainer, 0, sizeof(TCPA_KEY));

	if ((result = UnloadBlob_TSS_KEY(&offset64, keyData, &keyContainer))){
		print_error("UnloadBlob_TSS_KEY", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_Close(hContext);
		Tspi_Context_CloseObject(hContext, hIdentKey);
		Tspi_Context_CloseObject(hContext, hPcrComposite);
		return result;
	}

	offset64 = 0;
	Trspi_LoadBlob_TCPA_VERSION(&offset64, tpmVersion, keyContainer.ver);


	keyPubSize = (unsigned long)keyContainer.pubKey.keyLength;
	memcpy(keyPub, keyContainer.pubKey.key, keyPubSize);

//	keyPubSize = (unsigned long)keyContainer.pubKey.keyLength;
//	keyPub = keyContainer.pubKey.key;
//	keyPub = malloc(keyPubSize);
//	memcpy(keyPub, keyContainer.pubKey.key, keyPubSize);




/*
	if ((result = Tspi_GetAttribData(hIdentKey2, TSS_TSPATTRIB_KEY_BLOB,
			       TSS_TSPATTRIB_KEYBLOB_BLOB, &identityBlobSize, &identityBlob))) {
		print_error("Tspi_GetAttribData", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_Close(hContext);
		Tspi_Context_CloseObject(hContext, hIdentKey);
		Tspi_Context_CloseObject(hContext, hPcrComposite);
		return result;
	}

	offset = 0;

	if ((result = TestSuite_UnloadBlob_KEY(&offset, identityBlob, &idKey))){
		print_error("UnloadBlob_TSS_KEY", result);
		print_error_exit(nameOfFunction, err_string(result));
		Tspi_Context_Close(hContext);
		Tspi_Context_CloseObject(hContext, hIdentKey);
		Tspi_Context_CloseObject(hContext, hPcrComposite);
		return result;
	}
	offset = 0;

//	Trspi_LoadBlob_TCPA_VERSION((UINT16*)&offset, tpmVersion, idKey.ver);
*/
	pcrCompositeSize = validationData.ulDataLength;
	pcrComposite = validationData.rgbData;
//	pcrComposite = malloc(pcrCompositeSize);
//	memcpy(pcrComposite, validationData.rgbData, pcrCompositeSize);

	signedPCRSize = validationData.ulValidationDataLength;
	signedPCR = validationData.rgbValidationData;
//	signedPCR = malloc(signedPCRSize);
//	memcpy(signedPCR, validationData.rgbValidationData, signedPCRSize);

//	keyPubSize = (unsigned long)idKey.pubKey.keyLength;
//	keyPub = idKey.pubKey.key;
//	keyPub = malloc(keyPubSize);
//	memcpy(keyPub, idKey.pubKey.key, keyPubSize);

////////////create rc/////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	MYDBGLF(DINFO,"pcr_tpmSize: %d\n",selectpcrSize);
	pcr_tpm = malloc(selectpcrSize+1024);
	MYDBGLF(DINFO,"pcr_tpmSize: %d\n",selectpcrSize);
	for (i = 0, j = 0; i < pcrsizeOfSeclect* 8; i++) {
		/* is PCR number i selected ? */
		if (pcrSelect[i >> 3] & (1 << (i & 7))) {
			j++;
			memcpy(pcr_tpm + (j-1) * sizeof(TPM_PCRVALUE), &pcrValue[i].digest,sizeof(TPM_PCRVALUE));
		}
	}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	offset = 0;
	int size = 2 + (sizeOfSeclect[1]&0x3) + 4 + 1024; //TPP:  + 1024 = arbitrary way to stop invalid writes that are being reported by valgrind
	rc = malloc(size);
	MYDBGLF(DINFO,"requested malloc size for rc = %d\n",(size)); // size = 4 ?!?
	memcpy(rc, sizeOfSeclect, 2);
	offset += 2;
	for(i=0;i<sizeOfSeclect[1]&0x3;i++){
		rc[offset++]=pcrSelect[i];
	}

	for(i=0;i<4;i++){
	  rc[offset++] = (selectpcrSize>>(8*(3-i)))&0xFF; //TPP:  valgrind:  invalid write of size 1
	}

	MYDBGLF(DVERBOSE,"offset = %d\n", offset);
	//do the paddding what Tspi_TPM_Quote do to get the final pcr digest
	for(i = 0;i<selectpcrSize;i++){
		rc[i + offset]=pcr_tpm[i]; //TPP:  valgrind:  invalid write of size 1
	}

	//the value of pcr[0] is not all 0???
	for(i = 0;i<20;i++){
		rc[i + offset]=0;
	}

	for(i=0;i<16;i++){
		MYDBGLF(DINFO,"*******%d********\n", pcrSelect[i >> 3] & (1 << (i & 7)));
	}

	MYDBGLF(DINFO,"pcr_tpmSize = %d\npcr_tpm:\n", selectpcrSize);
	print_hex(pcr_tpm,selectpcrSize);

	sizeof_rc = offset+selectpcrSize;

	*receive_msg_size = 4*sizeof(pcrCompositeSize) + pcrCompositeSize + signedPCRSize + keyPubSize + sizeof_rc;
	*remote_data_size = sizeof(remote_verifier_data);

	*receive_msg= malloc(*receive_msg_size);
	*remote_data= malloc(sizeof(remote_verifier_data));
	MYDBGLF(DINFO,"receive_msg_size is %d\n", *receive_msg_size);
//	MYDBGLF(DINFO,"tpmVersion is %s \n", tpmVersion);
//	memcpy(*receive_msg, tpmVersion, 4);

	*((unsigned long *)(*receive_msg)) = pcrCompositeSize;
	*((unsigned long *)(*receive_msg) + 1) = signedPCRSize;
	*((unsigned long *)(*receive_msg) + 2) = keyPubSize;
	*((unsigned long *)(*receive_msg) + 3) = sizeof_rc;
	(*remote_data)->pcrCompositeSize = pcrCompositeSize;
	(*remote_data)->signedPCRSize = signedPCRSize;
/* 	(*remote_data)->keyPubSize = 0; */
/* 	(*remote_data)->credLen= 0; */

	memcpy((*receive_msg) + 4*sizeof(pcrCompositeSize), pcrComposite, pcrCompositeSize);
	(*remote_data)->pcrComposite= malloc(pcrCompositeSize);
	memcpy((*remote_data)->pcrComposite, pcrComposite, pcrCompositeSize);

	memcpy((*receive_msg) + 4*sizeof(pcrCompositeSize) + pcrCompositeSize, signedPCR, signedPCRSize);
	(*remote_data)->signedPCR= malloc(signedPCRSize);
	memcpy((*remote_data)->signedPCR, signedPCR, signedPCRSize);

	memcpy((*receive_msg) + 4*sizeof(pcrCompositeSize) + pcrCompositeSize + signedPCRSize, keyPub, keyPubSize);
//	(*remote_data)->keyPub= malloc(keyPubSize);
//	memcpy((*remote_data)->keyPub, keyPub, keyPubSize);

	memcpy((*receive_msg) + 4*sizeof(pcrCompositeSize) + pcrCompositeSize + signedPCRSize + keyPubSize, rc, sizeof_rc);
//	memcpy((*receive_msg) + 4*sizeof(pcrCompositeSize) + pcrCompositeSize + signedPCRSize + keyPubSize + sizeof_rc, cred, credLen);

//	(*remote_data)->cred= malloc(credLen);
//	memcpy((*remote_data)->cred, cred, credLen);

	(*remote_data)->sizeOfSeclect = sizeOfSeclect[1];
	memcpy((*remote_data)->pcrSelect, pcrSelect, 2);
	(*remote_data)->selectpcrSize = selectpcrSize;

//	(BYTE *)((**remote_data).pcrSelect) = (*receive_msg) + 4*sizeof(pcrCompositeSize) + pcrCompositeSize + signedPCRSize + keyPubSize + 2;
//	(BYTE *)((**remote_data).selectpcrSize) = (*receive_msg) + 4*sizeof(pcrCompositeSize) + pcrCompositeSize + signedPCRSize + keyPubSize + 2 + 2;
//	(*remote_data)->pcrs_value = (*receive_msg) + 4*sizeof(pcrCompositeSize) + pcrCompositeSize + signedPCRSize + keyPubSize + 2 + 2 + 4;

}

#ifdef UNUSED
/************************************************************************
 * Challenger code: verify PCRComposite's signature
 ************************************************************************/
{
//	BYTE *receive_msg;// = responds_msg;
//	UINT16 receive_msg_size;// = 3*sizeof(pcrCompositeSize) + pcrCompositeSize + signedPCRSize + keyPubSize;
	unsigned char sha1PCRComposite[20];
	unsigned char* getpcrComposite;
	unsigned char* getsignedPCR;
	unsigned char* getkeyPub;
	unsigned long getpcrCompositeSize;
	unsigned long getsignedPCRSize;
	unsigned long getkeyPubSize;
	unsigned char* tpmversion;

	BYTE  *getrc;
	unsigned long getsizeof_rc;
	BYTE pcr_digest[20];//,*pcr_tpm;
/*
	if ((result = ReceiveMessage(serverIP, clientIP,
					RA_REQ_PCR, &receive_msg_size, receive_msg))) {
		print_error("Tspi_TPM_GetRandom", result);
		return result;
	}
*/
	MYDBGLF(DERROR,"===== credential is right! =====\n");//verify the fake credential, never write the code

//	receive_msg_size = *receive_msg_size;
//	receive_msg = malloc(receive_msg_size);
//	memcpy(receive_msg, responds_msg, receive_msg_size);

//	memcpy(tpmversion, receive_msg, 4);
//	MYDBGLF(DVERBOSE,"tpmversion is %s", tpmversion);
	getpcrCompositeSize = *((unsigned long *)(*receive_msg));
	getsignedPCRSize = *((unsigned long *)(*receive_msg) + 1);
	getkeyPubSize = *((unsigned long *)(*receive_msg) + 2);
	getsizeof_rc = *((unsigned long *)(*receive_msg) + 3);

//	getpcrComposite = malloc(getpcrCompositeSize);
//	getsignedPCR = malloc(getsignedPCRSize);
//	getkeyPub = malloc(getkeyPubSize);

//	getrc = malloc(sizeof_rc);

	getpcrComposite = (*receive_msg) + 4*sizeof(getpcrCompositeSize);
	MYDBGLF(DVERBOSE,"##The content of the getpcrComposite is:##\n");
	print_hex(getpcrComposite, getpcrCompositeSize);
	MYDBGLF(DVERBOSE,"##The content of the getpcrComposite is over##\n");

	getsignedPCR = (*receive_msg) + 4*sizeof(getpcrCompositeSize) + getpcrCompositeSize;
	MYDBGLF(DVERBOSE,"##The content of the getsignedPCR is:##\n");
	print_hex(getsignedPCR, getsignedPCRSize);
	MYDBGLF(DVERBOSE,"##The content of the signedPCR is over##\n");

	getkeyPub = (*receive_msg) + 4*sizeof(getpcrCompositeSize) + getpcrCompositeSize + getsignedPCRSize;
	MYDBGLF(DVERBOSE,"##The content of the getkeyPub is:##\n");
	print_hex(getkeyPub, getkeyPubSize);
	MYDBGLF(DVERBOSE,"##The content of the getkeyPub is over##\n");

	getrc = (*receive_msg) + 4*sizeof(getpcrCompositeSize) + getpcrCompositeSize + getsignedPCRSize + getkeyPubSize;
	MYDBGLF(DVERBOSE,"##The content of the getrc is:##\n");
	print_hex(getrc, getsizeof_rc);
	MYDBGLF(DVERBOSE,"##The content of the getrc is over##\n");

	SHA_CTX b;
	SHA1_Init(&b);
	SHA1_Update(&b, getpcrComposite, getpcrCompositeSize);
	SHA1_Final(sha1PCRComposite, &b);
	printf("getpcrComposite:\n");
	print_hex(getpcrComposite, getpcrCompositeSize);
	printf("sha1:\n");
	print_hex(sha1PCRComposite, 20);

	//	Prepare to verify PCR signature
	unsigned char exp[] = {0x01, 0x00, 0x01};			//	65537	hex

	RSA *rsa = RSA_new();
	if(rsa == NULL)
	{
		printf("===== Generate The RSA key error =====\n");
		printf("ERROR ");
		return;
	}

	//	Set the public key value in the OpenSSL object
	rsa->n = BN_bin2bn(getkeyPub, getkeyPubSize, rsa->n);
	//	Set the public exponent
	rsa->e = BN_bin2bn(exp, sizeof(exp), rsa->e);

	if((rsa->n == NULL) || (rsa->e == NULL))
	{
		printf("===== Assign RSA key error =====\n");
		printf("ERROR ");
		return;
	}

	// Verify SHA1(TCPA_QUOTE_INFO) signature
	result1 = RSA_verify(NID_sha1, sha1PCRComposite, 20, getsignedPCR, getsignedPCRSize, rsa);
	if(result1 == 0)
	{
		printf("===== Error in PCR verify =====\n");
		printf("ERROR ");
		return;
	}
	else
		printf("===== PCR signature verify OK =====\n");


/************************************************************************
 * Challenger code: verify PCRValue with Composite
 ************************************************************************/
	SHA_CTX a;
	SHA1_Init(&a);
	SHA1_Update(&a, getrc, getsizeof_rc);
	SHA1_Final(pcr_digest, &a);
	printf("getrc:\n");
	print_hex(getrc, getsizeof_rc);
	printf("pcr_digest:\n");
	print_hex(pcr_digest, 20);

	MYDBGLF(DVERBOSE,"##The content of the pcr_digest is:##\n");
	print_hex(pcr_digest, 20);
	MYDBGLF(DVERBOSE,"##The content of the pcr_digest is over##\n");

	if(!memcmp(pcr_digest, pcrComposite + 8, 20))
		printf("===== PCR digest match! =====\n");
	else 	MYDBGLF(DERROR,"===== PCR digest not match! =====\n");

//	if(!memcmp(nonce, pcrComposite + 28, nonce_size))
//		printf("===== nonce match! =====\n");
//	else 	MYDBGLF(DIMPORTANT,"===== nonce not match! =====\n");
}
#endif

 MYDBGLF(DERROR,"\t1 PASS  :  %s  returned (%d) %s\n", fn, result, err_string(result));
 //print_success(fn, result);

 MYDBGLF(DVERBOSE,"Cleaning up %s  <<<end_test>>>\n", fn);
 //print_end_test(fn);

 MYDBGL(DINFO,"remote_data_size: %d  receive_msg_size: %d\n",*remote_data_size, *receive_msg_size);
 MYDBGL(DINFO,"Ending remote_attestation()\n");
 Tspi_Context_CloseObject(hContext, hIdentKey);
 Tspi_Context_FreeMemory(hContext, NULL);
 Tspi_Context_Close(hContext);
 RSA_free(rsa);

	return result;
}

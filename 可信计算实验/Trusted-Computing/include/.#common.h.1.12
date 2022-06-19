/*
 *
 *   Copyright (C) International Business Machines  Corp., 2004, 2005
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
 *      common.h
 *
 * DESCRIPTION
 *      This file contains various defines, including those for
 *		error print statements. This file must be included
 *		for any current test case to run.
 *
 * ALGORITHM
 *      None.
 *
 * USAGE
 *      Include common.h in all test cases
 *
 * HISTORY
 *
 * RESTRICTIONS
 *      None.
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "tss/tddl_error.h"
#include "tss/tcs_error.h"
#include "tss/tspi.h"

char *err_string(TSS_RESULT);
char parseArgs(int, char **);
void print_wrongVersion();
void print_NA();
int checkNonAPI(TSS_RESULT);
void print_wrongChar();
void UINT32ToArray(UINT32 i, BYTE * out);
UNICODE * get_server(char *);
void print_hex(BYTE *, UINT32);

TSS_RESULT create_key(TSS_HCONTEXT, TSS_FLAG, TSS_HKEY, TSS_HKEY *);
TSS_RESULT create_load_key(TSS_HCONTEXT, TSS_FLAG, TSS_HKEY, TSS_HKEY *);
TSS_RESULT set_secret(TSS_HCONTEXT, TSS_HOBJECT, TSS_HPOLICY *);
TSS_RESULT connect_load_srk(TSS_HCONTEXT *, TSS_HKEY *);
TSS_RESULT connect_load_all(TSS_HCONTEXT *, TSS_HKEY *, TSS_HTPM *);
TSS_RESULT bind_and_unbind(TSS_HCONTEXT, TSS_HKEY);
TSS_RESULT sign_and_verify(TSS_HCONTEXT, TSS_HKEY);
TSS_RESULT seal_and_unseal(TSS_HCONTEXT, TSS_HKEY, TSS_HENCDATA, TSS_HPCRS);
TSS_RESULT set_public_modulus(TSS_HCONTEXT, TSS_HKEY, UINT32, BYTE *);
TSS_RESULT set_srk_readable(TSS_HCONTEXT);


void TestSuite_LoadBlob_PUBKEY(UINT16 *, BYTE *, TCPA_PUBKEY *);
void TestSuite_LoadBlob(UINT16 *, UINT32, BYTE *, BYTE *);
void TestSuite_UnloadBlob(UINT16 *, UINT32, BYTE *, BYTE *);
void TestSuite_LoadBlob_BYTE(UINT16 *, BYTE, BYTE *);
void TestSuite_UnloadBlob_BYTE(UINT16 *, BYTE *, BYTE *);
void TestSuite_LoadBlob_BOOL(UINT16 *, TSS_BOOL, BYTE *);
void TestSuite_UnloadBlob_BOOL(UINT16 *, TSS_BOOL *, BYTE *);
void TestSuite_LoadBlob_UINT32(UINT16 *, UINT32, BYTE *);
void TestSuite_LoadBlob_UINT16(UINT16 *, UINT16, BYTE *);
void TestSuite_UnloadBlob_UINT32(UINT16 *, UINT32 *, BYTE *);
void TestSuite_UnloadBlob_UINT16(UINT16 *, UINT16 *, BYTE *);
void TestSuite_LoadBlob_RSA_KEY_PARMS(UINT16 *, BYTE *, TCPA_RSA_KEY_PARMS *);
void TestSuite_LoadBlob_TSS_VERSION(UINT16 *, BYTE *, TSS_VERSION);
void TestSuite_UnloadBlob_TCPA_VERSION(UINT16 *, BYTE *, TCPA_VERSION *);
void TestSuite_LoadBlob_TCPA_VERSION(UINT16 *, BYTE *, TCPA_VERSION);
void TestSuite_LoadBlob_KEY(UINT16 *, BYTE *, TCPA_KEY *);
void TestSuite_LoadBlob_KEY_PARMS(UINT16 *, BYTE *, TCPA_KEY_PARMS *);
void TestSuite_LoadBlob_STORE_PUBKEY(UINT16 *, BYTE *, TCPA_STORE_PUBKEY *);
void TestSuite_LoadBlob_SYMMETRIC_KEY(UINT16 *, BYTE *, TCPA_SYMMETRIC_KEY *);
TSS_RESULT TestSuite_UnloadBlob_SYMMETRIC_KEY(UINT16 *, BYTE *, TCPA_SYMMETRIC_KEY *);
TSS_RESULT TestSuite_UnloadBlob_KEY_PARMS(UINT16 *, BYTE *, TCPA_KEY_PARMS *);
TSS_RESULT TestSuite_UnloadBlob_KEY(UINT16 *, BYTE *, TCPA_KEY *);
TSS_RESULT TestSuite_UnloadBlob_KEY12(UINT16 *, BYTE *, TPM_KEY12 *);
TSS_RESULT TestSuite_UnloadBlob_STORE_PUBKEY(UINT16 *, BYTE *, TCPA_STORE_PUBKEY *);
TSS_RESULT TestSuite_UnloadBlob_PUBKEY(UINT16 *, BYTE *, TCPA_PUBKEY *);
void TestSuite_UnloadBlob_VERSION(UINT16 *, BYTE *, TCPA_VERSION *);
TSS_RESULT TestSuite_UnloadBlob_IDENTITY_PROOF(UINT16 *, BYTE *, TCPA_IDENTITY_PROOF *);
void TestSuite_LoadBlob_SYM_CA_ATTESTATION(UINT16 *, BYTE *, TCPA_SYM_CA_ATTESTATION *);
TSS_RESULT TestSuite_UnloadBlob_SYM_CA_ATTESTATION(UINT16 *, BYTE *, TCPA_SYM_CA_ATTESTATION *);
void TestSuite_LoadBlob_ASYM_CA_CONTENTS(UINT16 *, BYTE *, TCPA_ASYM_CA_CONTENTS *);
TSS_RESULT TestSuite_UnloadBlob_ASYM_CA_CONTENTS(UINT16 *, BYTE *, TCPA_ASYM_CA_CONTENTS *);
void TestSuite_LoadBlob_KEY_FLAGS(UINT16 *, BYTE *, TCPA_KEY_FLAGS *);
void TestSuite_UnloadBlob_KEY_FLAGS(UINT16 *, BYTE *, TCPA_KEY_FLAGS *);
TSS_RESULT TestSuite_UnloadBlob_IDENTITY_REQ(UINT16 *, BYTE *, TCPA_IDENTITY_REQ *);
BYTE *TestSuite_Native_To_UNICODE(BYTE *, unsigned *);
BYTE *TestSuite_UNICODE_To_Native(BYTE *, unsigned *);

TSS_RESULT TestSuite_SymEncrypt(UINT16 alg, BYTE mode, BYTE *key, BYTE *iv, BYTE *in, UINT32 in_len,
				BYTE *out, UINT32 *out_len);
TSS_RESULT TestSuite_SymDecrypt(UINT16 alg, BYTE mode, BYTE *key, BYTE *iv, BYTE *in, UINT32 in_len,
				BYTE *out, UINT32 *out_len);
int TestSuite_RSA_Public_Encrypt(unsigned char *in, unsigned int inlen, unsigned char *out,
				 unsigned int *outlen, unsigned char *pubkey, unsigned int pubsize,
				 unsigned int e, int padding);
int TestSuite_TPM_RSA_Encrypt(unsigned char *in, unsigned int inlen, unsigned char *out,
			      unsigned int *outlen, unsigned char *pubkey, unsigned int pubsize);
TSS_RESULT Testsuite_Verify_Signature(TSS_HCONTEXT, TSS_HKEY, TSS_VALIDATION *);
TSS_RESULT Testsuite_Is_Ordinal_Supported(TSS_HTPM, TPM_COMMAND_CODE);

int main_v1_1();
int main_v1_2(char);

extern TSS_UUID SRK_UUID;

#define NV_LOCKED
#define CLEAR_TEST_INDEX

#define TESTSUITE_UNSUPPORTED_TSS_VERSION		0
#define TESTSUITE_TEST_TSS_1_1				1
#define TESTSUITE_TEST_TSS_1_2				2

#define TESTSUITE_KEY_SECRET_MODE	TSS_SECRET_MODE_PLAIN
#define TESTSUITE_KEY_SECRET		"KEY PWD"
#define TESTSUITE_KEY_SECRET_LEN	strlen(TESTSUITE_KEY_SECRET)

#define TESTSUITE_NEW_SECRET_MODE	TSS_SECRET_MODE_PLAIN
#define TESTSUITE_NEW_SECRET		"NEW PWD"
#define TESTSUITE_NEW_SECRET_LEN	strlen(TESTSUITE_NEW_SECRET)

#define TESTSUITE_OWNER_SECRET_MODE	TSS_SECRET_MODE_PLAIN
#define TESTSUITE_OWNER_SECRET		getenv("TESTSUITE_OWNER_SECRET")
#define TESTSUITE_OWNER_SECRET_LEN	TESTSUITE_OWNER_SECRET == NULL ? 0 : strlen(TESTSUITE_OWNER_SECRET)

#define TESTSUITE_WRONG_OWNER_SECRET		getenv("TESTSUITE_WRONG_OWNER_SECRET")
#define TESTSUITE_WRONG_OWNER_SECRET_LEN	TESTSUITE_WRONG_OWNER_SECRET == NULL ? 0 : strlen(TESTSUITE_WRONG_OWNER_SECRET)

#define TESTSUITE_OPERATOR_SECRET_MODE	TSS_SECRET_MODE_PLAIN
#define TESTSUITE_OPERATOR_SECRET	getenv("TESTSUITE_OPERATOR_SECRET")
#define TESTSUITE_OPERATOR_SECRET_LEN	TESTSUITE_OPERATOR_SECRET == NULL ? 0 : strlen(TESTSUITE_OPERATOR_SECRET)

#define TESTSUITE_ENCDATA_SECRET_MODE	TSS_SECRET_MODE_PLAIN
#define TESTSUITE_ENCDATA_SECRET	"ENC PWD"
#define TESTSUITE_ENCDATA_SECRET_LEN	strlen(TESTSUITE_ENCDATA_SECRET)

#define TESTSUITE_DELEGATE_SECRET_MODE	TSS_SECRET_MODE_PLAIN
#define TESTSUITE_DELEGATE_SECRET	"DEL PWD"
#define TESTSUITE_DELEGATE_SECRET_LEN	strlen(TESTSUITE_DELEGATE_SECRET)

/* If TESTSUITE_NOAUTH_SRK is defined, no secret will be set in the SRK's policy
 * while any of the tests are run. If it remains undefined, the secret mode,
 * secret and secret length below will be used for the SRK in the tests.
 */
#undef  TESTSUITE_NOAUTH_SRK
#define TESTSUITE_SRK_SECRET_MODE	TSS_SECRET_MODE_PLAIN
#define TESTSUITE_SRK_SECRET		getenv("TESTSUITE_SRK_SECRET")
#define TESTSUITE_SRK_SECRET_LEN	TESTSUITE_SRK_SECRET == NULL ? 0 : strlen(TESTSUITE_SRK_SECRET)


#define print_error(function, result) \
	do { \
		printf("\t0 FAIL  :  %s  returned (%d) %s\n", function, result, err_string(result)); \
		fprintf(stderr, "%s\t0 FAIL  :  %s  returned (%d) %s\n", __FILE__, function, result, err_string(result));  \
	} while (0)

#define print_success(function, result) printf("\t1 PASS  :  %s  returned (%d) %s\n", function, result, err_string(result))

#define print_error_nonapi(function, result) \
	do { \
		printf("\t0 FAIL  :  %s  returned (%d) %s\n\t\t **This is not consistent with the API\n", function, result, err_string(result)); \
		fprintf(stderr, "\t0 FAIL  :  %s  returned (%d) %s\n", function, result, err_string(result));  \
	} while (0)

#define print_begin_test(function) printf("\n<<<test_start>>>\nTesting %s\n", function);
#define print_end_test(function) printf("Cleaning up %s\n<<<end_test>>>\n", function);
#if 0
#define print_error_exit(function,errstr) printf("%s testing failed with %s\n", function, errstr);
#else
#define print_error_exit(function,errstr)
#endif
#define print_verifyerr(v,expected,result) \
	fprintf(stderr, "Verifying " v " failed. Expected: 0x%x, got 0x%x\n", \
		expected, result);
#define print_verifystr(v,expected,result) \
	fprintf(stderr, "Verifying " v " failed. Expected: %s, got %s\n", \
		expected, result);


/* use get_server as a generic UNICODE conversion routine */
#define char_to_unicode	TestSuite_Native_To_UNICODE

#define GLOBALSERVER	NULL

#define TSS_ERROR_CODE(x)	(x & 0xFFF)
#define TSS_ERROR_LAYER(x)	(x & 0x3000)

#define NULL_HOBJECT	0
#define NULL_HKEY	NULL_HOBJECT
#define NULL_HPCRS	NULL_HOBJECT
#define NULL_HHASH	NULL_HOBJECT
#define NULL_HENCDATA	NULL_HOBJECT
#define NULL_HTPM	NULL_HOBJECT
#define NULL_HCONTEXT	NULL_HOBJECT
#define NULL_HDELFAMILY	NULL_HOBJECT
#define NULL_HPOLICY	NULL_HOBJECT

#define MIN(x,y)	(x) < (y) ? (x) : (y)

#endif

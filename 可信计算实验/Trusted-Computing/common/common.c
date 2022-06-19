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
 *      common.c
 *
 * DESCRIPTION
 *      This file contains a function to translate errors into strings,
 *	for the purpose of printed error statements. It also contains
 *	a function to assign an integer to a byte array.
 *
 * ALGORITHM
 *      None.
 *
 * USAGE
 *      Include common.o in compile arguments
 *
 * HISTORY
 *
 * RESTRICTIONS
 *      None.
 */

#include <stdio.h>
#include <wchar.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <iconv.h>
#include <langinfo.h>
#include <limits.h>

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/ui.h>

#define UI_MAX_SECRET_STRING_LENGTH	256
#define UI_MAX_POPUP_STRING_LENGTH	256

#include "common.h"

TSS_UUID SRK_UUID = TSS_UUID_SRK;

struct option long_options[] = {
	{"version",	required_argument,	NULL,	'v'},
	{0,0,0,0}
};

UNICODE *
get_server(char *server)
{
	if (server == NULL)
		return NULL;
	else
		return (UNICODE *)TestSuite_Native_To_UNICODE(server, NULL);
}

void
print_NA()
{
	printf("This testcase does not apply to the TSS version being tested.\n");
	exit(126);
}

void
print_wrongVersion()
{
	fprintf( stderr,
		"At this time this version is currently unimplemented\n" );
	exit(1);
}
void
print_wrongChar()
{
	fprintf( stderr, "You entered an incorrect parameter \n" );
	exit(1);
}
void printUsage(char *argv0)
{
	fprintf( stderr, "Usage: %s [options]\n", argv0);
	fprintf( stderr, "\t-v or --version\t\tThe version of the TSS you would like to test.\n" );
}
char parseArgs(int argc, char **argv)
{
	int option_index;
	int c;
	char version;

	if (argc <3){
		printUsage(argv[0]);
		exit(1);
	}

	while ((c = getopt_long(argc, argv, "v:", long_options,
				&option_index)) != EOF){
		switch(c){
			case 'v':
				if (!strncmp("1.1", optarg, 3))
					version = TESTSUITE_TEST_TSS_1_1;
				else if (!strncmp("1.2", optarg, 3))
					version = TESTSUITE_TEST_TSS_1_2;
				else
					version = TESTSUITE_UNSUPPORTED_TSS_VERSION;
				break;
			case ':':
				//fall through
			case '?':
				//fall through
			default:
				print_wrongChar();
				printUsage(argv[0]);
				exit(0);
		}
	}
	return version;
}
int checkNonAPI(TSS_RESULT result){
	/* allow all TPM errors to fall within the API */
	if (   (TSS_ERROR_LAYER(result) == TSS_LAYER_TPM) ||
		(TSS_ERROR_CODE(result) == TSS_E_INVALID_HANDLE) ||
		(TSS_ERROR_CODE(result) == TSS_E_INTERNAL_ERROR) ||
		(TSS_ERROR_CODE(result) == TSS_E_BAD_PARAMETER) ||
		(TSS_ERROR_CODE(result) == TSS_E_KEY_NO_MIGRATION_POLICY) ||
		(TSS_ERROR_CODE(result) == TSS_E_FAIL) ||
		(TSS_ERROR_CODE(result) == TSS_E_NOTIMPL) ||
		(TSS_ERROR_CODE(result) == TSS_E_PS_KEY_NOTFOUND) ||
		(TSS_ERROR_CODE(result) == TSS_E_KEY_ALREADY_REGISTERED) ||
		(TSS_ERROR_CODE(result) == TSS_E_CANCELED) ||
		(TSS_ERROR_CODE(result) == TSS_E_TIMEOUT) ||
		(TSS_ERROR_CODE(result) == TSS_E_OUTOFMEMORY) ||
		(TSS_ERROR_CODE(result) == TSS_E_TPM_UNEXPECTED) ||
		(TSS_ERROR_CODE(result) == TSS_E_COMM_FAILURE) ||
		(TSS_ERROR_CODE(result) == TSS_E_TPM_UNSUPPORTED_FEATURE) ||
		(TSS_ERROR_CODE(result) == TSS_E_INVALID_OBJECT_TYPE) ||
		(TSS_ERROR_CODE(result) == TSS_E_INVALID_OBJECT_INITFLAG) ||
		(TSS_ERROR_CODE(result) == TSS_E_NO_CONNECTION) ||
		(TSS_ERROR_CODE(result) == TSS_E_CONNECTION_FAILED) ||
		(TSS_ERROR_CODE(result) == TSS_E_CONNECTION_BROKEN) ||
		(TSS_ERROR_CODE(result) == TSS_E_HASH_INVALID_ALG) ||
		(TSS_ERROR_CODE(result) == TSS_E_HASH_INVALID_LENGTH) ||
		(TSS_ERROR_CODE(result) == TSS_E_HASH_NO_DATA) ||
		(TSS_ERROR_CODE(result) == TSS_E_SILENT_CONTEXT) ||
		(TSS_ERROR_CODE(result) == TSS_E_INVALID_ATTRIB_FLAG) ||
		(TSS_ERROR_CODE(result) == TSS_E_INVALID_ATTRIB_SUBFLAG) ||
		(TSS_ERROR_CODE(result) == TSS_E_NO_PCRS_SET) ||
		(TSS_ERROR_CODE(result) == TSS_E_KEY_NOT_LOADED) ||
		(TSS_ERROR_CODE(result) == TSS_E_KEY_NOT_SET) ||
		(TSS_ERROR_CODE(result) == TSS_E_VALIDATION_FAILED) ||
		(TSS_ERROR_CODE(result) == TSS_E_TSP_AUTHREQUIRED) ||
		(TSS_ERROR_CODE(result) == TSS_E_TSP_AUTH2REQUIRED) ||
		(TSS_ERROR_CODE(result) == TSS_E_TSP_AUTHFAIL) ||
		(TSS_ERROR_CODE(result) == TSS_E_TSP_AUTH2FAIL) ||
		(TSS_ERROR_CODE(result) == TSS_E_KEY_NO_MIGRATION_POLICY) ||
		(TSS_ERROR_CODE(result) == TSS_E_POLICY_NO_SECRET) ||
		(TSS_ERROR_CODE(result) == TSS_E_INVALID_OBJ_ACCESS) ||
		(TSS_ERROR_CODE(result) == TSS_E_INVALID_ENCSCHEME) ||
		(TSS_ERROR_CODE(result) == TSS_E_INVALID_SIGSCHEME) ||
		(TSS_ERROR_CODE(result) == TSS_E_ENC_INVALID_LENGTH) ||
		(TSS_ERROR_CODE(result) == TSS_E_ENC_NO_DATA) ||
		(TSS_ERROR_CODE(result) == TSS_E_ENC_INVALID_TYPE) ||
		(TSS_ERROR_CODE(result) == TSS_E_INVALID_KEYUSAGE) ||
		(TSS_ERROR_CODE(result) == TSS_E_VERIFICATION_FAILED) ||
		(TSS_ERROR_CODE(result) == TSS_E_HASH_NO_IDENTIFIER) ||
		(TSS_ERROR_CODE(result) == TSS_SUCCESS))
			return 0;

	else
		return 1;
}

void
UINT32ToArray(UINT32 i, BYTE * out)
{
	out[0] = (BYTE) ((i >> 24) & 0xFF);
	out[1] = (BYTE) ((i >> 16) & 0xFF);
	out[2] = (BYTE) ((i >> 8) & 0xFF);
	out[3] = (BYTE) (i & 0xFF);
	return;
}

void
print_hex( BYTE *buf, UINT32 len )
{
	UINT32 i = 0, j;

	while (i < len) {
		for (j=0; (j < 16) && (i < len); j++, i++)
			printf("%02x ", buf[i] & 0xff);
		printf("\n");
	}
}

char *
err_string(TSS_RESULT r)
{
	/* Check the return code to see if it is common to all layers.
	 * If so, return it.
	 */
	switch (TSS_ERROR_CODE(r)) {
		case TSS_SUCCESS:			return "TSS_SUCCESS";
		default:
			break;
	}

	/* The return code is either unknown, or specific to a layer */
	if (TSS_ERROR_LAYER(r) == TSS_LAYER_TPM) {
		switch (TSS_ERROR_CODE(r)) {
			case TPM_E_AUTHFAIL:			return "TPM_E_AUTHFAIL";
			case TPM_E_BAD_PARAMETER:		return "TPM_E_BAD_PARAMETER";
			case TPM_E_BADINDEX:			return "TPM_E_BADINDEX";
			case TPM_E_AUDITFAILURE:		return "TPM_E_AUDITFAILURE";
			case TPM_E_CLEAR_DISABLED:		return "TPM_E_CLEAR_DISABLED";
			case TPM_E_DEACTIVATED:			return "TPM_E_DEACTIVATED";
			case TPM_E_DISABLED:			return "TPM_E_DISABLED";
			case TPM_E_FAIL:			return "TPM_E_FAIL";
			case TPM_E_BAD_ORDINAL:			return "TPM_E_BAD_ORDINAL";
			case TPM_E_INSTALL_DISABLED:		return "TPM_E_INSTALL_DISABLED";
			case TPM_E_INVALID_KEYHANDLE:		return "TPM_E_INVALID_KEYHANDLE";
			case TPM_E_KEYNOTFOUND:			return "TPM_E_KEYNOTFOUND";
			case TPM_E_INAPPROPRIATE_ENC:		return "TPM_E_INAPPROPRIATE_ENC";
			case TPM_E_MIGRATEFAIL:			return "TPM_E_MIGRATEFAIL";
			case TPM_E_INVALID_PCR_INFO:		return "TPM_E_INVALID_PCR_INFO";
			case TPM_E_NOSPACE:			return "TPM_E_NOSPACE";
			case TPM_E_NOSRK:			return "TPM_E_NOSRK";
			case TPM_E_NOTSEALED_BLOB:		return "TPM_E_NOTSEALED_BLOB";
			case TPM_E_OWNER_SET:			return "TPM_E_OWNER_SET";
			case TPM_E_RESOURCES:			return "TPM_E_RESOURCES";
			case TPM_E_SHORTRANDOM:			return "TPM_E_SHORTRANDOM";
			case TPM_E_SIZE:			return "TPM_E_SIZE";
			case TPM_E_WRONGPCRVAL:			return "TPM_E_WRONGPCRVAL";
			case TPM_E_BAD_PARAM_SIZE:		return "TPM_E_BAD_PARAM_SIZE";
			case TPM_E_SHA_THREAD:			return "TPM_E_SHA_THREAD";
			case TPM_E_SHA_ERROR:			return "TPM_E_SHA_ERROR";
			case TPM_E_FAILEDSELFTEST:		return "TPM_E_FAILEDSELFTEST";
			case TPM_E_AUTH2FAIL:			return "TPM_E_AUTH2FAIL";
			case TPM_E_BADTAG:			return "TPM_E_BADTAG";
			case TPM_E_IOERROR:			return "TPM_E_IOERROR";
			case TPM_E_ENCRYPT_ERROR:		return "TPM_E_ENCRYPT_ERROR";
			case TPM_E_DECRYPT_ERROR:		return "TPM_E_DECRYPT_ERROR";
			case TPM_E_INVALID_AUTHHANDLE:		return "TPM_E_INVALID_AUTHHANDLE";
			case TPM_E_NO_ENDORSEMENT:		return "TPM_E_NO_ENDORSEMENT";
			case TPM_E_INVALID_KEYUSAGE:		return "TPM_E_INVALID_KEYUSAGE";
			case TPM_E_WRONG_ENTITYTYPE:		return "TPM_E_WRONG_ENTITYTYPE";
			case TPM_E_INVALID_POSTINIT:		return "TPM_E_INVALID_POSTINIT";
			case TPM_E_INAPPROPRIATE_SIG:		return "TPM_E_INAPPROPRIATE_SIG";
			case TPM_E_BAD_KEY_PROPERTY:		return "TPM_E_BAD_KEY_PROPERTY";
			case TPM_E_BAD_MIGRATION:		return "TPM_E_BAD_MIGRATION";
			case TPM_E_BAD_SCHEME:			return "TPM_E_BAD_SCHEME";
			case TPM_E_BAD_DATASIZE:		return "TPM_E_BAD_DATASIZE";
			case TPM_E_BAD_MODE:			return "TPM_E_BAD_MODE";
			case TPM_E_BAD_PRESENCE:		return "TPM_E_BAD_PRESENCE";
			case TPM_E_BAD_VERSION:			return "TPM_E_BAD_VERSION";
			case TPM_E_NO_WRAP_TRANSPORT:		return "TPM_E_NO_WRAP_TRANSPORT";
			case TPM_E_AUDITFAIL_UNSUCCESSFUL:	return "TPM_E_AUDITFAIL_UNSUCCESSFUL";
			case TPM_E_AUDITFAIL_SUCCESSFUL:	return "TPM_E_AUDITFAIL_SUCCESSFUL";
			case TPM_E_NOTRESETABLE:		return "TPM_E_NOTRESETABLE";
			case TPM_E_NOTLOCAL:			return "TPM_E_NOTLOCAL";
			case TPM_E_BAD_TYPE:			return "TPM_E_BAD_TYPE";
			case TPM_E_INVALID_RESOURCE:		return "TPM_E_INVALID_RESOURCE";
			case TPM_E_NOTFIPS:			return "TPM_E_NOTFIPS";
			case TPM_E_INVALID_FAMILY:		return "TPM_E_INVALID_FAMILY";
			case TPM_E_NO_NV_PERMISSION:		return "TPM_E_NO_NV_PERMISSION";
			case TPM_E_REQUIRES_SIGN:		return "TPM_E_REQUIRES_SIGN";
			case TPM_E_KEY_NOTSUPPORTED:		return "TPM_E_KEY_NOTSUPPORTED";
			case TPM_E_AUTH_CONFLICT:		return "TPM_E_AUTH_CONFLICT";
			case TPM_E_AREA_LOCKED:			return "TPM_E_AREA_LOCKED";
			case TPM_E_BAD_LOCALITY:		return "TPM_E_BAD_LOCALITY";
			case TPM_E_READ_ONLY:			return "TPM_E_READ_ONLY";
			case TPM_E_PER_NOWRITE:			return "TPM_E_PER_NOWRITE";
			case TPM_E_FAMILYCOUNT:			return "TPM_E_FAMILYCOUNT";
			case TPM_E_WRITE_LOCKED:		return "TPM_E_WRITE_LOCKED";
			case TPM_E_BAD_ATTRIBUTES:		return "TPM_E_BAD_ATTRIBUTES";
			case TPM_E_INVALID_STRUCTURE:		return "TPM_E_INVALID_STRUCTURE";
			case TPM_E_KEY_OWNER_CONTROL:		return "TPM_E_KEY_OWNER_CONTROL";
			case TPM_E_BAD_COUNTER:			return "TPM_E_BAD_COUNTER";
			case TPM_E_NOT_FULLWRITE:		return "TPM_E_NOT_FULLWRITE";
			case TPM_E_CONTEXT_GAP:			return "TPM_E_CONTEXT_GAP";
			case TPM_E_MAXNVWRITES:			return "TPM_E_MAXNVWRITES";
			case TPM_E_NOOPERATOR:			return "TPM_E_NOOPERATOR";
			case TPM_E_RESOURCEMISSING:		return "TPM_E_RESOURCEMISSING";
			case TPM_E_DELEGATE_LOCK:		return "TPM_E_DELEGATE_LOCK";
			case TPM_E_DELEGATE_FAMILY:		return "TPM_E_DELEGATE_FAMILY";
			case TPM_E_DELEGATE_ADMIN:		return "TPM_E_DELEGATE_ADMIN";
			case TPM_E_TRANSPORT_NOTEXCLUSIVE:	return "TPM_E_TRANSPORT_NOTEXCLUSIVE";
			case TPM_E_OWNER_CONTROL:		return "TPM_E_OWNER_CONTROL";
			case TPM_E_DAA_RESOURCES:		return "TPM_E_DAA_RESOURCES";
			case TPM_E_DAA_INPUT_DATA0:		return "TPM_E_DAA_INPUT_DATA0";
			case TPM_E_DAA_INPUT_DATA1:		return "TPM_E_DAA_INPUT_DATA1";
			case TPM_E_DAA_ISSUER_SETTINGS:		return "TPM_E_DAA_ISSUER_SETTINGS";
			case TPM_E_DAA_TPM_SETTINGS:		return "TPM_E_DAA_TPM_SETTINGS";
			case TPM_E_DAA_STAGE:			return "TPM_E_DAA_STAGE";
			case TPM_E_DAA_ISSUER_VALIDITY:		return "TPM_E_DAA_ISSUER_VALIDITY";
			case TPM_E_DAA_WRONG_W:			return "TPM_E_DAA_WRONG_W";
			case TPM_E_BAD_HANDLE:			return "TPM_E_BAD_HANDLE";
			case TPM_E_BAD_DELEGATE:		return "TPM_E_BAD_DELEGATE";
			case TPM_E_BADCONTEXT:			return "TPM_E_BADCONTEXT";
			case TPM_E_TOOMANYCONTEXTS:		return "TPM_E_TOOMANYCONTEXTS";
			case TPM_E_MA_TICKET_SIGNATURE:		return "TPM_E_MA_TICKET_SIGNATURE";
			case TPM_E_MA_DESTINATION:		return "TPM_E_MA_DESTINATION";
			case TPM_E_MA_SOURCE:			return "TPM_E_MA_SOURCE";
			case TPM_E_MA_AUTHORITY:		return "TPM_E_MA_AUTHORITY";
			case TPM_E_PERMANENTEK:			return "TPM_E_PERMANENTEK";
			case TPM_E_BAD_SIGNATURE:		return "TPM_E_BAD_SIGNATURE";
			case TPM_E_NOCONTEXTSPACE:		return "TPM_E_NOCONTEXTSPACE";
			case TPM_E_RETRY:			return "TPM_E_RETRY";
			case TPM_E_NEEDS_SELFTEST:		return "TPM_E_NEEDS_SELFTEST";
			case TPM_E_DOING_SELFTEST:		return "TPM_E_DOING_SELFTEST";
			case TPM_E_DEFEND_LOCK_RUNNING:		return "TPM_E_DEFEND_LOCK_RUNNING";
			case TPM_E_DISABLED_CMD:		return "TPM_E_DISABLED_CMD";
			default:				return "UNKNOWN TPM ERROR";
		}
	} else if (TSS_ERROR_LAYER(r) == TSS_LAYER_TDDL) {
		switch (TSS_ERROR_CODE(r)) {
			case TSS_E_FAIL:			return "TSS_E_FAIL";
			case TSS_E_BAD_PARAMETER:		return "TSS_E_BAD_PARAMETER";
			case TSS_E_INTERNAL_ERROR:		return "TSS_E_INTERNAL_ERROR";
			case TSS_E_NOTIMPL:			return "TSS_E_NOTIMPL";
			case TSS_E_PS_KEY_NOTFOUND:		return "TSS_E_PS_KEY_NOTFOUND";
			case TSS_E_KEY_ALREADY_REGISTERED:	return "TSS_E_KEY_ALREADY_REGISTERED";
			case TSS_E_CANCELED:			return "TSS_E_CANCELED";
			case TSS_E_TIMEOUT:			return "TSS_E_TIMEOUT";
			case TSS_E_OUTOFMEMORY:			return "TSS_E_OUTOFMEMORY";
			case TSS_E_TPM_UNEXPECTED:		return "TSS_E_TPM_UNEXPECTED";
			case TSS_E_COMM_FAILURE:		return "TSS_E_COMM_FAILURE";
			case TSS_E_TPM_UNSUPPORTED_FEATURE:	return "TSS_E_TPM_UNSUPPORTED_FEATURE";
			case TDDL_E_COMPONENT_NOT_FOUND:	return "TDDL_E_COMPONENT_NOT_FOUND";
			case TDDL_E_ALREADY_OPENED:		return "TDDL_E_ALREADY_OPENED";
			case TDDL_E_BADTAG:			return "TDDL_E_BADTAG";
			case TDDL_E_INSUFFICIENT_BUFFER:	return "TDDL_E_INSUFFICIENT_BUFFER";
			case TDDL_E_COMMAND_COMPLETED:		return "TDDL_E_COMMAND_COMPLETED";
			case TDDL_E_COMMAND_ABORTED:		return "TDDL_E_COMMAND_ABORTED";
			case TDDL_E_ALREADY_CLOSED:		return "TDDL_E_ALREADY_CLOSED";
			case TDDL_E_IOERROR:			return "TDDL_E_IOERROR";
			default:				return "UNKNOWN TDDL ERROR";
		}
	} else if (TSS_ERROR_LAYER(r) == TSS_LAYER_TCS) {
		switch (TSS_ERROR_CODE(r)) {
			case TSS_E_FAIL:			return "TSS_E_FAIL";
			case TSS_E_BAD_PARAMETER:		return "TCS_E_BAD_PARAMETER";
			case TSS_E_INTERNAL_ERROR:		return "TCS_E_INTERNAL_ERROR";
			case TSS_E_NOTIMPL:			return "TCS_E_NOTIMPL";
			case TSS_E_PS_KEY_NOTFOUND:		return "TSS_E_PS_KEY_NOTFOUND";
			case TSS_E_KEY_ALREADY_REGISTERED:	return "TCS_E_KEY_ALREADY_REGISTERED";
			case TSS_E_CANCELED:			return "TSS_E_CANCELED";
			case TSS_E_TIMEOUT:			return "TSS_E_TIMEOUT";
			case TSS_E_OUTOFMEMORY:			return "TCS_E_OUTOFMEMORY";
			case TSS_E_TPM_UNEXPECTED:		return "TSS_E_TPM_UNEXPECTED";
			case TSS_E_COMM_FAILURE:		return "TSS_E_COMM_FAILURE";
			case TSS_E_TPM_UNSUPPORTED_FEATURE:	return "TSS_E_TPM_UNSUPPORTED_FEATURE";
			case TCS_E_KEY_MISMATCH:		return "TCS_E_KEY_MISMATCH";
			case TCS_E_KM_LOADFAILED:		return "TCS_E_KM_LOADFAILED";
			case TCS_E_KEY_CONTEXT_RELOAD:		return "TCS_E_KEY_CONTEXT_RELOAD";
			case TCS_E_BAD_INDEX:			return "TCS_E_BAD_INDEX";
			case TCS_E_INVALID_CONTEXTHANDLE:	return "TCS_E_INVALID_CONTEXTHANDLE";
			case TCS_E_INVALID_KEYHANDLE:		return "TCS_E_INVALID_KEYHANDLE";
			case TCS_E_INVALID_AUTHHANDLE:		return "TCS_E_INVALID_AUTHHANDLE";
			case TCS_E_INVALID_AUTHSESSION:		return "TCS_E_INVALID_AUTHSESSION";
			case TCS_E_INVALID_KEY:			return "TCS_E_INVALID_KEY";
			default:				return "UNKNOWN TCS ERROR";
		}
	} else {
		switch (TSS_ERROR_CODE(r)) {
			case TSS_E_FAIL:			return "TSS_E_FAIL";
			case TSS_E_BAD_PARAMETER:		return "TSS_E_BAD_PARAMETER";
			case TSS_E_INTERNAL_ERROR:		return "TSS_E_INTERNAL_ERROR";
			case TSS_E_NOTIMPL:			return "TSS_E_NOTIMPL";
			case TSS_E_PS_KEY_NOTFOUND:		return "TSS_E_PS_KEY_NOTFOUND";
			case TSS_E_KEY_ALREADY_REGISTERED:	return "TSS_E_KEY_ALREADY_REGISTERED";
			case TSS_E_CANCELED:			return "TSS_E_CANCELED";
			case TSS_E_TIMEOUT:			return "TSS_E_TIMEOUT";
			case TSS_E_OUTOFMEMORY:			return "TSS_E_OUTOFMEMORY";
			case TSS_E_TPM_UNEXPECTED:		return "TSS_E_TPM_UNEXPECTED";
			case TSS_E_COMM_FAILURE:		return "TSS_E_COMM_FAILURE";
			case TSS_E_TPM_UNSUPPORTED_FEATURE:	return "TSS_E_TPM_UNSUPPORTED_FEATURE";
			case TSS_E_INVALID_OBJECT_TYPE:		return "TSS_E_INVALID_OBJECT_TYPE";
			case TSS_E_INVALID_OBJECT_INITFLAG:	return "TSS_E_INVALID_OBJECT_INITFLAG";
			case TSS_E_INVALID_HANDLE:		return "TSS_E_INVALID_HANDLE";
			case TSS_E_NO_CONNECTION:		return "TSS_E_NO_CONNECTION";
			case TSS_E_CONNECTION_FAILED:		return "TSS_E_CONNECTION_FAILED";
			case TSS_E_CONNECTION_BROKEN:		return "TSS_E_CONNECTION_BROKEN";
			case TSS_E_HASH_INVALID_ALG:		return "TSS_E_HASH_INVALID_ALG";
			case TSS_E_HASH_INVALID_LENGTH:		return "TSS_E_HASH_INVALID_LENGTH";
			case TSS_E_HASH_NO_DATA:		return "TSS_E_HASH_NO_DATA";
			case TSS_E_SILENT_CONTEXT:		return "TSS_E_SILENT_CONTEXT";
			case TSS_E_INVALID_ATTRIB_FLAG:		return "TSS_E_INVALID_ATTRIB_FLAG";
			case TSS_E_INVALID_ATTRIB_SUBFLAG:	return "TSS_E_INVALID_ATTRIB_SUBFLAG";
			case TSS_E_INVALID_ATTRIB_DATA:		return "TSS_E_INVALID_ATTRIB_DATA";
			case TSS_E_NO_PCRS_SET:			return "TSS_E_NO_PCRS_SET";
			case TSS_E_KEY_NOT_LOADED:		return "TSS_E_KEY_NOT_LOADED";
			case TSS_E_KEY_NOT_SET:			return "TSS_E_KEY_NOT_SET";
			case TSS_E_VALIDATION_FAILED:		return "TSS_E_VALIDATION_FAILED";
			case TSS_E_TSP_AUTHREQUIRED:		return "TSS_E_TSP_AUTHREQUIRED";
			case TSS_E_TSP_AUTH2REQUIRED:		return "TSS_E_TSP_AUTH2REQUIRED";
			case TSS_E_TSP_AUTHFAIL:		return "TSS_E_TSP_AUTHFAIL";
			case TSS_E_TSP_AUTH2FAIL:		return "TSS_E_TSP_AUTH2FAIL";
			case TSS_E_KEY_NO_MIGRATION_POLICY:	return "TSS_E_KEY_NO_MIGRATION_POLICY";
			case TSS_E_POLICY_NO_SECRET:		return "TSS_E_POLICY_NO_SECRET";
			case TSS_E_INVALID_OBJ_ACCESS:		return "TSS_E_INVALID_OBJ_ACCESS";
			case TSS_E_INVALID_ENCSCHEME:		return "TSS_E_INVALID_ENCSCHEME";
			case TSS_E_INVALID_SIGSCHEME:		return "TSS_E_INVALID_SIGSCHEME";
			case TSS_E_ENC_INVALID_LENGTH:		return "TSS_E_ENC_INVALID_LENGTH";
			case TSS_E_ENC_NO_DATA:			return "TSS_E_ENC_NO_DATA";
			case TSS_E_ENC_INVALID_TYPE:		return "TSS_E_ENC_INVALID_TYPE";
			case TSS_E_INVALID_KEYUSAGE:		return "TSS_E_INVALID_KEYUSAGE";
			case TSS_E_VERIFICATION_FAILED:		return "TSS_E_VERIFICATION_FAILED";
			case TSS_E_HASH_NO_IDENTIFIER:		return "TSS_E_HASH_NO_IDENTIFIER";
			case TSS_E_PS_KEY_EXISTS:		return "TSS_E_PS_KEY_EXISTS";
			case TSS_E_PS_BAD_KEY_STATE:		return "TSS_E_PS_BAD_KEY_STATE";
			case TSS_E_EK_CHECKSUM:			return "TSS_E_EK_CHECKSUM";
			case TSS_E_DELEGATION_NOTSET:		return "TSS_E_DELEGATION_NOTSET";
			case TSS_E_DELFAMILY_NOTFOUND:		return "TSS_E_DELFAMILY_NOTFOUND";
			case TSS_E_DELFAMILY_ROWEXISTS:		return "TSS_E_DELFAMILY_ROWEXISTS";
			case TSS_E_VERSION_MISMATCH:		return "TSS_E_VERSION_MISMATCH";
			case TSS_E_DAA_AR_DECRYPTION_ERROR:	return "TSS_E_DAA_AR_DECRYPTION_ERROR";
			case TSS_E_DAA_AUTHENTICATION_ERROR:	return "TSS_E_DAA_AUTHENTICATION_ERROR";
			case TSS_E_DAA_CHALLENGE_RESPONSE_ERROR:return "TSS_E_DAA_CHALLENGE_RESPONSE_ERROR";
			case TSS_E_DAA_CREDENTIAL_PROOF_ERROR:	return "TSS_E_DAA_CREDENTIAL_PROOF_ERROR";
			case TSS_E_DAA_CREDENTIAL_REQUEST_PROOF_ERROR:return "TSS_E_DAA_CREDENTIAL_REQUEST_PROOF_ERROR";
			case TSS_E_DAA_ISSUER_KEY_ERROR:	return "TSS_E_DAA_ISSUER_KEY_ERROR";
			case TSS_E_DAA_PSEUDONYM_ERROR:		return "TSS_E_DAA_PSEUDONYM_ERROR";
			case TSS_E_INVALID_RESOURCE:		return "TSS_E_INVALID_RESOURCE";
			case TSS_E_NV_AREA_EXIST:		return "TSS_E_NV_AREA_EXIST";
			case TSS_E_NV_AREA_NOT_EXIST:		return "TSS_E_NV_AREA_NOT_EXIST";
			case TSS_E_TSP_TRANS_AUTHFAIL:		return "TSS_E_TSP_TRANS_AUTHFAIL";
			case TSS_E_TSP_TRANS_AUTHREQUIRED:	return "TSS_E_TSP_TRANS_AUTHREQUIRED";
			case TSS_E_TSP_TRANS_NOTEXCLUSIVE:	return "TSS_E_TSP_TRANS_NOTEXCLUSIVE";
			case TSS_E_NO_ACTIVE_COUNTER:		return "TSS_E_NO_ACTIVE_COUNTER";
			case TSS_E_TSP_TRANS_NO_PUBKEY:		return "TSS_E_TSP_TRANS_NO_PUBKEY";
			case TSS_E_TSP_TRANS_FAIL:		return "TSS_E_TSP_TRANS_FAIL";
			default:				return "UNKNOWN TSS ERROR";
		}
	}
}

/* functions provided to ease testcase writing */

/* create a key off the SRK */
TSS_RESULT
create_key(TSS_HCONTEXT hContext, TSS_FLAG initFlags, TSS_HKEY hSRK, TSS_HKEY *hKey)
{
	TSS_RESULT result;
	TSS_HPOLICY hPolicy;

		//Create Object
	result = Tspi_Context_CreateObject(hContext, TSS_OBJECT_TYPE_RSAKEY, initFlags, hKey);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_CreateObject", result);
		return(result);
	}

	if (initFlags & TSS_KEY_AUTHORIZATION) {
		if ((result = set_secret(hContext, *hKey, &hPolicy)))
			return result;
	}

		//CreateKey
	result = Tspi_Key_CreateKey(*hKey, hSRK, 0);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Key_CreateKey", result);
		return(result);
	}

	return TSS_SUCCESS;
}

/* create and load a key off the SRK */
TSS_RESULT
create_load_key(TSS_HCONTEXT hContext, TSS_FLAG initFlags,
		TSS_HKEY hSRK, TSS_HKEY *hKey)
{
	TSS_RESULT result;

	if ((result = create_key(hContext, initFlags, hSRK, hKey)))
		return result;

	result = Tspi_Key_LoadKey(*hKey, hSRK);
        if (result != TSS_SUCCESS) {
                print_error("Tspi_Key_LoadKey", result);
                return(result);
        }

	return TSS_SUCCESS;
}

/* set the secret for an object to a 0 length string */
TSS_RESULT
set_secret(TSS_HCONTEXT hContext, TSS_HOBJECT hObj, TSS_HPOLICY *hPolicy)
{
	TSS_RESULT result;
	TSS_HPOLICY hLocalPolicy;
	UINT32 secret_mode, secret_len;
	BYTE *secret;

	if (hPolicy == NULL) {
		secret_len = TESTSUITE_SRK_SECRET_LEN;
		secret_mode = TESTSUITE_SRK_SECRET_MODE;
		secret = TESTSUITE_SRK_SECRET;
	} else {
		secret_len = TESTSUITE_KEY_SECRET_LEN;
		secret_mode = TESTSUITE_KEY_SECRET_MODE;
		secret = TESTSUITE_KEY_SECRET;
	}

	result = Tspi_Context_CreateObject(hContext, TSS_OBJECT_TYPE_POLICY, TSS_POLICY_USAGE,
					   &hLocalPolicy);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_CreateObject", result);
		return(result);
	}

	result = Tspi_Policy_SetSecret(hLocalPolicy, secret_mode, secret_len, secret);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Policy_SetSecret", result);
		return(result);
	}

	result = Tspi_Policy_AssignToObject(hLocalPolicy, hObj);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Policy_AssignToObject", result);
		return(result);
	}

	if (hPolicy)
		*hPolicy = hLocalPolicy;

	return TSS_SUCCESS;
}

// by wfw 2011-06-21
TSS_RESULT 
do_ui(BYTE *string, UINT32 *string_len, const char *popup, int verify)
{
	char pin_buf[UI_MAX_SECRET_STRING_LENGTH];
	char verify_buf[UI_MAX_SECRET_STRING_LENGTH];
	char *popup_nl;
	UI *ui;
	TSS_RESULT ret = TSS_E_FAIL;

	popup_nl = malloc(strlen((char *)popup) + 2);
	if (!popup_nl)
		return TSS_E_OUTOFMEMORY;

	ui = UI_new();
	if (!ui)
		goto no_ui;

	sprintf(popup_nl, "%s\n", (char *)popup);
	if (!UI_add_info_string(ui, popup_nl)) {
		printf("add info fail\n");
		goto out;
	}

	if (!UI_add_input_string(ui, "Enter PIN:", 0, pin_buf, 1, UI_MAX_SECRET_STRING_LENGTH)) {
		printf("add input fail\n");
		goto out;
	}

	if (verify &&
	    !UI_add_verify_string(ui, "Verify PIN:", 0, verify_buf, 1, UI_MAX_SECRET_STRING_LENGTH, pin_buf)) {
		printf("Add verify fail\n");
		goto out;
	}

	if (UI_process(ui))
		goto out;

	ret = TSS_SUCCESS;

	memset(string, 0, UI_MAX_SECRET_STRING_LENGTH);
	strcpy(string, pin_buf);
	*string_len = strlen(pin_buf);
 out:
	UI_free(ui);
 no_ui:
	free(popup_nl);
	return ret;
}

// by wfw 2011-06-18
TSS_RESULT
set_popup_secret(TSS_HCONTEXT hContext, 
		TSS_HOBJECT hObj, 
		TSS_FLAG initFlag, 
		const char *pPrompt, 
		int verify)
{
	TSS_RESULT result;
	TSS_HPOLICY hPolicy;
	char pin[UI_MAX_SECRET_STRING_LENGTH + 1];
	UINT32 u32PinLen = UI_MAX_SECRET_STRING_LENGTH;

	result = Tspi_GetPolicyObject(hObj, initFlag, &hPolicy);
	result = -1;	// when use old policy, there is some problems, so set result to -1
	if (TSS_SUCCESS != result) {
		//printf("create new policy\n");
		result = Tspi_Context_CreateObject(hContext, 
							TSS_OBJECT_TYPE_POLICY, 
							initFlag, 
							&hPolicy);
		if (TSS_SUCCESS != result) {
			print_error("Tspi_Context_CreateObject", result);
			return result;
		}

		result = do_ui(pin, &u32PinLen, pPrompt, verify);
		if (TSS_SUCCESS != result) {
			print_error("do_ui", result);
			return result;
		}

		result = Tspi_Policy_SetSecret(hPolicy, 
							TSS_SECRET_MODE_PLAIN, 
							u32PinLen, 
							pin);
		if (TSS_SUCCESS != result) {
			print_error("Tspi_Policy_SetSecret", result);
			return result;
		}

		result = Tspi_Policy_AssignToObject(hPolicy, hObj);
		if (TSS_SUCCESS != result) {
			print_error("Tspi_Policy_AssignToObject", result);
			return result;
		}
	} else {
		//printf("use old policy\n");
		result = do_ui(pin, &u32PinLen, pPrompt, verify);
		if (TSS_SUCCESS != result) {
			print_error("do_ui", result);
			return result;
		}

		result = Tspi_Policy_SetSecret(hPolicy, 
						TSS_SECRET_MODE_PLAIN, 
						u32PinLen, 
						pin);
		if (TSS_SUCCESS != result) {
			print_error("Tspi_Policy_SetSecret", result);
			return result;
		}
	}

	return result;

}

/* connect, load the SRK */
TSS_RESULT
connect_load_srk(TSS_HCONTEXT *hContext, TSS_HKEY *hSRK)
{
	TSS_RESULT result;

		// Create Context
	result = Tspi_Context_Create( hContext );
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_Context_Create", result );
		return( result );
	}

		// Connect to Context
	result = Tspi_Context_Connect(*hContext, get_server(GLOBALSERVER));
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_Context_Connect", result );
		Tspi_Context_FreeMemory( *hContext, NULL );
		Tspi_Context_Close( *hContext );
		return( result );
	}

		//Load Key by UUID for SRK
	result = Tspi_Context_LoadKeyByUUID(*hContext, TSS_PS_TYPE_SYSTEM,
			SRK_UUID, hSRK);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_LoadKeyByUUID", result);
		Tspi_Context_Close(*hContext);
		return(result);
	}

	if ((result = set_secret(*hContext, *hSRK, NULL))) {
		Tspi_Context_Close(*hContext);
		return result;
	}

	return TSS_SUCCESS;
}

/* connect, load the SRK and get the TPM handle */
TSS_RESULT
connect_load_all(TSS_HCONTEXT *hContext, TSS_HKEY *hSRK, TSS_HTPM *hTPM)
{
	TSS_RESULT result;

	if ((result = connect_load_srk(hContext, hSRK)))
		return result;

		// Retrieve TPM object of context
	result = Tspi_Context_GetTpmObject( *hContext, hTPM );
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_Context_GetTpmObject", result );
		Tspi_Context_FreeMemory( *hContext, NULL );
		Tspi_Context_Close( *hContext );
		return( result );
	}

	return TSS_SUCCESS;
}

TSS_RESULT
bind_and_unbind(TSS_HCONTEXT hContext, TSS_HKEY hKey)
{

	TSS_RESULT result;
	TSS_HENCDATA hEncData;
	BYTE rgbDataToBind[] = "932brh3270yrnc7y0nrj28c89cjrmj4398jng4399mch8";
	UINT32 ulDataLength = sizeof(rgbDataToBind);
	BYTE *rgbEncryptedData, *prgbDataToUnBind;
	UINT32 ulEncryptedDataLength, pulDataLength;

	result = Tspi_Context_CreateObject( hContext,
					    TSS_OBJECT_TYPE_ENCDATA,
					    TSS_ENCDATA_BIND, &hEncData );
	if ( result != TSS_SUCCESS )
	{
		print_error("Tspi_Context_CreateObject ", result);
		return result;
	}

	printf("Data before binding:\n");
	print_hex(rgbDataToBind, ulDataLength);

	result = Tspi_Data_Bind( hEncData, hKey, ulDataLength, rgbDataToBind );
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_Data_Bind", result );
		return result;
	}

	result = Tspi_GetAttribData(hEncData, TSS_TSPATTRIB_ENCDATA_BLOB,
			TSS_TSPATTRIB_ENCDATABLOB_BLOB,
			&ulEncryptedDataLength, &rgbEncryptedData);
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_GetAttribData", result );
		return result;
	}

	printf("Data after binding:\n");
	print_hex(rgbEncryptedData, ulEncryptedDataLength);

	result = Tspi_Data_Unbind( hEncData, hKey, &pulDataLength,
			&prgbDataToUnBind );
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_Data_Unbind", result );
		if( !(checkNonAPI(result)) )
		{
			print_error( "Tspi_Data_Unbind", result );
		}
		else
		{
			print_error_nonapi( "Tspi_Data_Unbind", result );
		}
	}
	else
	{
		printf("Data after unbinding:\n");
		print_hex(prgbDataToUnBind, pulDataLength);

		if (pulDataLength != ulDataLength) {
			printf("ERROR: Size of decrypted data does not match!"
			       " (%u != %u)\n", pulDataLength, ulDataLength);
			result = TSS_E_FAIL;
		} else if (memcmp(prgbDataToUnBind, rgbDataToBind, ulDataLength)) {
			printf("ERROR: Content of decrypted data does not match!\n");
			result = TSS_E_FAIL;
		} else {
			result = TSS_SUCCESS;
		}
	}

	return result;
}

TSS_RESULT
sign_and_verify(TSS_HCONTEXT hContext, TSS_HKEY hKey)
{
	TSS_RESULT result;
	TSS_HHASH hHash;
	BYTE rgbDataToSign[] = "09876543210987654321";
	UINT32 ulDataLength = 20; // with work w/ either SS
	BYTE *rgbSignedData, *rgbVerifiedData;
	UINT32 ulSignedDataLength, ulVerifiedDataLength;

	result = Tspi_Context_CreateObject( hContext,
					    TSS_OBJECT_TYPE_HASH,
					    TSS_HASH_SHA1, &hHash );
	if ( result != TSS_SUCCESS )
	{
		print_error("Tspi_Context_CreateObject ", result);
		return result;
	}

	printf("Data before signing:\n");
	print_hex(rgbDataToSign, ulDataLength);

	result = Tspi_Hash_SetHashValue(hHash, ulDataLength, rgbDataToSign);
	if (result != TSS_SUCCESS)
	{
		print_error( "Tspi_Hash_SetHashValue", result );
		return result;
	}

	result = Tspi_Hash_Sign( hHash, hKey, &ulSignedDataLength, &rgbSignedData );
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_Hash_Sign", result );
		return result;
	}

	printf("Data after signing:\n");
	print_hex(rgbSignedData, ulSignedDataLength);

	result = Tspi_Hash_VerifySignature(hHash, hKey, ulSignedDataLength,
					   rgbSignedData );
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_Hash_VerifySignature", result );
		if( !(checkNonAPI(result)) )
		{
			print_error( "Tspi_Hash_VerifySignature", result );
		}
		else
		{
			print_error_nonapi("Tspi_Hash_VerifySignature",
					   result);
		}
	}
	else
	{
		if ((result = Tspi_Hash_GetHashValue(hHash,
						     &ulVerifiedDataLength,
						     &rgbVerifiedData))) {
			print_error( "Tspi_Hash_GetHashValue", result );
			return result;
		}

		printf("Data after verifying:\n");
		print_hex(rgbVerifiedData, ulVerifiedDataLength);

		if (ulVerifiedDataLength != ulDataLength) {
			printf("ERROR: Size of verified data does not match!"
			       " (%u != %u)\n", ulDataLength, ulVerifiedDataLength);
			result = TSS_E_FAIL;
		} else if (memcmp(rgbVerifiedData, rgbDataToSign, ulDataLength)) {
			printf("ERROR: Content of decrypted data does not match!\n");
			result = TSS_E_FAIL;
		} else {
			result = TSS_SUCCESS;
		}
	}

	return result;
}

TSS_RESULT
seal_and_unseal(TSS_HCONTEXT hContext, TSS_HKEY hKey, TSS_HENCDATA hEncData,
		TSS_HPCRS hPcrs)
{

	TSS_RESULT result;
	BYTE rgbDataToSeal[] = "932brh3270yrnc7y0nrj28c89cjrmj4398jng4399mch8";
	UINT32 ulDataLength = sizeof(rgbDataToSeal);
	BYTE *rgbEncryptedData, *prgbDataToUnseal;
	UINT32 ulEncryptedDataLength, pulDataLength;

	printf("Data before sealing:\n");
	print_hex(rgbDataToSeal, ulDataLength);

	result = Tspi_Data_Seal(hEncData, hKey, ulDataLength, rgbDataToSeal,
				hPcrs);
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_Data_Seal", result );
		return result;
	}

	result = Tspi_GetAttribData(hEncData, TSS_TSPATTRIB_ENCDATA_BLOB,
				    TSS_TSPATTRIB_ENCDATABLOB_BLOB,
				    &ulEncryptedDataLength, &rgbEncryptedData);
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_GetAttribData", result );
		return result;
	}

	printf("Data after sealing:\n");
	print_hex(rgbEncryptedData, ulEncryptedDataLength);
	Tspi_Context_FreeMemory(hContext, rgbEncryptedData);

	result = Tspi_Data_Unseal(hEncData, hKey, &pulDataLength,
				  &prgbDataToUnseal );
	if ( result != TSS_SUCCESS )
	{
		print_error( "Tspi_Data_Unseal", result );
		if( !(checkNonAPI(result)) )
		{
			print_error( "Tspi_Data_Unseal", result );
		}
		else
		{
			print_error_nonapi( "Tspi_Data_Unseal", result );
		}
	}
	else
	{
		printf("Data after unsealing:\n");
		print_hex(prgbDataToUnseal, pulDataLength);

		if (pulDataLength != ulDataLength) {
			printf("ERROR: Size of decrypted data does not match!"
			       " (%u != %u)\n", pulDataLength, ulDataLength);
			result = TSS_E_FAIL;
		} else if (memcmp(prgbDataToUnseal, rgbDataToSeal, ulDataLength)) {
			printf("ERROR: Content of decrypted data does not match!\n");
			result = TSS_E_FAIL;
		} else {
			result = TSS_SUCCESS;
		}
	}

	Tspi_Context_FreeMemory(hContext, prgbDataToUnseal);

	return result;
}

TSS_RESULT
set_public_modulus(TSS_HCONTEXT hContext, TSS_HKEY hKey, UINT32 size_n, BYTE *n)
{
	UINT16 offset;
	UINT32 blob_size;
	BYTE *blob, pub_blob[1024];
	TCPA_PUBKEY pub_key;
	TSS_RESULT result;

	/* Get the TCPA_PUBKEY blob from the key object. */
	result = Tspi_GetAttribData(hKey, TSS_TSPATTRIB_KEY_BLOB, TSS_TSPATTRIB_KEYBLOB_PUBLIC_KEY,
				    &blob_size, &blob);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_GetAttribData", result);
		return result;
	}

	offset = 0;
	result = TestSuite_UnloadBlob_PUBKEY(&offset, blob, &pub_key);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_GetAttribData", result);
		return result;
	}

	Tspi_Context_FreeMemory(hContext, blob);
	/* Free the first dangling reference, putting 'n' in its place */
	free(pub_key.pubKey.key);
	pub_key.pubKey.keyLength = size_n;
	pub_key.pubKey.key = n;

	offset = 0;
	TestSuite_LoadBlob_PUBKEY(&offset, pub_blob, &pub_key);

	/* Free the second dangling reference */
	free(pub_key.algorithmParms.parms);

	/* set the public key data in the TSS object */
	result = Tspi_SetAttribData(hKey, TSS_TSPATTRIB_KEY_BLOB, TSS_TSPATTRIB_KEYBLOB_PUBLIC_KEY,
				    (UINT32)offset, pub_blob);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_SetAttribData", result);
		return result;
	}

	return TSS_SUCCESS;
}

TSS_RESULT
set_srk_readable(TSS_HCONTEXT hContext)
{
	TSS_HTPM hTPM;
	TSS_HPOLICY hPolicy;
	TSS_RESULT result;

	result = Tspi_Context_GetTpmObject( hContext, &hTPM );
	if ( result != TSS_SUCCESS ) {
		print_error( "Tspi_Context_GetTpmObject", result );
		return result;
	}

	result = Tspi_GetPolicyObject( hTPM, TSS_POLICY_USAGE, &hPolicy );
	if ( result != TSS_SUCCESS ) {
		print_error( "Tspi_GetPolicyObject", result );
		return result;
	}

	result = Tspi_Policy_SetSecret( hPolicy, TESTSUITE_OWNER_SECRET_MODE,
					TESTSUITE_OWNER_SECRET_LEN, TESTSUITE_OWNER_SECRET);
	if ( result != TSS_SUCCESS ) {
		print_error( "Tspi_Policy_SetSecret", result );
		return result;
	}

	return Tspi_TPM_SetStatus( hTPM, TSS_TPMSTATUS_DISABLEPUBSRKREAD, FALSE );
}

void
TestSuite_LoadBlob_KEY_FLAGS(UINT16 * offset, BYTE * blob, TCPA_KEY_FLAGS * flags)
{
        UINT32 tempFlag = 0;

        if (*flags & migratable)
                tempFlag |= migratable;
        if (*flags & redirection)
                tempFlag |= redirection;
        if (*flags & volatileKey)
                tempFlag |= volatileKey;
        TestSuite_LoadBlob_UINT32(offset, tempFlag, blob);
}

void
TestSuite_UnloadBlob_KEY_FLAGS(UINT16 * offset, BYTE * blob, TCPA_KEY_FLAGS * flags)
{
        UINT32 tempFlag = 0;
        memset(flags, 0x00, sizeof(TCPA_KEY_FLAGS));

        TestSuite_UnloadBlob_UINT32(offset, &tempFlag, blob);

        if (tempFlag & redirection)
                *flags |= redirection;
        if (tempFlag & migratable)
                *flags |= migratable;
        if (tempFlag & volatileKey)
                *flags |= volatileKey;
}

TSS_RESULT
TestSuite_UnloadBlob_SYMMETRIC_KEY(UINT16 *offset, BYTE *blob, TCPA_SYMMETRIC_KEY *key)
{
	TestSuite_UnloadBlob_UINT32(offset, &key->algId, blob);
	TestSuite_UnloadBlob_UINT16(offset, &key->encScheme, blob);
	TestSuite_UnloadBlob_UINT16(offset, &key->size, blob);

	if (key->size > 0) {
		key->data = malloc(key->size);
		if (key->data == NULL) {
			key->size = 0;
			return TSS_E_OUTOFMEMORY;
		}
		TestSuite_UnloadBlob(offset, key->size, blob, key->data);
	} else {
		key->data = NULL;
	}

	return TSS_SUCCESS;
}


void
TestSuite_LoadBlob_PUBKEY(UINT16 *offset, BYTE *blob, TCPA_PUBKEY *pubKey)
{
	TestSuite_LoadBlob_KEY_PARMS(offset, blob, &pubKey->algorithmParms);
	TestSuite_LoadBlob_STORE_PUBKEY(offset, blob, &pubKey->pubKey);
}

TSS_RESULT
TestSuite_UnloadBlob_PUBKEY(UINT16 * offset, BYTE * blob, TCPA_PUBKEY * pubKey)
{
	TSS_RESULT result;

	if ((result = TestSuite_UnloadBlob_KEY_PARMS(offset, blob, &pubKey->algorithmParms)))
		return result;
	if ((result = TestSuite_UnloadBlob_STORE_PUBKEY(offset, blob, &pubKey->pubKey))) {
		free(pubKey->pubKey.key);
		free(pubKey->algorithmParms.parms);
		pubKey->pubKey.key = NULL;
		pubKey->pubKey.keyLength = 0;
		pubKey->algorithmParms.parms = NULL;
		pubKey->algorithmParms.parmSize = 0;
		return result;
	}

	return TSS_SUCCESS;
}

void
TestSuite_LoadBlob(UINT16 * offset, UINT32 size, BYTE * container, BYTE * object)
{
	if (size == 0)
		return;
	memcpy(&container[(*offset)], object, size);
	(*offset) += (UINT16) size;
}

void
TestSuite_UnloadBlob(UINT16 * offset, UINT32 size, BYTE * container, BYTE * object)
{
	if (size == 0)
		return;
	memcpy(object, &container[(*offset)], size);
	(*offset) += (UINT16) size;
}

void
TestSuite_LoadBlob_BYTE(UINT16 * offset, BYTE data, BYTE * blob)
{
	blob[*offset] = data;
	(*offset)++;
}

void
TestSuite_UnloadBlob_BYTE(UINT16 * offset, BYTE * dataOut, BYTE * blob)
{
	*dataOut = blob[*offset];
	(*offset)++;
}

void
TestSuite_LoadBlob_BOOL(UINT16 * offset, TSS_BOOL data, BYTE * blob)
{
	blob[*offset] = (BYTE) data;
	(*offset)++;
}

void
TestSuite_UnloadBlob_BOOL(UINT16 * offset, TSS_BOOL * dataOut, BYTE * blob)
{
	*dataOut = blob[*offset];
	(*offset)++;
}

void
TestSuite_LoadBlob_UINT32(UINT16 * offset, UINT32 in, BYTE * blob)
{
	UINT32ToArray(in, &blob[*offset]);
	*offset += 4;
}

void
TestSuite_LoadBlob_UINT16(UINT16 * offset, UINT16 in, BYTE * blob)
{
	UINT16ToArray(in, &blob[*offset]);
	*offset += sizeof(UINT16);
}

void
TestSuite_UnloadBlob_UINT32(UINT16 * offset, UINT32 * out, BYTE * blob)
{
	*out = Decode_UINT32(&blob[*offset]);
	*offset += sizeof(UINT32);
}

void
TestSuite_UnloadBlob_UINT16(UINT16 * offset, UINT16 * out, BYTE * blob)
{
	*out = Decode_UINT16(&blob[*offset]);
	*offset += sizeof(UINT16);
}

void
TestSuite_LoadBlob_RSA_KEY_PARMS(UINT16 * offset, BYTE * blob, TCPA_RSA_KEY_PARMS * parms)
{
	TestSuite_LoadBlob_UINT32(offset, parms->keyLength, blob);
	TestSuite_LoadBlob_UINT32(offset, parms->numPrimes, blob);
	TestSuite_LoadBlob_UINT32(offset, parms->exponentSize, blob);

	if (parms->exponentSize > 0)
		TestSuite_LoadBlob(offset, parms->exponentSize, blob, parms->exponent);
}

void
TestSuite_LoadBlob_TSS_VERSION(UINT16 * offset, BYTE * blob, TSS_VERSION version)
{
	blob[(*offset)++] = version.bMajor;
	blob[(*offset)++] = version.bMinor;
	blob[(*offset)++] = version.bRevMajor;
	blob[(*offset)++] = version.bRevMinor;
}

void
TestSuite_UnloadBlob_TCPA_VERSION(UINT16 * offset, BYTE * blob, TCPA_VERSION * out)
{
	out->major = blob[(*offset)++];
	out->minor = blob[(*offset)++];
	out->revMajor = blob[(*offset)++];
	out->revMinor = blob[(*offset)++];
}

void
TestSuite_LoadBlob_TCPA_VERSION(UINT16 * offset, BYTE * blob, TCPA_VERSION version)
{
	blob[(*offset)++] = version.major;
	blob[(*offset)++] = version.minor;
	blob[(*offset)++] = version.revMajor;
	blob[(*offset)++] = version.revMinor;
}

void
TestSuite_LoadBlob_KEY(UINT16 * offset, BYTE * blob, TCPA_KEY * key)
{
	TestSuite_LoadBlob_TCPA_VERSION(offset, blob, key->ver);
	TestSuite_LoadBlob_UINT16(offset, key->keyUsage, blob);
	TestSuite_LoadBlob_KEY_FLAGS(offset, blob, &key->keyFlags);
	blob[(*offset)++] = key->authDataUsage;
	TestSuite_LoadBlob_KEY_PARMS(offset, blob, &key->algorithmParms);
	TestSuite_LoadBlob_UINT32(offset, key->PCRInfoSize, blob);
	TestSuite_LoadBlob(offset, key->PCRInfoSize, blob, key->PCRInfo);
	TestSuite_LoadBlob_STORE_PUBKEY(offset, blob, &key->pubKey);
	TestSuite_LoadBlob_UINT32(offset, key->encSize, blob);
	TestSuite_LoadBlob(offset, key->encSize, blob, key->encData);
}

void
TestSuite_LoadBlob_KEY_PARMS(UINT16 * offset, BYTE * blob, TCPA_KEY_PARMS * keyInfo)
{
	TestSuite_LoadBlob_UINT32(offset, keyInfo->algorithmID, blob);
	TestSuite_LoadBlob_UINT16(offset, keyInfo->encScheme, blob);
	TestSuite_LoadBlob_UINT16(offset, keyInfo->sigScheme, blob);
	TestSuite_LoadBlob_UINT32(offset, keyInfo->parmSize, blob);

	if (keyInfo->parmSize > 0)
		TestSuite_LoadBlob(offset, keyInfo->parmSize, blob, keyInfo->parms);
}

void
TestSuite_LoadBlob_STORE_PUBKEY(UINT16 * offset, BYTE * blob, TCPA_STORE_PUBKEY * store)
{
	TestSuite_LoadBlob_UINT32(offset, store->keyLength, blob);
	TestSuite_LoadBlob(offset, store->keyLength, blob, store->key);
}

TSS_RESULT
TestSuite_UnloadBlob_KEY_PARMS(UINT16 * offset, BYTE * blob, TCPA_KEY_PARMS * keyParms)
{
	TestSuite_UnloadBlob_UINT32(offset, &keyParms->algorithmID, blob);
	TestSuite_UnloadBlob_UINT16(offset, &keyParms->encScheme, blob);
	TestSuite_UnloadBlob_UINT16(offset, &keyParms->sigScheme, blob);
	TestSuite_UnloadBlob_UINT32(offset, &keyParms->parmSize, blob);

	if (keyParms->parmSize > 0) {
		keyParms->parms = malloc(keyParms->parmSize);
		if (keyParms->parms == NULL) {
			fprintf(stderr, "malloc of %d bytes failed.", keyParms->parmSize);
			return TSS_E_OUTOFMEMORY;
		}
		TestSuite_UnloadBlob(offset, keyParms->parmSize, blob, keyParms->parms);
	} else {
		keyParms->parms = NULL;
	}

	return TSS_SUCCESS;
}

TSS_RESULT
TestSuite_UnloadBlob_KEY12(UINT16 * offset, BYTE * blob, TPM_KEY12 * key)
{
	TSS_RESULT result;

	TestSuite_UnloadBlob_UINT16(offset, &key->tag, blob);
	TestSuite_UnloadBlob_UINT16(offset, &key->fill, blob);
	TestSuite_UnloadBlob_UINT16(offset, &key->keyUsage, blob);
	TestSuite_UnloadBlob_KEY_FLAGS(offset, blob, &key->keyFlags);
	key->authDataUsage = blob[(*offset)++];
	if ((result = TestSuite_UnloadBlob_KEY_PARMS(offset, (BYTE *) blob, &key->algorithmParms)))
		return result;
	TestSuite_UnloadBlob_UINT32(offset, &key->PCRInfoSize, blob);

	if (key->PCRInfoSize > 0) {
		key->PCRInfo = malloc(key->PCRInfoSize);
		if (key->PCRInfo == NULL) {
			fprintf(stderr, "malloc of %d bytes failed.", key->PCRInfoSize);
			return TSS_E_OUTOFMEMORY;
		}
		TestSuite_UnloadBlob(offset, key->PCRInfoSize, blob, key->PCRInfo);
	} else {
		key->PCRInfo = NULL;
	}

	if ((result = TestSuite_UnloadBlob_STORE_PUBKEY(offset, blob, &key->pubKey)))
		return result;
	TestSuite_UnloadBlob_UINT32(offset, &key->encSize, blob);

	if (key->encSize > 0) {
		key->encData = malloc(key->encSize);
		if (key->encData == NULL) {
			fprintf(stderr, "malloc of %d bytes failed.", key->encSize);
			return TSS_E_OUTOFMEMORY;
		}
		TestSuite_UnloadBlob(offset, key->encSize, blob, key->encData);
	} else {
		key->encData = NULL;
	}

	return result;
}

TSS_RESULT
TestSuite_UnloadBlob_KEY(UINT16 * offset, BYTE * blob, TCPA_KEY * key)
{
	TSS_RESULT result;

	TestSuite_UnloadBlob_TCPA_VERSION(offset, blob, &key->ver);
	TestSuite_UnloadBlob_UINT16(offset, &key->keyUsage, blob);
	TestSuite_UnloadBlob_KEY_FLAGS(offset, blob, &key->keyFlags);
	key->authDataUsage = blob[(*offset)++];
	if ((result = TestSuite_UnloadBlob_KEY_PARMS(offset, (BYTE *) blob, &key->algorithmParms)))
		return result;
	TestSuite_UnloadBlob_UINT32(offset, &key->PCRInfoSize, blob);

	if (key->PCRInfoSize > 0) {
		key->PCRInfo = malloc(key->PCRInfoSize);
		if (key->PCRInfo == NULL) {
			fprintf(stderr, "malloc of %d bytes failed.", key->PCRInfoSize);
			return TSS_E_OUTOFMEMORY;
		}
		TestSuite_UnloadBlob(offset, key->PCRInfoSize, blob, key->PCRInfo);
	} else {
		key->PCRInfo = NULL;
	}

	if ((result = TestSuite_UnloadBlob_STORE_PUBKEY(offset, blob, &key->pubKey)))
		return result;
	TestSuite_UnloadBlob_UINT32(offset, &key->encSize, blob);

	if (key->encSize > 0) {
		key->encData = malloc(key->encSize);
		if (key->encData == NULL) {
			fprintf(stderr, "malloc of %d bytes failed.", key->encSize);
			return TSS_E_OUTOFMEMORY;
		}
		TestSuite_UnloadBlob(offset, key->encSize, blob, key->encData);
	} else {
		key->encData = NULL;
	}

	return result;
}

TSS_RESULT
TestSuite_UnloadBlob_STORE_PUBKEY(UINT16 * offset, BYTE * blob, TCPA_STORE_PUBKEY * store)
{
	TestSuite_UnloadBlob_UINT32(offset, &store->keyLength, blob);

	if (store->keyLength > 0) {
		store->key = malloc(store->keyLength);
		if (store->key == NULL) {
			fprintf(stderr, "malloc of %d bytes failed.", store->keyLength);
			return TSS_E_OUTOFMEMORY;
		}
		TestSuite_UnloadBlob(offset, store->keyLength, blob, store->key);
	} else {
		store->key = NULL;
	}

	return TSS_SUCCESS;
}

void
TestSuite_UnloadBlob_VERSION(UINT16 * offset, BYTE * blob, TCPA_VERSION * out)
{
	TestSuite_UnloadBlob_BYTE(offset, &out->major, blob);
	TestSuite_UnloadBlob_BYTE(offset, &out->minor, blob);
	TestSuite_UnloadBlob_BYTE(offset, &out->revMajor, blob);
	TestSuite_UnloadBlob_BYTE(offset, &out->revMinor, blob);
}

void
TestSuite_LoadBlob_SYMMETRIC_KEY(UINT16 *offset, BYTE *blob, TCPA_SYMMETRIC_KEY *key)
{
	TestSuite_LoadBlob_UINT32(offset, key->algId, blob);
	TestSuite_LoadBlob_UINT16(offset, key->encScheme, blob);
	TestSuite_LoadBlob_UINT16(offset, key->size, blob);

	if (key->size > 0)
		TestSuite_LoadBlob(offset, key->size, blob, key->data);
}

TSS_RESULT
TestSuite_UnloadBlob_IDENTITY_PROOF(UINT16 *offset, BYTE *blob, TCPA_IDENTITY_PROOF *proof)
{
	TSS_RESULT result;

	/* helps when an error occurs */
	memset(proof, 0, sizeof(TCPA_IDENTITY_PROOF));

	TestSuite_UnloadBlob_VERSION(offset, blob, (TCPA_VERSION *)&proof->ver);
	TestSuite_UnloadBlob_UINT32(offset, &proof->labelSize, blob);
	TestSuite_UnloadBlob_UINT32(offset, &proof->identityBindingSize, blob);
	TestSuite_UnloadBlob_UINT32(offset, &proof->endorsementSize, blob);
	TestSuite_UnloadBlob_UINT32(offset, &proof->platformSize, blob);
	TestSuite_UnloadBlob_UINT32(offset, &proof->conformanceSize, blob);

	if ((result = TestSuite_UnloadBlob_PUBKEY(offset, blob,
					      &proof->identityKey))) {
		proof->labelSize = 0;
		proof->identityBindingSize = 0;
		proof->endorsementSize = 0;
		proof->platformSize = 0;
		proof->conformanceSize = 0;
		return result;
	}

	if (proof->labelSize > 0) {
		proof->labelArea = malloc(proof->labelSize);
		if (proof->labelArea == NULL) {
			result = TSS_E_OUTOFMEMORY;
			goto error;
		}
		TestSuite_UnloadBlob(offset, proof->labelSize, blob, proof->labelArea);
	} else {
		proof->labelArea = NULL;
	}

	if (proof->identityBindingSize > 0) {
		proof->identityBinding = malloc(proof->identityBindingSize);
		if (proof->identityBinding == NULL) {
			result = TSS_E_OUTOFMEMORY;
			goto error;
		}
		TestSuite_UnloadBlob(offset, proof->identityBindingSize, blob,
				 proof->identityBinding);
	} else {
		proof->identityBinding = NULL;
	}

	if (proof->endorsementSize > 0) {
		proof->endorsementCredential = malloc(proof->endorsementSize);
		if (proof->endorsementCredential == NULL) {
			result = TSS_E_OUTOFMEMORY;
			goto error;
		}
		TestSuite_UnloadBlob(offset, proof->endorsementSize, blob,
				 proof->endorsementCredential);
	} else {
		proof->endorsementCredential = NULL;
	}

	if (proof->platformSize > 0) {
		proof->platformCredential = malloc(proof->platformSize);
		if (proof->platformCredential == NULL) {
			result = TSS_E_OUTOFMEMORY;
			goto error;
		}
		TestSuite_UnloadBlob(offset, proof->platformSize, blob,
				 proof->platformCredential);
	} else {
		proof->platformCredential = NULL;
	}

	if (proof->conformanceSize > 0) {
		proof->conformanceCredential = malloc(proof->conformanceSize);
		if (proof->conformanceCredential == NULL) {
			result = TSS_E_OUTOFMEMORY;
			goto error;
		}
		TestSuite_UnloadBlob(offset, proof->conformanceSize, blob,
				 proof->conformanceCredential);
	} else {
		proof->conformanceCredential = NULL;
	}

	return TSS_SUCCESS;
error:
	proof->labelSize = 0;
	proof->identityBindingSize = 0;
	proof->endorsementSize = 0;
	proof->platformSize = 0;
	proof->conformanceSize = 0;
	free(proof->labelArea);
	proof->labelArea = NULL;
	free(proof->identityBinding);
	proof->identityBinding = NULL;
	free(proof->endorsementCredential);
	proof->endorsementCredential = NULL;
	free(proof->conformanceCredential);
	proof->conformanceCredential = NULL;
	/* free identityKey */
	free(proof->identityKey.pubKey.key);
	free(proof->identityKey.algorithmParms.parms);
	proof->identityKey.pubKey.key = NULL;
	proof->identityKey.pubKey.keyLength = 0;
	proof->identityKey.algorithmParms.parms = NULL;
	proof->identityKey.algorithmParms.parmSize = 0;

	return result;
}

void
TestSuite_LoadBlob_SYM_CA_ATTESTATION(UINT16 *offset, BYTE *blob,
				  TCPA_SYM_CA_ATTESTATION *sym)
{
	TestSuite_LoadBlob_UINT32(offset, sym->credSize, blob);
	TestSuite_LoadBlob_KEY_PARMS(offset, blob, &sym->algorithm);
	TestSuite_LoadBlob(offset, sym->credSize, blob, sym->credential);
}

TSS_RESULT
TestSuite_UnloadBlob_SYM_CA_ATTESTATION(UINT16 *offset, BYTE *blob,
				    TCPA_SYM_CA_ATTESTATION *sym)
{
	TSS_RESULT result;

	TestSuite_UnloadBlob_UINT32(offset, &sym->credSize, blob);
	if ((result = TestSuite_UnloadBlob_KEY_PARMS(offset, blob,
						 &sym->algorithm))) {
		sym->credSize = 0;
		return result;
	}

	if (sym->credSize > 0) {
		if ((sym->credential = malloc(sym->credSize)) == NULL) {
			free(sym->algorithm.parms);
			sym->algorithm.parmSize = 0;
			sym->credSize = 0;
			return TSS_E_OUTOFMEMORY;
		}
		TestSuite_UnloadBlob(offset, sym->credSize, blob, sym->credential);
	} else {
		sym->credential = NULL;
	}

	return TSS_SUCCESS;
}

void
TestSuite_LoadBlob_ASYM_CA_CONTENTS(UINT16 *offset, BYTE *blob,
				TCPA_ASYM_CA_CONTENTS *asym)
{
	TestSuite_LoadBlob_SYMMETRIC_KEY(offset, blob, &asym->sessionKey);
	TestSuite_LoadBlob(offset, TCPA_SHA1_160_HASH_LEN, blob,
		       (BYTE *)&asym->idDigest);
}

TSS_RESULT
TestSuite_UnloadBlob_ASYM_CA_CONTENTS(UINT16 *offset, BYTE *blob,
				  TCPA_ASYM_CA_CONTENTS *asym)
{
	TSS_RESULT result;

	if ((result = TestSuite_UnloadBlob_SYMMETRIC_KEY(offset, blob,
						     &asym->sessionKey)))
		return result;

	TestSuite_UnloadBlob(offset, TCPA_SHA1_160_HASH_LEN, blob,
			 (BYTE *)&asym->idDigest);

	return TSS_SUCCESS;
}

static int
hacky_strlen(char *codeset, BYTE *string)
{
	BYTE *ptr = string;
	int len = 0;

	if (strcmp("UTF-16", codeset) == 0) {
		while (!(ptr[0] == '\0' && ptr[1] == '\0')) {
			len += 2;
			ptr += 2;
		}
	} else if (strcmp("UTF-32", codeset) == 0) {
		while (!(ptr[0] == '\0' && ptr[1] == '\0' &&
			 ptr[2] == '\0' && ptr[3] == '\0')) {
			len += 4;
			ptr += 4;
		}
	} else {
		/* default to 8bit chars */
		while (*ptr++ != '\0') {
			len++;
		}
	}

	return len;
}

static inline int
char_width(char *codeset)
{
	if (strcmp("UTF-16", codeset) == 0) {
		return 2;
	} else if (strcmp("UTF-32", codeset) == 0) {
		return 4;
	}

	return 1;
}

#define MAX_BUF_SIZE	4096

BYTE *
TestSuite_Native_To_UNICODE(BYTE *string, unsigned *size)
{
	char *ptr, *ret, *outbuf, tmpbuf[MAX_BUF_SIZE] = { 0, };
	unsigned len = 0, tmplen;
	iconv_t cd = 0;
	size_t rc, outbytesleft, inbytesleft;

	if (string == NULL)
		goto alloc_string;

	if ((cd = iconv_open("UTF-16LE", nl_langinfo(CODESET))) == (iconv_t)-1) {
		fprintf(stderr, "iconv_open: %s", strerror(errno));
		return NULL;
	}

	if ((tmplen = hacky_strlen(nl_langinfo(CODESET), string)) == 0) {
		fprintf(stderr, "hacky_strlen returned 0");
		goto alloc_string;
	}

	do {
		len++;
		outbytesleft = len;
		inbytesleft = tmplen;
		outbuf = tmpbuf;
		ptr = (char *)string;
		errno = 0;

		rc = iconv(cd, &ptr, &inbytesleft, &outbuf, &outbytesleft);
	} while (rc == (size_t)-1 && errno == E2BIG);

	if (len > MAX_BUF_SIZE) {
		fprintf(stderr, "string too long.");
		iconv_close(cd);
		return NULL;
	}

alloc_string:
	/* add terminating bytes of the correct width */
	len += char_width("UTF-16");
	if ((ret = calloc(1, len)) == NULL) {
		fprintf(stderr, "malloc of %u bytes failed.", len);
		iconv_close(cd);
		return NULL;
	}

	memcpy(ret, &tmpbuf, len);
	if (size)
		*size = len;

	if (cd)
		iconv_close(cd);

	return (BYTE *)ret;

}

BYTE *
TestSuite_UNICODE_To_Native(BYTE *string, unsigned *size)
{
	char *ret, *ptr, *outbuf, tmpbuf[MAX_BUF_SIZE] = { 0, };
	unsigned len = 0, tmplen;
	iconv_t cd;
	size_t rc, outbytesleft, inbytesleft;

	if (string == NULL) {
		if (size)
			*size = 0;
		return NULL;
	}

	if ((cd = iconv_open(nl_langinfo(CODESET), "UTF-16LE")) == (iconv_t)-1) {
		fprintf(stderr, "iconv_open: %s", strerror(errno));
		return NULL;
	}

	if ((tmplen = hacky_strlen("UTF-16", string)) == 0) {
		fprintf(stderr, "hacky_strlen returned 0");
		return 0;
	}

	do {
		len++;
		outbytesleft = len;
		inbytesleft = tmplen;
		outbuf = tmpbuf;
		ptr = (char *)string;
		errno = 0;

		rc = iconv(cd, &ptr, &inbytesleft, &outbuf, &outbytesleft);
	} while (rc == (size_t)-1 && errno == E2BIG);

	/* add terminating bytes of the correct width */
	len += char_width(nl_langinfo(CODESET));
	if (len > MAX_BUF_SIZE) {
		fprintf(stderr, "string too long.");
		iconv_close(cd);
		return NULL;
	}

	if ((ret = calloc(1, len)) == NULL) {
		fprintf(stderr, "malloc of %d bytes failed.", len);
		iconv_close(cd);
		return NULL;
	}

	memcpy(ret, &tmpbuf, len);
	if (size)
		*size = len;
	iconv_close(cd);

	return (BYTE *)ret;
}

TSS_RESULT
TestSuite_UnloadBlob_IDENTITY_REQ(UINT16 *offset, BYTE *blob, TCPA_IDENTITY_REQ *req)
{
	TestSuite_UnloadBlob_UINT32(offset, &req->asymSize, blob);
	TestSuite_UnloadBlob_UINT32(offset, &req->symSize, blob);
	/* XXX */
	TestSuite_UnloadBlob_KEY_PARMS(offset, blob, &req->asymAlgorithm);
	TestSuite_UnloadBlob_KEY_PARMS(offset, blob, &req->symAlgorithm);

	if (req->asymSize > 0) {
		req->asymBlob = malloc(req->asymSize);
		if (req->asymBlob == NULL) {
			req->asymSize = 0;
			return TSS_E_OUTOFMEMORY;
		}
		TestSuite_UnloadBlob(offset, req->asymSize, blob, req->asymBlob);
	} else {
		req->asymBlob = NULL;
	}

	if (req->symSize > 0) {
		req->symBlob = malloc(req->symSize);
		if (req->symBlob == NULL) {
			req->symSize = 0;
			req->asymSize = 0;
			free(req->asymBlob);
			req->asymBlob = NULL;
			return TSS_E_OUTOFMEMORY;
		}
		TestSuite_UnloadBlob(offset, req->symSize, blob, req->symBlob);
	} else {
		req->symBlob = NULL;
	}

	return TSS_SUCCESS;
}

#define EVP_SUCCESS 1

void
print_openssl_errors()
{
	ERR_load_crypto_strings();
	ERR_print_errors_fp(stderr);
}

TSS_RESULT
TestSuite_Hash(UINT32 HashType, UINT32 BufSize, BYTE* Buf, BYTE* Digest)
{
	EVP_MD_CTX md_ctx;
	unsigned int result_size;
	int rv;

	switch (HashType) {
		case TSS_HASH_SHA1:
			rv = EVP_DigestInit(&md_ctx, EVP_sha1());
			break;
		default:
			rv = TSS_E_BAD_PARAMETER;
			goto out;
			break;
	}

	if (rv != EVP_SUCCESS) {
		rv = TSS_E_INTERNAL_ERROR;
		goto err;
	}

	rv = EVP_DigestUpdate(&md_ctx, Buf, BufSize);
	if (rv != EVP_SUCCESS) {
		rv = TSS_E_INTERNAL_ERROR;
		goto err;
	}

	result_size = EVP_MD_CTX_size(&md_ctx);
	rv = EVP_DigestFinal(&md_ctx, Digest, &result_size);
	if (rv != EVP_SUCCESS) {
		rv = TSS_E_INTERNAL_ERROR;
		goto err;
	} else
		rv = TSS_SUCCESS;

	goto out;

err:
	print_openssl_errors();
out:
	return rv;
}

TSS_RESULT
TestSuite_SymEncrypt(UINT16 alg, BYTE mode, BYTE *key, BYTE *iv, BYTE *in, UINT32 in_len, BYTE *out,
		     UINT32 *out_len)
{
	TSS_RESULT result = TSS_SUCCESS;
	EVP_CIPHER_CTX ctx;
	EVP_CIPHER *cipher;
	BYTE *def_iv = NULL, *outiv_ptr;
	UINT32 tmp;
	int iv_len, outiv_len;

	if (*out_len > INT_MAX)
		outiv_len = INT_MAX;
	else
		outiv_len = *(int *)out_len;

	/* TPM 1.1 had no defines for symmetric encryption modes, must use CBC */
	switch (mode) {
		case TCPA_ES_NONE:
		case TSS_ES_NONE:
			break;
		default:
			fprintf(stderr, "Invalid mode in doing symmetric decryption");
			return TSS_E_INTERNAL_ERROR;
	}

	switch (alg) {
		case TSS_ALG_AES:
		case TCPA_ALG_AES:
			cipher = (EVP_CIPHER *)EVP_aes_128_cbc();
			break;
		case TSS_ALG_DES:
		case TCPA_ALG_DES:
			cipher = (EVP_CIPHER *)EVP_des_cbc();
			break;
		case TSS_ALG_3DES:
		case TCPA_ALG_3DES:
			cipher = (EVP_CIPHER *)EVP_des_ede3_cbc();
			break;
		default:
			return TSS_E_INTERNAL_ERROR;
			break;
	}

	EVP_CIPHER_CTX_init(&ctx);

	/* If the iv passed in is NULL, create a new random iv and prepend it to the ciphertext */
	iv_len = EVP_CIPHER_iv_length(cipher);
	if (iv == NULL) {
		def_iv = malloc(iv_len);
		if (def_iv == NULL) {
			fprintf(stderr, "malloc of %d bytes failed.", iv_len);
			return TSS_E_OUTOFMEMORY;
		}
		RAND_bytes(def_iv, iv_len);

		memcpy(out, def_iv, iv_len);
		outiv_ptr = &out[iv_len];
		outiv_len -= iv_len;
	} else {
		def_iv = iv;
		outiv_ptr = out;
	}

	if (!EVP_EncryptInit(&ctx, (const EVP_CIPHER *)cipher, key, def_iv)) {
		result = TSS_E_INTERNAL_ERROR;
		print_openssl_errors();
		goto done;
	}

	if ((UINT32)outiv_len < in_len + (EVP_CIPHER_CTX_block_size(&ctx) * 2) - 1) {
		fprintf(stderr, "Not enough space to do symmetric encryption");
		result = TSS_E_INTERNAL_ERROR;
		goto done;
	}

	if (!EVP_EncryptUpdate(&ctx, outiv_ptr, &outiv_len, in, in_len)) {
		result = TSS_E_INTERNAL_ERROR;
		print_openssl_errors();
		goto done;
	}

	if (!EVP_EncryptFinal(&ctx, outiv_ptr + outiv_len, (int *)&tmp)) {
		result = TSS_E_INTERNAL_ERROR;
		print_openssl_errors();
		goto done;
	}

	outiv_len += tmp;
	*out_len = outiv_len;
done:
	if (def_iv != iv) {
		*out_len += iv_len;
		free(def_iv);
	}
	EVP_CIPHER_CTX_cleanup(&ctx);
	return result;
}

TSS_RESULT
TestSuite_SymDecrypt(UINT16 alg, BYTE mode, BYTE *key, BYTE *iv, BYTE *in, UINT32 in_len, BYTE *out,
		     UINT32 *out_len)
{
	TSS_RESULT result = TSS_SUCCESS;
	EVP_CIPHER_CTX ctx;
	EVP_CIPHER *cipher;
	BYTE *def_iv = NULL, *iniv_ptr;
	UINT32 tmp;
	int iv_len, iniv_len;

	if (in_len > INT_MAX)
		return TSS_E_BAD_PARAMETER;

	/* TPM 1.1 had no defines for symmetric encryption modes, must use CBC */
	switch (mode) {
		case TCPA_ES_NONE:
		case TSS_ES_NONE:
			break;
		default:
			fprintf(stderr, "Invalid mode in doing symmetric decryption");
			return TSS_E_INTERNAL_ERROR;
	}

	switch (alg) {
		case TSS_ALG_AES:
		case TCPA_ALG_AES:
			cipher = (EVP_CIPHER *)EVP_aes_128_cbc();
			break;
		case TSS_ALG_DES:
		case TCPA_ALG_DES:
			cipher = (EVP_CIPHER *)EVP_des_cbc();
			break;
		case TSS_ALG_3DES:
		case TCPA_ALG_3DES:
			cipher = (EVP_CIPHER *)EVP_des_ede3_cbc();
			break;
		default:
			return TSS_E_INTERNAL_ERROR;
			break;
	}

	EVP_CIPHER_CTX_init(&ctx);

	/* If the iv is NULL, assume that its prepended to the ciphertext */
	if (iv == NULL) {
		iv_len = EVP_CIPHER_iv_length(cipher);
		def_iv = malloc(iv_len);
		if (def_iv == NULL) {
			fprintf(stderr, "malloc of %d bytes failed.", iv_len);
			return TSS_E_OUTOFMEMORY;
		}

		memcpy(def_iv, in, iv_len);
		iniv_ptr = &in[iv_len];
		iniv_len = in_len - iv_len;
	} else {
		def_iv = iv;
		iniv_ptr = in;
		iniv_len = in_len;
	}

	if (!EVP_DecryptInit(&ctx, cipher, key, def_iv)) {
		result = TSS_E_INTERNAL_ERROR;
		print_openssl_errors();
		goto done;
	}

	if (!EVP_DecryptUpdate(&ctx, out, (int *)out_len, iniv_ptr, iniv_len)) {
		result = TSS_E_INTERNAL_ERROR;
		print_openssl_errors();
		goto done;
	}

	if (!EVP_DecryptFinal(&ctx, out + *out_len, (int *)&tmp)) {
		result = TSS_E_INTERNAL_ERROR;
		print_openssl_errors();
		goto done;
	}

	*out_len += tmp;
done:
	if (def_iv != iv)
		free(def_iv);
	EVP_CIPHER_CTX_cleanup(&ctx);
	return result;
}

int
TestSuite_RSA_Public_Encrypt(unsigned char *in, unsigned int inlen,
			     unsigned char *out, unsigned int *outlen,
			     unsigned char *pubkey, unsigned int pubsize,
			     unsigned int e, int padding)
{
	int rv, e_size = 3;
	unsigned char exp[] = { 0x01, 0x00, 0x01 };
	RSA *rsa = RSA_new();

	if (rsa == NULL) {
		rv = TSS_E_OUTOFMEMORY;
		goto err;
	}

	switch (e) {
		case 0:
			/* fall through */
		case 65537:
			break;
		case 17:
			exp[0] = 17;
			e_size = 1;
			break;
		case 3:
			exp[0] = 3;
			e_size = 1;
			break;
		default:
			rv = TSS_E_INTERNAL_ERROR;
			goto out;
			break;
	}

	switch (padding) {
		case RSA_PKCS1_OAEP_PADDING:
		case RSA_PKCS1_PADDING:
		case RSA_NO_PADDING:
			break;
		default:
			rv = TSS_E_INTERNAL_ERROR;
			goto out;
			break;
	}

	/* set the public key value in the OpenSSL object */
	rsa->n = BN_bin2bn(pubkey, pubsize, rsa->n);
	/* set the public exponent */
	rsa->e = BN_bin2bn(exp, e_size, rsa->e);

	if (rsa->n == NULL || rsa->e == NULL) {
		rv = TSS_E_OUTOFMEMORY;
		goto err;
	}

	rv = RSA_public_encrypt(inlen, in, out, rsa, padding);
	if (rv == -1) {
		rv = TSS_E_INTERNAL_ERROR;
		goto err;
	}

	/* RSA_public_encrypt returns the size of the encrypted data */
	*outlen = rv;
	rv = TSS_SUCCESS;
	goto out;

err:
	print_openssl_errors();
out:
	if (rsa)
		RSA_free(rsa);
	return rv;
}

int
TestSuite_TPM_RSA_Encrypt(unsigned char *dataToEncrypt,
			  unsigned int dataToEncryptLen,
			  unsigned char *encryptedData,
			  unsigned int *encryptedDataLen,
			  unsigned char *publicKey,
			  unsigned int keysize)
{
	int rv;
	unsigned char exp[] = { 0x01, 0x00, 0x01 }; /* 65537 hex */
	unsigned char oaepPad[] = "TCPA";
	int oaepPadLen = 4;
	RSA *rsa = RSA_new();
	BYTE encodedData[256];
	int encodedDataLen;

	if (rsa == NULL) {
		rv = TSS_E_OUTOFMEMORY;
		goto err;
	}

	/* set the public key value in the OpenSSL object */
	rsa->n = BN_bin2bn(publicKey, keysize, rsa->n);
	/* set the public exponent */
	rsa->e = BN_bin2bn(exp, sizeof(exp), rsa->e);

	if (rsa->n == NULL || rsa->e == NULL) {
		rv = TSS_E_OUTOFMEMORY;
		goto err;
	}

	/* padding constraint for PKCS#1 OAEP padding */
	if ((int)dataToEncryptLen >= (RSA_size(rsa) - ((2 * SHA_DIGEST_LENGTH) + 1))) {
		rv = TSS_E_INTERNAL_ERROR;
		goto err;
	}

	encodedDataLen = MIN(RSA_size(rsa), 256);

	/* perform our OAEP padding here with custom padding parameter */
	rv = RSA_padding_add_PKCS1_OAEP(encodedData, encodedDataLen, dataToEncrypt,
					dataToEncryptLen, oaepPad, oaepPadLen);
	if (rv != EVP_SUCCESS) {
		rv = TSS_E_INTERNAL_ERROR;
		goto err;
	}

	/* call OpenSSL with no additional padding */
	rv = RSA_public_encrypt(encodedDataLen, encodedData, encryptedData, rsa, RSA_NO_PADDING);
	if (rv == -1) {
		rv = TSS_E_INTERNAL_ERROR;
		goto err;
	}

	/* RSA_public_encrypt returns the size of the encrypted data */
	*encryptedDataLen = rv;
	rv = TSS_SUCCESS;
	goto out;

err:
	print_openssl_errors();
out:
	if (rsa)
		RSA_free(rsa);
	return rv;
}

/* Testsuite_Transport_Init/Final: wrappers for executing APIs inside a logged transport session */
TSS_RESULT
Testsuite_Transport_Init(TSS_HCONTEXT hContext,
			 TSS_HKEY hSRK,
			 TSS_HTPM hTPM,
			 TSS_BOOL useKeys,
			 TSS_BOOL encrypt,
			 TSS_HKEY *hWrappingKey,
			 TSS_HKEY *hSigningKey)
{
	TSS_RESULT result;
	TSS_HPOLICY hTPMPolicy, hPolicy;
	UINT32 pubSRKLen;
	BYTE* pubSRK;

        result = Tspi_GetPolicyObject( hTPM, TSS_POLICY_USAGE, &hTPMPolicy );
        if ( result != TSS_SUCCESS )
        {
                print_error( "Tspi_GetPolicyObject", result );
		return result;
        }

        result = Tspi_Policy_SetSecret( hTPMPolicy, TESTSUITE_OWNER_SECRET_MODE,
                                        TESTSUITE_OWNER_SECRET_LEN, TESTSUITE_OWNER_SECRET);
        if ( result != TSS_SUCCESS )
        {
                print_error( "Tspi_Policy_SetSecret", result );
		return result;
        }

	/* if the code executing in the transport session uses keys, we must get the SRK pub
	 * from the TPM to avoid errors when closing the session */
	if (useKeys) {
		result = Tspi_TPM_OwnerGetSRKPubKey(hTPM, &pubSRKLen, &pubSRK);
		if ( result != TSS_SUCCESS )
		{
			print_error( "Tspi_TPM_OwnerGetSRKPubKey", result );
			return result;
		}

		result = Tspi_Context_FreeMemory(hContext, pubSRK);
		if ( result != TSS_SUCCESS )
		{
			print_error( "Tspi_Context_FreeMemory", result );
			return result;
		}
	}

	result = Tspi_Context_CreateObject(hContext, TSS_OBJECT_TYPE_RSAKEY,
					   TSS_KEY_SIZE_512 | TSS_KEY_TYPE_LEGACY |
					   TSS_KEY_AUTHORIZATION, hWrappingKey);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_CreateObject", result);
		return result;
	}

	result = Tspi_Context_CreateObject(hContext, TSS_OBJECT_TYPE_POLICY, TSS_POLICY_USAGE,
					   &hPolicy);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_CreateObject", result);
		return result;
	}

	result = Tspi_Policy_SetSecret(hPolicy, TESTSUITE_KEY_SECRET_MODE, TESTSUITE_KEY_SECRET_LEN,
				       TESTSUITE_KEY_SECRET);
	if ( result != TSS_SUCCESS )
	{
		print_error("Tspi_Policy_SetSecret", result);
		return result;
	}

	result = Tspi_Policy_AssignToObject(hPolicy, *hWrappingKey);
	if ( result != TSS_SUCCESS )
	{
		print_error("Tspi_Policy_AssignToObject", result);
		return result;
	}

	result = Tspi_Key_CreateKey(*hWrappingKey, hSRK, 0);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Key_CreateKey", result);
		return result;
	}

	result = Tspi_Key_LoadKey(*hWrappingKey, hSRK);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Key_LoadKey", result);
		return result;
	}

	if (hSigningKey) {
		// Create the key used to sign the transport session
		result = Tspi_Context_CreateObject(hContext, TSS_OBJECT_TYPE_RSAKEY,
				TSS_KEY_SIZE_512 | TSS_KEY_TYPE_SIGNING |
				TSS_KEY_NO_AUTHORIZATION, hSigningKey);
		if (result != TSS_SUCCESS) {
			print_error("Tspi_Context_CreateObject", result);
			return result;
		}

		result = Tspi_Key_CreateKey(*hSigningKey, hSRK, 0);
		if (result != TSS_SUCCESS) {
			print_error("Tspi_Key_CreateKey", result);
			return result;
		}

		result = Tspi_Key_LoadKey(*hSigningKey, hSRK);
		if (result != TSS_SUCCESS) {
			print_error("Tspi_Key_LoadKey", result);
			return result;
		}
	}

	// Set the encryption key to use to be hWrappingKey
	result = Tspi_Context_SetTransEncryptionKey(hContext, *hWrappingKey);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_Context_SetTransEncryptionKey", result);
		return result;
	}

	// Enable the transport session
	result = Tspi_SetAttribUint32(hContext, TSS_TSPATTRIB_CONTEXT_TRANSPORT,
				      TSS_TSPATTRIB_CONTEXTTRANS_CONTROL,
				      TSS_TSPATTRIB_ENABLE_TRANSPORT);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_SetAttribUint32", result);
		return result;
	}

	/* Turn logging on. Skipping this step would give us TPM_BAD_MODE when we call
	 * CloseSignTransport */
	result = Tspi_SetAttribUint32(hContext, TSS_TSPATTRIB_CONTEXT_TRANSPORT,
				      TSS_TSPATTRIB_CONTEXTTRANS_MODE,
				      TSS_TSPATTRIB_TRANSPORT_AUTHENTIC_CHANNEL);
	if (result != TSS_SUCCESS) {
		print_error("Tspi_SetAttribUint32", result);
		return result;
	}

	if (encrypt) {
		result = Tspi_SetAttribUint32(hContext, TSS_TSPATTRIB_CONTEXT_TRANSPORT,
					      TSS_TSPATTRIB_CONTEXTTRANS_MODE,
					      TSS_TSPATTRIB_TRANSPORT_DEFAULT_ENCRYPTION);
		if (result != TSS_SUCCESS) {
			print_error("Tspi_SetAttribUint32", result);
			return result;
		}
	}

	return TSS_SUCCESS;
}


TSS_RESULT
Testsuite_Transport_Final(TSS_HCONTEXT hContext, TSS_HKEY hSigningKey)
{
	TSS_RESULT result;

	if (hSigningKey) {
		result = Tspi_Context_CloseSignTransport(hContext, hSigningKey, NULL);
	} else {
		result = Tspi_SetAttribUint32(hContext, TSS_TSPATTRIB_CONTEXT_TRANSPORT,
					      TSS_TSPATTRIB_CONTEXTTRANS_CONTROL,
					      TSS_TSPATTRIB_DISABLE_TRANSPORT);
	}

	return result;
}

/* Checks a command's resulting TPM signature against validation data */
TSS_RESULT
Testsuite_Verify_Signature(TSS_HCONTEXT hContext, TSS_HKEY hIdentKey, TSS_VALIDATION *valData)
{
	TSS_RESULT	result;
	TSS_HHASH	hHash;

	// create hash
	result = Tspi_Context_CreateObject(hContext, TSS_OBJECT_TYPE_HASH, TSS_HASH_SHA1, &hHash);
	if (result != TSS_SUCCESS) {
		print_error("Testsuite_Verify_Signature : Tspi_Context_CreateObject (hash)",
			    result);
		return result;
	}

	result = Tspi_Hash_UpdateHashValue(hHash, valData->ulDataLength, valData->rgbData);
	if (result != TSS_SUCCESS) {
		print_error("Testsuite_Verify_Signature : Tspi_Hash_SetHashValue", result);
		Tspi_Context_CloseObject(hContext, hHash);
		return result;
	}

	result = Tspi_Hash_VerifySignature(hHash, hIdentKey, valData->ulValidationDataLength,
					   valData->rgbValidationData);
	if (result != TSS_SUCCESS) {
		print_error("Testsuite_Verify_Signature : Tspi_Hash_Verify_Signature", result);
		Tspi_Context_CloseObject(hContext, hHash);
		return result;
	}

	Tspi_Context_CloseObject(hContext, hHash);
	return result;
}

TSS_RESULT
Testsuite_Is_Ordinal_Supported(TSS_HTPM hTPM, TPM_COMMAND_CODE ordinal)
{
	TSS_RESULT result = TSS_SUCCESS;
	UINT32 subCap, pulRespLen;
	BYTE *prgbRespData;

	subCap = ordinal;
	result = Tspi_TPM_GetCapability(hTPM, TSS_TPMCAP_ORD, sizeof(UINT32), (BYTE *)&subCap,
					&pulRespLen, &prgbRespData);
	if ( result != TSS_SUCCESS )
	{
		print_error("Tspi_TPM_GetCapability", result );
		return result;
	}

	if (pulRespLen == sizeof(TPM_BOOL)) {
		if (*(TPM_BOOL *)prgbRespData == FALSE) {
			/* TPM doesn't support this ordinal */
			result = TSS_E_FAIL;
		}
	} else if (pulRespLen == sizeof(UINT32)) {
		if (*(TPM_BOOL *)prgbRespData == FALSE) {
			/* TPM doesn't support this ordinal */
			result = TSS_E_FAIL;
		}
	} else {
		fprintf(stderr, "%s: unknown response size from Tspi_TPM_GetCapability: %u\n",
			__FUNCTION__, pulRespLen);
		result = TSS_E_FAIL;
	}

	return result;
}

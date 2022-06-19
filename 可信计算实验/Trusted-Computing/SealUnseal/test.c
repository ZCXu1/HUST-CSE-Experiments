/**
 * @file test.c
 * @author WangFengwei Email: 110.visual@gmail.com
 * @brief test
 * @created 2011-06-20
 * @modified
 */

#include "common.h"

#include <stdio.h>
#include <string.h>

#include <openssl/ui.h>

#define UI_MAX_SECRET_STRING_LENGTH	256
#define UI_MAX_POPUP_STRING_LENGTH	256

int 
main(int argc, char **argv)
{
	char string[UI_MAX_SECRET_STRING_LENGTH + 1];
	UINT32 strLen;
	do_ui(string, &strLen, "123\n", 0);
	printf("%s\n",string);
	return 0;
}

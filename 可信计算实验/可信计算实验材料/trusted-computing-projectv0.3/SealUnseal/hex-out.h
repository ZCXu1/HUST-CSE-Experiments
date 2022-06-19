/**
 * @file hex-out.h
 * @author WangFengwei Email: visual_110@gmail.com
 * @brief Output data in in hexadecimal format
 * @created 2011-04-01
 * @modified 
 */

#ifndef HEX_OUT_H_
#define HEX_OUT_H_

#include <stdio.h>

void fprint_hex(FILE *stream, unsigned const char *, int len);
#define print_hex(X, Y)		fprint_hex(stdout, X, Y)
#define error_hex(X, Y)		fprint_hex(stderr, X, Y)

#include <ctype.h>
/**
 * Dump the specific buffer to a device(stdout or stderr) in hexadecimal format
 * @param data[in] pointer to a buffer which will be dumped
 * @param length[in] length of buffer(data)
 * @return none
 */
void
fprint_hex(FILE *stream, unsigned const char *data, int length)
{
	int i, j, numrows, index;
	
	numrows = (length + 15) >> 4;

	for(i = 0; i < numrows; i++) {
		/* print the byte counts */
		fprintf(stream, "%.8x|  ", i*16);

		/* print the first 8 hex values */
		for(j = 0; j<8; j++){
			index = i * 16 + j;
			if(index < length) {
				fprintf(stream, "%.2x ", data[index]);
			}
			else{
				fprintf(stream, "   ");
			}
		}
		fprintf(stream, " ");

		/* print the second 8 hex values */
		for(; j < 16; j++) {
			index = i * 16 + j;
			if(index < length){
				fprintf(stream, "%.2x ", data[index]);
			}
			else{
				fprintf(stream, "   ");
			}
		}
		fprintf(stream, " ");

		/* print the ascii values */
		for(j = 0; j < 16; j++){
			index = i * 16 + j;
			if(index < length){
				if(isprint((int)data[index])) {
					fprintf(stream, "%c", data[index]);
				}
				else{
					fprintf(stream, ".");
				}
			}
		}
		fprintf(stream, "\n");
	}
}


#endif // #ifndef HEX_OUT_H_


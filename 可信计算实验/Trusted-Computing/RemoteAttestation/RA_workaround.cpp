#include "common.h"
#include "remote_attestation.h"

#include "../mydebug.h"
int currlevel=DINFO;

#include "../crypto-provider/test_invoke_struct.h"
#include "../crypto-provider/invoke_program.cpp"

static void dump_buffer(char *buf, int size);


int main() {

  invoke_struct var_invoke;
  RemoteProgramCommunicate myCommunicator;

  MYDBGL(DINFO,"Client beginning.\n");

  //
  //  Receive message from external program
  //
  myCommunicator.waitForMessage(var_invoke);


  //
  //  Create our result from the input
  //  

  // Do processing
  main_v1_1();
  result_struct var_result;
  //  memcpy((void*) &var_result, (void*) &var_invoke, sizeof(var_invoke)); //dst, src  //OLD
  var_result.a = (char) var_invoke.a;
  var_result.b = (char) var_invoke.b;
  var_result.END = 0xDEADBEEF;
  var_result.result = 1;


  //
  //   Send reply message back to external program
  //  
  myCommunicator.sendReply(var_result);


  MYDBGL(DINFO,"Client terminating.\n");

  return 0;
}



int main_v1_1(void){
  BYTE nonce[21] ="\x0\x1\x2\x3\x4\x5\x6\x7\x8\x9\xA\xB\xC\xD\xE\xF\x10\x11\x12\x13"; //For some reason gcc/g++ required 21 instead of 20 here when inside crypto-provider;  maybe only mentioned because of -Wall?
  BYTE *receive_msg;
  int receive_msg_size = 0;

  remote_verifier_data* remote_data;
  int remote_data_size = 0;
  
  remote_attestation(nonce, 20, &receive_msg, &receive_msg_size, &remote_data, &remote_data_size);
  MYDBGL(DINFO,"remote_data_size %d\n",remote_data_size);
  if (currlevel >= DINFO) {
    dump_buffer((char*)remote_data, remote_data_size);
    MYDBGL(DINFO,"receive_msg_size %d\n",receive_msg_size);
    dump_buffer((char*) receive_msg, receive_msg_size);
  }

  remote_attestation(nonce, 20, &receive_msg, &receive_msg_size, &remote_data, &remote_data_size);
  MYDBGL(DINFO,"remote_data_size %d\n",remote_data_size);
  if (currlevel >= DINFO) {
    dump_buffer((char*) remote_data, remote_data_size);
    MYDBGL(DINFO,"receive_msg_size %d\n",receive_msg_size);
    dump_buffer((char*) receive_msg, receive_msg_size);
  }
  return 0;
}

static void dump_buffer(char *buf, int size) {
  int i;
  for(i=0;i<size;i++) {
    char* addr=buf+i;
    char c = *addr;
    if ( c>= 32 && c <= 126) {  //127 is DEL
    }
    else {
      c='X';
    }

    if ( (i % 4) == 0) {
      printf(" ");
    }

    if ( (i % 16) == 0) {
      printf("\n");
      printf("Addr %p contents: ",addr);
    }

    printf("%c", c);

    if (i==100) break; //Break prematurely
  }
  printf("\n");

  for(i=0;i<size;i++) {
    char* addr=buf+i;

    if ( (i % 16) == 0) {
      printf("\n");
      printf("Addr %p contents: ",addr);
    }

    if ( (i % 4) == 0) {
      printf(" 0x");
    }


    printf("%2x", *addr);

    if (i==100) break; //Break prematurely
  }
  printf("\n");

}

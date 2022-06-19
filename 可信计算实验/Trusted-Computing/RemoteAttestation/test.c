#include "common.h"
#include "remote_attestation.h"

#include "../mydebug.h"
//int currlevel=DIMPORTANT;
int currlevel=DINFO;

int
main(int argc, char **argv) {
#ifdef CHECK_VERSION
	char version;
	version = parseArgs( argc, argv );
	if (version) {
	  realmain();
	}
	else
		print_wrongVersion();
#else
	realmain();
#endif
	
	return 0;
}

int realmain() {

  BYTE nonce[21] ="\x0\x1\x2\x3\x4\x5\x6\x7\x8\x9\xA\xB\xC\xD\xE\xF\x10\x11\x12\x13"; //For some reason gcc/g++ required 21 instead of 20 here when inside crypto-provider;  maybe only mentioned because of -Wall?

  remote_verifier_data* remote_data;
  int remote_data_size = 0;
  BYTE *receive_msg;
  int receive_msg_size = 0;

  BYTE * data = malloc(MAX_STUFFIT_SIZE);
  int size = 0;
int i = 0;
  
  //Get the verification information
  remote_attestation(nonce, 20, &receive_msg, &receive_msg_size, &remote_data, &remote_data_size);

  MYDBGL(DINFO,"After calling remote_attestation()\n");
  MYDBGL(DINFO,"remote_data_size: %d  receive_msg_size: %d\n",remote_data_size, receive_msg_size);
  if (currlevel >= DINFO) {
    MYDBGL(DINFO," -- remote data -- \n");
    print_hex((char*) remote_data,remote_data_size);
    MYDBGL(DINFO," -- receive msg -- \n");
    print_hex(receive_msg,receive_msg_size);
  }
  
  MYDBGL(DINFO,"pcrCompositeSize %d\n",remote_data->pcrCompositeSize);
  MYDBGL(DINFO,"signedPCRSize %d\n",remote_data->signedPCRSize);
		
  MYDBGLF(DINFO,"##The content of the pcrComposite is:##\n");
  print_hex((BYTE *)remote_data->pcrComposite, remote_data->pcrCompositeSize);
  MYDBGLF(DINFO,"##The content of the pcrComposite is over##\n");
  MYDBGLF(DINFO,"##The content of the signedPCRSize is:##\n");
  print_hex(remote_data->signedPCR, remote_data->signedPCRSize);
  MYDBGLF(DINFO,"##The content of the signedPCRSize is over##\n"); 
  
  MYDBGL(DINFO,"sizeOfSeclect %d\n",remote_data->sizeOfSeclect);
  MYDBGLF(DINFO,"##The content of the PCR Select is:##\n");
  print_hex(remote_data->pcrSelect, 2);
  MYDBGLF(DINFO,"##The content of the PCR Select is over##\n");
/*   MYDBGLF(DINFO,"selectpcrSize %d\n",remote_data->selectpcrSize);  */
  
  pcrComposite_data *pcrComposite = remote_data->pcrComposite;
  MYDBGLF(DINFO,"##The content of the tpmVersion is:##\n");
  print_hex(pcrComposite->tpmVersion, 4);
  MYDBGLF(DINFO,"##The content of the tpmVersion is over##\n"); 

  MYDBGLF(DINFO,"##tpmCmd is ");
  for(i=0;i<4;i++){
  	printf("%c", pcrComposite->tpmCmd[i]);
  } 
  printf("\n");
  MYDBGLF(DINFO,"##The content of the pcr_digest is:##\n");
  print_hex(pcrComposite->pcr_digest, 20);
  MYDBGLF(DINFO,"##The content of the pcr_digest is over##\n"); 
  MYDBGLF(DINFO,"##The content of the nonce is:##\n");
  print_hex(pcrComposite->nonce, 20);
  MYDBGLF(DINFO,"##The content of the nonce is over##\n"); 
  
  //return 0; // Why did Weiqi put a return here?  Ahh, because it segfaults without it.

  //Put it in a serial form
  serializeVerificationValues(&data, &size, remote_data, remote_data_size, receive_msg, receive_msg_size);

  assert(data != 0);
  assert(size > 0);

  //Reuse the variables, so free and clear/malloc them
  free(remote_data);
  free(receive_msg);

  remote_data = (remote_verifier_data*) malloc(MAX_REMOTE_DATA_SIZE);
  remote_data_size =0;
  receive_msg = (BYTE*) malloc(MAX_RECEIVE_MSG_SIZE);
  receive_msg_size = 0;

  deserializeVerificationValues(data, size, remote_data, &remote_data_size, receive_msg, &receive_msg_size);

  MYDBGL(DINFO,"After calling deserializeVerificationValues()\n");
  MYDBGL(DINFO,"remote_data_size: %d  receive_msg_size: %d\n",remote_data_size, receive_msg_size);
  if (currlevel >= DINFO) {
    MYDBGL(DINFO," -- remote data -- \n");
    print_hex((char*) remote_data,remote_data_size);
    MYDBGL(DINFO," -- receive msg -- \n");
    print_hex(receive_msg,receive_msg_size);
  }

  //
  //  Print output to show correct
  //
  MYDBGL(DINFO,"\n");
  MYDBGL(DIMPORTANT,"----------Printing output to show correct.----------\n");
  MYDBGL(DINFO,"\n");
  
  //Reuse the variables, so free and clear/malloc them
  //free(data);  What is data used for up to this point anyway?  It's already zero.
  data = malloc(MAX_STUFFIT_SIZE);
  size = 0;

  // The easy cheap way to do this in a manner which can be easily diffed with the original serialization is to just do another serialization.
  serializeVerificationValues(&data, &size, remote_data, remote_data_size, receive_msg, receive_msg_size);


#ifdef SECOND_TIME
  MYDBGL(DIMPORTANT," ------------- Second time -------------\n");
  remote_attestation(nonce, 20, &receive_msg, &receive_msg_size, &remote_data, &remote_data_size);
  MYDBGL(DINFO,"remote_data_size %d\n",remote_data_size);
  if (currlevel >= DINFO) {
	  //	stuff_it((char*) remote_data, remote_data_size);
	  
	  
  MYDBGL(DINFO,"pcrCompositeSize %d\n",remote_data->pcrCompositeSize);
  MYDBGL(DINFO,"signedPCRSize %d\n",remote_data->signedPCRSize);
  MYDBGL(DINFO,"keyPubSize %d\n",remote_data->keyPubSize);
		
  MYDBGLF(DINFO,"##The content of the pcrComposite is:##\n");
  stuff_it((BYTE *)remote_data->pcrComposite, remote_data->pcrCompositeSize);
  MYDBGLF(DINFO,"##The content of the pcrComposite is over##\n");   
  MYDBGLF(DINFO,"##The content of the signedPCRSize is:##\n");
  stuff_it(remote_data->signedPCR, remote_data->signedPCRSize);
  MYDBGLF(DINFO,"##The content of the signedPCRSize is over##\n"); 
  MYDBGLF(DINFO,"##The content of the keyPubSize is:##\n");
  stuff_it(remote_data->keyPub, remote_data->keyPubSize);
  MYDBGLF(DINFO,"##The content of the keyPubSize is over##\n"); 
  
  MYDBGL(DINFO,"sizeOfSeclect %d\n",remote_data->sizeOfSeclect);
  MYDBGLF(DINFO,"##The content of the PCR Select is:##\n");
  stuff_it(remote_data->pcrSelect, 2);
  MYDBGLF(DINFO,"##The content of the PCR Select is over##\n");
  MYDBGL(DINFO,"selectpcrSize %d\n",remote_data->selectpcrSize); 
  MYDBGL(DINFO,"credLen %d\n",remote_data->credLen); 
  
  MYDBGLF(DINFO,"##The content of the pcrs_value is:##\n");
  stuff_it(remote_data->pcrs_value, remote_data->selectpcrSize);
  MYDBGLF(DINFO,"##The content of the pcrs_value is over##\n"); 

  MYDBGLF(DINFO,"##The content of the cred is:##\n");
  stuff_it(remote_data->cred, remote_data->credLen);
  MYDBGLF(DINFO,"##The content of the cred is over##\n"); 
  
  pcrComposite_data *pcrComposite = remote_data->pcrComposite;
  MYDBGLF(DINFO,"##The content of the tpmVersion is:##\n");
  stuff_it(pcrComposite->tpmVersion, 4);
  MYDBGLF(DINFO,"##The content of the tpmVersion is over##\n"); 

  MYDBGLF(DINFO,"##tpmCmd is ");
  for(i=0;i<4;i++){
  	printf("%c", pcrComposite->tpmCmd[i]);
  } 
  printf("\n");
  MYDBGLF(DINFO,"##The content of the pcr_digest is:##\n");
  stuff_it(pcrComposite->pcr_digest, 20);
  MYDBGLF(DINFO,"##The content of the pcr_digest is over##\n"); 
  MYDBGLF(DINFO,"##The content of the nonce is:##\n");
  stuff_it(pcrComposite->nonce, 20);
  MYDBGLF(DINFO,"##The content of the nonce is over##\n"); 

  
  MYDBGL(DINFO,"receive_msg_size %d\n",receive_msg_size);
  stuff_it(receive_msg_size, sizeof(receive_msg_size));
  stuff_it(receive_msg, receive_msg_size);
  }
#endif //SECOND_TIME
}

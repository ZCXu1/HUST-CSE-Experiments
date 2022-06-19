#include "common.h"
#include "remote_attestation.h"

#include "../mydebug.h"


static char * stuffitCurrPointer = 0;
static char* stuffitBasePointer = 0;
static int stuffitTotalSize = 0;

//Static buffer has the wrong semantics, because clients get a buffer that's only valid for a while.
//Better to require them to free the buffer.
//static char stuffitBuffer[MAX_STUFFIT_SIZE];

// Push size bytes into our data stream
// Must call initialize_stuffit() before first invocation (including to reset it before a new sequence of invocations that are a separate serialization)
void stuff_it(BYTE * data, int size) {
  assert(stuffitCurrPointer);
  memcpy(stuffitCurrPointer, (char*) data, size);
  if (currlevel >= DINFO) {
    print_hex(data, size);
  }

  stuffitCurrPointer += size;
  stuffitTotalSize+=size;
  assert(stuffitTotalSize < MAX_STUFFIT_SIZE);
  assert(stuffitBasePointer + stuffitTotalSize == stuffitCurrPointer);
  MYDBGL(DINFO,"size: %d  stuffitTotalSize: %d\n",size, stuffitTotalSize);
}

initialize_stuffit() {
  stuffitTotalSize = 0;

  stuffitCurrPointer = (char*) malloc(MAX_STUFFIT_SIZE);
  stuffitBasePointer = stuffitCurrPointer;

  assert(stuffitCurrPointer);
}


// Static data for retrieve_it, initialized in set_retrieve_location_and_size
static char* retrievePointer = 0;
static int retrievedSizeRemaining = 0;
static char* retrieveEndPointer = 0;

//Get size bytes out of our data stream
void retrieve_it(BYTE * data, int size) {
  assert(retrievePointer);
  assert(retrievedSizeRemaining >= size);

  //Get the data
  memcpy( (char*) data, retrievePointer, size);
  MYDBGL(DINFO,"size: %d  sizeRemaining: %d\n",size, retrievedSizeRemaining);
  if (currlevel >= DINFO) {
    print_hex(data, size);
  }

  //Update retrievePointer
  retrievePointer += size;

  //Decrease bytes remaining
  retrievedSizeRemaining-=size;

  assert(retrieveEndPointer == retrievePointer + retrievedSizeRemaining);
}

void set_retrieve_location_and_size(char* data, int size) {
  assert(data);
  assert(size);
  assert(size < MAX_STUFFIT_SIZE);

  retrievePointer = data;
  retrievedSizeRemaining = size;
  retrieveEndPointer = retrievePointer + retrievedSizeRemaining;
}


//Should be nothing left -- check.  Could check to see that pointer has changed appropriately, but then would need to track original pointer.
void finish_retrieve(){ 
  assert(retrievePointer);
  assert(retrievedSizeRemaining == 0);
  assert(retrieveEndPointer == retrievePointer);
}



// Serialize remote_data and receive_msg into data
void serializeVerificationValues(BYTE ** data, int* size, remote_verifier_data* remote_data, int remote_data_size, BYTE *receive_msg, int receive_msg_size) {
  int i;
  assert(data != 0);
  assert(*data != 0);

  initialize_stuffit();

  MYDBGL(DINFO,"Stuffing the verification structure.\n");

  MYDBGL(DINFO,"remote_data_size %d\n",remote_data_size);
  //Do we need the total remote data size?  Might need it for something.
  stuff_it( (BYTE*)  &(remote_data_size), sizeof(remote_data_size)); 

  MYDBGL(DINFO,"pcrCompositeSize %d\n",remote_data->pcrCompositeSize);
  stuff_it( (BYTE*) &(remote_data->pcrCompositeSize), sizeof(remote_data->pcrCompositeSize)); 
  MYDBGL(DINFO,"signedPCRSize %d\n",remote_data->signedPCRSize);
  stuff_it( (BYTE*) &(remote_data->signedPCRSize), sizeof(remote_data->signedPCRSize)); 
/*   MYDBGL(DINFO,"keyPubSize %d\n",remote_data->keyPubSize); */
/*   stuff_it( (BYTE*) &(remote_data->keyPubSize), sizeof(remote_data->keyPubSize));  */
  
  MYDBGLF(DINFO,"##The content of the pcrComposite is:##\n");
  stuff_it((BYTE *)remote_data->pcrComposite, remote_data->pcrCompositeSize);
  MYDBGLF(DINFO,"##The content of the pcrComposite is over##\n");  
  MYDBGLF(DINFO,"##The content of the signedPCRSize is:##\n");
  stuff_it(remote_data->signedPCR, remote_data->signedPCRSize);
  MYDBGLF(DINFO,"##The content of the signedPCRSize is over##\n"); 
/*   MYDBGLF(DINFO,"##The content of the keyPub is:##\n"); */
/*   stuff_it(remote_data->keyPub, remote_data->keyPubSize); */
/*   MYDBGLF(DINFO,"##The content of the keyPub is over##\n");  */

  MYDBGL(DINFO,"sizeOfSeclect %d\n",remote_data->sizeOfSeclect);
  stuff_it( &(remote_data->sizeOfSeclect), sizeof(remote_data->sizeOfSeclect)); 
  MYDBGLF(DINFO,"##The content of the PCR Select is:##\n");
  stuff_it(remote_data->pcrSelect, 2);
  MYDBGLF(DINFO,"##The content of the PCR Select is over##\n");
  MYDBGL(DINFO,"selectpcrSize %d\n",remote_data->selectpcrSize);
  stuff_it( (BYTE*) &(remote_data->selectpcrSize), sizeof(remote_data->selectpcrSize));
/*   MYDBGL(DINFO,"credLen %d\n",remote_data->credLen);  */
/*   stuff_it( (BYTE*) &(remote_data->credLen), sizeof(remote_data->credLen));  */

/*   MYDBGLF(DINFO,"##The content of the pcrs_value is:##\n"); */
/*   stuff_it(remote_data->pcrs_value, remote_data->selectpcrSize); */
/*   MYDBGLF(DINFO,"##The content of the pcrs_value is over##\n");  */

/*   MYDBGLF(DINFO,"##The content of the cred is:##\n"); */
/*   stuff_it(remote_data->cred, remote_data->credLen); */
/*   MYDBGLF(DINFO,"##The content of the cred is over##\n");  */

  pcrComposite_data *pcrComposite = remote_data->pcrComposite;
  MYDBGLF(DINFO,"##The content of the tpmVersion is:##\n");
  stuff_it(pcrComposite->tpmVersion, 4);
  MYDBGLF(DINFO,"##The content of the tpmVersion is over##\n"); 

  MYDBGLF(DINFO,"##tpmCmd is ");
  for(i=0;i<4;i++){
    MYDBG(DINFO,"%c \n", pcrComposite->tpmCmd[i]);
    stuff_it(&(pcrComposite->tpmCmd[i]), sizeof(pcrComposite->tpmCmd[i]));
  } 
  MYDBG(DINFO,"\n");
  MYDBGLF(DINFO,"##The content of the pcr_digest is:##\n");
  stuff_it(pcrComposite->pcr_digest, 20);
  MYDBGLF(DINFO,"##The content of the pcr_digest is over##\n"); 
  MYDBGLF(DINFO,"##The content of the nonce is:##\n");
  stuff_it(pcrComposite->nonce, 20);
  MYDBGLF(DINFO,"##The content of the nonce is over##\n"); 

  MYDBGL(DINFO,"receive_msg_size %d\n",receive_msg_size);
  stuff_it((char*) (&receive_msg_size), sizeof(receive_msg_size));
  MYDBGL(DINFO,"##Start of receive_msg##\n");
  stuff_it(receive_msg, receive_msg_size);
  MYDBGL(DINFO,"##The content of the receive_msg is over##\n");

  *data=stuffitBasePointer;
  *size = stuffitTotalSize;
}


// Deserialize remote_data and receive_msg using state kept by stuff_it routine (state = stuffit*)
void deserializeVerificationValues(BYTE* data, int size, remote_verifier_data* remote_data, int* remote_data_size, BYTE* receive_msg, int* receive_msg_size) {

  assert(data);
  assert(size > 0);

  int receive_msg_size_value = 0;
  int i;

  int remote_data_size_value = 0;

  set_retrieve_location_and_size(data, size);


  //
  //  Retrieve
  //
  MYDBGL(DINFO,"-----------------Retrieve phase------------------\n");
  //Do we need the total remote data size?  Might need it for something.
  retrieve_it( (BYTE*)  &(remote_data_size_value), sizeof(remote_data_size_value)); 
  MYDBGL(DINFO,"remote_data_size_value %d\n",remote_data_size_value);

  //Could do malloc here now that know exactly how big this one should be.

  //Need to deal specially with the pointers in the struct;  allocate them and then have where they point at filled in by retrieve_it

  retrieve_it( (BYTE*) &(remote_data->pcrCompositeSize), sizeof(remote_data->pcrCompositeSize)); 
  MYDBGL(DINFO,"pcrCompositeSize %d\n",remote_data->pcrCompositeSize);
  retrieve_it( (BYTE*) &(remote_data->signedPCRSize), sizeof(remote_data->signedPCRSize)); 
  MYDBGL(DINFO,"signedPCRSize %d\n",remote_data->signedPCRSize);
/*   retrieve_it( (BYTE*) &(remote_data->keyPubSize), sizeof(remote_data->keyPubSize));  */
/*   MYDBGL(DINFO,"keyPubSize %d\n",remote_data->keyPubSize); */
  
  remote_data -> pcrComposite = (pcrComposite_data*) malloc (remote_data -> pcrCompositeSize);
  MYDBGLF(DINFO,"##The content of the pcrComposite is:##\n");
  retrieve_it((BYTE *)remote_data->pcrComposite, remote_data->pcrCompositeSize);
  MYDBGLF(DINFO,"##The content of the pcrComposite is over##\n");  

  remote_data -> signedPCR = (char*) malloc (remote_data -> signedPCRSize);
  MYDBGLF(DINFO,"##The content of the signedPCRSize is:##\n");
  retrieve_it(remote_data->signedPCR, remote_data->signedPCRSize);
  MYDBGLF(DINFO,"##The content of the signedPCRSize is over##\n"); 

/*   remote_data -> keyPub = (char*) malloc (remote_data -> keyPubSize); */
/*   MYDBGLF(DINFO,"##The content of the keyPub is:##\n"); */
/*   retrieve_it(remote_data->keyPub, remote_data->keyPubSize); */
/*   MYDBGLF(DINFO,"##The content of the keyPub is over##\n");  */

  retrieve_it( &(remote_data->sizeOfSeclect), sizeof(remote_data->sizeOfSeclect)); 
  MYDBGL(DINFO,"sizeOfSeclect %d\n",remote_data->sizeOfSeclect);
  MYDBGLF(DINFO,"##The content of the PCR Select is:##\n");
  retrieve_it(remote_data->pcrSelect, 2);
  MYDBGLF(DINFO,"##The content of the PCR Select is over##\n");
  retrieve_it( (BYTE*) &(remote_data->selectpcrSize), sizeof(remote_data->selectpcrSize));
  MYDBGL(DINFO,"selectpcrSize %d\n",remote_data->selectpcrSize);
/*   retrieve_it( (BYTE*) &(remote_data->credLen), sizeof(remote_data->credLen));  */
/*   MYDBGL(DINFO,"credLen %d\n",remote_data->credLen);  */

/*   remote_data -> pcrs_value = (char*) malloc (remote_data -> selectpcrSize); */
/*   MYDBGLF(DINFO,"##The content of the pcrs_value is:##\n"); */
/*   retrieve_it(remote_data->pcrs_value, remote_data->selectpcrSize); */
/*   MYDBGLF(DINFO,"##The content of the pcrs_value is over##\n");  */

/*   remote_data -> cred = (char*) malloc (remote_data -> credLen); */
/*   MYDBGLF(DINFO,"##The content of the cred is:##\n"); */
/*   retrieve_it(remote_data->cred, remote_data->credLen); */
/*   MYDBGLF(DINFO,"##The content of the cred is over##\n");  */

  remote_data -> pcrComposite = (pcrComposite_data*) malloc (remote_data -> pcrCompositeSize);
  //retrieve_it(remote_data->pcrComposite, remote_data->pcrCompositeSize); // pcrComposite is stuffed and retrieved field-by-field, for all 4 fields
  pcrComposite_data *pcrComposite = remote_data->pcrComposite;
  MYDBGLF(DINFO,"##The content of the tpmVersion is:##\n");
  retrieve_it(pcrComposite->tpmVersion, 4);
  MYDBGLF(DINFO,"##The content of the tpmVersion is over##\n"); 

  MYDBGLF(DINFO,"##tpmCmd is ");
  for(i=0;i<4;i++){
    MYDBG(DINFO,"%c \n", pcrComposite->tpmCmd[i]);
    retrieve_it(&(pcrComposite->tpmCmd[i]), sizeof(pcrComposite->tpmCmd[i]));
  } 
  MYDBG(DINFO,"\n");
  MYDBGLF(DINFO,"##The content of the pcr_digest is:##\n");
  retrieve_it(pcrComposite->pcr_digest, 20);
  MYDBGLF(DINFO,"##The content of the pcr_digest is over##\n"); 
  MYDBGLF(DINFO,"##The content of the nonce is:##\n");
  retrieve_it(pcrComposite->nonce, 20);
  MYDBGLF(DINFO,"##The content of the nonce is over##\n"); 

  //Do I actually need the receive_msg at all?  Not sure I do
  retrieve_it( (BYTE*) (&receive_msg_size_value), sizeof(receive_msg_size_value));
  MYDBGL(DINFO,"receive_msg_size_value %d\n",receive_msg_size_value);

  MYDBGL(DINFO,"##Start of receive_msg##\n");
  retrieve_it(receive_msg, receive_msg_size_value);
  MYDBGL(DINFO,"##End of receive_msg##\n");

  finish_retrieve();

  assert(remote_data_size);
  *remote_data_size = remote_data_size_value;

  assert(receive_msg_size);
  *receive_msg_size = receive_msg_size_value;
}

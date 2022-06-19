#ifdef __cplusplus
extern "C" {
#endif

#define MAX_STUFFIT_SIZE 8192
#define MAX_REMOTE_DATA_SIZE 256
#define MAX_RECEIVE_MSG_SIZE MAX_STUFFIT_SIZE
  
  //INCLUDE_UNUSED controls fields that are only used by Create_AIK.c

  typedef struct {
    BYTE tpmVersion[4]; 
    BYTE tpmCmd[4];
    BYTE pcr_digest[20];
    BYTE nonce[20];
  } pcrComposite_data;
  
  typedef struct {
    unsigned long pcrCompositeSize; //Example field
    unsigned long signedPCRSize;
#ifdef INCLUDE_UNUSED
    unsigned long keyPubSize; // no longer used?
    unsigned long credLen;  // no longer used?
#endif
    pcrComposite_data * pcrComposite;
    unsigned char * signedPCR;
#ifdef INCLUDE_UNUSED
    unsigned char * keyPub; // no longer used?
    unsigned char * cred;  //no longer used?
#endif
    BYTE sizeOfSeclect; //SRTM is 2 , DRTM is 3
    BYTE pcrSelect[2];
    int selectpcrSize;
#ifdef INCLUDE_UNUSED
    BYTE *pcrs_value; // no longer used?
#endif
  } remote_verifier_data;
  
  //Aka getVerificationValues()
  int remote_attestation(BYTE * nonce_from_client, int sizeofclientnonce,
			 BYTE **receive_msg, int * receive_msg_size,
			 remote_verifier_data** remote_data, int * remote_data_size);

  //The data passed is the address of a BYTE* pointer.  Afterwards, that pointer will point to the serialized data.
  //Calling free() on *data is the responsibility of the caller.
  void serializeVerificationValues(BYTE** data, int* size, remote_verifier_data* remote_data, int remote_data_size, BYTE *receive_msg, int receive_msg_size);

  //data points at the serialized byte stream
  void deserializeVerificationValues(BYTE* data, int size, remote_verifier_data* remote_data, int* remote_data_size, BYTE* receive_msg, int* receive_msg_size);
  
  
#ifdef __cplusplus
}
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/sm3.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/sm2.h>
#include <openssl/pkcs7.h>


using namespace std;


const char initialPKey[] = "\
-----BEGIN EC PARAMETERS----- \n\
BggqgRzPVQGCLQ== \n\
-----END EC PARAMETERS----- \n\
-----BEGIN EC PRIVATE KEY----- \n\
MHcCAQEEINQhCKslrI3tKt6cK4Kxkor/LBvM8PSv699Xea7kTXTToAoGCCqBHM9V \n\
AYItoUQDQgAEH7rLLiFASe3SWSsGbxFUtfPY//pXqLvgM6ROyiYhLkPxEulwrTe8 \n\
kv5R8/NA7kSSvcsGIQ9EPWhr6HnCULpklw== \n\
-----END EC PRIVATE KEY----- \n\
";

const char certificate[] = "\
-----BEGIN CERTIFICATE----- \n\
MIIB/zCCAaagAwIBAgIJAKKa0PAt9M1FMAoGCCqBHM9VAYN1MFsxCzAJBgNVBAYT \n\
AkNOMQ4wDAYDVQQIDAVIdUJlaTEOMAwGA1UEBwwFV3VIYW4xDTALBgNVBAoMBEhV \n\
U1QxDDAKBgNVBAsMA0NTRTEPMA0GA1UEAwwGY2Fyb290MB4XDTIwMDkyMDIwNTkx \n\
OVoXDTMwMDkxODIwNTkxOVowWzELMAkGA1UEBhMCQ04xDjAMBgNVBAgMBUh1QmVp \n\
MQ4wDAYDVQQHDAVXdUhhbjENMAsGA1UECgwESFVTVDEMMAoGA1UECwwDQ1NFMQ8w \n\
DQYDVQQDDAZjYXJvb3QwWTATBgcqhkjOPQIBBggqgRzPVQGCLQNCAASJ8mm28JJR \n\
bZKLr6DCo1+KWimpKEsiTfZM19Zi5ao7Au6YLosyN71256MWmjwkwXxJeLa0lCfm \n\
kF/YWCX6qGQ0o1MwUTAdBgNVHQ4EFgQUAL5hW3RUzqvsiTzIc1gUHeK5uzQwHwYD \n\
VR0jBBgwFoAUAL5hW3RUzqvsiTzIc1gUHeK5uzQwDwYDVR0TAQH/BAUwAwEB/zAK \n\
BggqgRzPVQGDdQNHADBEAiAaZMmvE5zzXHx/TBgdUhjtpRH3Jpd6OZ+SOAfMtKxD \n\
LAIgdKq/v2Jkmn37Y9U8FHYDfFqk5I0qlQOAmuvbVUi3yvM= \n\
-----END CERTIFICATE----- \n\
";



EVP_PKEY *getMyPkey(const char *private_key)
{
    BIO *bio_pkey = BIO_new_mem_buf((char *)private_key, strlength(private_key));
    if (bio_pkey == NULL)
        return NULL;
    return  PEM_read_bio_PrivateKey(bio_pkey, NULL, NULL, NULL);
}

X509 *getMyX509(const char *cert)
{
    BIO *bio;
    bio = BIO_new(BIO_s_mem());
    BIO_puts(bio, cert);
    return  PEM_read_bio_X509(bio, NULL, NULL, NULL);
}

void pkcs7()
{
    BIO *bio_out,*mem;
    PKCS7 *p7;
    X509_STORE *ca;
    size_t length;
    char buf[70],ans[5000];
    string str="";
    int i,countOfSign;
    EVP_PKEY* pKey = getMyPkey(initialPKey);

    ca=X509_STORE_new();
    X509_STORE_add_cert(ca,getMyX509(certificate));

    while(fgets(buf,sizeof(buf),stdin)){
        str+=buf;
    }
    str+='\n';
    const char *s=str.c_str();
    mem=BIO_new_mem_buf(s,strlength(s));
    p7 = PKCS7_new();
    p7= PEM_read_bio_PKCS7(mem, NULL, NULL, NULL);
    if(p7==NULL){
        printf("ERROR\n");
        return;
    }
    bio_out=PKCS7_dataDecode(p7,pKey,NULL,NULL);
    length=BIO_read(bio_out,ans,sizeof(ans));
    if(length==-2){
        printf("ERROR\n");
        return;
    }
    STACK_OF(PKCS7_SIGNER_INFO) *so=PKCS7_get_signer_info(p7);
    if(so==NULL){
        printf("ERROR\n");
        return;
    }
    countOfSign=sk_PKCS7_SIGNER_INFO_num(so);
    PKCS7_SIGNER_INFO *si;
    for(i=0;i<countOfSign;i++){
        si=sk_PKCS7_SIGNER_INFO_value(so,i);
        X509_STORE_CTX* ct=X509_STORE_CTX_new();
        if(PKCS7_dataVerify(ca,ct,bio_out,p7,si)!=1){
            printf("ERROR\n");
            return;
        }
    }
    for(i=0;i<length;i++){
        printf("%c",ans[i]);
    }
    return;
}


int main()
{
    OpenSSL_add_all_algorithms();
    pkcs7();
    return 0;
}
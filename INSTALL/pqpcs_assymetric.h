#include "Argon2/include/argon2.h"
#include "pqpcs_miscellaneous.h"
#include <netdb.h>
#include <netinet/in.h>
#include <openssl/ec.h>
#include <openssl/ecdh.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <openssl/sha.h>
#include <openssl/err.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
#define CURVE_NAME NID_X9_62_prime256v1 //place here your name curve included in openssl ! By default it's NID_X9_62_prime256v1
#define POINT_CONVERSION_FORM POINT_CONVERSION_UNCOMPRESSED
#define HASHLEN SHA512_DIGEST_LENGTH
#define SALTLEN 16
uint32_t t_cost = 3;            // (1)-pass computation
uint32_t m_cost = (1<<16);      // 64 mebibytes memory usage
uint32_t parallelism = 1;       // number of threads and lanes
typedef int32_t SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

unsigned char *pqp_assymetric_ecdh_ecdsa_low(EC_KEY *key,EC_KEY *peerkey, unsigned char *secret_md, SOCKET socket_ec,int32_t IO);
EC_KEY *pqp_assymetric_generate_keys(EC_KEY *eckey);
void *get_peerkey_low(SOCKET socket_ec,EC_KEY *peerkey);
void *send_key_low(EC_KEY *key,SOCKET socket_ec);
void hash(unsigned char *message,unsigned char *md, size_t message_len);


unsigned char *pqp_assymetric_ecdh_ecdsa_low(EC_KEY *key,EC_KEY *peerkey, unsigned char *secret_md, SOCKET socket_ec,int32_t IO){
unsigned char *secret=NULL;

size_t *secret_len=NULL;
int field_size=0,ret=0,i=0;
u_int8_t *dgst=NULL,*mesg=NULL;
ECDSA_SIG *sig=NULL;
BIGNUM *r=BN_secure_new(),*s=BN_secure_new();
char *error_buf=NULL,*r_buf=NULL,*s_buf=NULL;

   key=pqp_assymetric_generate_keys(key);
   peerkey=pqp_assymetric_generate_keys(peerkey);

   if(!EC_KEY_check_key(key))
	fatal(strcat("while verifying key ",  ERR_error_string(ERR_get_error(), NULL)));
   else printf("key verified\n");

   dgst=OPENSSL_malloc(HASHLEN);

   switch(IO){
	case 1 :
		sig = ECDSA_SIG_new();
		get_peerkey_low(socket_ec,peerkey);
		send_key_low(key,socket_ec);
		
		r_buf=ec_malloc(64,sizeof(char));
		s_buf=ec_malloc(64,sizeof(char));
		
		if(recv(socket_ec,r_buf,64*sizeof(char),0)<0)
			fatal("while sending digest message");
		
		if(recv(socket_ec,s_buf,64*sizeof(char),0)<0)
			fatal("while sending digest message");
		
		if(recv(socket_ec,dgst,HASHLEN*sizeof(u_int8_t),0)<0)
			fatal("while sending digest message");
		
		BN_hex2bn(&r,r_buf);
		BN_hex2bn(&s,s_buf);
		
		ECDSA_SIG_set0(sig,r,s);
		
		ret = ECDSA_do_verify(dgst, HASHLEN, sig, peerkey);
		
		if (ret == -1){
			printf("Verification failure :\n");
			fatal(ERR_error_string(ERR_get_error(),error_buf));
		}
		else if (ret == 0){
			printf("Incorrect signature : stop all comunications\n");
			exit(EXIT_FAILURE);
		}
		else if(ret == 1)
			printf("Correct signature\n");
	break;
	case 0 :
		send_key_low(key,socket_ec);
		get_peerkey_low(socket_ec,peerkey);
		
		mesg=ec_malloc(HASHLEN,sizeof(u_int8_t));
		
		for(i=0;i<HASHLEN;i++)
			mesg[i]=arc4random_uniform(256);
		
		hash(mesg,dgst,HASHLEN);
		
		sig = ECDSA_do_sign(dgst, HASHLEN, key);
		
		ECDSA_SIG_get0(sig,&r,&s);

		if(send(socket_ec,BN_bn2hex(r),64*sizeof(char),0)<0)
			fatal("while sending digest message");
		
		if(send(socket_ec,BN_bn2hex(s),64*sizeof(char),0)<0)
			fatal("while sending digest message");
		
		if(send(socket_ec,dgst,HASHLEN*sizeof(u_int8_t),0)<0)
			fatal("while sending digest message");
	
		ret = ECDSA_do_verify(dgst, HASHLEN, sig, key);
		
		if (ret == -1 || ret ==0){
			printf("BAD self-signature verification :\n");
			fatal(ERR_error_string(ERR_get_error(),error_buf));
		}
		else if(ret == 1)
			printf("Self-signature generation [CHECK] OK\n");

		clean_mem(mesg,HASHLEN);
				
	break;
	default : fatal("Error in in/out in pqp_assymetric_ecdh_low");
   }			
   
   // Calculate the size of the buffer for the shared secret
   field_size = EC_GROUP_get_degree(EC_KEY_get0_group(key));
   secret_len=ec_malloc(1,sizeof(size_t));
   *secret_len = ((field_size+7)/8);
   // Allocate the memory for the shared secret
   if(NULL == (secret = OPENSSL_malloc(*secret_len)))
      fatal("while allocating memory for the shared secret");

   // Derive the shared secret
	
   if((*secret_len=ECDH_compute_key(secret, *secret_len, EC_KEY_get0_public_key(peerkey),key, NULL))==0)
      fatal("while checking key");

	secret_md=ec_malloc(HASHLEN,sizeof(unsigned char));
	
	hash(secret,secret_md,*secret_len);

	if(*secret_len<=0){
		clean_mem(secret,*secret_len);
		OPENSSL_free(dgst);
		EC_KEY_free(key);
		EC_KEY_free(peerkey);
		return NULL;
	}
	
clean_mem(secret,*secret_len);
OPENSSL_free(dgst);
EC_KEY_free(key);
EC_KEY_free(peerkey);
	return secret_md;
}

EC_KEY *pqp_assymetric_generate_keys(EC_KEY *eckey){

	eckey=EC_KEY_new_by_curve_name(CURVE_NAME);

	if(eckey==NULL)
		fatal("while asigning EC_KEY to eckey");

	if(EC_KEY_generate_key(eckey)==0)
		fatal("while generating key");
return eckey;
}

void *get_peerkey_low(SOCKET socket_ec,EC_KEY *peerkey){
size_t pub_len=0;
char *buf=NULL;

EC_POINT *peerkey_pub=NULL;	
	if((peerkey_pub=EC_POINT_new(EC_KEY_get0_group(peerkey)))==NULL)
		fatal("while creating ec_point");

	if(recv(socket_ec,&pub_len,sizeof(size_t),0)<0)
		fatal("while receving public key length");

	buf=OPENSSL_malloc(pub_len*sizeof(char));
	
	if(recv(socket_ec,buf,pub_len*sizeof(char),0)<0)
      fatal("while receving public key");
      
    if(EC_POINT_hex2point(EC_KEY_get0_group(peerkey),buf,peerkey_pub,NULL)==NULL)
		fatal("while converting hex2point");
				
    if(EC_KEY_set_public_key(peerkey,peerkey_pub)==0)
		fatal("while associated public key at peerkey");
		
EC_POINT_free(peerkey_pub);
OPENSSL_free(buf);
pub_len=0;
    return NULL;
}

void *send_key_low(EC_KEY *key,SOCKET socket_ec){
size_t pub_len=0;
char *buf=NULL;

	if((buf = EC_POINT_point2hex(EC_KEY_get0_group(key),EC_KEY_get0_public_key(key),EC_GROUP_get_point_conversion_form(EC_KEY_get0_group(key)),NULL))==NULL)
		fatal("while converting EC_POINT into hex string");
	pub_len = strlen(buf);
	
	if(send(socket_ec,&pub_len,sizeof(size_t),0)<0)
	  fatal("while sending public key length");
	
	if(send(socket_ec,buf,pub_len*sizeof(unsigned char),0)<0)
      fatal("while sending public key");

OPENSSL_free(buf);
pub_len=0;
return NULL;
}

void hash(unsigned char *message,unsigned char *md, size_t message_len){
	uint8_t salt[SALTLEN];
	memset( salt, 0x89, SALTLEN );
	argon2id_hash_raw(t_cost, m_cost, parallelism, message,message_len, salt, SALTLEN, md, HASHLEN);
}




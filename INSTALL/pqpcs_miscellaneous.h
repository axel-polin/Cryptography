#include <bsd/bsd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *complete_password(char *password,u_int32_t pass_len){
	u_int8_t a=0;
	int32_t i=0;
	size_t password_size=strlen(password);
	if((i=password_size)+1<pass_len){
		while(i!=pass_len){
			a=arc4random_uniform(93)+33;
			password[i]=a;
			i++;
		}
		printf("PASSWORD CHANGED : %s\n",password);
	 }
	 return password;	 	
}

void fatal(const char *message) {
	char error_message[100];
	char message_fatal[100]="[!!] Fatal Error ";
	strcpy(error_message, message_fatal);
	strncat(error_message, message, 83);
	perror(error_message);
	exit(-1);
}

void clean(const char *buffer,FILE *fp){
char *p=strchr(buffer,'\n');
	if(p!=NULL)
		*p=0;
	else {
		u_int32_t c;
		while((c=fgetc(fp))!='\n'&& c!= EOF);
	}
}

void *clean_mem(void *ptr, size_t size_ptr){
int32_t i=0;
u_int8_t ran=0;
	for(i=0;i<3;i++){
		ran=arc4random_uniform(256);
		ptr=memset(ptr,ran,size_ptr);
	}
	free(ptr);
return ptr;
}

void *ec_malloc(u_int32_t nb,u_int32_t size){
void *ptr = NULL;
   ptr = calloc(nb,size);
   if(ptr == NULL)
      fatal("in ec_malloc() on memory allocation");
   return ptr;
}

void init(void){
#ifdef WIN32
    WSADATA wsa;
    u_int32_t err = WSAStartup(MAKEWORD(2, 2), &wsa);
    if(err < 0)
    {
        puts("WSAStartup failed !");
        exit(EXIT_FAILURE);
    }
#endif
}

void end(void){
#ifdef WIN32
    WSACleanup();
#endif
}

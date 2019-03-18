#include "pqpcs_assymetric.h"
#include <dirent.h>
#include <signal.h>
#include <sys/stat.h>

#ifdef _WIN32

#include <windows.h>
#include <winsock2.h> 

#elif __linux__ 

#include <sys/types.h>
#include <bsd/stdlib.h>

#elif __FreeBSD__ || __NetBSD__ || __OpenBSD__ || __bsdi__ || __DragonFly__

#include <sys/types.h>

 #ifndef _SYS_DIRENT_H_
	#define _SYS_DIRENT_H_
	
 #endif
#endif

#define PORT_MAT 2048
#define PORT_FILE 2049
#define PORT_CHAT 2050
#define MAX_CLIENT 10
#define MAX_LENGTH 200
#define h_addr h_addr_list[0] /* for backward compatibility */

void manage_matrice(char *mode);
u_int32_t manage_data(char *mode, u_int32_t data, u_int8_t password, char *scope);
u_int32_t manage_bridge(char *mode, u_int32_t datas,u_int8_t IO);

void step0();
void send_file(char *hostname, u_int32_t size, u_int32_t *data, char *mode);
void *recv_file(void *arg);
int32_t search_ip_addr(char *ip,char *mac);

void free_struct();

struct ppqcs {
	u_int32_t nb_entry_mat;
	u_int32_t nb_round;
	u_int32_t PASS;
	u_int32_t nb_ip_in_list; 
	u_int32_t *MAT;
	u_int32_t table[5][5];
	u_int32_t data;
	unsigned char *secret;
	char *mesg_chat;
	char *datafile;
	char *char_PASS1;
	char *char_PASS2;
	char *char_PASS3;
	char *iplist;
	char *path_to_file;
	char *CONF;
	char *PATH;
	char *PATH_NET;
	char *IP_LIST;
	char *RECV_DIR;
	char *RECV_FILE;
	char *TABLE;
	SOCKET sockfd;
	SOCKET sockfd2;
	SOCKET socket_ec;
	SOCKET yes;
	SOCKET new_sockfd;
	SOCKET new_sockfd2;
	SOCKET check_connection;
} pqp={256,100,0,0,NULL,{{0},{0}},0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0,0,0,1,0,0,0};

struct hostent *hostinfo=NULL,*hostinfo2=NULL;
struct stat file_stat;
struct dirent* dir=NULL;

IN_ADDR ip={0},ip2={0};
SOCKADDR_IN sin1={0},sin2={0},new_sin={0},new_sin2={0};

pthread_t thread_net1=0,thread_net2=0,thread_proxy1=0;
u_int32_t *server_buffer=0,file_size=0;

void step0(){
	FILE *fd = NULL;
	char *conf_file=NULL;
	pqp.CONF=ec_malloc(100,sizeof(char));
	conf_file=ec_malloc(111,sizeof(char));
	pqp.RECV_FILE=ec_malloc(107,sizeof(char));
	pqp.IP_LIST=ec_malloc(109,sizeof(char));
	pqp.PATH=ec_malloc(100,sizeof(char));
	pqp.PATH_NET=ec_malloc(100,sizeof(char));
	pqp.TABLE=ec_malloc(100,sizeof(char));
		if((pqp.CONF=getenv("PQPCONF"))==NULL)
			fatal("PQPCONF env variable NOT FOUND !!");
		strcpy(conf_file,pqp.CONF);
		strcat(conf_file,"/pqpcsconf");
		if((pqp.RECV_DIR=getenv("PQPRECVDIR"))==NULL)
			fatal("PQPRECVDIR env variable NOT FOUND !!");
		strcpy(pqp.RECV_FILE,pqp.RECV_DIR);
		strcat(pqp.RECV_FILE,"/file_");
		strcpy(pqp.IP_LIST,pqp.CONF);
		strcat(pqp.IP_LIST,"/ip_list");
		
		if((fd = fopen(conf_file,"r")) == NULL){
			printf("Error while reading conffile : Are you launch PQPCS Installer ? If not, launch /path/to/PQPCS/install.sh and follow the instructions\n");
			printf("If the problem persist, contact autor at TrEeSr00t@protonmail.com or your network admin ;)\n");
			exit(0);
		}	
		if(fscanf(fd,"%u %u %u %s %s %s",&pqp.nb_round,&pqp.PASS,&pqp.nb_ip_in_list,pqp.PATH,pqp.PATH_NET,pqp.TABLE) == -1)
			fatal("in step0() while reading file");
			
		if(fclose(fd) == EOF)
		fatal("in step0() while closing file");
	
		
		if(pqp.nb_round<=0 || pqp.nb_round>=100000)
			fatal("illegal value in File : invalid number of round (0<value<100000)");
		if(pqp.PASS<=1 || pqp.PASS>=1024)
			fatal("illegal value in File : invalid password length (1<value<1024)");
		if(pqp.nb_ip_in_list<=0)
			fatal("illegal value in File : invalid number of ip in ip list (>0)");
		pqp.nb_entry_mat =  256;
		pqp.MAT = ec_malloc(pqp.nb_entry_mat,sizeof(u_int32_t));
		//pqp.MAT_net = ec_malloc(pqp.nb_entry_mat,sizeof(u_int32_t));
		pqp.datafile = ec_malloc(5,sizeof(char));
		pqp.char_PASS1 = ec_malloc(pqp.PASS,sizeof(char));
		pqp.char_PASS2 = ec_malloc(pqp.PASS,sizeof(char));
		pqp.char_PASS3 = ec_malloc(pqp.PASS,sizeof(char));
		//choice = ec_malloc(2,sizeof(char));
		pqp.iplist = ec_malloc(1,strlen("255.255.255.255\n"));
		pqp.path_to_file = ec_malloc(100,sizeof(char));
		pqp.mesg_chat = ec_malloc(200,sizeof(char));
		//pqp.tmp_file see step2();
		//pqp.server_buffer see *recv_file();
		conf_file=NULL;
		free(conf_file);
}

void manage_matrice(char *mode){
int32_t i=0,j=0,k=0;
u_int32_t buffer1=0,buffer2=0;
u_int32_t ptr=0;
EC_KEY *key=NULL,*peerkey=NULL;

		if(strcmp(mode,"read")==0){
			j=0;
			for(i=0;i<pqp.nb_entry_mat;i++){
				pqp.MAT[i]=(pqp.MAT[i]^((u_int32_t) pqp.char_PASS1[j]));
				j++;
				if(j==pqp.PASS-1)
					j=0;
			}
		}
		else if(strcmp(mode,"create")==0){
			for(k=0;k<pqp.nb_entry_mat;k++)
				pqp.MAT[k]=k;
					
			for(i=0;i<pqp.nb_round;i++){
				for(j=0;j<pqp.nb_entry_mat;j++){
					int32_t a = arc4random_uniform(pqp.nb_entry_mat);
					buffer1=pqp.MAT[a];
					buffer2=pqp.MAT[j];
					pqp.MAT[j]=buffer1;
					pqp.MAT[a]=buffer2;
				}
				j=0;
			}
		}
		else if(strcmp(mode,"share")==0){
			sin2.sin_addr.s_addr = htonl(INADDR_ANY);
			sin2.sin_family = AF_INET;
			sin2.sin_port = htons(PORT_MAT);
			
			if(bind(pqp.sockfd, (SOCKADDR *) &sin2, sizeof sin2)<0)
				fatal("while binding socket");
		
			if(listen(pqp.sockfd, MAX_CLIENT)<0)
				fatal("while listening connexion");
			socklen_t sin_size = sizeof new_sin;
			
			while(1){
				
				if(daemon(1,0)==-1)
					fatal("while starting daemon");
				signal(SIGTERM, free_struct);
				signal(SIGINT, free_struct);
					
				if((pqp.new_sockfd = accept(pqp.sockfd, (SOCKADDR *)&new_sin,&sin_size))<0)
					fatal("accepting connection");
	
				if(search_ip_addr(inet_ntoa(new_sin.sin_addr),NULL)==0){				
					if((pqp.secret=pqp_assymetric_ecdh_ecdsa_low(key,peerkey,pqp.secret,pqp.new_sockfd,0))==NULL)
						fatal("while computing ecdh and ecdsa");

					for(i=0;i<pqp.nb_entry_mat;i++){
						ptr=manage_bridge("run",pqp.MAT[i],0);
						ptr=ptr^(u_int32_t) pqp.secret[k];
						if(send(pqp.new_sockfd,&ptr,4,0)<0)
							fatal("while sending matrice");
						k++;
						if(k==strlen((const char *) pqp.secret))
							k=0;
					}
					ptr='\n';
					manage_bridge("run",ptr,0);
					k++;
					if(k==strlen((const char *)pqp.secret))
						k=0;
					ptr=ptr^(u_int32_t) pqp.secret[k];
					if(send(pqp.new_sockfd,&ptr,4,0)<0)
						fatal("add '\\n' symbol in matrice");
					k=0;
					ptr=0;
					clean_mem(pqp.secret,strlen((const char *) pqp.secret));
				}
				printf("Ip address not found");
				close(pqp.new_sockfd);
				if((pqp.new_sockfd = accept(pqp.sockfd, (SOCKADDR *)&new_sin,&sin_size))<0)
					fatal("accepting connection");
				}
				close(pqp.sockfd);
				close(pqp.new_sockfd);
		}
		else if(strcmp(mode,"wait")==0){
			if((pqp.secret=pqp_assymetric_ecdh_ecdsa_low(key,peerkey,pqp.secret,pqp.sockfd,1))==NULL)
				fatal("while computing ecdh and ecdsa");
				
			for(i=0;i<pqp.nb_entry_mat;i++){
				if(recv(pqp.sockfd,&ptr,4,0)<0)
					fatal("while recv matrice");
				ptr=ptr^(u_int32_t) pqp.secret[k];
				ptr=manage_bridge("run",ptr,1);
				pqp.MAT[i]=ptr;
					j++;
					k++;
					if(j==pqp.PASS)
						j=0;
					if(k==strlen((const char *) pqp.secret))
						k=0;
			}
			if(recv(pqp.sockfd,&ptr,4,0)<0)
				fatal("while receving final char \\n");
			k++;
			if(k==strlen((const char *)pqp.secret))
				k=0;
			ptr=ptr^(u_int32_t) pqp.secret[k];
			manage_bridge("run",ptr,1);
			if(ptr!='\n')
				printf("Warning : final char of communication not recevied : %u\n",ptr);
			ptr=0;
		}
}

u_int32_t manage_data(char *mode, u_int32_t data, u_int8_t password, char *scope){
int32_t i=0,j=0;
	if(strcmp(mode,"encrypt")==0){	
		i=0;
		j=0;
		if(strcmp(scope,"LOCAL")==0){
				while(data!=pqp.MAT[j] && j<pqp.nb_entry_mat){
					j++;
				}
				if(data!=pqp.MAT[j]){
					printf("Error while crypting file : matrice not complete");
					printf("%u\t%u\n",data,pqp.MAT[j]);
					exit(-1);
				}
				data=j^password;
				j=0;
		}
		if(strcmp(scope,"NET")==0){
			j=0;
			while(data!=pqp.MAT[j] && j<pqp.nb_entry_mat){
				j++;
			}
			if(data!=pqp.MAT[j]){
				printf("Error while crypting file : matrice not complete");
				printf("%u\t%u\n",data,pqp.MAT[i]);
				exit(-1);
			}
			data=j;
			j=0;
		}
	}
	else if(strcmp(mode,"decrypt")==0){
		if(strcmp(scope,"LOCAL")==0)
			data=pqp.MAT[data^password];
		else if(strcmp(scope,"NET")==0)
			data=pqp.MAT[data];
	}
	else fatal("bad mode in manage_data");
	return data;		
}

u_int32_t manage_bridge(char *mode, u_int32_t datas,u_int8_t IO){
u_int32_t buffer_save=0, buffer=0, buffer2=0, buffer3=0, buffer4=0;
int32_t a=0,k=0,i=0,j=0,l=0;

	if(strcmp("create",mode)==0){
		for(i=0;i<5;i++){
				for(j=0;j<5;j++){
					if(j==0)
						pqp.table[i][j]=i;
					else pqp.table[i][j]=(j-1);
				}
			}
			for(k=0;k<pqp.nb_round;k++){
				for(i=0;i<5;i++){
					a=arc4random_uniform(5);
					buffer3=pqp.table[i][0];
					buffer4=pqp.table[a][0];
					pqp.table[i][0]=buffer4;
					pqp.table[a][0]=buffer3;
				
					for(j=1;j<5;j++){
						a=(arc4random_uniform(4))+1;
						buffer3=pqp.table[i][j];
						buffer4=pqp.table[i][a];
						pqp.table[i][j]=buffer4;
						pqp.table[i][a]=buffer3;
					}
				}
			}
			k=0;
	}
	else if(strcmp("run",mode)==0){
		switch(IO){
			case 1 :
				datas=ntohs(datas);
				buffer_save=datas;
				for(k=0;k<4;k++){
					datas=buffer_save;
					while(pqp.table[j][0]!=k && j<=5)
						j++;
					if(pqp.table[j][0]!=k)
						fatal("in manage_bridge run 1");
					datas=datas<<(30-(j*2));
					datas=datas>>30;
					buffer=pqp.table[j][datas+1];
					buffer=buffer<<(2*k);
					buffer2=buffer^buffer2;
					j=0;
					i=0;
				}
				datas=buffer2;
			break;
			case 0 :
				buffer_save=datas;
					for(k=0;k<4;k++){
						j=0;
						i=1;
						datas=buffer_save;
						datas=datas<<(30-(k*2));
						datas=datas>>30;
						while(pqp.table[j][0]!=k){
							j++;
						}
						if(pqp.table[j][0]!=k)
							fatal("table and datas not compatible");
						while(datas!=pqp.table[j][i])
							i++;
						buffer=i-1;
						buffer=buffer<<(2*j);
						buffer2=buffer^buffer2;
						buffer=0;
					}
				i=0;
				j=0;
				i=arc4random_uniform(3);
				i=i+1;
				while(4!=pqp.table[j][0])
					j++;
				if(pqp.table[j][0]!=4)
					fatal("table and datas not compatible");
				buffer=pqp.table[j][i];
				buffer=buffer<<(2*j);
				buffer2=buffer^buffer2;
				datas=buffer2;
				datas=htons(datas);
			break;
			default : fatal("bad bridge mode (1/0 input/output)");
		}
	}
	else if(strcmp("encrypt",mode)==0){
		for(l=0;l<25;l++){
			pqp.table[i][j]=pqp.table[i][j]^(u_int32_t)pqp.char_PASS3[k];
			if(j==4){
				i++;
				j=-1;
			}
			k++;
			if(k==pqp.PASS-1)
				k=0;
			j++;
		}
	}
	else if(strcmp("decrypt",mode)==0){
		for(l=0;l<25;l++){
			pqp.table[i][j]=pqp.table[i][j]^(u_int32_t)pqp.char_PASS3[k];
			if(j==4){
				i++;
				j=-1;
			}
			k++;
			if(k==pqp.PASS-1)
				k=0;
			j++;
		}
	}
	return datas;
}

void send_file(char *hostname, u_int32_t size, u_int32_t *data, char *mode){
	int32_t i=0;
	u_int32_t ptr=0;
	u_int32_t size_new=0;
	
		if((pqp.sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
			fatal("in socket");
	
		if(setsockopt(pqp.sockfd, SOL_SOCKET, SO_REUSEADDR, &pqp.yes, sizeof(int)) == -1)
			fatal("setting socket option SO_REUSEADDR");
		
		else if((hostinfo2=gethostbyname(hostname))==NULL)
				fatal("while acquiring host info");
			
		sin2.sin_addr = *(IN_ADDR *) hostinfo2->h_addr;
		sin2.sin_port = htons(PORT_FILE);
		sin2.sin_family = AF_INET;
		
		if(connect(pqp.sockfd,(SOCKADDR *) &sin2, sizeof(SOCKADDR)) == -1){
			fatal("Connection to dest. failed quit...");
		}
		else {
			if(strcmp(mode,"-c")!=0){
				stat(pqp.path_to_file,&file_stat);
				size = file_stat.st_size;
			}
				size_new=size;
				
				size=manage_bridge("run",size,0);
				if(send(pqp.sockfd,&size,4,0)<0)
					fatal("while sending file size");	
			
				size=size_new;
				
				for(i=0;i<size_new;i++){
					ptr=manage_bridge("run",data[i],0);
					if(send(pqp.sockfd,&ptr,4,0)<0)
						fatal("while sending file");
					data[i]=0;
				}
			printf("DONE !\n");
			close(pqp.sockfd);
		}
}

void *recv_file(void *arg){
	int32_t i=0;
	u_int32_t ptr=0;
	char *choice=ec_malloc(6,sizeof(char));
	
	if((pqp.sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
			fatal("in socket");
	
		if(setsockopt(pqp.sockfd, SOL_SOCKET, SO_REUSEADDR, &pqp.yes, sizeof(int)) == -1)
			fatal("setting socket option SO_REUSEADDR");
	
	sin2.sin_addr.s_addr = htonl(INADDR_ANY);
	sin2.sin_family = AF_INET;
	sin2.sin_port = htons(PORT_FILE);
		
	if(bind(pqp.sockfd, (SOCKADDR *) &sin2, sizeof sin2)<0)
		fatal("while binding socket");
	
	
	if(listen(pqp.sockfd,MAX_CLIENT)<0)
		fatal("while listening connexion");
	socklen_t sin_size = sizeof new_sin;
	
	while(1){	
		pqp.new_sockfd = accept(pqp.sockfd,(SOCKADDR *)&new_sin,&sin_size);
		
		if(recv(pqp.new_sockfd,&file_size,4,0)<0)
			fatal("while receving file size");
		file_size=manage_bridge("run",file_size,1);	
		
		if(file_size>=1024){
			printf("Big file in input continue ? (y/N)");
			if(fgets(choice,5,stdin)==NULL)
				fatal("while reading stdin");
			clean(choice,stdin);
			if(strcmp(choice,"Y")==0 || strcmp(choice,"y")==0 || strcmp(choice,"yes")==0){
				printf("File size recv : reallocating buffer (=%u)....\n",file_size);
				server_buffer = ec_malloc(file_size,sizeof(u_int32_t));
		
				
				for(i=0;i<file_size;i++){
					if(recv(pqp.new_sockfd,&ptr,4,0)<0)
						fatal("while receving file");
					ptr=manage_bridge("run",ptr,1);
					server_buffer[i]=ptr;
				}
			}
			else printf("Not accept file...");
		}
		else {
			server_buffer = ec_malloc(file_size,sizeof(u_int32_t));
				
			for(i=0;i<file_size;i++){
				if(recv(pqp.new_sockfd,&ptr,4,0)<0)
					fatal("while receving file");
				ptr=manage_bridge("run",ptr,1);
				server_buffer[i]=ptr;
			}
		}
		pthread_exit(NULL);
	}
	close(pqp.new_sockfd);
}

int32_t search_ip_addr(char *ip,char *mac){
	FILE *fd=NULL;
	char *check=NULL,*check2=NULL;
if((fd = fopen(pqp.IP_LIST,"r")) == NULL)
		fatal("in check_password() while opening ip_list file");
if(mac!=NULL){
	char *line=NULL;
	line=ec_malloc(strlen("255.255.255.255 ff:ff:ff:ff:ff:ff\n"),sizeof(char));
	
		while(fgets(line,strlen("255.255.255.255 ff:ff:ff:ff:ff:ff"),fd)!=NULL){
			if((check=strstr(line,ip))!=NULL && (check=strstr(line,mac))!=NULL)
				break;
		}
	
		if(fclose(fd) == EOF)
			fatal("in step0() while closing file");
	
		//line="255.255.255.255 ff:ff:ff:ff:ff:ff";
		line=NULL;
		free(line);
	
		if(check==NULL && check2==NULL)
			return 1;
		return 0;
}

char *line=NULL;
line=ec_malloc(strlen("255.255.255.255\n"),sizeof(char));
	
	while(fgets(line,strlen("255.255.255.255"),fd)!=NULL){
		if((check=strstr(line,ip))!=NULL)
			break;
	}
	
	if(fclose(fd) == EOF)
		fatal("in step0() while closing file");
	
	//line="255.255.255.255";
	free(line);

	if(check==NULL)
		return 1;
	return 0;

return EXIT_FAILURE;
}

void free_struct(){
	printf("Flushing structure... ");
	pqp.MAT=NULL;
	pqp.mesg_chat=NULL;
	pqp.datafile=NULL;
	pqp.char_PASS1=NULL;
	pqp.char_PASS2=NULL;
	pqp.char_PASS3=NULL;
	pqp.iplist=NULL;
	pqp.path_to_file=NULL;
	pqp.CONF=NULL;
	pqp.PATH=NULL;
	pqp.PATH_NET=NULL;
	pqp.IP_LIST=NULL;
	pqp.RECV_DIR=NULL;
	pqp.RECV_FILE=NULL;
	pqp.TABLE=NULL;
	
	free(pqp.MAT);
	free(pqp.datafile);
	free(pqp.char_PASS1);
	free(pqp.char_PASS2);
	free(pqp.char_PASS3);
	free(pqp.iplist);
	free(pqp.path_to_file);
	free(pqp.CONF);
	free(pqp.RECV_FILE);
	free(pqp.IP_LIST);
	free(pqp.PATH);
	free(pqp.PATH_NET);
	free(pqp.RECV_DIR);
	free(pqp.TABLE);
	printf("DONE !\n");
}

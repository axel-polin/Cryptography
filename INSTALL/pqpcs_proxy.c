/* 15.11.2017
 * 
 * This is PQPCS proxy for network operation ! 
 * Release 3_BETA
 * 
 */

#include "pqpcs.h"

void *proxy_1(void *arg);
void *proxy_2(void *arg);

int main(int argc, char **argv){
FILE *fd=NULL;
int32_t i=0,j=0,k=0;
char *conf_irc_mat=NULL;
char *conf_irc_table=NULL;
void *arg=NULL;
u_int32_t table_save[5][5]={{0},{0}};

conf_irc_mat=ec_malloc(111,sizeof(char));
conf_irc_table=ec_malloc(111,sizeof(char));

	init();
	step0();
	
	if((pqp.CONF=getenv("PQPCONF"))==NULL)
		fatal("PQPCONF env variable NOT FOUND !!");
	strcpy(conf_irc_mat,pqp.CONF);
	strcat(conf_irc_mat,"/irc_matrice");
		
	printf("Reading Matrice...\n");
	
	printf("Enter password for IRC matrice :");
		if(fgets(pqp.char_PASS1,pqp.PASS,stdin)==NULL)
			fatal("while reading pqp.char_PASS1");
		clean(pqp.char_PASS1,stdin);	
	if((fd = fopen(conf_irc_mat,"r")) == NULL){
		printf("Matrice doesn't exist creating it...\n");
		pqp.char_PASS1=complete_password(pqp.char_PASS1,pqp.PASS);
		manage_matrice("create");
		if((fd = fopen(conf_irc_mat,"w")) == NULL)
			fatal("in step1() while opening file");
		j=0;
		for(i=0;i<pqp.nb_entry_mat;i++){
			if(fprintf(fd," %d ",(pqp.MAT[i]^((u_int32_t)pqp.char_PASS1[j]))) == -1)
				fatal("in step1() while writing matrice");
			j++;
			if(j==pqp.PASS)
				j=0;
		}
		if(fclose(fd) == EOF)
			fatal("in step1() while closing file");
		}
		j=0;
		for(i=0;i<pqp.nb_entry_mat;i++){
			if(fscanf(fd," %u ",&pqp.MAT[i]) == -1)
				fatal("in step1() while reading file");
		}
			manage_matrice("read");
			if(fclose(fd) == EOF)
				fatal("in step1() while closing file");
	
	printf("DONE !\n\nReading Table...\n");
	printf("Enter password for IRC TABLE :");
	
	if(fgets(pqp.char_PASS3,pqp.PASS,stdin)==NULL)
		fatal("while reading pqp.char_PASS3");
	clean(pqp.char_PASS3,stdin);

	strcpy(conf_irc_table,pqp.CONF);
	strcat(conf_irc_table,"/irc_table");
	
	if((fd=fopen(conf_irc_table,"r"))==NULL){
			printf("Table doesn't exist creating it...\n");
			if((fd=fopen(conf_irc_table,"w"))==NULL)
				fatal("while creating table file");
			else {
				pqp.char_PASS3=complete_password(pqp.char_PASS3,pqp.PASS);
				manage_bridge("create",0,0);
				for(i=0;i<5;i++)
					for(j=0;j<5;j++)
						table_save[i][j]=pqp.table[i][j];
				manage_bridge("encrypt",0,0);
				for(i=0;i<5;i++){
					for(j=0;j<5;j++){
						fprintf(fd," %u ",pqp.table[i][j]);
						k++;
						if(k==pqp.PASS)
							k=0;
						pqp.table[i][j]=table_save[i][j];
						table_save[i][j]=0;
					}
				}
			}
		}
		else {
		i=0;
		j=0;
		k=0;
		while(fscanf(fd," %u ",&pqp.table[i][j])!=-1){
			if(j==4){
				i+=1;
				j=-1;
			}
			j++;
		}
		manage_bridge("decrypt",0,0);
		}
	
	printf("DONE !\n\nCreating Sockets...\n");

	if((pqp.sockfd = socket(PF_INET, SOCK_STREAM,0)) == -1)
			fatal("in socket");
	
	if(setsockopt(pqp.sockfd, SOL_SOCKET, SO_REUSEADDR, &pqp.yes, sizeof(int32_t)) == -1)
		fatal("setting socket option SO_REUSEADDR");
		
	hostinfo=gethostbyname("127.0.0.1");
	
	sin1.sin_addr.s_addr = htonl(INADDR_ANY);
	sin1.sin_family = AF_INET;
	sin1.sin_port = htons(2061);
	
	if(bind(pqp.sockfd, (SOCKADDR *) &sin1, sizeof sin1)<0)
		fatal("while binding socket");
	
	if(listen(pqp.sockfd,MAX_CLIENT)<0)
		fatal("while listening connexion");
		
	if((pqp.sockfd2 = socket(PF_INET, SOCK_STREAM,0)) == -1)
		fatal("in socket");
	
	if(setsockopt(pqp.sockfd2, SOL_SOCKET, SO_REUSEADDR, &pqp.yes, sizeof(int32_t)) == -1)
		fatal("setting socket option SO_REUSEADDR");

	sin2.sin_addr = *(IN_ADDR *) hostinfo->h_addr;
	sin2.sin_family = AF_INET;
	sin2.sin_port = htons(2060);

	if(connect(pqp.sockfd2,(SOCKADDR *) &sin2, sizeof(SOCKADDR)) == -1)
		fatal("while connecting socket");
		
	printf("DONE !\n\nCreating Threads and start daemon...\n");
	
	if(daemon(1,0)==-1)
		fatal("while starting daemon");
	signal(SIGTERM, free_struct);
	signal(SIGINT, free_struct);
	while(1){
		socklen_t sin_size = sizeof new_sin;
		
		pqp.new_sockfd = accept(pqp.sockfd,(SOCKADDR *)&new_sin,&sin_size);

		if(pthread_create(&thread_net2,NULL,proxy_1,NULL)!=0)
			fatal("while creating thread...");
		proxy_2(arg);
		close(pqp.new_sockfd);
		close(pqp.sockfd2);
	}
	end();
	return 0;
}

void *proxy_1(void *arg){
	EC_KEY *key=NULL,*peerkey=NULL;
	unsigned char *secret_md=NULL;
	
	secret_md=pqp_assymetric_ecdh_ecdsa_low(key,peerkey,secret_md,pqp.new_sockfd,1);
	
	while(1){
		if(recv(pqp.new_sockfd,&pqp.data,sizeof(pqp.data),0)<0)
			fatal("while receving datas from output");
		
		manage_data("encrypt",pqp.data,0,"NET");
		manage_bridge("run", (u_int32_t) pqp.data,0);
		
		if(send(pqp.sockfd2,&pqp.data,sizeof(pqp.data),0)<0)
			fatal("while sending datas at input");
	}
}

void *proxy_2(void *arg){
	EC_KEY *key=NULL,*peerkey=NULL;
	unsigned char *secret_md=NULL;
	
	secret_md=pqp_assymetric_ecdh_ecdsa_low(key,peerkey,secret_md,pqp.sockfd2,0);
	
	while(1){
		if(recv(pqp.sockfd2,&pqp.data,sizeof(pqp.data),0)<0)
			fatal("while receving from input");
			
		manage_bridge("run", (u_int32_t) pqp.data,1);
		manage_data("decrypt",pqp.data,0,"NET");
		
		if(send(pqp.new_sockfd,&pqp.data,sizeof(pqp.data),0)<0)
			fatal("while sending datas at output");
	}
}



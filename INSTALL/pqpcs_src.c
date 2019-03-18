/* 
* 01.11.2018
*RELEASE_unstable 8 version please be careful this software maybe unstable.
*You shouldn't use this software on critical FILE
* The program mode 0 is fully functionnal. (encrypt && decrypt)
* The program mode 1 is fully functionnal. (encrypt && decrypt)
* The program mode 2 is fully functionnal. (encrypt && decrypt)
* The program mode 3 is fully functionnal. (encrypt)
* The recursive mode functionnal (but only one directory)
* Chat mode functionnal (only between two peer).
* IRC server built, see pqpcs_proxy to use with irc client (release 1).
*/

#include "pqpcs.h" /*the include specific file for pqp */

/*
 * Copyright 2017 TreesRoot <TrEeSr00t@protonmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


u_int32_t prog_mode_en=0;
u_int32_t prog_mode_de=0;
u_int32_t num_file=0; 
char *choice=0;
u_int32_t argc_s=0;
char *argv_s[5]={0};
u_int32_t stop_mesg=0;
u_int32_t *tmp_file=0;

void step1();
void step2();
void step_net();
void wait_matrice();
void recursive(u_int8_t way,u_int8_t it);

void usage(char *prog_name,u_int8_t way){ /* the usage function for PQP */
	printf("Usage %s <encrypt> or <decrypt> <mode> ",prog_name); /* the generic command */
	if(way==1) // there are different way for usage :
		printf("[-r | -c] <dest ip>\n"); // 1 for encrypt mode 3 
	else if(way==2)
		printf("<choice of display>\n"); // 2 for decrypt mode 2
	else if(way==3)
		printf("[-r]\n"); // 3 for mode 0 or 1 (encrypt/decrypt)
	else if(way==4)
		printf("\n"); // 4 for diaplay generic command
	else fatal("bad way for usage correct source code"); // if there is error in the source code
	printf("\n"); // add return at the line
	printf("Use man pqpcs to look for man page\n"); //Display message about man page
	exit(0); // exit proprely no error
}

int main(int argc, char **argv){
	
	if(geteuid()==0){ //check if the user is super-user, and warn the user about risk
		printf("PQP system needn't super-user access, it's deprecated you should quit...\n");
		sleep(2);	
	}
	init(); //windows function only for Windows OS.
	u_int32_t i=0;	
	argc_s=argc; // copy number of argument in the struct pqp.
		if(argc>2 && argc<=5){ //if the number of argument is superior to two (e.g ./prog_name <encrypt or decrypt> mode)
                                       //or inferior or equal at five (e.g ./prog_name encrypt 3 -c 127.0.0.1) continue.
			if(strcmp(argv[1],"encrypt")!=0 && strcmp(argv[1],"decrypt")!=0) // test if the first argument match with encrypt or decrypt
				fatal("bad argument"); // if the fist argument not match error.
			step0(); //run step0 before other action.
			if(strcmp(argv[1],"encrypt")==0){
				choice=argv[2]; //copy argument 2 (the mode) in pqp struct
			if(strcmp(choice,"0")==0){ //search the right mode in 0,1,2,3
				prog_mode_en=0;
			}	
			else if(strcmp(choice,"1")==0){
				prog_mode_en=1;
			}
			else if(strcmp(choice,"2")==0){
				prog_mode_en=2;
			}
			else if(strcmp(choice,"3")==0){
				prog_mode_en=3;
			}
			else usage(argv[0],3); // if nothing choice match display usage
			printf("~~~~Hello ! Welcome on PPQCS !~~~~~\n\n"); 
			printf("You have choosed encrypt...\n");
			printf("Crypting begining on mode %u\n",prog_mode_en);
			switch(prog_mode_en){
				case 0 :
					for(i=0;i<4;i++){
						argv_s[i]=argv[i]; //copy all arguments in other arguments save table
					}
					if(argc==4)
						if(strcmp(argv_s[3],"-r")==0){ // enter in recursive mode
							printf("Enter directory to encrypt :");
							pqp.path_to_file=argv_s[4]
							//if(fgets(pqp.path_to_file,100,stdin)==NULL) // get path to file from keyboard
							//	fatal("while acquiring path file into stdin");
							//clean(pqp.path_to_file,stdin); // always clean keyboard buffer before continue
							if(strstr(pqp.path_to_file,".pqp")!=NULL && argc<4){
								printf("Document is already encrypted exit...\n");
							free_struct();
							}
							recursive(0,0);
						}
						else usage(argv[0],3); // if argument 3 (the modifier mode) not match with -r
					else if(argc<4){
						pqp.path_to_file=argv_s[3]
						//printf("Enter file to encrypt :");
						//if(fgets(pqp.path_to_file,100,stdin)==NULL)
						//	fatal("while acquiring path file into stdin");
						//clean(pqp.path_to_file,stdin); // always clean keyboard buffer before continue
						if(strstr(pqp.path_to_file,".pqp")!=NULL && argc<4){ // test if the file is already encrypted
							printf("Document is already encrypted exit...\n");
							free_struct(); // freeing pqp struct
						}
						else{	
							step1();
							step2();
						}
					}
					else usage(argv[0],3); // if the number of argument is not equal to 4 (or inferior at 4) display usage
				break;
				case 1 :
					for(i=0;i<4;i++){
						argv_s[i]=argv[i]; //copy all arguments in pqp struct
					}
					if(argc==4)
						if(strcmp(argv_s[3],"-r")==0){ // enter in recursive mode
							pqp.path_to_file=argv_s[4]
							//printf("Enter directory to encrypt :");
							//if(fgets(pqp.path_to_file,100,stdin)==NULL)
							//	fatal("while reading pqp.path_to_file");
							//clean(pqp.path_to_file,stdin); // always clean keyboard buffer before continue
							if(strstr(pqp.path_to_file,".pqp")!=NULL && argc<4){
								printf("Document is already encrypted exit...\n");
							free_struct();
							}
							recursive(1,0);
						}
						else usage(argv[0],3);
					else if(argc<4){
						printf("Enter file to encrypt :");
						pqp.path_to_file=argv_s[3]
						//if(fgets(pqp.path_to_file,100,stdin)==NULL)
						//	fatal("while reading pqp.path_to_file");
						//clean(pqp.path_to_file,stdin); // always clean keyboard buffer before continue
						if(strstr(pqp.path_to_file,".pqp")!=NULL && argc<4){ // test if the file is already encrypted
							printf("Document is already encrypted exit...\n");
						free_struct(); // freeing pqp struct
						}
						step_net(); // run the network function
					}
					else usage(argv[0],3); // if the number of argument is not equal to 4 (or inferior at 4) display usage
				break;
				case 2 :
					if(argc!=3){ //check if the number of argument is different that 3 ->
						usage(argv[0],3); // display usage
					}
					for(i=0;i<3;i++){
						argv_s[i]=argv[i]; //copy all arguments in pqp struct
					}
					step_net();
				break;
				case 3 :
					for(i=0;i<5;i++){
						argv_s[i]=argv[i]; //copy all arguments in pqp struct
					}
					if(argc==5){
						if(strcmp(argv_s[3],"-r")==0){ // enter in recursive mode
							pqp.path_to_file=argv_s[6]
							//printf("Enter directory to send :");
							//if(fgets(pqp.path_to_file,100,stdin)==NULL)
							//	fatal("while reading pqp.path_to_file");
							//clean(pqp.path_to_file,stdin); // always clean keyboard buffer before continue
							recursive(1,0);
						}
						else if(strcmp(argv_s[3],"-c")==0){
							
							printf("Begin chat...\n");
							step_net();
						}
						else usage(argv[0],1);
					}
					else if(argc==4){
						pqp.path_to_file=argv_s[5]
						//printf("Enter file to send :");
						//if(fgets(pqp.path_to_file,100,stdin)==NULL)
						//	fatal("while reading pqp.path_to_file");
						//clean(pqp.path_to_file,stdin);
						step_net();
					}
					else usage(argv[0],1);
				break;
				default :
					fatal("program mode bad value");
			}
			free_struct();
			printf("OK\n");
		}
		else if(strcmp(argv[1],"decrypt")==0){

			choice=argv[2];
			if(strcmp(choice,"0")==0){
				prog_mode_de=0;
			}
			else if(strcmp(choice,"1")==0){
				prog_mode_de=1;
			}
			else if(strcmp(choice,"2")==0){
				prog_mode_de=2;
			}
			else usage(argv[0],3);
			printf("~~~~Hello ! Welcome on PPQCS !~~~~~\n\n");
			printf("You have choosed decrypt...\n");
			printf("Decrypting begining on mode %u\n",prog_mode_de);
			switch(prog_mode_de){
				case 0 :
					for(i=0;i<4;i++){
						argv_s[i]=argv[i];
					}
					if(argc==4){
						if(strcmp(argv_s[3],"-r")==0){
							pqp.path_to_file=argv_s[4]
							//printf("Enter directory to decrypt :");
							//if(fgets(pqp.path_to_file,100,stdin)==NULL)
							//	fatal("while reading pap.path_to_file");
							//clean(pqp.path_to_file,stdin);
						if(strstr(pqp.path_to_file,".pqp")==NULL && argc<4){
							printf("Document is not encrypted exit...\n");
							exit(0);
						}
							recursive(0,0);
						}
						else usage(argv[0],3);
					}
					else {
						pqp.path_to_file=argv_s[3]
						//printf("Enter file to decrypt :");
						//if(fgets(pqp.path_to_file,100,stdin)==NULL)
						//	fatal("while reading pqp.path_to_file");
						//clean(pqp.path_to_file,stdin);
						if(strstr(pqp.path_to_file,".pqp")==NULL && argc<4){
							printf("Document is not encrypted exit...\n");
						exit(0);
					}
						step1();
						step2();
					}
				break;
				case 1 :
					for(i=0;i<4;i++){
						argv_s[i]=argv[i];
					}
					if(argc==4 && strcmp(argv_s[3],"-r")==0){
						pqp.path_to_file=argv_s[4]
						//printf("Enter directory to decrypt :");
						//if(fgets(pqp.path_to_file,100,stdin)==NULL)
						//	fatal("while acquiring path file into stdin");
						//clean(pqp.path_to_file,stdin);
						//if(strstr(pqp.path_to_file,".pqp")==NULL && argc<4){
						//	printf("Document is not encrypted exit...\n");
						//	exit(0);
						//}
						recursive(1,0);
					}
					else {
						pqp.path_to_file=argv_s[3]
						//printf("Enter file to decrypt :");
						//if(fgets(pqp.path_to_file,100,stdin)==NULL)
						//	fatal("while acquiring path file into stdin");
						//clean(pqp.path_to_file,stdin);
						//if(strstr(pqp.path_to_file,".pqp")==NULL && argc<4){
						//	printf("Document is not encrypted exit...\n");
						//	exit(0);
						//} 
						step_net();
					}
				break;
				case 2 :
					if(argc<4){
						usage(argv[0],2);
					}
					for(i=0;i<4;i++){
						argv_s[i]=argv[i];
					}
					step_net();
				break ;
				default :
				fatal("program mode bad value");
				break;
			}
		printf("OK\n");
		}
		}
		else {
			usage(argv[0],4);
		}
		end();
		return EXIT_SUCCESS;
}

//////////////////////////////////////////////////////

void step1(){
	int32_t i=0,j=0;
	FILE* fd = NULL;
	char *file = NULL;
		if((strcmp(argv_s[1],"encrypt")==0 && (prog_mode_en==2 || prog_mode_en==3)) || prog_mode_de==2){
			file=pqp.PATH_NET;
			printf("Enter password for NET matrice :");
			if(fgets(pqp.char_PASS1,pqp.PASS,stdin)==NULL)
				fatal("while reading pqp.char_PASS1");
			clean(pqp.char_PASS1,stdin);	
		}
		else if(prog_mode_en==0 || prog_mode_de==0){
			file=pqp.PATH;
			if(argc_s!=4){
				printf("Enter password for LOCAL matrice :");
				if(fgets(pqp.char_PASS1,pqp.PASS,stdin)==NULL)
					fatal("while reading pqp.char_PASS1");
				clean(pqp.char_PASS1,stdin);
			}	
		}
		else fatal("bad program mode in step1()");
		if(strcmp(argv_s[1],"encrypt")==0){
			if((fd = fopen(file,"r")) == NULL){
				printf("Matrice doesn't exist creating it...\n");
				pqp.char_PASS1=complete_password(pqp.char_PASS1,pqp.PASS);
				manage_matrice("create");
				if((fd = fopen(file,"w")) == NULL)
					fatal("in step1() while opening file");

				j=0;
				for(i=0;i<pqp.nb_entry_mat;i++){
					if(fprintf(fd," %u ",(pqp.MAT[i]^((u_int32_t)pqp.char_PASS1[j]))) == -1)
						fatal("in step1() while writing matrice");
					j++;
					if(j==pqp.PASS-1)
						j=0;
				}
				if(fclose(fd) == EOF)
					fatal("in step1() while closing file");
			}
			else {
				j=0;
				for(i=0;i<pqp.nb_entry_mat;i++){
					if(fscanf(fd," %u ",&pqp.MAT[i]) == -1)
						fatal("in step1() while reading file");
				}
				manage_matrice("read");
				if(fclose(fd) == EOF)
					fatal("in step1() while closing file");
			}
		}
		else if(strcmp(argv_s[1],"decrypt")==0){
			if((fd = fopen(file,"r")) == NULL)
				fatal("in step1() when opening matrice");
			j=0;
			for(i=0;i<pqp.nb_entry_mat;i++){
				if(fscanf(fd," %u ",&pqp.MAT[i]) == -1)
					fatal("in step1() while reading file");
			}
			manage_matrice("read");
		}
}

void step2(){
	FILE *fd = NULL;
	u_int32_t i=0,len_cut=0;
	int32_t k=0;
	u_int32_t size=0;
	char name_cut[105]={0},num_char_file[10]={0};
	
	if(*pqp.path_to_file!='\0'){
		if(stat(pqp.path_to_file,&file_stat)==-1)
			fatal("while acquiring stat for file");
		size=file_stat.st_size;	
		tmp_file=ec_malloc(size,sizeof(u_int32_t));
	}
	
	char path[100]={0};
	char name[105]={0};
		if(strcmp(argv_s[1],"encrypt")==0){
			switch(prog_mode_en){
				case 0:	
					if(argc_s==4){
						if(strcmp(argv_s[3],"-r")!=0)
							fatal("bad argument");
					}
					else{
						printf("Enter password for FILE:");
						if(fgets(pqp.char_PASS2,pqp.PASS,stdin)==NULL)
							fatal("while reading pqp.char_PASS2");
						clean(pqp.char_PASS2,stdin);
						pqp.char_PASS2=complete_password(pqp.char_PASS2,pqp.PASS);
					}
					if((fd = fopen(pqp.path_to_file,"rb+")) == NULL)
						fatal("in step2() while opening file");
				printf("Generating crypted text for %s please wait... :\n",pqp.path_to_file);
				i=0;
				k=0;
				fseek(fd,0,SEEK_SET);
				for(i=0;i<size;i++){
					if(fread(&pqp.data, 1,1, fd)!=1)
						fatal("while reading file");
					fseek(fd,i,SEEK_SET);
					pqp.data=manage_data("encrypt",pqp.data,(u_int8_t) pqp.char_PASS2[k],"LOCAL");
					fwrite(&pqp.data,1,1,fd);
					if(k==pqp.PASS-1)
						k=-1;
					k++;
				}
				break;
			case 1 : case 3 :
				pqp.data=0;
				i=0;
					if(prog_mode_en==3 && argc_s==5 && strcmp(argv_s[3],"-c")==0){
						printf("mesg :");
						if(fgets(pqp.mesg_chat,200,stdin)==NULL)
							fatal("while reading pqp.mesg_chat");
						clean(pqp.mesg_chat,stdin);
						
						stop_mesg=strlen(pqp.mesg_chat);
						tmp_file=ec_malloc(stop_mesg,sizeof(u_int32_t));
						
						for(i=0;i<stop_mesg;i++){
							pqp.mesg_chat[i]=manage_data("encrypt",pqp.mesg_chat[i],0,"NET");
							tmp_file[i]=pqp.mesg_chat[i];
							pqp.mesg_chat[i]='\0';
						}
						send_file(argv_s[4],stop_mesg,tmp_file,argv_s[3]);
					}
					else {
						if((fd = fopen(pqp.path_to_file,"rb+")) == NULL)
							fatal("in step2() while opening file");
					
						printf("Generating encrypted texte please wait... :\n");
				
						fseek(fd,0,SEEK_SET);
						for(i=0;i<size;i++){
							fseek(fd,i,SEEK_SET);
							if(fread(&pqp.data, 1,1, fd)!=1)
								fatal("while reading file");
							if(pqp.data>255){
								printf("Error data > 255 NOT in MAT range\n");
								exit(-1);
							}
							pqp.data=manage_data("encrypt",pqp.data,0,"NET");
							if(prog_mode_en==3 && argc_s!=5)
								tmp_file[i]=pqp.data;
							if(prog_mode_en==1){
								fseek(fd,i,SEEK_SET);
								if(fwrite(&pqp.data,1,1,fd)!=1)
									fatal("while writing datas in file");
							}
						}
						fclose(fd);
					}
			break;
			}
			switch(prog_mode_en){
				case 0 :
					strcpy(name,pqp.path_to_file);
					strcat(name,".pqp");
					rename(pqp.path_to_file,name);
					printf("file reanamed %s and password saved\n",name);
				break;
			}
		}
		else if(strcmp(argv_s[1],"decrypt")==0){
			i=0;
			switch(prog_mode_de){
				case 0 :
					if(argc_s==4){
						if(strcmp(argv_s[3],"-r")!=0)
							fatal("bad argument");
					}
					else {
						printf("Enter password for FILE:");
						if(fgets(pqp.char_PASS2,pqp.PASS,stdin)==NULL)
							fatal("while reading pqp.char_PASS2");
						clean(pqp.char_PASS2,stdin);
					}
					if((fd = fopen(pqp.path_to_file,"rb+")) == NULL)
						fatal("in step2() while opening file");
					
					printf("Text decrypting... :\n");
					i=0;
					k=0;
					fseek(fd,0,SEEK_SET);
					for(i=0;i<size;i++){
						if(fread(&pqp.data, 1,1, fd)!=1)
							fatal("while reading file");
						fseek(fd,i,SEEK_SET);
						pqp.data=manage_data("decrypt",pqp.data,(u_int8_t) pqp.char_PASS2[k],"LOCAL");
						fwrite(&pqp.data,1,1,fd);
						if(k==pqp.PASS-1)
							k=-1;
						k++;
					}
					strcpy(name_cut,pqp.path_to_file);
					len_cut=strlen(name_cut);
					for(i=0;i<5;i++){
						name_cut[len_cut-i]='\0';
					}
					rename(pqp.path_to_file,name_cut);
				break;
				case 1 :
					printf("Preparing matrice from net...\n");
					
					if((fd = fopen(pqp.path_to_file,"r+")) == NULL)
						fatal("in step2() while opening file");
					printf("Text decrypting... :\n");
					fseek(fd,0,SEEK_SET);
					for(i=0;i<size;i++){
						fseek(fd,i,SEEK_SET);
						if(fread(&pqp.data, 1,1, fd)!=1)
							fatal("while reading file");	
						
						if(pqp.data>255){
							printf("Error : data > 255");
							exit(-1);
						}
						pqp.data=manage_data("decrypt",pqp.data,0,"NET");
						fseek(fd,i,SEEK_SET);
						if(fwrite(&pqp.data,1,1,fd)!=1)
							fatal("while writing file");
					}
				break;
				case 2 :
					if(strcmp(argv_s[3],"1")==0){
						strcpy(path,pqp.RECV_FILE);
						sprintf(num_char_file,"%u",num_file);
						strncat(path,num_char_file,sizeof(char));
						if((fd=fopen(path,"w+"))==NULL)
							fatal("while opening receving file");
						fseek(fd,0,SEEK_SET);
						for(i=0;i<file_size;i++){
							fseek(fd,i,SEEK_SET);
							server_buffer[i]=manage_data("decrypt",server_buffer[i],0,"NET");
							if(fwrite(&server_buffer[i],1,1,fd)!=1)
								fatal("while writting file");
						}
					}
					else if(strcmp(argv_s[3],"2")==0){
						for(i=0;i<file_size;i++){
							server_buffer[i]=manage_data("decrypt",server_buffer[i],0,"NET");
							printf("%s",(char *) &server_buffer[i]);
						}
						printf("\n");
					}
					else fatal("argv 2 not match with '1' or '2'"); 
			}
		}		
}

void step_net(){
	FILE *fd=NULL;
	u_int32_t k=0,i=0,j=0;
	u_int32_t table_save[5][5]={{0},{0}};
	
	if((pqp.sockfd = socket(PF_INET, SOCK_STREAM,0)) == -1)
			fatal("in socket");
	
	if(setsockopt(pqp.sockfd, SOL_SOCKET, SO_REUSEADDR, &pqp.yes, sizeof(u_int32_t)) == -1)
		fatal("setting socket option SO_REUSEADDR");
			
	if(argc_s==4 && (strcmp(argv_s[3],"-r")==0))
		printf("\n");
	else {
		printf("Enter password for TABLE :");
		if(fgets(pqp.char_PASS3,pqp.PASS,stdin)==NULL)
			fatal("while reading pqp.char_PASS3");
		clean(pqp.char_PASS3,stdin);
	}
		if((fd=fopen(pqp.TABLE,"r"))==NULL){
				printf("Table doesn't exist creating it...\n");
			if((fd=fopen(pqp.TABLE,"w"))==NULL)
				fatal("while creating table file");
				
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
		else {
		i=0;
		j=0;
		k=0;
		
		while(fscanf(fd," %u ",&pqp.table[i][j])!=-1){
			j++;
			if(j==5){
				i+=1;
				j=0;
			}
		}
		manage_bridge("decrypt",0,0);
		}
		if(fclose(fd) == EOF)
			fatal("in step0() while closing file");
				
	if(strcmp(argv_s[1],"encrypt")==0){			
		switch(prog_mode_en){
		case 1 :
			wait_matrice();
		break;
		case 2:
			step1();
			manage_matrice("share");
		break;
		case 3 :
			wait_matrice();
			if(argc_s==5 && strcmp(argv_s[3],"-c")==0){
				while(1){
					step2();
				}
			}
			else if(argc_s==4){
				send_file(argv_s[3],0,tmp_file,"");
			}
		break;
		default :
			fatal("mode not supported");
		break;
		}
	}
	else if(strcmp(argv_s[1],"decrypt")==0){
		switch(prog_mode_de){
			case 1 :
				wait_matrice();
			break;
			case 2 :
				step1();
				while(1){
					if(pthread_create(&thread_net1,NULL,recv_file,NULL)!=0)
						fatal("while creating thread...");
					if(pthread_join(thread_net1,NULL)!=0)
						fatal("while join in thread...");
					close(pqp.sockfd);
					close(pqp.new_sockfd);
					step2();
					num_file++;
				}
			break;
			default : fatal("mode not supported");
		}
	}
}

void wait_matrice(){
	u_int32_t i=0;
	FILE *fd_net;
	char *erase_EOL=NULL;
			if((pqp.sockfd = socket(PF_INET, SOCK_STREAM,0)) == -1)
				fatal("in socket");
	
			if(setsockopt(pqp.sockfd, SOL_SOCKET, SO_REUSEADDR, &pqp.yes, sizeof(u_int32_t)) == -1)
				fatal("setting socket option SO_REUSEADDR");
			
			if((fd_net=fopen(pqp.IP_LIST,"r"))==NULL){
				printf("Error while reading ip_list : Are you launch PQPCS Installer ? If not, launch /path/to/PQPCS/install.sh and follow the instructions\n");
				printf("If the problem persist, contact author at TrEeSr00t@protonmail.com or your network admin ;)\n");
				exit(0);
			}
			if((fd_net=fopen(pqp.IP_LIST,"r"))==NULL)
				fatal("while opening ip file");

			for(i=0;i<pqp.nb_ip_in_list;i++){
	
				if(fgets(pqp.iplist,strlen("255.255.255.255\n"),fd_net)==NULL)
					fatal("while reading ip file");
				clean(pqp.iplist,fd_net);

				erase_EOL=strchr(pqp.iplist,'\n');
			
				if(erase_EOL!=NULL)
					erase_EOL=0;
			
				hostinfo=gethostbyname(pqp.iplist);
				
				if(hostinfo==NULL)
					printf("Client unreachable switch...\n");
				else {
				
					sin1.sin_addr = *(IN_ADDR *) hostinfo->h_addr;
					sin1.sin_port = htons(PORT_MAT);
					sin1.sin_family = AF_INET;
			
					printf("Client reachable : %s [%s] on ip=%s:%u\n",hostinfo->h_name,*hostinfo->h_aliases,pqp.iplist,PORT_MAT);

					if(connect(pqp.sockfd,(SOCKADDR *) &sin1, sizeof(SOCKADDR)) == -1){
						printf("Client busy/close/or not listening... (in wait_matrice):\n");
						//if(i==(pqp.nb_ip_in_list-1)){
						//printf("Nothing hosts available quitting program sorry...\n");
						free_struct();
						exit(-1);
						//}
					}
					else {
						manage_matrice("wait");
						close(pqp.sockfd);
						
						if(strcmp(argv_s[1],"encrypt")==0){
						switch(prog_mode_en){
							case 1 : case 3 :
								step2();
							break;
							default :
								fatal("bad mode in wait_matrice");
						}
					}
					else if(strcmp(argv_s[1],"decrypt")==0){
						switch(prog_mode_de){
							case 1 : case 2 :
								step2();
							break;
							default :
								fatal("bad mode in wait_matrice");
						}
					}
					}
					if(fclose(fd_net)==-1)
						fatal("while closing ip list file");
				}
			}
}

void recursive(u_int8_t way,u_int8_t it){
	DIR *dirp=NULL;
	char file_name[100]={'\0'};
	struct stat stat_file;

	if(it==0){
		if((dirp=opendir(pqp.path_to_file))==NULL)
			fatal("while opening encrypt directory");
		
		printf("Enter password for LOCAL matrice:");
		if(fgets(pqp.char_PASS1,pqp.PASS,stdin)==NULL)
			fatal("while reading pqp.char_PASS1");
		clean(pqp.char_PASS1,stdin);
		printf("Enter password for FILES:");
		if(fgets(pqp.char_PASS2,pqp.PASS,stdin)==NULL)
			fatal("while reading pap.char_PASS2");
		clean(pqp.char_PASS2,stdin);
		printf("Enter password for TABLE :");
		if(fgets(pqp.char_PASS3,pqp.PASS,stdin)==NULL)
			fatal("while reading pqp.char_PASS3");
		clean(pqp.char_PASS3,stdin);
	}	
		while((dir=readdir(dirp))!=NULL){
			strcpy(file_name,"");
			strcpy(file_name,pqp.path_to_file);
			strcat(pqp.path_to_file,"/");
			strcat(pqp.path_to_file,dir->d_name);
			
			stat(pqp.path_to_file, &stat_file);
			
			if(strcmp(dir->d_name,".")==0 || strcmp(dir->d_name,"..")==0)
				printf("-\n");
			else if(S_ISDIR(stat_file.st_mode))
				recursive(way,1);
			else{
				printf("%s\n",pqp.path_to_file);	
				if(strcmp(argv_s[1],"encrypt")==0 && strstr(pqp.path_to_file,".pqp")!=NULL){
						printf("Document is already encrypted switch...\n");
				}
				else if(strcmp(argv_s[1],"decrypt")==0 && strstr(pqp.path_to_file,".pqp")==NULL){
						printf("Document is not encrypted exit...\n");
				}
				else switch(way){
						case 0 :
							step1();
							step2();
						break;
						case 1 :
							step_net();
						break;
						default : fatal("bad recursive way\n");
					}
			}
			strcpy(pqp.path_to_file,file_name);
		}
		if(closedir(dirp)==-1)
			fatal("while closing encrypt directory");
}


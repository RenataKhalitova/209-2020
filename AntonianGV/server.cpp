
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#define PORT    5555
#define BUFLEN  1024


int   readFromClient(int fd, char *buf);
void  writeToClient (int fd, char *buf);
char * show (char**, int);
char * add (char**, int, char*, int);
char * del (char**, int, int);
char * search (char **, int, char*, int);
void itoa (int, char*);
void reverse (char*);
int kek (int);

void itoa(int n, char s[]) {
     int i, sign;
 
     if ((sign = n) < 0) 
         n = -n;          
     i = 0;
     do {     
         s[i++] = n % 10 + '0';   
     } while ((n /= 10) > 0);    
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
}
 void reverse(char s[])
 {
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }
 int kek (int N) {
	int k = 1;
	while (N/10) { 
		N = N/10;
		k++;
	}
	return k;
}
char * show (char** text, int N) {
	char * answer;
	int cur_answer = 0;
	int i;
	answer = (char*)malloc(N*1024+1);
	for (i = 0; i < N; i++) {
		strcpy (&answer[cur_answer], text[i]);
		cur_answer = cur_answer + strlen(text[i])+1;
		answer[cur_answer-1] = '\n';
	}
	answer[cur_answer] = '\0';
	return answer;
}
char* add (char** text, int N, char * adding, int paragraph) {
	char * answer;
	int i;	
	answer = (char*)malloc(25);
	for (i = N; i >= paragraph; i--) {
		strcpy (text[i], (const char*)text[i-1]);
	}
	strcpy (text[paragraph-1], (const char*)adding);
	strcpy (answer, "Done\n");
	return answer;
}
char * search (char** text, int N, char * string, int depth) {
	char * answer;
	int i, j, length, number = 0, noch, ilength;
	int haha = depth;
	int cur_number = 1;
	int cur_answer = 0;
	bool numbers[1024];
	length = strlen (string);
	answer = (char*)malloc(N*1024);
	answer[cur_answer] = '\0';
	if (depth < strlen(string)) {
		strcpy (answer, "Incorrect Depth\n");
		return answer;
	}
	for (i = 0; i < N; i++) {
		ilength = strlen(text[i]);
		for (j = 0; j < ilength; j++){
			numbers[j] = false;
			if (strncmp(&text[i][j], string, length) == 0) {
				number++;
				numbers[j] = true;
			}
		}
		noch = 1;
		for (j = 0; j < ilength; j++){
			if (numbers[j]) {
				itoa (i+1, &answer[cur_answer]);
				cur_answer = cur_answer + kek(i+1)+1;
				answer[cur_answer-1] = ' ';
				itoa (number, &answer[cur_answer]);
				cur_answer = cur_answer + kek(number)+1;
				answer[cur_answer-1] = ' ';
				itoa (noch, &answer[cur_answer]);
				cur_answer = cur_answer + kek(number)+1;
				answer[cur_answer-1] = ' ';
				noch++;
				if ((j >= (depth-length)/2 + (depth - length)%2)&&((j + length + (depth-length)/2) <= ilength)) strncpy (&answer[cur_answer],&text[i][j-(depth-length)/2-(depth-length)%2], depth);
				if ((j < (depth-length)/2 + (depth - length)%2)&&((j + length + (depth-length)/2) <= ilength)) strncpy (&answer[cur_answer],&text[i][0],depth);
				if ((j >= (depth-length)/2 + (depth - length)%2)&&((j + length + (depth-length)/2) > ilength)) strncpy (&answer[cur_answer],&text[i][ilength-depth], depth);
				//if ((j < (depth-length)/2 + (depth - length)%2)&&((j + length + (depth-length)/2) > ilength)) {
				if (depth > ilength) {
					strncpy (&answer[cur_answer],&text[i][0], ilength);
					depth = ilength;
				}
				cur_answer = cur_answer+depth+1;	
				depth = haha;
				answer[cur_answer-1] = '\n';
			}		
		}
		number = 0;
	}
	answer[cur_answer] = '\0';
	return answer;
}



char * del (char ** text, int N, int paragraph) {
	char * answer;
	int i;
	answer = (char*)malloc(25);
	for (i = paragraph-1; i < N-1; i++) {
		strcpy (text[i], text[i+1]);
	}
	strcpy (answer, "Done\n");
	return answer;
}

int  main (void)
{

    int     i, err, opt=1;
    int     sock, new_sock;
    fd_set  active_set, read_set;
    struct  sockaddr_in  addr;
    struct  sockaddr_in  client;
    char    buf[BUFLEN];
    socklen_t  size;
	char* answer;
	int str_length;
	char c = '0';
	int k, j, Par_Num = 0, length; 
	char ** text;
	int chislo;
	FILE* in;

   
    sock = socket (PF_INET, SOCK_STREAM, 0);
    if ( sock<0 ) {
        perror ("Server: cannot create socket");
        exit (EXIT_FAILURE);
    }
    setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char*)&opt,sizeof(opt));


    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    err = bind(sock,(struct sockaddr*)&addr,sizeof(addr));
    if ( err<0 ) {
        perror ("Server: cannot bind socket");
        exit (EXIT_FAILURE);
    }

   
    err = listen(sock,3);
    if ( err<0 ) {
        perror ("Server: listen queue failure");
        exit(EXIT_FAILURE);
    }

    
    FD_ZERO(&active_set);
    FD_SET(sock, &active_set);
	in = fopen ("text.txt","r");
	while (c != EOF) {
		c = fgetc(in);
		if (c == '\n') Par_Num++;
	}
	fclose(in);
	in = fopen ("text.txt","r");
	text = (char**)malloc(Par_Num*sizeof(char*));
	if (text == NULL) {
		printf ("Mem Error\n");
		exit(1);
	}
	for (k = 0; k < Par_Num; k++) {
		text[k] = (char*)malloc(1026);
	}
	for (k = 0; k < Par_Num; k++) {
		fgets (text[k], 1024, in);
		length = strlen(text[k]);
		if (text[k][length-1] == '\n'||text[k][length-1] == 13) text[k][length-1] = '\0';
		if (text[k][length-2] == '\n'||text[k][length-2] == 13) text[k][length-2] = '\0';
	}
	fclose(in);
   
    while (1) {
        
        read_set = active_set;
        if ( select(FD_SETSIZE,&read_set,NULL,NULL,NULL)<0 ) {
            perror("Server: select  failure");
            exit (EXIT_FAILURE);
        }

        
        for (i=0; i<FD_SETSIZE; i++) {
            if ( FD_ISSET(i,&read_set) ) {
                if ( i==sock ) {
                    
                    size = sizeof(client);
                    new_sock = accept(sock,(struct sockaddr*)&client,&size);
                    if ( new_sock<0 ) {
                        perror("accept");
                        exit (EXIT_FAILURE);
                    }
                    fprintf (stdout, "Server: connect from host %s, port %hu.\n",
                            inet_ntoa(client.sin_addr),
                            ntohs(client.sin_port));
                    FD_SET(new_sock, &active_set);
                } else {
                    
                    err = readFromClient(i,buf);
                    if ( err<0 ) {
                        
                        close (i);
                        FD_CLR(i,&active_set);
                    } else {
                        
                        if ( strstr(buf,"stop") ) {
                            close(i);
                            FD_CLR (i,&active_set);
                        } else {
							if (strcmp(buf, "quit") == 0 ) {
								answer = (char*)malloc(7);
								strcpy (answer, "Quit\n");
								puts (answer);
								free (answer);
								in = fopen ("text.txt","w");
								for (k = 0; k < Par_Num; k++) {
									fprintf (in, "%s\n", text[k]);
									free(text[k]);
								}
								free(text);
								fclose(in);
								return 0;
							}
							if (strncmp(buf, "show", 4) == 0) answer = show(text, Par_Num);
							if (strncmp(buf, "add", 3) == 0) {
								k = 3;
								while (buf[k] < '0'&&buf[k]>'9') k++;
								chislo = atoi(&buf[i]);
								k = k + kek(chislo)+1;
								while (buf[k] == ' ') k++;
								if (chislo < 0||chislo > Par_Num+1) {
									answer = (char*)malloc(22);
									strcpy (answer, "Incorrect number\n");
								}
								else {
									text = (char**)realloc(text, (Par_Num+1)*sizeof(char*));
									text[Par_Num] = (char*)malloc(1026);
									answer = add (text, Par_Num, &buf[k], chislo);
									Par_Num++;
								}
							}
							if (strncmp(buf, "search", 6) == 0) {
								k = 6;
								while (buf[k] == ' ') k++;
								chislo = atoi(&buf[k]);
								k = k + kek(chislo) + 1;
								while (buf[k] == ' ') k++;
								answer = search (text, Par_Num, &buf[k], chislo);
							}
							if (strncmp(buf, "delete", 6) == 0) {
								k = 6;
								while (buf[k] < '0'&&buf[k]>'9') k++;
								chislo = atoi(&buf[k]);
								if (chislo < 0||chislo > Par_Num) {
									answer = (char*)malloc(22);
									strcpy (answer, "Incorrect number\n");
								}
								else {
									answer = del (text, Par_Num, chislo);
									free (text[Par_Num-1]);
									text = (char**)realloc(text, (Par_Num-1)*sizeof(char*));
									Par_Num--;
								}
							}
							if (answer == NULL) {
								answer = (char*)malloc(20);
								strcpy (answer, "Incorrect request\n");
							}
							
                            writeToClient(i,answer);
							free (answer);
							answer = NULL;
                        }
                    }
                }
            }
        }
    }
	return 0;
}



int  readFromClient (int fd, char *buf)
{
    int  nbytes;

    nbytes = read(fd,buf,BUFLEN);
    if ( nbytes<0 ) {
        // ошибка чтения
        perror ("Server: read failure");
        return -1;
    } else if ( nbytes==0 ) {
        // больше нет данных
        return -1;
    } else {
        // есть данные
        fprintf(stdout,"Server got message: %s\n",buf);
        return 0;
    }
}



void  writeToClient (int fd, char *buf)
{
    int  nbytes;
    unsigned char *s;
	nbytes = strlen(buf);
    //for (s=(unsigned char*)buf; *s; s++) *s = toupper(*s);
	write (fd, &nbytes, 4);
    nbytes = write(fd,buf,strlen(buf)+1);
    fprintf(stdout,"Write back: %s\nnbytes=%d\n",buf,nbytes);
    
    if ( nbytes<0 ) {
        perror ("Server: write failure");
    }
}


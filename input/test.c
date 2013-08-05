/*
 * =====================================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/05/2013 07:06:00 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  billowkiller (), billowkiller@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include<stdio.h>
#include<pthread.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>

#define MAXWORD 50

pthread_t ntid;

void printids(const char *s){
pid_t pid;
  pthread_t tid;

  pid = getpid();
  tid = pthread_self();
  printf("%s pid %u tid %u (0x%x)\n",s,(unsigned int)pid,(unsigned int)tid,(unsigned 
int)tid);
}

void* input(void *arg)
{

	char buf[MAXWORD];
	while(1)
	{
		 printf("\n\n");
        printf("/*************************************************************\\\n");
        printf("/* Please follow the command below to add rules and keywords. *\\\n");
        printf("/*-----------------------------------------------------------*\\\n");
        printf("/* RULE < userid group objectid group type resouceid group *\\\n");
        printf("/* KEYWORD < keyword *\\\n");
        printf("/* help *\\\n");
        printf("/*************************************************************\\\n\n\n");

		bzero(buf, MAXWORD);
		scanf("%s", buf);
		if(!strcmp(buf, "RULE"))
		{
			scanf("%s\n", buf);
			printf("buf");
		}
		else if(!strcmp(buf, "KEYWORD"))
		{
			scanf("%s\n", buf);
			printf("buf");
		}
		else if(!strcmp(buf, "help"))
		{
			printf("\n\ntype:\n");
			printf("FRIEND 1\n");
			printf(" STATUS 2\n");
			printf(" NOTE 3\n");
			printf(" COMMENT 4\n");
			printf(" PHOTO 5\n");
			printf(" MEDIA_SET 6\n");
			printf(" ADD_FRIEND 7\n");
			printf(" EDIT_NOTE 8\n");
		}
	}
  return ((void *)0);
}
void *thr_fn(void *arg){
  
  return ((void *)0);
}

int main(){
  int err;

  //err = pthread_create(&ntid,NULL,thr_fn,NULL);
  err = pthread_create(&ntid,NULL,input,NULL);
  if(err != 0){
   printf("can't create thread: %s\n",strerror(err));
   return 1;
  }

  while(1)
	  printf("nihao");
  return 0;
}

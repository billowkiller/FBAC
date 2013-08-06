/*
 * =====================================================================================
 *
 *       Filename:  input.c
 *
 *    Description:  test input UI
 *
 *        Version:  1.0
 *        Created:  08/04/2013 11:06:11 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  billowkiller (), billowkiller@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "input.h"
#define PS(X) printf(#X " = %s\n", X ) //PR(str)

inline void error()
{
	printf("ERROR: FORMAT!\n\n");
}

int fetch_id(char *id, char *nid, char *uid)
{
	Py_Initialize();
	if (!Py_IsInitialized())
    {
        printf("初始化错误\n");
        return 0;
    }
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('./input/')");

    PyObject* pModule = NULL;
    PyObject* pFunc = NULL;
    PyObject* pArg = NULL;
    PyObject* pRetVal = NULL;

    pModule = PyImport_ImportModule("fetch");
    pFunc = PyObject_GetAttrString(pModule,"fetchID");

    pArg = Py_BuildValue("(s)", id);
    pRetVal = PyObject_CallObject(pFunc, pArg);

    char *str = PyString_AsString(pRetVal);
	char *delim = strchr(str, ' ');
	memcpy(nid, str, delim-str);
	strcpy(uid, delim+1);
    Py_Finalize();

	return 1;
}

static char * add_db(char *id, char *group, int is_user)
{
	char delims[] = ",";
	char *result = NULL;
	result = strtok(id, delims );
	if(is_user)
	{
		while( result != NULL ) 
		{
			if(result[0] == '*')
			{
				insert_user(result, result, group);
				break;
			}
			char nid[MAXWORD], uid[MAXWORD];
			bzero(nid, MAXWORD);
			bzero(uid, MAXWORD);
			
			fetch_id(result, nid, uid);
			//insert db
			insert_user(uid, nid, group);
			result = strtok( NULL, delims);
		}
	}
	else
	{
		while( result != NULL ) 
		{
			//insert db
			insert_res(result, group);
			result = strtok( NULL, delims );
		}
	}
}

static void add_rule()
{
	char group1[MAXWORD];
	char group2[MAXWORD];
	char group3[MAXWORD];
	char userid[MAXWORD];	
	char subjectid[MAXWORD];	
	char resourceid[MAXWORD];	
	char type;
	int type_num;
	sprintf(group1, "%ld", time(NULL));
	strcpy(group2, group1);
	strcat(group2, "u");
	strcpy(group3, group1);
	strcat(group3, "o");
	bzero(subjectid, MAXWORD);
	scanf("%s %s %c %s", userid, subjectid, &type, resourceid);
	if(type == '*')
		type_num = 0;
	else
		type_num = type - '0';
	printf("RULE OK\n");
	printf("%s %s %s\n", userid, subjectid, resourceid);
	if(resourceid[0] != '*')
		add_db(resourceid, group1, 0);
	else
		strcpy(group1, "*");
	printf("1\n");
	if(userid[0] != '*')
		add_db(userid, group2, 1);
	else
		strcpy(group2, "*");
	printf("2\n");
	printf("subjectid = %s\n", subjectid);
	if(subjectid[0] != '*')
		add_db(subjectid, group3, 1);
	else
		strcpy(group3, "*");
	printf("3\n");
    //add_rule	
	insert_rule(group2, group3, type_num, group1);
	printf("4\n");
}

void add_kw()
{
	char kw[MAXWORD];
	scanf("%s", kw);
	add_kw_list(kw);	
	FILE *file = fopen("keywords", "a");
	fprintf(file, "%s\n", kw);
	fclose(file);

}

void* input(void *arg)
{

	char buf[MAXWORD];
	while(1)
	{
		 printf("\n\n");
        printf("/*************************************************************\\\n");
        printf(" Please follow the command below to add rules and keywords. \n");
        printf(" -----------------------------------------------------------\n");
        printf(" RULE < userid subjectid type resouceid \n");
        printf(" KEYWORD < keyword \n");
        printf(" help \n");
        printf("/*************************************************************\\\n\n");

		bzero(buf, MAXWORD);
		scanf("%s", buf);
		if(!strcmp(buf, "RULE"))
		{
			scanf("%s", buf);
			add_rule();
		}
		else if(!strcmp(buf, "KEYWORD"))
		{
			scanf("%s", buf);
			add_kw();
		}
		else if(!strcmp(buf, "help"))
		{
			printf("\n\nTYPE:\n");
			printf("	 FRIEND 1\n");
			printf("	 STATUS 2\n");
			printf("	 NOTE 3\n");
			printf("	 COMMENT 4\n");
			printf("	 PHOTO 5\n");
			printf("	 MEDIA_SET 6\n");
			printf("	 ADD_FRIEND 7\n");
			printf("	 EDIT_NOTE 8\n");
		}
	}
}

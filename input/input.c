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

#include <stdio.h>
#include <Python.h>
#include <string.h>
#define MAXWORD 50

extern void insert_user(char*, char*, char*);
extern void insert_res(char*, char*);
extern void insert_rule(char*, char*, int, char*);

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
	PyRun_SimpleString("sys.path.append('./')");

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
	nid[delim-str+1] = '\0';
	strcpy(uid, delim+1);
	uid[strlen(uid)] = '\0';
    Py_Finalize();

	return 1;
}

//void add_db(char *id, char *group, int is_user)
//{
//	char delims[] = ",";
//	char *result = NULL;
//	result = strtok(id, delims );
//	if(is_user)
//	{
//		while( result != NULL ) 
//		{
//			char nid[MAXWORD], uid[MAXWORD];
//			fetch_id(result, nid, uid);
//			//insert db
//			insert_user(uid, nid, group);
//			result = strtok( NULL, delims );
//		}
//	}
//	else
//	{
//		while( result != NULL ) 
//		{
//			//insert db
//			insert_res(result, group);
//			result = strtok( NULL, delims );
//		}
//	}
//}

void add_rule()
{
	char userid[MAXWORD];	
	char group1[MAXWORD];	
	char objectid[MAXWORD];	
	char group2[MAXWORD];	
	char resourceid[MAXWORD];	
	char group3[MAXWORD];	
	int type;
	scanf("%s", userid);
	scanf("%s", group1);
	scanf("%s", objectid);
	scanf("%s", group3);
	scanf("%d", &type);
	scanf("%s", resourceid);
	scanf("%s", group3);
	add_db(userid, group1, 1);
	add_db(objectid, group2, 1);
	add_db(resourceid, group3, 0);
    //add_rule	
	insert_rule(group1, group2, type, group3);
}

void add_kw()
{
	char kw[MAXWORD];
	scanf("%s", kw);
	FILE *file = fopen("../keywords", "a");
	fprintf(file, "%s\n", kw);
	fclose(file);
}

int input()
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
			add_rule();
		}
		else if(!strcmp(buf, "KEYWORD"))
		{
			scanf("%s\n", buf);
			add_kw();
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
}

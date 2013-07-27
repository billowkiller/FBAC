#include <stdio.h>
#include <sqlite3.h>
#include <string.h>
#define MAXL 500
#define sMAX 1000

char** query(char *sql, int* nrow, int* ncolumn, sqlite3* db)
{
    char *zErrMsg;
    char **azResult;
    sqlite3_get_table(db, sql, &azResult, nrow, ncolumn, &zErrMsg);
    //fprintf(stderr,"%s\n",zErrMsg);
    return azResult;
}

int exeNonQuery(char* sql, sqlite3* db)//return 0 if Ok
{
    char *zErrMsg;
    int r=sqlite3_exec(db, sql, 0, 0, &zErrMsg);
    return r;
}
int findv_rL(char* res_type, char* sub_role, int res_type_num, char* res_from_id, char* res_from_role, char * res_tag, sqlite3* db)
{
    char selectStr[sMAX]="select * from v_roleList";
    char* wh=" where";

    //if(sub_role||res_type||res_from_id||res_from_role||res_tag)
    //{

        strcat(selectStr,wh);
    //}
    if(sub_role)
    {
        char role[2*MAXL]=" sub_role=";
        char sub_r[MAXL]="(select role from user_roleList where id='";
        strcat(sub_r,sub_role);
        strcat(sub_r,"')");
        strcat(role,sub_r);
        strcat(role," and");
        strcat(selectStr,role);
    }
    if(res_type_num)
    {
        char type[MAXL]=" res_type_num=";
        char tp[10];
        sprintf(tp,"%d",res_type_num);

        strcat(type,tp);
        strcat(type," and");
        strcat(selectStr,type);
    }
    if(res_from_id)
    {
        char from_id[MAXL]=" res_from_id='";
        strcat(from_id,res_from_id);
        strcat(from_id,"' and");
        strcat(selectStr,from_id);
    }
    if(res_from_role)
    {
        char from_role[2*MAXL]=" res_from_role=";
        char from_r[MAXL]="(select tag from resource_tagList where id='";
        strcat(from_r,res_from_role);
        strcat(from_r,"')");
        strcat(from_role,from_r);
        strcat(from_role," and");
        strcat(selectStr,from_role);
    }
    if(res_tag)
    {
        char tag[2*MAXL]=" res_tag=";
        char res_t[MAXL]="(select tag from resource_tagList where id='";
        strcat(res_t,res_tag);
        strcat(res_t,"')");
        strcat(tag,res_t);
        strcat(tag," and");
        strcat(selectStr,tag);
    }

    int len=strlen(selectStr)-4;
    *(selectStr+len)='\0';
    //printf("%d\n",strlen(selectStr));
    //printf("%s\n",selectStr);
    int row=0,column;
    query(selectStr,&row,&column,db);
    return row;
}

int finda_rL(char*res_type, char* sub_role,int res_type_num, char* res_from_id, char* res_from_role, char * action, sqlite3* db)
{
    char selectStr[sMAX]="select * from a_roleList";
    char* wh=" where";

    if(sub_role||res_type||res_from_id||res_from_role||action)
    {

        strcat(selectStr,wh);
    }
    if(sub_role)
    {
        char role[2*MAXL]=" sub_role=";
        char sub_r[MAXL]="(select role from user_roleList where id='";
        strcat(sub_r,sub_role);
        strcat(sub_r,"')");
        strcat(role,sub_r);
        strcat(role," and");
        strcat(selectStr,role);
    }
    if(res_type_num)
    {
        char type[MAXL]=" res_type_num=";
        char tp[10];
        sprintf(tp,"%d",res_type_num);

        strcat(type,tp);
        strcat(type," and");
        strcat(selectStr,type);
    }
    if(res_from_id)
    {
        char from_id[MAXL]=" res_from_id='";
        strcat(from_id,res_from_id);
        strcat(from_id,"' and");
        strcat(selectStr,from_id);
    }
    if(res_from_role)
    {
        char from_role[2*MAXL]=" res_from_role=";
        char from_r[MAXL]="(select tag from resource_tagList where id='";
        strcat(from_r,res_from_role);
        strcat(from_r,"')");
        strcat(from_role,from_r);
        strcat(from_role," and");
        strcat(selectStr,from_role);
    }
    if(action)
    {
        char act[MAXL]=" action='";
        strcat(act,action);
        strcat(act,"' and");
        strcat(selectStr,act);
    }

    int len=strlen(selectStr)-4;
    *(selectStr+len)='\0';
    int row=0,column;
printf("selectStr = %s\n", selectStr);
    query(selectStr,&row,&column,db);
    return row;
}

int findt(char* rid, char* sid, char* res_id, int res_type_num, char* table, sqlite3* db)
{
    char selectStr[sMAX]="select * from ";
    strcat(selectStr,table);
    char* wh=" where";
    strcat(selectStr,wh);
    if(sid)
    {
        char role[2*MAXL]=" (sub_role=";
        char sub_r[MAXL]="(select role from user_roleList where id='";
        strcat(sub_r,sid);
        strcat(sub_r,"')");
        strcat(role,sub_r);
        strcat(role," or sub_role='*')");
        strcat(role," and");
        strcat(selectStr,role);
    }
    if(rid)
    {
        char from_id[MAXL]=" (res_from_id='";
        strcat(from_id,rid);
        strcat(from_id,"' or res_from_id='*')");
        strcat(from_id," and");
        strcat(selectStr,from_id);

        char from_role[2*MAXL]=" (res_from_role=";
        char from_r[MAXL]="(select role from user_roleList where id='";
        strcat(from_r,rid);
        strcat(from_r,"')");
        strcat(from_role,from_r);
        strcat(from_role," or res_from_role='*')");
        strcat(from_role," and");
        strcat(selectStr,from_role);
    }
    if(table=="v_roleList"&&res_id)
    {
        char type[2*MAXL]=" (res_tag=";
        char res_tag[MAXL]="(select tag from resource_tagList where id='";
        strcat(res_tag,res_id);
        strcat(res_tag,"')");
        strcat(type,res_tag);
        strcat(type," or res_tag='*')");
        strcat(type," and");
        strcat(selectStr,type);
    }
	char type_n[2*MAXL]=" (res_type_num=";
	char tp[10];
	sprintf(tp,"%d",res_type_num);

	strcat(type_n,tp);
	strcat(type_n," or res_type_num=0)");
	strcat(type_n," and");
	strcat(selectStr,type_n);

    int len=strlen(selectStr)-4;
    *(selectStr+len)='\0';
    int row=0,column;
    query(selectStr,&row,&column,db);
    return row;
}

int find_db(char* rid, char* sid,  int res_type_num, char* res_id, sqlite3* db)
{
    int a=0,v=0;
    a=findt(rid, sid, res_id, res_type_num, "a_roleList", db);
    v=findt(rid, sid, res_id, res_type_num, "v_roleList", db);
    return a+v;
}

int iskeyword(char* word, sqlite3* db)//if(word=='\0') return 0
{
    if(word[0] == '\0')
		return 0;
    //select * from key_word where string = 38@word
    char select[100];
    strcpy(select,"select * from key_word where string = '");
    strcat(select,word);
    strcat(select,"'");
    //printf("select:%s len:%d,strlen:%d",select,len,strlen(select));
    int row=0,column;
    query(select,&row,&column,db);
    return row;
}

//int main( )
//{
//    char* dbpath="/home/wutao/FBAC/config/fbac.db";
//    sqlite3 *db=NULL;
//    char *zErrMsg = 0;
//    int rc;
//    rc = sqlite3_open(dbpath, &db);
//    if( rc )
//    {
//        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
//        sqlite3_close(db);
//        return 0;
//    }
//
//   // int n=find_db("barackobama", "100000179669235", 8, "\0",db);
//	int n = iskeyword("Obama", db);
//    printf("%d\n",n);
//	sqlite3_close(db); //close database
//    return 0;
//}



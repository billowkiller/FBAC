#include <stdio.h>
#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>

extern sqlite3 *db;

char** query(char *sql, int* nrow, int* ncolumn)
{
    char *zErrMsg;
    char **azResult;
    sqlite3_get_table(db, sql, &azResult, nrow, ncolumn, &zErrMsg);
    //fprintf(stderr,"%s\n",zErrMsg);
    return azResult;
}

int exeNonQuery(char* sql)//return 0 if Ok
{
    char *zErrMsg;
    int r=sqlite3_exec(db, sql, 0, 0, &zErrMsg);
    return r;
}

int find_db(char* sid, char* rfid, int res_type_num, char* rid)
{
    //select * from  rule_list where (res_type_num= or res_type_num=0) and (sub_group=(select ugroup from user_group where uid='') or sub_group='*') and (res_from_group=(select ugroup from user_group where nid='') or res_from_group='*') and (res_group=(select rgroup from res_group where id='') or res_group='*')
	int len=369;
    char buf[10];
    sprintf(buf,"%d",res_type_num);
    len=len+strlen(buf)+strlen(sid)+2*strlen(rfid)+strlen(rid);
    char sql[len];
    memset(sql,0,sizeof(sql));
    strcpy(sql,"select * from  rule_list where (res_type_num=");
    strcat(sql,buf);
    strcat(sql," or res_type_num=0) and (sub_group in (select ugroup from user_group where uid='");
    strcat(sql,sid);
    strcat(sql,"' or uid='*') or sub_group='*') and (res_from_group in (select ugroup from user_group where nid='");
    strcat(sql,rfid);

    strcat(sql,"' or uid='");
    strcat(sql,rfid);


    strcat(sql,"' or nid='*' or uid='*') or res_from_group='*') and (res_group in (select rgroup from res_group where id='");
    strcat(sql,rid);
    strcat(sql,"' or id='*') or res_group='*')");
    int row=0,column;
    //printf("%s len:%d\n",sql,strlen(sql));
    query(sql,&row,&column);
    return row;
}
void insert_user(char* uid, char* nid ,char* group)
{
    //insert into user_group (uid,ugroup,nid) values ('','','')

    int len=58;
    len=len+strlen(uid)+strlen(nid)+strlen(group);
    char sql[len];
    memset(sql,0,sizeof(sql));
    strcpy(sql,"insert into user_group (uid,ugroup,nid) values ('");
    strcat(sql,uid);
    strcat(sql,"','");
    strcat(sql,group);
    strcat(sql,"','");
    strcat(sql,nid);
    strcat(sql,"')");
    exeNonQuery(sql);

}

void insert_res(char* id, char* group)
{
    //insert into res_group (id,rgroup) values ('','')
    int flag=0;
    int len=49;
    len=len+strlen(id)+strlen(group);
    char sql[len];
    memset(sql,0,sizeof(sql));
    strcpy(sql,"insert into res_group (id,rgroup) values ('");
    strcat(sql,id);
    strcat(sql,"','");
    strcat(sql,group);
    strcat(sql,"')");
    exeNonQuery(sql);
}

void insert_rule(char* sg, char* rfg, int type, char* rg)
{
    //insert into rule_list (res_type_num,sub_group,res_from_group,res_group) values (,'','','')
    int len=91;
    char buf[10];
    sprintf(buf,"%d",type);
    len=len+strlen(buf)+strlen(sg)+strlen(rfg)+strlen(rg);
    char sql[len];
    memset(sql,0,sizeof(sql));
    strcpy(sql,"insert into rule_list (res_type_num,sub_group,res_from_group,res_group) values (");
    strcat(sql,buf);
    strcat(sql,",'");
    strcat(sql,sg);
    strcat(sql,"','");
    strcat(sql,rfg);
    strcat(sql,"','");
    strcat(sql,rg);
    strcat(sql,"')");
    exeNonQuery(sql);
}

static void _cleartable(char* table)
{
    //delete from
    int len=13+strlen(table);
    char sql[len];
    strcpy(sql,"delete from ");
    strcat(sql,table);
    exeNonQuery(sql);
}

void clear_db_table()
{
    _cleartable("res_group");
    _cleartable("user_group");
    _cleartable("rule_list");
}

//sqlite3 *db=NULL;
//int main( )
//{
//    char* dbpath="/home/wutao/FBAC/config/fbac.db";
//
//    char *zErrMsg = 0;
//    int rc;
//    //open the database file.If the file is not exist,it will create a file.
//    rc = sqlite3_open(dbpath, &db);
//    if( rc )
//    {
//        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
//        sqlite3_close(db);
//        return 0;
//    }
//
//	insert_res("a.135774426629277.1073741827.100005901611", "a");
//    sqlite3_close(db); //close database
//    return 0;
//
//}

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

int find_db(int res_type_num, char* sid, char* rfid, char* rid)//sid为访问者id ,被访问者rfid为输入name,rid为resourceid
{
    //select * from  rule_list where (res_type_num= or res_type_num=0) and (sub_group=(select ugroup from user_group where uid='') or sub_group='*') and (res_from_group=(select ugroup from user_group where nid='') or res_from_group='*') and (res_group=(select rgroup from res_group where id='') or res_group='*')
    int len=307;
    char buf[10];
    sprintf(buf,"%d",res_type_num);
    len=len+strlen(buf)+strlen(sid)+strlen(rfid)+strlen(rid);
    char sql[len];
    memset(sql,0,sizeof(sql));
    strcpy(sql,"select * from  rule_list where (res_type_num=");
    strcat(sql,buf);
    strcat(sql," or res_type_num=0) and (sub_group=(select ugroup from user_group where uid='");
    strcat(sql,sid);
    strcat(sql,"') or sub_group='*') and (res_from_group=(select ugroup from user_group where nid='");
    strcat(sql,rfid);
    strcat(sql,"') or res_from_group='*') and (res_group=(select rgroup from res_group where id='");
    strcat(sql,rid);
    strcat(sql,"') or res_group='*')");
    int row=0,column;
    //printf("%s\n",sql);
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
void insert_rule(int type, char* sg, char* rfg, char* rg)
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
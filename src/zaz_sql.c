/*-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-
 ^    _/_/_/_/                              _/      _/  _/    _/  _/_/_/  ^
 ^   _/        _/_/      _/_/_/  _/  _/_/  _/_/  _/_/  _/    _/  _/    _/ ^
 ^  _/_/_/  _/_/_/_/  _/    _/  _/_/      _/  _/  _/  _/    _/  _/    _/  ^
 ^ _/      _/        _/    _/  _/        _/      _/  _/    _/  _/    _/   ^
 ^_/        _/_/_/    _/_/_/  _/        _/      _/    _/_/    _/_/_/      ^
 ^------------------------------------------------------------------------^
 ^ fearMUD Copyright 2001-2004 by the fearMUD team:                       ^
 ^ Philip Jackson (Zazel): t3hphil@gmail.com		                  ^
 ^-----------------------------------------------------------------------*/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "/usr/include/mysql/mysql.h"
#include "merc.h"
#include "memwatch.h"

int sql_connect(char *host, char *port, char *database, char *user, char *password) 
{
	mysql_init(&mysql);
	if (!(mysql_real_connect(&mysql,host,user,password, database, 0, NULL, 0)))  {
		//log_string(LOG_ERR, "MYsql connect error");
		return 0;
	}
	if (mysql_select_db(&mysql,database)) {
		//log_string(LOG_ERR, "Mysql database select problem");
		return 0;
	}
	return 1;

}

void sql_disconnect()
{
    if (res != NULL) {
	mysql_free_result(res);
	res = NULL;
    }
    mysql_close(&mysql);
}



int sql_execute(char *query)
{
char blarg[MSL];
    if (mysql_real_query(&mysql, query, strlen(query)) != 0) {
	    xprintf(blarg, "sql_execute error on: %s", query);
//        wiznet(blarg,NULL,NULL, WIZ_SQL, 0, 12);
	log_string(LOG_GAME, blarg);

	return 0;
    }
    return 1;
}

int sql_query(char *query)
{

	if (res != NULL) {
		mysql_free_result(res);
		res = NULL;
	}
	if (!(mysql_query(&mysql, query)))
		//log_string(LOG_ERR, "mysql query error");
	if (!(res = mysql_store_result(&mysql))) {
		//log_string(LOG_ERR, "mysql store result error");
		res = NULL;
		return 0;
	}
	return 1;
}

SQL_ROW sql_fetch_row()
{

    return mysql_fetch_row(res);
}

int sql_num_rows()
{
    if (res)
    	return mysql_num_rows(res);
    else
	return 0;
}

int sql_num_fields()
{
    if (res)
    	return mysql_num_fields(res);
    else
	return 0;
}


// sql example
int main1()
{
	int i;
	char buf[200];
	if (!sql_query("select * from ban"))
		//log_string(LOG_ERR, "sql query error");
	while((row = sql_fetch_row())) {
		for (i=0 ; i <sql_num_fields(); i++)  {
			xprintf(buf, "%s\n",row[i]);
			//log_string(LOG_ERR, buf);
		}
	}
	return 0;
}


		

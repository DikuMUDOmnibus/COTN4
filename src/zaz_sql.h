
/*-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-
 ^    _/_/_/_/                              _/      _/  _/    _/  _/_/_/  ^
 ^   _/        _/_/      _/_/_/  _/  _/_/  _/_/  _/_/  _/    _/  _/    _/ ^
 ^  _/_/_/  _/_/_/_/  _/    _/  _/_/      _/  _/  _/  _/    _/  _/    _/  ^
 ^ _/      _/        _/    _/  _/        _/      _/  _/    _/  _/    _/   ^
 ^_/        _/_/_/    _/_/_/  _/        _/      _/    _/_/    _/_/_/      ^
 ^------------------------------------------------------------------------^
 ^ fearMUD Copyright 2001-2004 by the fearMUD team:                       ^
 ^ Philip Jackson (Zazel): bigphilwastaken@hotmail.com                    ^
 ^-----------------------------------------------------------------------*/
 	   	

#include <stdio.h>
#include <string.h>
#include "/usr/include/mysql/mysql.h"

static  MYSQL mysql;
static  MYSQL_RES *res;
static  MYSQL_ROW row;

int sql_connect(char *host, char *port, char *database, char *user, char *password);
int sql_execute(char *query);
int sql_query(char *query);
void sql_disconnect();

typedef MYSQL_ROW SQL_ROW;

SQL_ROW sql_fetch_row();
int sql_num_rows();
int sql_num_fields();


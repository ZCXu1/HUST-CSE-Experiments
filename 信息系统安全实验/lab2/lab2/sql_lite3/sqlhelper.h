#ifndef SQLHELPER_H
#define SQLHELPER_H

#include "sqlite3.h"

int handle_db(sqlite3 *db, const char* sql);

int open_db();

void init_db();

int use_db(const char* u_name, const char* u_passwd, int Login);
int register_db(const char* u_name, const char* u_passwd);
int select_db(const char* u_name, const char* u_passwd, int Login);
int handle_db(sqlite3 *db, const char* sql);

#endif

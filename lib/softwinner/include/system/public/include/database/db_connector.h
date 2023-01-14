#ifndef _DBCon_H
#define _DBCon_H
#include <sqlite3.h>
#include <stdio.h>

#define ERROR(f, l, x) LOGE("%s() [%d], code=%d errmsg=%s\n", f, l, sqlite3_errcode(x), sqlite3_errmsg(x));
namespace EyeseeLinux {

typedef struct sqlite3 SQLCon;

class DBCon
{
public:
	bool open(const char *dbName);
	bool close(void);
	SQLCon* getConnect(void);
	bool lock(void);
	void unlock(void);
	static DBCon* getInstance(const char *dbName);
	void freeInstance();
private:
	DBCon(const char *dbName);
	~DBCon();
	static DBCon* mDBCon;
	SQLCon *mSQLCon;
	bool mLock;
};
} /* EyeseeLinux */
#endif //_DBCon_H

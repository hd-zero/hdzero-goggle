#ifndef _DBCTRL_H
#define _DBCTRL_H
#include <database/db_connector.h>
#include <sqlite3.h>
#include <string>

namespace EyeseeLinux {

#define DB_PATH "/tmp/sqlite"
#define DB_NAME "sunxi.db"
#define DB_FILE DB_PATH "/" DB_NAME

typedef enum
{
	DB_UINT32 = 1,
	DB_INT64,
	DB_UINT16,
	DB_UINT8,
	DB_TEXT,
	DB_BLOB,
	DB_FLOAT,
	DB_DOUBLE,
	DB_DATETIME
}DBC_TYPE;

struct sql_tb
{
    char item[32];
    char type[32];
};

class DBCtrl
{
public:
	DBCtrl();
	~DBCtrl();
    int init();
	void setSQL(const std::string sql);
    std::string getSQL(void) const;
    std::string getTableName(void) const;
	int insertRecord(void);
	int bufPrepare(void);
	void bufRelease(void);
	int prepare(void);
	int finish(void);
	int deleteRecord(void);
	int executeSQL(void);
	int sqlite3Exec(void);
	unsigned int getRecordCnt(void);
	int getOneLine(void *pValue[]);
	int getFilterFirstLine(void *pValue[]);
	void * getFilterFirstElement(std::string column, int idx);
	void *getElement(std::string columnName, int columnIdx, unsigned int rowNumber);
	int createTable(std::string tbName, struct sql_tb *tb, int cnt);
	void setRecord(std::string columnName, void *pValue);
	void setColumnType(DBC_TYPE *type, int *typeSize);
	void setFilter(const std::string filter);
	int deleteTable(std::string tbName);
private:
	void setTableName(const std::string name);
	void setColumnCnt(const int num);
	void createRequerySQL(void);
	void createAddSQL(void);
    std::string mName;
    std::string mSQL;
	int mColumnCnt;
	int mTmpNum;
	DBC_TYPE *mColumnType;
	int *mColumnTypeSize;
	unsigned int *mValue;
    std::string *mColumnName;
	unsigned char *mFieldKey;
	SQLCon *mSQLCon;
	DBCon *mDBCon;
	sqlite3_stmt *mSqlstmt;
    std::string mFilter;

	unsigned int	result_uint32;
	long long		result_int64;
	unsigned short	result_uint16;
	unsigned char	result_uint8;
	float			result_float;
	double			result_double;
	char (*queryBuf)[100];
};
}
#endif //DBCtrl

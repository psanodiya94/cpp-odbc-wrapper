#ifndef ODBC_EXECUTOR_H
#define ODBC_EXECUTOR_H

#include <odbccpp/odbcinterface.h>

namespace ps {
    namespace odbc {
        class OdbcExecutor : public OdbcInterface {
        public:
            SQLRETURN SQLAllocHandle(
                SQLSMALLINT HandleType,
                SQLHANDLE InputHandle,
                SQLHANDLE* OutputHandle
            ) override;

            SQLRETURN SQLSetEnvAttr(
                SQLHENV EnvironmentHandle,
                SQLINTEGER Attribute,
                SQLPOINTER Value,
                SQLINTEGER StringLength
            ) override;

            SQLRETURN SQLConnect(
                SQLHDBC ConnectionHandle,
                SQLWCHAR* ServerName,
                SQLSMALLINT NameLength1,
                SQLWCHAR* UserName,
                SQLSMALLINT NameLength2,
                SQLWCHAR* Authentication,
                SQLSMALLINT NameLength3
            ) override;

            SQLRETURN SQLDisconnect(
                SQLHDBC ConnectionHandle
            ) override;

            SQLRETURN SQLFreeHandle(
                SQLSMALLINT HandleType,
                SQLHANDLE Handle
            ) override;

            SQLRETURN SQLExecDirect(
                SQLHSTMT StatementHandle,
                SQLWCHAR* StatementText,
                SQLINTEGER TextLength
            ) override;

            SQLRETURN SQLNumResultCols(
                SQLHSTMT StatementHandle,
                SQLSMALLINT* ColumnCount
            ) override;

            SQLRETURN SQLFetch(
                SQLHSTMT StatementHandle
            ) override;

            SQLRETURN SQLGetData(
                SQLHSTMT StatementHandle,
                SQLUSMALLINT ColumnNumber,
                SQLSMALLINT TargetType,
                SQLPOINTER TargetValue,
                SQLLEN BufferLength,
                SQLLEN* StrLen_or_Ind
            ) override;

            SQLRETURN SQLRowCount(
                SQLHSTMT StatementHandle,
                SQLLEN* RowCount
            ) override;

            SQLRETURN SQLGetDiagRec(
                SQLSMALLINT HandleType,
                SQLHANDLE Handle,
                SQLSMALLINT RecNumber,
                SQLWCHAR* SQLState,
                SQLINTEGER* NativeError,
                SQLWCHAR* MessageText,
                SQLSMALLINT BufferLength,
                SQLSMALLINT* TextLength
            ) override;
        };
    }
}
#endif // ODBC_EXECUTOR_H

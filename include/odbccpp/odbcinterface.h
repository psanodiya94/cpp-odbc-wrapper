#ifndef ODBC_INTERFACE_H
#define ODBC_INTERFACE_H

#include <sql.h>
#include <sqlext.h>
#include <string>

namespace ps {
    namespace odbc {
        class OdbcInterface {
        public:
            virtual ~OdbcInterface() = default;
            virtual SQLRETURN SQLAllocHandle(SQLSMALLINT HandleType, SQLHANDLE InputHandle, SQLHANDLE* OutputHandle) = 0;
            virtual SQLRETURN SQLSetEnvAttr(SQLHENV EnvironmentHandle, SQLINTEGER Attribute, SQLPOINTER Value, SQLINTEGER StringLength) = 0;
            virtual SQLRETURN SQLConnect(SQLHDBC ConnectionHandle, SQLWCHAR* ServerName, SQLSMALLINT NameLength1, 
                                        SQLWCHAR* UserName, SQLSMALLINT NameLength2, SQLWCHAR* Authentication, SQLSMALLINT NameLength3) = 0;
            virtual SQLRETURN SQLDisconnect(SQLHDBC ConnectionHandle) = 0;
            virtual SQLRETURN SQLFreeHandle(SQLSMALLINT HandleType, SQLHANDLE Handle) = 0;
            virtual SQLRETURN SQLExecDirect(SQLHSTMT StatementHandle, SQLWCHAR* StatementText, SQLINTEGER TextLength) = 0;
            virtual SQLRETURN SQLNumResultCols(SQLHSTMT StatementHandle, SQLSMALLINT* ColumnCount) = 0;
            virtual SQLRETURN SQLFetch(SQLHSTMT StatementHandle) = 0;
            virtual SQLRETURN SQLGetData(SQLHSTMT StatementHandle, SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType, 
                                        SQLPOINTER TargetValue, SQLLEN BufferLength, SQLLEN* StrLen_or_Ind) = 0;
            virtual SQLRETURN SQLRowCount(SQLHSTMT StatementHandle, SQLLEN* RowCount) = 0;
            virtual SQLRETURN SQLGetDiagRec(SQLSMALLINT HandleType, SQLHANDLE Handle, SQLSMALLINT RecNumber, 
                                        SQLWCHAR* SQLState, SQLINTEGER* NativeError, SQLWCHAR* MessageText, 
                                        SQLSMALLINT BufferLength, SQLSMALLINT* TextLength) = 0;
        };
    }
}
#endif // ODBC_INTERFACE_H

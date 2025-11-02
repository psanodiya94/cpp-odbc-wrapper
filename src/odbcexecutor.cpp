#include <odbccpp/odbcexecutor.h>

#include <sql.h>
#include <sqlext.h>

namespace ps {
    namespace odbc {
        SQLRETURN OdbcExecutor::SQLAllocHandle(SQLSMALLINT HandleType, SQLHANDLE InputHandle, SQLHANDLE* OutputHandle) {
            return ::SQLAllocHandle(HandleType, InputHandle, OutputHandle);
        }

        SQLRETURN OdbcExecutor::SQLSetEnvAttr(SQLHENV EnvironmentHandle, SQLINTEGER Attribute, SQLPOINTER Value, SQLINTEGER StringLength) {
            return ::SQLSetEnvAttr(EnvironmentHandle, Attribute, Value, StringLength);
        }

        SQLRETURN OdbcExecutor::SQLConnect(SQLHDBC ConnectionHandle, SQLWCHAR* ServerName, SQLSMALLINT NameLength1,
                            SQLWCHAR* UserName, SQLSMALLINT NameLength2, SQLWCHAR* Authentication, SQLSMALLINT NameLength3) {
            return ::SQLConnectW(ConnectionHandle, ServerName, NameLength1, UserName, NameLength2, Authentication, NameLength3);
        }

        SQLRETURN OdbcExecutor::SQLDisconnect(SQLHDBC ConnectionHandle) {
            return ::SQLDisconnect(ConnectionHandle);
        }

        SQLRETURN OdbcExecutor::SQLFreeHandle(SQLSMALLINT HandleType, SQLHANDLE Handle) {
            return ::SQLFreeHandle(HandleType, Handle);
        }

        SQLRETURN OdbcExecutor::SQLExecDirect(SQLHSTMT StatementHandle, SQLWCHAR* StatementText, SQLINTEGER TextLength) {
            return ::SQLExecDirectW(StatementHandle, StatementText, TextLength);
        }

        SQLRETURN OdbcExecutor::SQLNumResultCols(SQLHSTMT StatementHandle, SQLSMALLINT* ColumnCount) {
            return ::SQLNumResultCols(StatementHandle, ColumnCount);
        }

        SQLRETURN OdbcExecutor::SQLFetch(SQLHSTMT StatementHandle) {
            return ::SQLFetch(StatementHandle);
        }

        SQLRETURN OdbcExecutor::SQLGetData(SQLHSTMT StatementHandle, SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType,
                            SQLPOINTER TargetValue, SQLLEN BufferLength, SQLLEN* StrLen_or_Ind) {
            return ::SQLGetData(StatementHandle, ColumnNumber, TargetType, TargetValue, BufferLength, StrLen_or_Ind);
        }

        SQLRETURN OdbcExecutor::SQLRowCount(SQLHSTMT StatementHandle, SQLLEN* RowCount) {
            return ::SQLRowCount(StatementHandle, RowCount);
        }

        SQLRETURN OdbcExecutor::SQLGetDiagRec(SQLSMALLINT HandleType, SQLHANDLE Handle, SQLSMALLINT RecNumber,
                            SQLWCHAR* SQLState, SQLINTEGER* NativeError, SQLWCHAR* MessageText,
                            SQLSMALLINT BufferLength, SQLSMALLINT* TextLength) {
            return ::SQLGetDiagRecW(HandleType, Handle, RecNumber, SQLState, NativeError, MessageText, BufferLength, TextLength);
        }
    }
}
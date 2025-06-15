#ifndef ODBC_EXECUTOR_H
#define ODBC_EXECUTOR_H

#include <odbccpp/odbcinterface.h>

namespace ps {
    namespace odbc {
        /**
         * @class OdbcExecutor
         * @brief Implements the ODBC interface for executing ODBC operations.
         *
         * This class provides concrete implementations of ODBC API functions
         * for managing database connections, executing queries, and retrieving results.
         */
        class OdbcExecutor : public OdbcInterface {
        public:
            /**
             * @brief Allocates an ODBC handle.
             * 
             * @param HandleType The type of handle to allocate (e.g., environment, connection, statement).
             * @param InputHandle The input handle, depending on the handle type.
             * @param OutputHandle Pointer to the allocated handle.
             * @return SQLRETURN indicating success or failure.
             */
            SQLRETURN SQLAllocHandle(
                SQLSMALLINT HandleType,
                SQLHANDLE InputHandle,
                SQLHANDLE* OutputHandle
            ) override;

            /**
             * @brief Sets attributes for the ODBC environment.
             * 
             * @param EnvironmentHandle The environment handle.
             * @param Attribute The attribute to set.
             * @param Value Pointer to the value to set.
             * @param StringLength The length of the value string.
             * @return SQLRETURN indicating success or failure.
             */
            SQLRETURN SQLSetEnvAttr(
                SQLHENV EnvironmentHandle,
                SQLINTEGER Attribute,
                SQLPOINTER Value,
                SQLINTEGER StringLength
            ) override;

            /**
             * @brief Establishes a connection to a database.
             * 
             * @param ConnectionHandle The connection handle.
             * @param ServerName The name of the server to connect to.
             * @param NameLength1 The length of the server name.
             * @param UserName The username for authentication.
             * @param NameLength2 The length of the username.
             * @param Authentication The password for authentication.
             * @param NameLength3 The length of the password.
             * @return SQLRETURN indicating success or failure.
             */
            SQLRETURN SQLConnect(
                SQLHDBC ConnectionHandle,
                SQLWCHAR* ServerName,
                SQLSMALLINT NameLength1,
                SQLWCHAR* UserName,
                SQLSMALLINT NameLength2,
                SQLWCHAR* Authentication,
                SQLSMALLINT NameLength3
            ) override;

            /**
             * @brief Disconnects from the database.
             * 
             * @param ConnectionHandle The connection handle to disconnect.
             * @return SQLRETURN indicating success or failure.
             */
            SQLRETURN SQLDisconnect(
                SQLHDBC ConnectionHandle
            ) override;

            /**
             * @brief Frees an ODBC handle.
             * 
             * @param HandleType The type of handle to free.
             * @param Handle The handle to free.
             * @return SQLRETURN indicating success or failure.
             */
            SQLRETURN SQLFreeHandle(
                SQLSMALLINT HandleType,
                SQLHANDLE Handle
            ) override;

            /**
             * @brief Executes a SQL statement directly.
             * 
             * @param StatementHandle The statement handle.
             * @param StatementText The SQL statement to execute.
             * @param TextLength The length of the SQL statement.
             * @return SQLRETURN indicating success or failure.
             */
            SQLRETURN SQLExecDirect(
                SQLHSTMT StatementHandle,
                SQLWCHAR* StatementText,
                SQLINTEGER TextLength
            ) override;

            /**
             * @brief Retrieves the number of result columns in a statement.
             * 
             * @param StatementHandle The statement handle.
             * @param ColumnCount Pointer to store the number of columns.
             * @return SQLRETURN indicating success or failure.
             */
            SQLRETURN SQLNumResultCols(
                SQLHSTMT StatementHandle,
                SQLSMALLINT* ColumnCount
            ) override;

            /**
             * @brief Fetches the next row of data from the result set.
             * 
             * @param StatementHandle The statement handle.
             * @return SQLRETURN indicating success or failure.
             */
            SQLRETURN SQLFetch(
                SQLHSTMT StatementHandle
            ) override;

            /**
             * @brief Retrieves data for a specific column in the current row.
             * 
             * @param StatementHandle The statement handle.
             * @param ColumnNumber The column number to retrieve data from.
             * @param TargetType The C data type of the target buffer.
             * @param TargetValue Pointer to the target buffer.
             * @param BufferLength The length of the target buffer.
             * @param StrLen_or_Ind Pointer to store the length or indicator value.
             * @return SQLRETURN indicating success or failure.
             */
            SQLRETURN SQLGetData(
                SQLHSTMT StatementHandle,
                SQLUSMALLINT ColumnNumber,
                SQLSMALLINT TargetType,
                SQLPOINTER TargetValue,
                SQLLEN BufferLength,
                SQLLEN* StrLen_or_Ind
            ) override;

            /**
             * @brief Retrieves the number of rows affected by an executed statement.
             * 
             * @param StatementHandle The statement handle.
             * @param RowCount Pointer to store the number of affected rows.
             * @return SQLRETURN indicating success or failure.
             */
            SQLRETURN SQLRowCount(
                SQLHSTMT StatementHandle,
                SQLLEN* RowCount
            ) override;

            /**
             * @brief Retrieves diagnostic information for the last ODBC function call.
             * 
             * @param HandleType The type of handle.
             * @param Handle The handle for which diagnostics are retrieved.
             * @param RecNumber The diagnostic record number.
             * @param SQLState Pointer to store the SQL state.
             * @param NativeError Pointer to store the native error code.
             * @param MessageText Pointer to store the diagnostic message.
             * @param BufferLength The length of the message buffer.
             * @param TextLength Pointer to store the length of the diagnostic message.
             * @return SQLRETURN indicating success or failure.
             */
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

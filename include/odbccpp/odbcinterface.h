#ifndef ODBC_INTERFACE_H
#define ODBC_INTERFACE_H

#include <sql.h>
#include <sqlext.h>
#include <string>

namespace ps {
    namespace odbc {
        /**
         * @class OdbcInterface
         * @brief Defines the interface for ODBC operations.
         *
         * This abstract class provides a set of pure virtual methods that must be implemented
         * by any class that provides ODBC functionality, such as managing database connections,
         * executing queries, and retrieving results.
         */
        class OdbcInterface {
        public:
            /**
             * @brief Virtual destructor for the interface.
             */
            virtual ~OdbcInterface() = default;

            /**
             * @brief Allocates an ODBC handle.
             * 
             * @param HandleType The type of handle to allocate (e.g., environment, connection, statement).
             * @param InputHandle The input handle, depending on the handle type.
             * @param OutputHandle Pointer to the allocated handle.
             * @return SQLRETURN indicating success or failure.
             */
            virtual SQLRETURN SQLAllocHandle(SQLSMALLINT HandleType, SQLHANDLE InputHandle, SQLHANDLE* OutputHandle) = 0;

            /**
             * @brief Sets attributes for the ODBC environment.
             * 
             * @param EnvironmentHandle The environment handle.
             * @param Attribute The attribute to set.
             * @param Value Pointer to the value to set.
             * @param StringLength The length of the value string.
             * @return SQLRETURN indicating success or failure.
             */
            virtual SQLRETURN SQLSetEnvAttr(SQLHENV EnvironmentHandle, SQLINTEGER Attribute, SQLPOINTER Value, SQLINTEGER StringLength) = 0;

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
            virtual SQLRETURN SQLConnect(SQLHDBC ConnectionHandle, SQLWCHAR* ServerName, SQLSMALLINT NameLength1, 
                                         SQLWCHAR* UserName, SQLSMALLINT NameLength2, SQLWCHAR* Authentication, SQLSMALLINT NameLength3) = 0;

            /**
             * @brief Disconnects from the database.
             * 
             * @param ConnectionHandle The connection handle to disconnect.
             * @return SQLRETURN indicating success or failure.
             */
            virtual SQLRETURN SQLDisconnect(SQLHDBC ConnectionHandle) = 0;

            /**
             * @brief Frees an ODBC handle.
             * 
             * @param HandleType The type of handle to free.
             * @param Handle The handle to free.
             * @return SQLRETURN indicating success or failure.
             */
            virtual SQLRETURN SQLFreeHandle(SQLSMALLINT HandleType, SQLHANDLE Handle) = 0;

            /**
             * @brief Executes a SQL statement directly.
             * 
             * @param StatementHandle The statement handle.
             * @param StatementText The SQL statement to execute.
             * @param TextLength The length of the SQL statement.
             * @return SQLRETURN indicating success or failure.
             */
            virtual SQLRETURN SQLExecDirect(SQLHSTMT StatementHandle, SQLWCHAR* StatementText, SQLINTEGER TextLength) = 0;

            /**
             * @brief Retrieves the number of result columns in a statement.
             * 
             * @param StatementHandle The statement handle.
             * @param ColumnCount Pointer to store the number of columns.
             * @return SQLRETURN indicating success or failure.
             */
            virtual SQLRETURN SQLNumResultCols(SQLHSTMT StatementHandle, SQLSMALLINT* ColumnCount) = 0;

            /**
             * @brief Fetches the next row of data from the result set.
             * 
             * @param StatementHandle The statement handle.
             * @return SQLRETURN indicating success or failure.
             */
            virtual SQLRETURN SQLFetch(SQLHSTMT StatementHandle) = 0;

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
            virtual SQLRETURN SQLGetData(SQLHSTMT StatementHandle, SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType, 
                                         SQLPOINTER TargetValue, SQLLEN BufferLength, SQLLEN* StrLen_or_Ind) = 0;

            /**
             * @brief Retrieves the number of rows affected by an executed statement.
             * 
             * @param StatementHandle The statement handle.
             * @param RowCount Pointer to store the number of affected rows.
             * @return SQLRETURN indicating success or failure.
             */
            virtual SQLRETURN SQLRowCount(SQLHSTMT StatementHandle, SQLLEN* RowCount) = 0;

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
            virtual SQLRETURN SQLGetDiagRec(SQLSMALLINT HandleType, SQLHANDLE Handle, SQLSMALLINT RecNumber, 
                                            SQLWCHAR* SQLState, SQLINTEGER* NativeError, SQLWCHAR* MessageText, 
                                            SQLSMALLINT BufferLength, SQLSMALLINT* TextLength) = 0;
        };
    }
}
#endif // ODBC_INTERFACE_H

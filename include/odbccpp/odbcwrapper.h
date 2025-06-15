#ifndef ODBC_WRAPPER_H
#define ODBC_WRAPPER_H

#include <odbccpp/odbcinterface.h>

#include <string>
#include <vector>
#include <memory>

namespace ps {
    namespace odbc {
        /**
         * @class OdbcWrapper
         * @brief Provides a high-level wrapper for ODBC operations.
         *
         * This class simplifies the use of ODBC by managing handles, connections,
         * and queries, while providing an interface for executing SQL commands and
         * retrieving results.
         */
        class OdbcWrapper {
        private:
            SQLHENV                         m_hEnv = SQL_NULL_HENV; ///< ODBC environment handle.
            SQLHDBC                         m_hDbc = SQL_NULL_HDBC; ///< ODBC connection handle.
            SQLHSTMT                        m_hStmt = SQL_NULL_HSTMT; ///< ODBC statement handle.
            bool                            m_connected = false; ///< Indicates whether the connection is active.
            std::unique_ptr<OdbcInterface>  m_odbc = nullptr; ///< Pointer to the ODBC interface implementation.
        
            /**
             * @brief Handles ODBC errors by retrieving diagnostic information.
             * 
             * @param handle The ODBC handle where the error occurred.
             * @param handleType The type of the handle (e.g., environment, connection, statement).
             * @param retCode The return code from the ODBC function.
             */
            void handleError(SQLHANDLE handle, SQLSMALLINT handleType, SQLRETURN retCode);
        
        public:
            /**
             * @brief Constructs an OdbcWrapper object.
             * 
             * @param odbcImpl A unique pointer to an ODBC interface implementation. Defaults to nullptr.
             */
            OdbcWrapper(std::unique_ptr<OdbcInterface> odbcImpl = nullptr);

            /**
             * @brief Destroys the OdbcWrapper object and releases resources.
             */
            ~OdbcWrapper();

            /**
             * @brief Initializes the ODBC environment and allocates necessary handles.
             */
            void initialize();

            /**
             * @brief Establishes a connection to the database.
             * 
             * @param dsn The Data Source Name (DSN) for the database.
             * @param user The username for authentication.
             * @param password The password for authentication.
             * @return True if the connection is successful, false otherwise.
             */
            bool connect(const std::wstring& dsn, const std::wstring& user, const std::wstring& password);

            /**
             * @brief Disconnects from the database and releases the connection handle.
             */
            void disconnect();

            /**
             * @brief Executes a SQL query that retrieves data.
             * 
             * @param query The SQL query to execute.
             * @return True if the query executes successfully, false otherwise.
             */
            bool executeQuery(const std::wstring& query);

            /**
             * @brief Executes a SQL query that modifies data.
             * 
             * @param query The SQL query to execute.
             * @return True if the query executes successfully, false otherwise.
             */
            bool executeUpdate(const std::wstring& query);

            /**
             * @brief Fetches the results of the last executed query.
             * 
             * @return A vector of rows, where each row is a vector of strings representing column values.
             */
            std::vector<std::vector<std::wstring>> fetchResults();

            /**
             * @brief Checks if the database connection is active.
             * 
             * @return True if connected, false otherwise.
             */
            bool isConnected() const { return m_connected; }

            /**
             * @brief Retrieves the ODBC interface implementation.
             * 
             * @return A pointer to the ODBC interface implementation.
             */
            OdbcInterface* getOdbcInterface() const { return m_odbc.get(); }

            /**
             * @brief Retrieves the ODBC environment handle.
             * 
             * @return The ODBC environment handle.
             */
            SQLHENV getHEnv() const { return m_hEnv; }

            /**
             * @brief Retrieves the ODBC connection handle.
             * 
             * @return The ODBC connection handle.
             */
            SQLHDBC getHDbc() const { return m_hDbc; }

            /**
             * @brief Retrieves the ODBC statement handle.
             * 
             * @return The ODBC statement handle.
             */
            SQLHSTMT getHStmt() const { return m_hStmt; }
        };
    }
}
#endif // ODBC_WRAPPER_H

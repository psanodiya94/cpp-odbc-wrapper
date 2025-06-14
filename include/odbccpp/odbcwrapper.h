#ifndef ODBC_WRAPPER_H
#define ODBC_WRAPPER_H

#include <odbccpp/odbcinterface.h>

#include <string>
#include <vector>
#include <memory>

namespace ps {
    namespace odbc {
        class OdbcWrapper {
        private:
            SQLHENV                         m_hEnv = SQL_NULL_HENV;
            SQLHDBC                         m_hDbc = SQL_NULL_HDBC;
            SQLHSTMT                        m_hStmt = SQL_NULL_HSTMT;
            bool                            m_connected = false;
            std::unique_ptr<OdbcInterface>  m_odbc = nullptr;
        
            void handleError(SQLHANDLE handle, SQLSMALLINT handleType, SQLRETURN retCode);
        
        public:
            OdbcWrapper(std::unique_ptr<OdbcInterface> odbcImpl = nullptr);
            ~OdbcWrapper();

            void initialize();

            bool connect(const std::wstring& dsn, const std::wstring& user, const std::wstring& password);
            void disconnect();

            bool executeQuery(const std::wstring& query);
            bool executeUpdate(const std::wstring& query);

            std::vector<std::vector<std::wstring>> fetchResults();

            bool isConnected() const { return m_connected; }

            OdbcInterface* getOdbcInterface() const { return m_odbc.get(); }
            SQLHENV getHEnv() const { return m_hEnv; }
            SQLHDBC getHDbc() const { return m_hDbc; }
            SQLHSTMT getHStmt() const { return m_hStmt; }
        };
    }
}
#endif // ODBC_WRAPPER_H

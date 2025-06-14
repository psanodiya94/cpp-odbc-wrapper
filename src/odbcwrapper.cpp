#include <odbccpp/odbcexecutor.h>
#include <odbccpp/odbcwrapper.h>
#include <odbccpp/odbclogger.h>

#include <stdexcept>
#include <codecvt> 
#include <locale>

namespace ps {
    namespace odbc {
        OdbcWrapper::OdbcWrapper(std::unique_ptr<OdbcInterface> odbcImpl) 
            : m_odbc(odbcImpl ? std::move(odbcImpl) : std::make_unique<OdbcExecutor>()) {
            OdbcLogger::logInfo("Entering OdbcWrapper constructor");
            OdbcLogger::logInfo("Exiting OdbcWrapper constructor");
        }

        OdbcWrapper::~OdbcWrapper() {
            OdbcLogger::logInfo("Entering OdbcWrapper destructor");
            disconnect();
            if (m_odbc.get() != nullptr) {
                if (m_hStmt != SQL_NULL_HSTMT) {
                    m_odbc->SQLFreeHandle(SQL_HANDLE_STMT, m_hStmt);
                }
                if (m_hDbc != SQL_NULL_HDBC) {
                    m_odbc->SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc);
                }
                if (m_hEnv != SQL_NULL_HENV) {
                    m_odbc->SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
                }
                
                m_odbc.reset();
            }
            OdbcLogger::logInfo("Exiting OdbcWrapper destructor");
        }

        void OdbcWrapper::handleError(SQLHANDLE handle, SQLSMALLINT handleType, SQLRETURN retCode) {
            OdbcLogger::logInfo("Entering handleError");
            SQLWCHAR sqlState[6], msg[SQL_MAX_MESSAGE_LENGTH];
            SQLINTEGER nativeError;
            SQLSMALLINT msgLen;

            while (m_odbc->SQLGetDiagRec(handleType, handle, 1, sqlState, &nativeError, 
                                msg, sizeof(msg) / sizeof(SQLWCHAR), &msgLen) == SQL_SUCCESS) {
                // Convert SQLWCHAR* to std::wstring for logging
                std::wstring sqlStateStr(reinterpret_cast<wchar_t*>(sqlState));
                std::wstring msgStr(reinterpret_cast<wchar_t*>(msg));
                
                // Convert std::wstring to std::string for spdlog
                std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                std::string sqlStateNarrow = converter.to_bytes(sqlStateStr);
                std::string msgNarrow = converter.to_bytes(msgStr);

                OdbcLogger::logError(fmt::format("SQLSTATE: {}, Message: {}, Native Error: {}", 
                                                sqlStateNarrow, msgNarrow, nativeError));

                if (retCode == SQL_ERROR) {
                    throw std::runtime_error("ODBC Error: " + std::string(msgStr.begin(), msgStr.end()));
                }
            }
            OdbcLogger::logInfo("Exiting handleError");
        }

        void OdbcWrapper::initialize() {
            OdbcLogger::logInfo("Entering initialize");
            SQLRETURN ret = m_odbc->SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv);
            if (SQL_SUCCEEDED(ret)) {
                ret = m_odbc->SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
                if (SQL_SUCCEEDED(ret)) {
                    ret = m_odbc->SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &m_hDbc);
                    if (!SQL_SUCCEEDED(ret)) {
                        handleError(m_hEnv, SQL_HANDLE_ENV, ret);
                    }
                } else {
                    handleError(m_hEnv, SQL_HANDLE_ENV, ret);
                }
            } else {
                handleError(SQL_NULL_HANDLE, SQL_HANDLE_ENV, ret);
            }
            OdbcLogger::logInfo("Exiting initialize");
        }

        bool OdbcWrapper::connect(const std::wstring& dsn, const std::wstring& user, const std::wstring& password) {
            OdbcLogger::logInfo("Entering connect");
            SQLRETURN ret = m_odbc->SQLConnect(m_hDbc, (SQLWCHAR*)dsn.c_str(), SQL_NTS,
                                    (SQLWCHAR*)user.c_str(), SQL_NTS,
                                    (SQLWCHAR*)password.c_str(), SQL_NTS);
            
            if (SQL_SUCCEEDED(ret)) {
                m_connected = true;
                m_odbc->SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hStmt);
                OdbcLogger::logInfo("Exiting connect with success");
                return true;
            }
            
            handleError(m_hDbc, SQL_HANDLE_DBC, ret);
            OdbcLogger::logInfo("Exiting connect with failure");
            return false;
        }

        void OdbcWrapper::disconnect() {
            OdbcLogger::logInfo("Entering disconnect");
            if (m_connected) {
                OdbcLogger::logInfo("Deallocating resources");
                if (m_hStmt != SQL_NULL_HSTMT) {
                    OdbcLogger::logInfo("Freeing statement handle");
                    m_odbc->SQLFreeHandle(SQL_HANDLE_STMT, m_hStmt);
                    m_hStmt = SQL_NULL_HSTMT;
                }
        
                if (m_hDbc != SQL_NULL_HDBC) {
                    OdbcLogger::logInfo("Disconnecting from database");
                    m_odbc->SQLDisconnect(m_hDbc);
                }
                
                m_connected = false;
            }
            OdbcLogger::logInfo("Exiting disconnect");
        }

        bool OdbcWrapper::executeQuery(const std::wstring& query) {
            OdbcLogger::logInfo("Entering executeQuery");
            if (!m_connected) {
                spdlog::warn("Exiting executeQuery with failure (not connected)");
                return false;
            }

            SQLRETURN ret = m_odbc->SQLExecDirect(m_hStmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
            if (SQL_SUCCEEDED(ret)) {
                OdbcLogger::logInfo("Exiting executeQuery with success");
                return true;
            }

            handleError(m_hStmt, SQL_HANDLE_STMT, ret);
            OdbcLogger::logInfo("Exiting executeQuery with failure");
            return false;
        }

        std::vector<std::vector<std::wstring>> OdbcWrapper::fetchResults() {
            OdbcLogger::logInfo("Entering fetchResults");
            std::vector<std::vector<std::wstring>> results;
            if (!m_connected) {
                spdlog::warn("Exiting fetchResults with empty results (not connected)");
                return results;
            }

            SQLSMALLINT numCols;
            m_odbc->SQLNumResultCols(m_hStmt, &numCols);

            while (m_odbc->SQLFetch(m_hStmt) == SQL_SUCCESS) {
                std::vector<std::wstring> row;
                for (SQLSMALLINT i = 1; i <= numCols; i++) {
                    SQLWCHAR buffer[1024];
                    SQLLEN indicator;
                    m_odbc->SQLGetData(m_hStmt, i, SQL_C_WCHAR, buffer, sizeof(buffer), &indicator);
                    
                    if (indicator != SQL_NULL_DATA) {
                        row.push_back(std::wstring(reinterpret_cast<wchar_t*>(buffer)));
                    } else {
                        row.push_back(L"NULL");
                    }
                }
                results.push_back(row);
            }

            OdbcLogger::logInfo("Exiting fetchResults with results");
            return results;
        }

        bool OdbcWrapper::executeUpdate(const std::wstring& query) {
            OdbcLogger::logInfo("Entering executeUpdate");
            if (!m_connected) {
                spdlog::warn("Exiting executeUpdate with failure (not connected)");
                return false;
            }

            SQLRETURN ret = m_odbc->SQLExecDirect(m_hStmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
            if (SQL_SUCCEEDED(ret)) {
                m_odbc->SQLRowCount(m_hStmt, nullptr); // Consume results if any
                OdbcLogger::logInfo("Exiting executeUpdate with success");
                return true;
            }

            handleError(m_hStmt, SQL_HANDLE_STMT, ret);
            OdbcLogger::logInfo("Exiting executeUpdate with failure");
            return false;
        }
    }
}
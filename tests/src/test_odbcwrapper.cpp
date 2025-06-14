#include <odbccpp/odbclogger.h>
#include <test_odbcwrapper.h>
#include <cwchar>

namespace ps {
    namespace test {
        TEST_F(OdbcWrapperTest, Constructor_InitializesEnvironmentAndConnection) {
            OdbcLogger::logInfo("Entering Constructor_InitializesEnvironmentAndConnection");
            
            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, testing::_))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLSetEnvAttr(testing::_, SQL_ATTR_ODBC_VERSION, testing::_, 0))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_DBC, testing::_, testing::_))
                .WillOnce(testing::Return(SQL_SUCCESS));

            wrapper->initialize();

            EXPECT_TRUE(wrapper->getOdbcInterface() != nullptr);
            EXPECT_TRUE(wrapper->isConnected() == false);

            OdbcLogger::logInfo("Exiting Constructor_InitializesEnvironmentAndConnection");
        }

        TEST_F(OdbcWrapperTest, Connect_SuccessfulConnection) {
            OdbcLogger::logInfo("Entering Connect_SuccessfulConnection");

            EXPECT_CALL(*mock, SQLConnect(testing::_, testing::_, SQL_NTS, testing::_, SQL_NTS, testing::_, SQL_NTS))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_STMT, testing::_, testing::_))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLDisconnect(nullptr)) // Handle NULL case
                .Times(testing::AnyNumber())
                .WillRepeatedly(testing::Return(SQL_SUCCESS));

            bool result = wrapper->connect(L"MyDSN", L"user", L"pass");
            EXPECT_TRUE(result);

            OdbcLogger::logInfo("Exiting Connect_SuccessfulConnection");
        }

        TEST_F(OdbcWrapperTest, Connect_FailureHandlesError) {
            OdbcLogger::logInfo("Entering Connect_FailureHandlesError");

            EXPECT_CALL(*mock, SQLConnect(testing::_, testing::_, SQL_NTS, testing::_, SQL_NTS, testing::_, SQL_NTS))
                .WillOnce(testing::Return(SQL_ERROR));
            EXPECT_CALL(*mock, SQLGetDiagRec(SQL_HANDLE_DBC, testing::_, 1, testing::_, testing::_, testing::_, testing::_, testing::_))
                .WillOnce(testing::Return(SQL_NO_DATA));

            bool result = wrapper->connect(L"MyDSN", L"user", L"pass");
            EXPECT_FALSE(result);

            OdbcLogger::logInfo("Exiting Connect_FailureHandlesError");
        }

        TEST_F(OdbcWrapperTest, ExecuteQuery_SuccessfulExecution) {
            OdbcLogger::logInfo("Entering ExecuteQuery_SuccessfulExecution");

            // Simulate successful connection
            EXPECT_CALL(*mock, SQLConnect(testing::_, testing::_, SQL_NTS, testing::_, SQL_NTS, testing::_, SQL_NTS))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_STMT, testing::_, testing::_))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLDisconnect(nullptr)) // Handle NULL case
                .Times(testing::AnyNumber())
                .WillRepeatedly(testing::Return(SQL_SUCCESS));
            
            wrapper->connect(L"MyDSN", L"user", L"pass"); // Set isConnected to true

            EXPECT_CALL(*mock, SQLExecDirect(testing::_, testing::_, SQL_NTS))
                .WillOnce(testing::Return(SQL_SUCCESS));

            bool result = wrapper->executeQuery(L"SELECT * FROM table");
            EXPECT_TRUE(result);

            OdbcLogger::logInfo("Exiting ExecuteQuery_SuccessfulExecution");
        }

        TEST_F(OdbcWrapperTest, ExecuteQuery_FailsIfNotConnected) {
            OdbcLogger::logInfo("Entering ExecuteQuery_FailsIfNotConnected");

            bool result = wrapper->executeQuery(L"SELECT * FROM table");
            EXPECT_FALSE(result);

            OdbcLogger::logInfo("Exiting ExecuteQuery_FailsIfNotConnected");
        }

        TEST_F(OdbcWrapperTest, ExecuteQuery_FailureHandlesError) {
            OdbcLogger::logInfo("Entering ExecuteQuery_FailureHandlesError");

            // Simulate successful connection
            EXPECT_CALL(*mock, SQLConnect(testing::_, testing::_, SQL_NTS, testing::_, SQL_NTS, testing::_, SQL_NTS))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_STMT, testing::_, testing::_))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLDisconnect(nullptr)) // Handle NULL case
                .Times(testing::AnyNumber())
                .WillRepeatedly(testing::Return(SQL_SUCCESS));
            
            wrapper->connect(L"MyDSN", L"user", L"pass"); // Set isConnected to true

            EXPECT_CALL(*mock, SQLExecDirect(testing::_, testing::_, SQL_NTS))
                .WillOnce(testing::Return(SQL_ERROR));
            EXPECT_CALL(*mock, SQLGetDiagRec(SQL_HANDLE_STMT, testing::_, 1, testing::_, testing::_, testing::_, testing::_, testing::_))
                .WillOnce(testing::Return(SQL_NO_DATA));

            bool result = wrapper->executeQuery(L"SELECT * FROM table");
            EXPECT_FALSE(result);

            OdbcLogger::logInfo("Exiting ExecuteQuery_FailureHandlesError");
        }

        TEST_F(OdbcWrapperTest, FetchResults_ReturnsCorrectData) {
            // Simulate successful connection
            EXPECT_CALL(*mock, SQLConnect(testing::_, testing::_, SQL_NTS, testing::_, SQL_NTS, testing::_, SQL_NTS))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_STMT, testing::_, testing::_))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLDisconnect(nullptr)) // Handle NULL case
                .Times(testing::AnyNumber())
                .WillRepeatedly(testing::Return(SQL_SUCCESS));
            
            wrapper->connect(L"MyDSN", L"user", L"pass"); // Set isConnected to true

            EXPECT_CALL(*mock, SQLExecDirect(testing::_, testing::_, SQL_NTS))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLNumResultCols(testing::_, testing::_))
                .WillOnce([](SQLHSTMT, SQLSMALLINT* cols) { *cols = 2; return SQL_SUCCESS; });
            EXPECT_CALL(*mock, SQLFetch(testing::_))
                .WillOnce(testing::Return(SQL_SUCCESS))
                .WillOnce(testing::Return(SQL_NO_DATA));
                EXPECT_CALL(*mock, SQLGetData(testing::_, 1, SQL_C_WCHAR, testing::_, testing::_, testing::_))
                .WillOnce([](SQLHSTMT, SQLUSMALLINT, SQLSMALLINT, SQLPOINTER buffer, SQLLEN len, SQLLEN* indicator) {
                    const wchar_t* data = L"data1";
                    size_t data_len = std::wcslen(data) + 1; // Include null terminator
                    if (len >= static_cast<SQLLEN>(data_len * sizeof(SQLWCHAR))) {
                        std::wcsncpy(reinterpret_cast<wchar_t*>(buffer), data, data_len);
                        *indicator = SQL_NTS;
                    } else {
                        *indicator = SQL_NULL_DATA; // Indicate insufficient buffer
                    }
                    return SQL_SUCCESS;
                });
            EXPECT_CALL(*mock, SQLGetData(testing::_, 2, SQL_C_WCHAR, testing::_, testing::_, testing::_))
                .WillOnce([](SQLHSTMT, SQLUSMALLINT, SQLSMALLINT, SQLPOINTER buffer, SQLLEN len, SQLLEN* indicator) {
                    const wchar_t* data = L"data2";
                    size_t data_len = std::wcslen(data) + 1; // Include null terminator
                    if (len >= static_cast<SQLLEN>(data_len * sizeof(SQLWCHAR))) {
                        std::wcsncpy(reinterpret_cast<wchar_t*>(buffer), data, data_len);
                        *indicator = SQL_NTS;
                    } else {
                        *indicator = SQL_NULL_DATA; // Indicate insufficient buffer
                    }
                    return SQL_SUCCESS;
                });

            wrapper->executeQuery(L"SELECT * FROM table");
            auto results = wrapper->fetchResults();
            EXPECT_EQ(results.size(), 1);
            EXPECT_EQ(results[0].size(), 2);
            EXPECT_EQ(results[0][0], L"data1");
            EXPECT_EQ(results[0][1], L"data2");
        }

        TEST_F(OdbcWrapperTest, ExecuteUpdate_SuccessfulExecution) {
            // Simulate successful connection
            EXPECT_CALL(*mock, SQLConnect(testing::_, testing::_, SQL_NTS, testing::_, SQL_NTS, testing::_, SQL_NTS))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_STMT, testing::_, testing::_))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLDisconnect(nullptr)) // Handle NULL case
                .Times(testing::AnyNumber())
                .WillRepeatedly(testing::Return(SQL_SUCCESS));
            
            wrapper->connect(L"MyDSN", L"user", L"pass"); // Set isConnected to true

            EXPECT_CALL(*mock, SQLExecDirect(testing::_, testing::_, SQL_NTS))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLRowCount(testing::_, testing::_))
                .WillOnce(testing::Return(SQL_SUCCESS));

            bool result = wrapper->executeUpdate(L"INSERT INTO table VALUES ('data')");
            EXPECT_TRUE(result);
        }

        TEST_F(OdbcWrapperTest, Disconnect_CleansUpProperly) {
            OdbcLogger::logInfo("Entering Disconnect_CleansUpProperly");

            SQLHANDLE* dummyDbcHandle = nullptr;
            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, testing::_))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLSetEnvAttr(testing::_, SQL_ATTR_ODBC_VERSION, testing::_, 0))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_DBC, testing::_, testing::_))
                .WillOnce([&dummyDbcHandle](SQLSMALLINT, SQLHANDLE, SQLHANDLE* dbcHandle) {
                    *dbcHandle = reinterpret_cast<SQLHDBC>(1); // Set dummy database connection handle
                    *dummyDbcHandle = *dbcHandle; // Store for later use
                    return SQL_SUCCESS;
                });

            wrapper->initialize();

            SQLHANDLE* dummyStmtHandle = nullptr;
            // Simulate successful connection
            EXPECT_CALL(*mock, SQLConnect(testing::_, testing::_, SQL_NTS, testing::_, SQL_NTS, testing::_, SQL_NTS))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_STMT, testing::_, testing::_))
                .WillOnce([&dummyStmtHandle](SQLSMALLINT, SQLHANDLE, SQLHANDLE* stmtHandle) {
                    *stmtHandle = reinterpret_cast<SQLHSTMT>(1); // Set dummy statement handle
                    *dummyStmtHandle = *stmtHandle; // Store for later use
                    return SQL_SUCCESS;
                });
            EXPECT_CALL(*mock, SQLDisconnect(nullptr)) // Handle NULL case
                .Times(testing::AnyNumber())
                .WillRepeatedly(testing::Return(SQL_SUCCESS));

            wrapper->connect(L"MyDSN", L"user", L"pass"); // Set isConnected to true
            EXPECT_TRUE(wrapper->isConnected());
            
            // Expectations for disconnect
            EXPECT_CALL(*mock, SQLFreeHandle(SQL_HANDLE_STMT, dummyStmtHandle))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLDisconnect(dummyDbcHandle))
                .WillOnce(testing::Return(SQL_SUCCESS));            

            wrapper->disconnect();

            EXPECT_FALSE(wrapper->isConnected());

            OdbcLogger::logInfo("Exiting Disconnect_CleansUpProperly");
        }
    }
}

int main(int argc, char **argv) {
    // Initialize the ODBC logger
    OdbcLogger::initialize("logs/odbc_test.log");

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#include <test_odbcwrapper.h>
#include <odbclogger.h>
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

        TEST_F(OdbcWrapperTest, FetchResults_NoData) {
            OdbcLogger::logInfo("Entering FetchResults_NoData");

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
                .WillOnce(testing::Return(SQL_NO_DATA)); // No rows to fetch

            wrapper->executeQuery(L"SELECT * FROM table");
            auto results = wrapper->fetchResults();
            EXPECT_TRUE(results.empty()); // Expect no results

            OdbcLogger::logInfo("Exiting FetchResults_NoData");
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

            SQLHANDLE dummyDbcHandle = reinterpret_cast<SQLHANDLE>(1); // Set dummy database connection handle
            SQLHANDLE dummyStmtHandle = reinterpret_cast<SQLHANDLE>(1); // Set dummy statement handle

            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, testing::_))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLSetEnvAttr(testing::_, SQL_ATTR_ODBC_VERSION, testing::_, 0))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_DBC, testing::_, testing::_))
                .WillOnce([&dummyDbcHandle](SQLSMALLINT, SQLHANDLE, SQLHANDLE* dbcHandle) {
                    *dbcHandle = dummyDbcHandle; // Assign dummy handle
                    return SQL_SUCCESS;
                });

            wrapper->initialize();

            EXPECT_CALL(*mock, SQLConnect(testing::_, testing::_, SQL_NTS, testing::_, SQL_NTS, testing::_, SQL_NTS))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_STMT, testing::_, testing::_))
                .WillOnce([&dummyStmtHandle](SQLSMALLINT, SQLHANDLE, SQLHANDLE* stmtHandle) {
                    *stmtHandle = dummyStmtHandle; // Assign dummy handle
                    return SQL_SUCCESS;
                });

            wrapper->connect(L"MyDSN", L"user", L"pass"); // Set isConnected to true
            EXPECT_TRUE(wrapper->isConnected());

            // Expectations for disconnect
            EXPECT_CALL(*mock, SQLFreeHandle(SQL_HANDLE_STMT, dummyStmtHandle))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLDisconnect(dummyDbcHandle))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLFreeHandle(SQL_HANDLE_DBC, dummyDbcHandle))
                .WillOnce(testing::Return(SQL_SUCCESS));

            wrapper->disconnect();

            EXPECT_FALSE(wrapper->isConnected());

            OdbcLogger::logInfo("Exiting Disconnect_CleansUpProperly");
        }

        TEST_F(OdbcWrapperTest, FetchResults_MultipleRows) {
            OdbcLogger::logInfo("Entering FetchResults_MultipleRows");

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
                .WillOnce(testing::Return(SQL_SUCCESS)) // First row
                .WillOnce(testing::Return(SQL_SUCCESS)) // Second row
                .WillOnce(testing::Return(SQL_NO_DATA)); // No more rows
            EXPECT_CALL(*mock, SQLGetData(testing::_, 1, SQL_C_WCHAR, testing::_, testing::_, testing::_))
                .WillRepeatedly([](SQLHSTMT, SQLUSMALLINT, SQLSMALLINT, SQLPOINTER buffer, SQLLEN len, SQLLEN* indicator) {
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
                .WillRepeatedly([](SQLHSTMT, SQLUSMALLINT, SQLSMALLINT, SQLPOINTER buffer, SQLLEN len, SQLLEN* indicator) {
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
            EXPECT_EQ(results.size(), 2); // Expect two rows
            EXPECT_EQ(results[0][0], L"data1");
            EXPECT_EQ(results[0][1], L"data2");
            EXPECT_EQ(results[1][0], L"data1");
            EXPECT_EQ(results[1][1], L"data2");

            OdbcLogger::logInfo("Exiting FetchResults_MultipleRows");
        }

        TEST_F(OdbcWrapperTest, FetchResults_NullData) {
            OdbcLogger::logInfo("Entering FetchResults_NullData");

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
                .WillOnce(testing::Return(SQL_NO_DATA)); // No more rows
            EXPECT_CALL(*mock, SQLGetData(testing::_, 1, SQL_C_WCHAR, testing::_, testing::_, testing::_))
                .WillOnce([](SQLHSTMT, SQLUSMALLINT, SQLSMALLINT, SQLPOINTER buffer, SQLLEN len, SQLLEN* indicator) {
                    *indicator = SQL_NULL_DATA; // Indicate NULL value
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
            EXPECT_EQ(results.size(), 1); // Expect one row
            EXPECT_EQ(results[0][0], L"NULL"); // NULL value should be empty
            EXPECT_EQ(results[0][1], L"data2");

            OdbcLogger::logInfo("Exiting FetchResults_NullData");
        }

        TEST_F(OdbcWrapperTest, Initialize_FailureInSQLAllocHandle) {
            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, testing::_))
                .WillOnce(testing::Return(SQL_ERROR)); // Simulate failure

            EXPECT_CALL(*mock, SQLGetDiagRec(SQL_HANDLE_ENV, testing::_, 1, testing::_, testing::_, testing::_, testing::_, testing::_))
                .WillOnce([](SQLSMALLINT, SQLHANDLE, SQLSMALLINT, SQLWCHAR* state, SQLINTEGER* nativeError, SQLWCHAR* messageText, SQLSMALLINT bufferLength, SQLSMALLINT* textLength) {
                    if (state) {
                        std::wcsncpy(reinterpret_cast<wchar_t*>(state), L"HY000", 6); // Example SQL state
                    }
                    if (nativeError) {
                        *nativeError = 12345; // Example error code
                    }
                    if (messageText && bufferLength > 0) {
                        std::wcsncpy(reinterpret_cast<wchar_t*>(messageText), L"Allocation failed", bufferLength / sizeof(SQLWCHAR));
                        if (textLength) {
                            *textLength = std::wcslen(L"Allocation failed");
                        }
                    }
                    return SQL_SUCCESS;
                });

            EXPECT_THROW(wrapper->initialize(), std::runtime_error); // Expect exception
        }

        TEST_F(OdbcWrapperTest, ExecuteQuery_Success) {
            EXPECT_CALL(*mock, SQLConnect(testing::_, testing::_, SQL_NTS, testing::_, SQL_NTS, testing::_, SQL_NTS))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_STMT, testing::_, testing::_))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLExecDirect(testing::_, testing::_, SQL_NTS))
                .WillOnce(testing::Return(SQL_SUCCESS));

            wrapper->connect(L"MyDSN", L"user", L"pass");
            EXPECT_NO_THROW(wrapper->executeQuery(L"SELECT * FROM table"));
        }

        TEST_F(OdbcWrapperTest, ExecuteQuery_Failure) {
            // Simulate successful connection
            EXPECT_CALL(*mock, SQLConnect(testing::_, testing::_, SQL_NTS, testing::_, SQL_NTS, testing::_, SQL_NTS))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_STMT, testing::_, testing::_))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLDisconnect(nullptr)) // Handle NULL case
                .Times(testing::AnyNumber())
                .WillRepeatedly(testing::Return(SQL_SUCCESS));
            
            wrapper->connect(L"MyDSN", L"user", L"pass"); // Set isConnected to true

            // Simulate failure in SQLExecDirect
            EXPECT_CALL(*mock, SQLExecDirect(testing::_, testing::_, SQL_NTS))
                .WillOnce(testing::Return(SQL_ERROR));

            // Add EXPECT_CALL for SQLGetDiagRec to handle diagnostic retrieval
            EXPECT_CALL(*mock, SQLGetDiagRec(SQL_HANDLE_STMT, testing::_, 1, testing::_, testing::_, testing::_, testing::_, testing::_))
                .WillOnce([](SQLSMALLINT, SQLHANDLE, SQLSMALLINT, SQLWCHAR* state, SQLINTEGER* nativeError, SQLWCHAR* messageText, SQLSMALLINT bufferLength, SQLSMALLINT* textLength) {
                    if (state) {
                        std::wcsncpy(reinterpret_cast<wchar_t*>(state), L"HY000", 6); // Example SQL state
                    }
                    if (nativeError) {
                        *nativeError = 12345; // Example error code
                    }
                    if (messageText && bufferLength > 0) {
                        std::wcsncpy(reinterpret_cast<wchar_t*>(messageText), L"Execution failed", bufferLength / sizeof(SQLWCHAR));
                        if (textLength) {
                            *textLength = std::wcslen(L"Execution failed");
                        }
                    }
                    return SQL_SUCCESS;
                });

            // Execute query and expect failure
            EXPECT_THROW(wrapper->executeQuery(L"SELECT * FROM table"), std::runtime_error); // Expect exception
        }

        TEST_F(OdbcWrapperTest, Connect_FailureInSQLConnect) {
            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, testing::_))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLSetEnvAttr(testing::_, SQL_ATTR_ODBC_VERSION, testing::_, 0))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_DBC, testing::_, testing::_))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLConnect(testing::_, testing::_, SQL_NTS, testing::_, SQL_NTS, testing::_, SQL_NTS))
                .WillOnce(testing::Return(SQL_ERROR)); // Simulate failure

            // Add EXPECT_CALL for SQLGetDiagRec to handle diagnostic retrieval
            EXPECT_CALL(*mock, SQLGetDiagRec(SQL_HANDLE_DBC, testing::_, 1, testing::_, testing::_, testing::_, testing::_, testing::_))
                .WillOnce([](SQLSMALLINT, SQLHANDLE, SQLSMALLINT, SQLWCHAR* state, SQLINTEGER* nativeError, SQLWCHAR* messageText, SQLSMALLINT bufferLength, SQLSMALLINT* textLength) {
                    if (state) {
                        std::wcsncpy(reinterpret_cast<wchar_t*>(state), L"HY000", 6); // Example SQL state
                    }
                    if (nativeError) {
                        *nativeError = 12345; // Example error code
                    }
                    if (messageText && bufferLength > 0) {
                        std::wcsncpy(reinterpret_cast<wchar_t*>(messageText), L"Connection failed", bufferLength / sizeof(SQLWCHAR));
                        if (textLength) {
                            *textLength = std::wcslen(L"Connection failed");
                        }
                    }
                    return SQL_SUCCESS;
                });

            wrapper->initialize();
            EXPECT_THROW(wrapper->connect(L"MyDSN", L"user", L"pass"), std::runtime_error); // Expect exception
        }

        TEST_F(OdbcWrapperTest, Disconnect_CleansUpResources) {
            // Simulate successful connection
            SQLHANDLE dummyEnvHandle = reinterpret_cast<SQLHANDLE>(1);
            SQLHANDLE dummyDbcHandle = reinterpret_cast<SQLHANDLE>(2);
            SQLHANDLE dummyStmtHandle = reinterpret_cast<SQLHANDLE>(3);

            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, testing::_))
                .WillOnce([&dummyEnvHandle](SQLSMALLINT, SQLHANDLE, SQLHANDLE* envHandle) {
                    *envHandle = dummyEnvHandle; // Assign dummy handle
                    return SQL_SUCCESS;
                });
            EXPECT_CALL(*mock, SQLSetEnvAttr(dummyEnvHandle, SQL_ATTR_ODBC_VERSION, testing::_, 0))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_DBC, dummyEnvHandle, testing::_))
                .WillOnce([&dummyDbcHandle](SQLSMALLINT, SQLHANDLE, SQLHANDLE* dbcHandle) {
                    *dbcHandle = dummyDbcHandle; // Assign dummy handle
                    return SQL_SUCCESS;
                });
            EXPECT_CALL(*mock, SQLConnect(dummyDbcHandle, testing::_, SQL_NTS, testing::_, SQL_NTS, testing::_, SQL_NTS))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_STMT, dummyDbcHandle, testing::_))
                .WillOnce([&dummyStmtHandle](SQLSMALLINT, SQLHANDLE, SQLHANDLE* stmtHandle) {
                    *stmtHandle = dummyStmtHandle; // Assign dummy handle
                    return SQL_SUCCESS;
                });

            wrapper->initialize();
            wrapper->connect(L"MyDSN", L"user", L"pass");

            // Expectations for disconnect
            EXPECT_CALL(*mock, SQLFreeHandle(SQL_HANDLE_STMT, dummyStmtHandle))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLDisconnect(dummyDbcHandle))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLFreeHandle(SQL_HANDLE_DBC, dummyDbcHandle))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLFreeHandle(SQL_HANDLE_ENV, dummyEnvHandle))
                .WillOnce(testing::Return(SQL_SUCCESS));

            wrapper->disconnect();

            EXPECT_FALSE(wrapper->isConnected());
        }
    }
}

int main(int argc, char **argv) {
    // Initialize the ODBC logger
    OdbcLogger::initialize("logs/odbc_test.log");

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
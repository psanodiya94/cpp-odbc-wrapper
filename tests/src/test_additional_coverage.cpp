#include <test_odbcwrapper.h>
#include <odbclogger.h>
#include <cwchar>

namespace ps {
    namespace test {
        /**
         * @test MultipleDisconnects_DoesNotCrash
         * @brief Tests that calling disconnect multiple times doesn't cause issues.
         */
        TEST_F(OdbcWrapperTest, MultipleDisconnects_DoesNotCrash) {
            OdbcLogger::logInfo("Entering MultipleDisconnects_DoesNotCrash");

            // First disconnect should be a no-op (not connected)
            EXPECT_NO_THROW(wrapper->disconnect());
            EXPECT_NO_THROW(wrapper->disconnect());

            OdbcLogger::logInfo("Exiting MultipleDisconnects_DoesNotCrash");
        }

        /**
         * @test Initialize_FailureInSetEnvAttr
         * @brief Tests initialization failure when SQLSetEnvAttr fails.
         */
        TEST_F(OdbcWrapperTest, Initialize_FailureInSetEnvAttr) {
            OdbcLogger::logInfo("Entering Initialize_FailureInSetEnvAttr");

            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, testing::_))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLSetEnvAttr(testing::_, SQL_ATTR_ODBC_VERSION, testing::_, 0))
                .WillOnce(testing::Return(SQL_ERROR));
            EXPECT_CALL(*mock, SQLGetDiagRec(SQL_HANDLE_ENV, testing::_, 1, testing::_, testing::_, testing::_, testing::_, testing::_))
                .WillOnce(testing::Return(SQL_NO_DATA));

            EXPECT_THROW(wrapper->initialize(), std::runtime_error);

            OdbcLogger::logInfo("Exiting Initialize_FailureInSetEnvAttr");
        }

        /**
         * @test Initialize_FailureInAllocConnectionHandle
         * @brief Tests initialization failure when allocating connection handle fails.
         */
        TEST_F(OdbcWrapperTest, Initialize_FailureInAllocConnectionHandle) {
            OdbcLogger::logInfo("Entering Initialize_FailureInAllocConnectionHandle");

            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, testing::_))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLSetEnvAttr(testing::_, SQL_ATTR_ODBC_VERSION, testing::_, 0))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_DBC, testing::_, testing::_))
                .WillOnce(testing::Return(SQL_ERROR));
            EXPECT_CALL(*mock, SQLGetDiagRec(SQL_HANDLE_ENV, testing::_, 1, testing::_, testing::_, testing::_, testing::_, testing::_))
                .WillOnce(testing::Return(SQL_NO_DATA));

            EXPECT_THROW(wrapper->initialize(), std::runtime_error);

            OdbcLogger::logInfo("Exiting Initialize_FailureInAllocConnectionHandle");
        }

        /**
         * @test ExecuteUpdate_FailsIfNotConnected
         * @brief Tests that executeUpdate fails when not connected.
         */
        TEST_F(OdbcWrapperTest, ExecuteUpdate_FailsIfNotConnected) {
            OdbcLogger::logInfo("Entering ExecuteUpdate_FailsIfNotConnected");

            bool result = wrapper->executeUpdate(L"INSERT INTO table VALUES (1)");
            EXPECT_FALSE(result);

            OdbcLogger::logInfo("Exiting ExecuteUpdate_FailsIfNotConnected");
        }

        /**
         * @test ExecuteUpdate_FailureHandlesError
         * @brief Tests that executeUpdate handles errors properly.
         */
        TEST_F(OdbcWrapperTest, ExecuteUpdate_FailureHandlesError) {
            OdbcLogger::logInfo("Entering ExecuteUpdate_FailureHandlesError");

            // Simulate successful connection
            EXPECT_CALL(*mock, SQLConnect(testing::_, testing::_, SQL_NTS, testing::_, SQL_NTS, testing::_, SQL_NTS))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_STMT, testing::_, testing::_))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLDisconnect(nullptr))
                .Times(testing::AnyNumber())
                .WillRepeatedly(testing::Return(SQL_SUCCESS));

            wrapper->connect(L"MyDSN", L"user", L"pass");

            // Simulate failure in SQLExecDirect
            EXPECT_CALL(*mock, SQLExecDirect(testing::_, testing::_, SQL_NTS))
                .WillOnce(testing::Return(SQL_ERROR));
            EXPECT_CALL(*mock, SQLGetDiagRec(SQL_HANDLE_STMT, testing::_, 1, testing::_, testing::_, testing::_, testing::_, testing::_))
                .WillOnce([](SQLSMALLINT, SQLHANDLE, SQLSMALLINT, SQLWCHAR* state, SQLINTEGER* nativeError, SQLWCHAR* messageText, SQLSMALLINT bufferLength, SQLSMALLINT* textLength) {
                    if (state) {
                        std::wcsncpy(reinterpret_cast<wchar_t*>(state), L"HY000", 6);
                    }
                    if (nativeError) {
                        *nativeError = 12345;
                    }
                    if (messageText && bufferLength > 0) {
                        std::wcsncpy(reinterpret_cast<wchar_t*>(messageText), L"Update failed", bufferLength / sizeof(SQLWCHAR));
                        if (textLength) {
                            *textLength = std::wcslen(L"Update failed");
                        }
                    }
                    return SQL_SUCCESS;
                });
            EXPECT_CALL(*mock, SQLRowCount(testing::_, testing::_))
                .Times(0); // Should not be called on error

            EXPECT_THROW(wrapper->executeUpdate(L"INSERT INTO table VALUES (1)"), std::runtime_error);

            OdbcLogger::logInfo("Exiting ExecuteUpdate_FailureHandlesError");
        }

        /**
         * @test FetchResults_FailsIfNotConnected
         * @brief Tests that fetchResults returns empty when not connected.
         */
        TEST_F(OdbcWrapperTest, FetchResults_FailsIfNotConnected) {
            OdbcLogger::logInfo("Entering FetchResults_FailsIfNotConnected");

            auto results = wrapper->fetchResults();
            EXPECT_TRUE(results.empty());

            OdbcLogger::logInfo("Exiting FetchResults_FailsIfNotConnected");
        }

        /**
         * @test GetHEnv_ReturnsCorrectHandle
         * @brief Tests that getHEnv returns the environment handle.
         */
        TEST_F(OdbcWrapperTest, GetHEnv_ReturnsCorrectHandle) {
            OdbcLogger::logInfo("Entering GetHEnv_ReturnsCorrectHandle");

            SQLHANDLE dummyEnvHandle = reinterpret_cast<SQLHANDLE>(1);

            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, testing::_))
                .WillOnce([&dummyEnvHandle](SQLSMALLINT, SQLHANDLE, SQLHANDLE* envHandle) {
                    *envHandle = dummyEnvHandle;
                    return SQL_SUCCESS;
                });
            EXPECT_CALL(*mock, SQLSetEnvAttr(testing::_, SQL_ATTR_ODBC_VERSION, testing::_, 0))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_DBC, testing::_, testing::_))
                .WillOnce(testing::Return(SQL_SUCCESS));

            wrapper->initialize();
            EXPECT_EQ(wrapper->getHEnv(), dummyEnvHandle);

            OdbcLogger::logInfo("Exiting GetHEnv_ReturnsCorrectHandle");
        }

        /**
         * @test GetHDbc_ReturnsCorrectHandle
         * @brief Tests that getHDbc returns the connection handle.
         */
        TEST_F(OdbcWrapperTest, GetHDbc_ReturnsCorrectHandle) {
            OdbcLogger::logInfo("Entering GetHDbc_ReturnsCorrectHandle");

            SQLHANDLE dummyDbcHandle = reinterpret_cast<SQLHANDLE>(2);

            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, testing::_))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLSetEnvAttr(testing::_, SQL_ATTR_ODBC_VERSION, testing::_, 0))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_DBC, testing::_, testing::_))
                .WillOnce([&dummyDbcHandle](SQLSMALLINT, SQLHANDLE, SQLHANDLE* dbcHandle) {
                    *dbcHandle = dummyDbcHandle;
                    return SQL_SUCCESS;
                });

            wrapper->initialize();
            EXPECT_EQ(wrapper->getHDbc(), dummyDbcHandle);

            OdbcLogger::logInfo("Exiting GetHDbc_ReturnsCorrectHandle");
        }

        /**
         * @test GetHStmt_ReturnsCorrectHandle
         * @brief Tests that getHStmt returns the statement handle.
         */
        TEST_F(OdbcWrapperTest, GetHStmt_ReturnsCorrectHandle) {
            OdbcLogger::logInfo("Entering GetHStmt_ReturnsCorrectHandle");

            SQLHANDLE dummyStmtHandle = reinterpret_cast<SQLHANDLE>(3);

            EXPECT_CALL(*mock, SQLConnect(testing::_, testing::_, SQL_NTS, testing::_, SQL_NTS, testing::_, SQL_NTS))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_STMT, testing::_, testing::_))
                .WillOnce([&dummyStmtHandle](SQLSMALLINT, SQLHANDLE, SQLHANDLE* stmtHandle) {
                    *stmtHandle = dummyStmtHandle;
                    return SQL_SUCCESS;
                });
            EXPECT_CALL(*mock, SQLDisconnect(nullptr))
                .Times(testing::AnyNumber())
                .WillRepeatedly(testing::Return(SQL_SUCCESS));

            wrapper->connect(L"MyDSN", L"user", L"pass");
            EXPECT_EQ(wrapper->getHStmt(), dummyStmtHandle);

            OdbcLogger::logInfo("Exiting GetHStmt_ReturnsCorrectHandle");
        }

        /**
         * @test HandleError_WithSuccessWithInfo
         * @brief Tests error handling for SQL_SUCCESS_WITH_INFO return codes.
         */
        TEST_F(OdbcWrapperTest, Connect_SuccessWithInfo) {
            OdbcLogger::logInfo("Entering Connect_SuccessWithInfo");

            EXPECT_CALL(*mock, SQLConnect(testing::_, testing::_, SQL_NTS, testing::_, SQL_NTS, testing::_, SQL_NTS))
                .WillOnce(testing::Return(SQL_SUCCESS_WITH_INFO));
            EXPECT_CALL(*mock, SQLAllocHandle(SQL_HANDLE_STMT, testing::_, testing::_))
                .WillOnce(testing::Return(SQL_SUCCESS));
            EXPECT_CALL(*mock, SQLGetDiagRec(SQL_HANDLE_DBC, testing::_, 1, testing::_, testing::_, testing::_, testing::_, testing::_))
                .WillOnce([](SQLSMALLINT, SQLHANDLE, SQLSMALLINT, SQLWCHAR* state, SQLINTEGER* nativeError, SQLWCHAR* messageText, SQLSMALLINT bufferLength, SQLSMALLINT* textLength) {
                    if (state) {
                        std::wcsncpy(reinterpret_cast<wchar_t*>(state), L"01000", 6);
                    }
                    if (nativeError) {
                        *nativeError = 0;
                    }
                    if (messageText && bufferLength > 0) {
                        std::wcsncpy(reinterpret_cast<wchar_t*>(messageText), L"Connection succeeded with info", bufferLength / sizeof(SQLWCHAR));
                        if (textLength) {
                            *textLength = std::wcslen(L"Connection succeeded with info");
                        }
                    }
                    return SQL_SUCCESS;
                });

            bool result = wrapper->connect(L"MyDSN", L"user", L"pass");
            EXPECT_TRUE(result); // Should still succeed

            OdbcLogger::logInfo("Exiting Connect_SuccessWithInfo");
        }
    }
}

int main(int argc, char **argv) {
    OdbcLogger::initialize("logs/odbc_additional_test.log");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

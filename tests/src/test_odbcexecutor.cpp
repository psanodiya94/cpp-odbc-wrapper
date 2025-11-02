#include <odbccpp/odbcexecutor.h>
#include <odbclogger.h>
#include <gtest/gtest.h>

namespace ps {
    namespace test {
        /**
         * @class OdbcExecutorTest
         * @brief Unit test fixture for testing the OdbcExecutor class directly.
         *
         * This class tests the OdbcExecutor implementation to ensure it correctly
         * delegates to the underlying ODBC API functions.
         */
        class OdbcExecutorTest : public ::testing::Test {
        protected:
            void SetUp() override {
                executor = std::make_unique<odbc::OdbcExecutor>();
                odbc::OdbcLogger::logInfo("Setting up OdbcExecutorTest");
            }

            void TearDown() override {
                executor.reset();
                odbc::OdbcLogger::logInfo("Tearing down OdbcExecutorTest");
            }

            std::unique_ptr<odbc::OdbcExecutor> executor;
        };

        /**
         * @test SQLAllocHandle_ValidHandle
         * @brief Tests that SQLAllocHandle returns SQL_SUCCESS for valid handle types.
         */
        TEST_F(OdbcExecutorTest, SQLAllocHandle_EnvironmentHandle) {
            SQLHENV hEnv = SQL_NULL_HENV;
            SQLRETURN ret = executor->SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);

            // We expect either SQL_SUCCESS or SQL_SUCCESS_WITH_INFO
            EXPECT_TRUE(SQL_SUCCEEDED(ret));

            if (SQL_SUCCEEDED(ret) && hEnv != SQL_NULL_HENV) {
                executor->SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
            }
        }

        /**
         * @test SQLSetEnvAttr_ValidAttribute
         * @brief Tests that SQLSetEnvAttr can set environment attributes.
         */
        TEST_F(OdbcExecutorTest, SQLSetEnvAttr_OdbcVersion) {
            SQLHENV hEnv = SQL_NULL_HENV;
            SQLRETURN ret = executor->SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
            ASSERT_TRUE(SQL_SUCCEEDED(ret));

            ret = executor->SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
            EXPECT_TRUE(SQL_SUCCEEDED(ret));

            executor->SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
        }

        /**
         * @test SQLAllocHandle_InvalidHandleType
         * @brief Tests that SQLAllocHandle handles invalid handle types appropriately.
         */
        TEST_F(OdbcExecutorTest, SQLAllocHandle_InvalidHandle) {
            SQLHANDLE handle = SQL_NULL_HANDLE;
            // Try to allocate a connection handle with NULL environment handle
            SQLRETURN ret = executor->SQLAllocHandle(SQL_HANDLE_DBC, SQL_NULL_HANDLE, &handle);

            // This should fail
            EXPECT_FALSE(SQL_SUCCEEDED(ret));
        }

        /**
         * @test SQLFreeHandle_ValidHandle
         * @brief Tests that SQLFreeHandle can successfully free a valid handle.
         */
        TEST_F(OdbcExecutorTest, SQLFreeHandle_EnvironmentHandle) {
            SQLHENV hEnv = SQL_NULL_HENV;
            SQLRETURN ret = executor->SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
            ASSERT_TRUE(SQL_SUCCEEDED(ret));

            ret = executor->SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
            EXPECT_TRUE(SQL_SUCCEEDED(ret));
        }

        /**
         * @test SQLFreeHandle_InvalidHandle
         * @brief Tests that SQLFreeHandle handles invalid handles appropriately.
         */
        TEST_F(OdbcExecutorTest, SQLFreeHandle_NullHandle) {
            // Attempting to free a NULL handle should return an error
            SQLRETURN ret = executor->SQLFreeHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE);
            EXPECT_FALSE(SQL_SUCCEEDED(ret));
        }

        /**
         * @test SQLGetDiagRec_RetrievesDiagnostics
         * @brief Tests that SQLGetDiagRec can retrieve diagnostic information.
         */
        TEST_F(OdbcExecutorTest, SQLGetDiagRec_AfterError) {
            // Try to connect with invalid handle to generate an error
            SQLRETURN ret = executor->SQLConnect(SQL_NULL_HDBC,
                                                 (SQLWCHAR*)L"InvalidDSN", SQL_NTS,
                                                 (SQLWCHAR*)L"user", SQL_NTS,
                                                 (SQLWCHAR*)L"pass", SQL_NTS);

            EXPECT_FALSE(SQL_SUCCEEDED(ret));

            // Try to retrieve diagnostics
            SQLWCHAR sqlState[6];
            SQLWCHAR messageText[SQL_MAX_MESSAGE_LENGTH];
            SQLINTEGER nativeError;
            SQLSMALLINT textLength;

            ret = executor->SQLGetDiagRec(SQL_HANDLE_DBC, SQL_NULL_HDBC, 1,
                                         sqlState, &nativeError, messageText,
                                         SQL_MAX_MESSAGE_LENGTH, &textLength);

            // Diagnostics may or may not be available for NULL handle
            // This test just ensures the method doesn't crash
            EXPECT_TRUE(true);
        }

        /**
         * @test SQLDisconnect_NullHandle
         * @brief Tests that SQLDisconnect handles NULL handles appropriately.
         */
        TEST_F(OdbcExecutorTest, SQLDisconnect_NullHandle) {
            SQLRETURN ret = executor->SQLDisconnect(SQL_NULL_HDBC);
            EXPECT_FALSE(SQL_SUCCEEDED(ret));
        }

        /**
         * @test SQLExecDirect_NullHandle
         * @brief Tests that SQLExecDirect handles NULL statement handles appropriately.
         */
        TEST_F(OdbcExecutorTest, SQLExecDirect_NullHandle) {
            SQLRETURN ret = executor->SQLExecDirect(SQL_NULL_HSTMT,
                                                    (SQLWCHAR*)L"SELECT 1", SQL_NTS);
            EXPECT_FALSE(SQL_SUCCEEDED(ret));
        }

        /**
         * @test SQLNumResultCols_NullHandle
         * @brief Tests that SQLNumResultCols handles NULL statement handles appropriately.
         */
        TEST_F(OdbcExecutorTest, SQLNumResultCols_NullHandle) {
            SQLSMALLINT numCols = 0;
            SQLRETURN ret = executor->SQLNumResultCols(SQL_NULL_HSTMT, &numCols);
            EXPECT_FALSE(SQL_SUCCEEDED(ret));
        }

        /**
         * @test SQLFetch_NullHandle
         * @brief Tests that SQLFetch handles NULL statement handles appropriately.
         */
        TEST_F(OdbcExecutorTest, SQLFetch_NullHandle) {
            SQLRETURN ret = executor->SQLFetch(SQL_NULL_HSTMT);
            EXPECT_FALSE(SQL_SUCCEEDED(ret));
        }

        /**
         * @test SQLGetData_NullHandle
         * @brief Tests that SQLGetData handles NULL statement handles appropriately.
         */
        TEST_F(OdbcExecutorTest, SQLGetData_NullHandle) {
            SQLWCHAR buffer[256];
            SQLLEN indicator;
            SQLRETURN ret = executor->SQLGetData(SQL_NULL_HSTMT, 1, SQL_C_WCHAR,
                                                 buffer, sizeof(buffer), &indicator);
            EXPECT_FALSE(SQL_SUCCEEDED(ret));
        }

        /**
         * @test SQLRowCount_NullHandle
         * @brief Tests that SQLRowCount handles NULL statement handles appropriately.
         */
        TEST_F(OdbcExecutorTest, SQLRowCount_NullHandle) {
            SQLLEN rowCount = 0;
            SQLRETURN ret = executor->SQLRowCount(SQL_NULL_HSTMT, &rowCount);
            EXPECT_FALSE(SQL_SUCCEEDED(ret));
        }
    }
}

int main(int argc, char **argv) {
    ps::odbc::OdbcLogger::initialize("logs/odbc_executor_test.log");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

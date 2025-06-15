#ifndef TEST_ODBC_WRAPPER_H
#define TEST_ODBC_WRAPPER_H

#include <odbccpp/odbcinterface.h>
#include <odbccpp/odbcwrapper.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

using namespace ps::odbc;

namespace ps {
    namespace test {
        /**
         * @class MockOdbcInterface
         * @brief Mock implementation of the OdbcInterface for unit testing.
         *
         * This class uses Google Mock to provide mock implementations of the OdbcInterface methods,
         * allowing for controlled testing of the OdbcWrapper class.
         */
        class MockOdbcInterface : public OdbcInterface {
        public:
            /**
             * @brief Mock method for SQLAllocHandle.
             */
            MOCK_METHOD3(SQLAllocHandle, SQLRETURN(SQLSMALLINT, SQLHANDLE, SQLHANDLE*));

            /**
             * @brief Mock method for SQLSetEnvAttr.
             */
            MOCK_METHOD4(SQLSetEnvAttr, SQLRETURN(SQLHENV, SQLINTEGER, SQLPOINTER, SQLINTEGER));

            /**
             * @brief Mock method for SQLConnect.
             */
            MOCK_METHOD7(SQLConnect, SQLRETURN(SQLHDBC, SQLWCHAR*, SQLSMALLINT, SQLWCHAR*, SQLSMALLINT, SQLWCHAR*, SQLSMALLINT));

            /**
             * @brief Mock method for SQLDisconnect.
             */
            MOCK_METHOD1(SQLDisconnect, SQLRETURN(SQLHDBC));

            /**
             * @brief Mock method for SQLFreeHandle.
             */
            MOCK_METHOD2(SQLFreeHandle, SQLRETURN(SQLSMALLINT, SQLHANDLE));

            /**
             * @brief Mock method for SQLExecDirect.
             */
            MOCK_METHOD3(SQLExecDirect, SQLRETURN(SQLHSTMT, SQLWCHAR*, SQLINTEGER));

            /**
             * @brief Mock method for SQLNumResultCols.
             */
            MOCK_METHOD2(SQLNumResultCols, SQLRETURN(SQLHSTMT, SQLSMALLINT*));

            /**
             * @brief Mock method for SQLFetch.
             */
            MOCK_METHOD1(SQLFetch, SQLRETURN(SQLHSTMT));

            /**
             * @brief Mock method for SQLGetData.
             */
            MOCK_METHOD6(SQLGetData, SQLRETURN(SQLHSTMT, SQLUSMALLINT, SQLSMALLINT, SQLPOINTER, SQLLEN, SQLLEN*));

            /**
             * @brief Mock method for SQLRowCount.
             */
            MOCK_METHOD2(SQLRowCount, SQLRETURN(SQLHSTMT, SQLLEN*));

            /**
             * @brief Mock method for SQLGetDiagRec.
             */
            MOCK_METHOD8(SQLGetDiagRec, SQLRETURN(SQLSMALLINT, SQLHANDLE, SQLSMALLINT, SQLWCHAR*, SQLINTEGER*, SQLWCHAR*, SQLSMALLINT, SQLSMALLINT*));
        };

        /**
         * @class OdbcWrapperTest
         * @brief Unit test fixture for testing the OdbcWrapper class.
         *
         * This class sets up a test environment for the OdbcWrapper class using Google Test and Google Mock.
         * It provides a mock implementation of the OdbcInterface and initializes the OdbcWrapper with it.
         */
        class OdbcWrapperTest : public ::testing::Test {
        protected:
            /**
             * @brief Sets up the test environment.
             *
             * This method initializes the mock OdbcInterface and the OdbcWrapper instance.
             * It also sets default behavior for the SQLGetDiagRec mock method.
             */
            void SetUp() override {
                std::unique_ptr<MockOdbcInterface> t_mock = std::make_unique<MockOdbcInterface>();
                wrapper = std::make_unique<OdbcWrapper>(std::move(t_mock));
                mock = static_cast<MockOdbcInterface*>(wrapper->getOdbcInterface());

                // Default behavior for SQLGetDiagRec
                ON_CALL(*mock, SQLGetDiagRec(testing::_, testing::_, testing::_, testing::_, testing::_, testing::_, testing::_, testing::_))
                    .WillByDefault([](SQLSMALLINT, SQLHANDLE, SQLSMALLINT, SQLWCHAR* state, SQLINTEGER* nativeError, SQLWCHAR* messageText, SQLSMALLINT bufferLength, SQLSMALLINT* textLength) {
                        if (state) {
                            std::wcsncpy(reinterpret_cast<wchar_t*>(state), L"HY000", 6); // Default SQL state
                        }
                        if (nativeError) {
                            *nativeError = 12345; // Default error code
                        }
                        if (messageText && bufferLength > 0) {
                            std::wcsncpy(reinterpret_cast<wchar_t*>(messageText), L"Default error message", bufferLength / sizeof(SQLWCHAR));
                            if (textLength) {
                                *textLength = std::wcslen(L"Default error message");
                            }
                        }
                        return SQL_SUCCESS;
                    });
            }

            /**
             * @brief Tears down the test environment.
             *
             * This method releases the OdbcWrapper instance and resets the mock pointer.
             */
            void TearDown() override {
                wrapper.reset();
                mock = nullptr;
            }

            std::unique_ptr<OdbcWrapper> wrapper; ///< The OdbcWrapper instance under test.
            MockOdbcInterface* mock; ///< Non-owning pointer to the mock OdbcInterface for setting expectations.
        };
    }
}

#endif // TEST_ODBC_WRAPPER_H

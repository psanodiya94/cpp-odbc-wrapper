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
        class MockOdbcInterface : public OdbcInterface {
        public:
            MOCK_METHOD3(SQLAllocHandle, SQLRETURN(SQLSMALLINT, SQLHANDLE, SQLHANDLE*));
            MOCK_METHOD4(SQLSetEnvAttr, SQLRETURN(SQLHENV, SQLINTEGER, SQLPOINTER, SQLINTEGER));
            MOCK_METHOD7(SQLConnect, SQLRETURN(SQLHDBC, SQLWCHAR*, SQLSMALLINT, SQLWCHAR*, SQLSMALLINT, SQLWCHAR*, SQLSMALLINT));
            MOCK_METHOD1(SQLDisconnect, SQLRETURN(SQLHDBC));
            MOCK_METHOD2(SQLFreeHandle, SQLRETURN(SQLSMALLINT, SQLHANDLE));
            MOCK_METHOD3(SQLExecDirect, SQLRETURN(SQLHSTMT, SQLWCHAR*, SQLINTEGER));
            MOCK_METHOD2(SQLNumResultCols, SQLRETURN(SQLHSTMT, SQLSMALLINT*));
            MOCK_METHOD1(SQLFetch, SQLRETURN(SQLHSTMT));
            MOCK_METHOD6(SQLGetData, SQLRETURN(SQLHSTMT, SQLUSMALLINT, SQLSMALLINT, SQLPOINTER, SQLLEN, SQLLEN*));
            MOCK_METHOD2(SQLRowCount, SQLRETURN(SQLHSTMT, SQLLEN*));
            MOCK_METHOD8(SQLGetDiagRec, SQLRETURN(SQLSMALLINT, SQLHANDLE, SQLSMALLINT, SQLWCHAR*, SQLINTEGER*, SQLWCHAR*, SQLSMALLINT, SQLSMALLINT*));
        };

        class OdbcWrapperTest : public ::testing::Test {
        protected:
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

            void TearDown() override {
                wrapper.reset();
                mock = nullptr;
            }

            std::unique_ptr<OdbcWrapper> wrapper;
            MockOdbcInterface* mock; // Non-owning pointer for setting expectations
        };
    }
}

#endif // TEST_ODBC_WRAPPER_H

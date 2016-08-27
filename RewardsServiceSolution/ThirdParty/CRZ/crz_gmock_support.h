#pragma once

#define CRZ_QTEST_QMOCK_APPLESS_MAIN(TestObject) \
int main(int argc, char *argv[]) \
{ \
    ::testing::GTEST_FLAG(throw_on_failure) = true; \
    ::testing::InitGoogleMock(&argc, argv); \
    TestObject tc; \
    QTEST_SET_MAIN_SOURCE_PATH \
    return QTest::qExec(&tc, argc, argv); \
}



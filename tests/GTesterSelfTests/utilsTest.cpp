#include <gtest/gtest.h>

#include <utils.h>

void PrintTo(const QString& str, ::std::ostream* os)
{
    *os << "QString(\"" << str.toStdString() << "\")";
}

TEST(UtilsTest, makeRelative)
{
    EXPECT_EQ(QString("to"), Utils::makeRelative("/to", "/other"));
    EXPECT_EQ(QString("to"), Utils::makeRelative("/long/path/to", "/long/path/other"));
    EXPECT_EQ(QString("../to"), Utils::makeRelative("/path/to", "/path/one/deeper"));
    EXPECT_EQ(QString("../../to"), Utils::makeRelative("/path/to", "/path/many/more/deeper"));
    EXPECT_EQ(QString("../to"), Utils::makeRelative("/another/path/to", "/another/path/one/deeper"));
}

TEST(UtilsTest, makeAbsolute)
{
    EXPECT_EQ(QString("/to"), Utils::makeAbsolute("to", "/other"));
    EXPECT_EQ(QString("/long/path/to"), Utils::makeAbsolute("to", "/long/path/other"));
    EXPECT_EQ(QString("/path/to"), Utils::makeAbsolute("../to", "/path/one/deeper"));
    EXPECT_EQ(QString("/path/to"), Utils::makeAbsolute("../../to", "/path/many/more/deeper"));
    EXPECT_EQ(QString("/another/path/to"), Utils::makeAbsolute("../to", "/another/path/one/deeper"));
}

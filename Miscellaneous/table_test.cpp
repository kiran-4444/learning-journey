#include "table.hpp"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <gtest/gtest.h>
#include <string>

class TableTest : public ::testing::Test
{
  protected:
    std::string table_name;

    Row create_dummy_row(uint32_t id, const char *username, const char *email)
    {
        Row row;
        row.id = id;

        std::strncpy(row.username, username, USERNAME_SIZE);
        std::strncpy(row.email, email, EMAIL_SIZE);

        row.username[USERNAME_SIZE - 1] = '\0';
        row.email[EMAIL_SIZE - 1] = '\0';

        return row;
    }

    void SetUp() override { table_name = std::to_string(rand()); }

    void TearDown() override { std::remove(table_name.c_str()); }
};

TEST_F(TableTest, HandlesRowInsertion)
{
    Table table(table_name.c_str());
    Row row = create_dummy_row(1, "username", "email");
    table.insert_row(row);

    EXPECT_EQ(table.get_row_count(), 1);
}

TEST_F(TableTest, HandlesRowFetch)
{
    Row row_1 = create_dummy_row(2, "username", "email");

    {
        Table table(table_name.c_str());
        table.insert_row(row_1);
    }

    Table table(table_name.c_str());
    Row row_2 = table.get_row(0);

    EXPECT_EQ(row_1.id, row_2.id);
}
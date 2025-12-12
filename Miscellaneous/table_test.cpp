#include "table.hpp"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <gtest/gtest.h>
#include <string>

Row create_row(uint32_t id)
{
    Row row;
    row.id = id;
    std::string username = "user" + std::to_string(id);
    std::string email = "person" + std::to_string(id) + "@example.com";

    std::strncpy(row.username, username.c_str(), USERNAME_SIZE);
    std::strncpy(row.email, email.c_str(), EMAIL_SIZE);
    return row;
}

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
    Row row = create_row(1);
    table.insert_row(row);

    EXPECT_EQ(table.get_row_count(), 1);
}

TEST_F(TableTest, HandlesRowFetch)
{
    Row row_1 = create_row(2);

    {
        Table table(table_name.c_str());
        table.insert_row(row_1);
    }

    Table table(table_name.c_str());
    Row row_2 = table.get_row(0);

    EXPECT_EQ(row_1.id, row_2.id);
}

TEST_F(TableTest, HandlesPageBoundaries)
{
    const int ROW_COUNT = 20;

    Table table(table_name.c_str());

    for (int i = 0; i < ROW_COUNT; i++)
    {
        table.insert_row(create_row(i));
    }

    EXPECT_EQ(table.get_row_count(), ROW_COUNT);

    // Verify a row that should live on Page 0
    Row r0 = table.get_row(0);
    EXPECT_EQ(r0.id, 0);

    // Verify a row that should live on Page 1 (Index 14 is the 15th row)
    Row r14 = table.get_row(14);
    EXPECT_EQ(r14.id, 14);

    // Verify the last row
    Row r19 = table.get_row(19);
    EXPECT_EQ(r19.id, 19);
}
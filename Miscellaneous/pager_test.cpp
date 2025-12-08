#include "pager.hpp"
#include <cstring>
#include <gtest/gtest.h>

class PagerTest : public ::testing::Test
{
  protected:
    std::string test_filename;

    void SetUp() override
    {
        test_filename = "test_db_ " + std::to_string(rand()) + ".bin";
    }

    void TearDown() override { std::remove(test_filename.c_str()); }
};

// Test persistence
TEST_F(PagerTest, HandlesPersistence)
{
    char data[]("Persistent Data");

    // This closes p1 after going out of scope.
    // Hence mimic-ing the sudden shutdown to Pager.
    {
        Pager p1(test_filename.c_str());
        char buffer[PAGE_SIZE] = {0};
        std::strcpy(buffer, data);
        p1.write_page(0, buffer);
    }

    Pager p2(test_filename.c_str());
    char read_buffer[PAGE_SIZE] = {0};
    p2.read_page(0, read_buffer);

    EXPECT_STREQ(data, read_buffer);
}

TEST_F(PagerTest, HandlesIsolatedOperations)
{

    Pager p(test_filename.c_str());
    char buffer_0[PAGE_SIZE] = {0};
    char buffer_1[PAGE_SIZE] = {0};

    std::strcpy(buffer_0, "Page 0: data");
    std::strcpy(buffer_1, "Page 1: data");

    p.write_page(0, buffer_0);
    p.write_page(1, buffer_1);

    char read_buffer[PAGE_SIZE] = {0};

    p.read_page(0, read_buffer);
    EXPECT_STREQ(read_buffer, buffer_0);

    p.read_page(1, read_buffer);
    EXPECT_STREQ(read_buffer, buffer_1);
}

TEST_F(PagerTest, HandlesLengthCalculation)
{
    Pager p(test_filename.c_str());

    char buffer[PAGE_SIZE] = {0};
    std::strcpy(buffer, "Page data");

    p.write_page(0, buffer);
    EXPECT_EQ(p.length(), 1);

    // Check overriding
    p.write_page(0, buffer);
    EXPECT_EQ(p.length(), 1);

    // Check Incremental length update
    p.write_page(1, buffer);
    p.write_page(2, buffer);
    EXPECT_EQ(p.length(), 3);
}

TEST_F(PagerTest, ThrowsOnOutOfBoundsRead)
{
    Pager p(test_filename.c_str());
    char buf[PAGE_SIZE];

    // Reading a page that doesn't exist should throw
    EXPECT_THROW(p.read_page(100, buf), std::runtime_error);
}

TEST_F(PagerTest, WriteToLargePageID)
{
    Pager p(test_filename.c_str());

    char buffer[PAGE_SIZE] = {0};
    std::strcpy(buffer, "large page id");

    p.write_page(1000, buffer);

    EXPECT_EQ(p.length(), 1001);
}

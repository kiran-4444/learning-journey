#include "pager.hpp"
#include "row.hpp"
#include <gtest/gtest.h>

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

TEST(RowTest, SerializeAndDeserializeCorrectly)
{
    Row original = {1, "username", "email"};
    char buffer[PAGE_SIZE] = {0};
    serialize(buffer, original);

    Row deserialized;
    deserialize(deserialized, buffer);

    EXPECT_EQ(original.id, deserialized.id);
    EXPECT_STREQ(original.username, deserialized.username);
    EXPECT_STREQ(original.email, deserialized.email);
}

TEST(RowTest, IsCompactlySerialized)
{
    Row row = create_dummy_row(2, "username", "email");
    const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;
    char buffer[ROW_SIZE] = {0};
    serialize(buffer, row);

    // If serialize serialized the row compactly (without padding), we'll get the struct
    // fields in the expected offsets

    uint32_t id_buffer;
    std::memcpy(&id_buffer, buffer, sizeof(uint32_t));
    EXPECT_EQ(id_buffer, row.id);

    char username[USERNAME_SIZE];
    std::memcpy(username, buffer + USERNAME_OFFSET, USERNAME_SIZE);
    EXPECT_STREQ(username, row.username);

    char email[EMAIL_SIZE];
    std::memcpy(email, buffer + EMAIL_OFFSET, EMAIL_SIZE);
    EXPECT_STREQ(email, row.email);
}

TEST(RowTest, MaxLength)
{
    std::string long_username(USERNAME_SIZE - 1, 'a');
    std::string long_email(EMAIL_SIZE - 1, 'b');

    Row original = create_dummy_row(999, long_username.c_str(), long_email.c_str());
    char buffer[PAGE_SIZE] = {0};

    serialize(buffer, original);

    Row deserialized;

    deserialize(deserialized, buffer);

    EXPECT_EQ(deserialized.id, original.id);
    EXPECT_STREQ(deserialized.username, original.username);
    EXPECT_STREQ(deserialized.email, original.email);
}
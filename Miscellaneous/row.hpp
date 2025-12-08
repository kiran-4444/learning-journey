#include <cstdint>
#include <cstring>

#include <cstdlib>
#include <cstring>
#include <sys/types.h>

const uint32_t ID_SIZE = sizeof(uint32_t);
const uint32_t USERNAME_SIZE = 32;
const uint32_t EMAIL_SIZE = 255;

const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;

const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

struct Row
{
    uint32_t id;
    char username[USERNAME_SIZE];
    char email[EMAIL_SIZE];
};

void serialize(char *destination, const Row &source)
{
    memcpy(destination, &source.id, ID_SIZE);
    memcpy(destination + USERNAME_OFFSET, &source.username, USERNAME_SIZE);
    memcpy(destination + EMAIL_OFFSET, &source.email, EMAIL_SIZE);
}

void deserialize(Row &destination, const char *source)
{
    memcpy(&destination.id, source, ID_SIZE);
    memcpy(&destination.username, source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&destination.email, source + EMAIL_OFFSET, EMAIL_SIZE);
}
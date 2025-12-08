#include <cstdint>
#include <cstring>

const uint32_t ID_SIZE = sizeof(uint32_t);
const uint32_t USERNAME_SIZE = 32;
const uint32_t EMAIL_SIZE = 255;

const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;

struct Row
{
    uint32_t id;
    char username[USERNAME_SIZE];
    char email[EMAIL_SIZE];
};

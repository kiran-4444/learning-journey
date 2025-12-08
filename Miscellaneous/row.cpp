#include "row.hpp"
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/types.h>

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

int main()
{
    Row row = {1213, "name", "email"};
    char buffer[4096] = {0};
    serialize(buffer, row);
    deserialize(row, buffer);
    std::cout << row.id << " " << row.username << " " << row.email << std::endl;
}
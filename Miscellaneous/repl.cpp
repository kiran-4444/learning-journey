#include "table.hpp"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

std::vector<std::string> split_by_whitespace(const std::string &str)
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (ss >> token)
    {
        tokens.push_back(token);
    }
    return tokens;
}

int main(int args, char **argv)
{
    std::string table_name = argv[1];
    Table table(table_name.c_str());

    while (true)
    {
        std::cout << table_name << " > ";
        std::string command;
        std::getline(std::cin, command);
        std::vector<std::string> parts = split_by_whitespace(command);

        if (parts[0] == "exit")
        {
            std::cout << "Bye!" << std::endl;
            break;
        }
        else if (parts[0] == "insert")
        {

            Row row;
            row.id = std::stoi(parts[1]);
            std::strncpy(row.username, parts[2].c_str(), USERNAME_SIZE);
            std::strncpy(row.email, parts[3].c_str(), EMAIL_SIZE);
            table.insert_row(row);
            std::cout << "Executed." << std::endl;
        }
        else if (parts[0] == "select")
        {
            uint32_t row_count = table.get_row_count();
            for (int i = 0; i < row_count; i++)
            {
                Row row = table.get_row(i);
                printf("(%d, %s, %s)\n", row.id, row.username, row.email);
            }
        }
    }
}
#include "table.hpp"
#include "thread_safe_queue.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <thread>
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

void db_worker(ThreadSafeQueue<std::function<void()>> &queue)
{
    while (true)
    {
        std::function<void()> job = queue.wait_and_pop();
        job();
    }
}

int main(int args, char **argv)
{
    std::string table_name = argv[1];
    Table table(table_name.c_str());

    ThreadSafeQueue<std::function<void()>> queue(10);
    std::thread worker_thread(db_worker, std::ref(queue));
    worker_thread.detach();

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

            auto task = std::make_shared<std::packaged_task<void()>>(
                [row, &table] { table.insert_row(row); });
            std::cout << "Executed." << std::endl;

            queue.push([task]() { (*task)(); });
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
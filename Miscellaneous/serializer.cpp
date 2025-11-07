#include "./raii_file_wrapper.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <unistd.h>

struct UserRecord
{
    uint32_t id;       // 4 bytes
    char username[32]; // 32 bytes
    double balance;    // 8 bytes
    bool is_active;    // 1 byte
};

// int main()
// {
//     UserRecord serialized = {1, "john doe", 123.4, false};
//     std::string filename = "user.dat";
//     std::ofstream ostrm(filename, std::ios::binary);
//     ostrm.write(reinterpret_cast<char *>(&serialized), sizeof serialized);
//     ostrm.close();

//     std::ifstream istrm(filename, std::ios::binary);
//     UserRecord deserialized;
//     istrm.read(reinterpret_cast<char *>(&deserialized), sizeof deserialized);

//     std::cout << "UserRecord{id=" << deserialized.id << ", username=" << deserialized.username
//               << ", balance=" << deserialized.balance << ", is_active=" << std::boolalpha << deserialized.is_active
//               << "}" << std::endl;
//     return 0;
// }

int main()
{
    const char *filename = "user.dat";
    File file{filename, O_CREAT | O_RDWR};

    UserRecord serialized = {1, "john doe", 123.4, false};
    if (file.fwrite(reinterpret_cast<char *>(&serialized), sizeof serialized) != sizeof serialized)
    {
        std::cerr << "Failed to write record to file" << std::endl;
        return 1;
    }

    UserRecord deserialized;
    file.fseek(0, SEEK_SET);
    if (file.fread(reinterpret_cast<char *>(&deserialized), sizeof deserialized) != sizeof deserialized)
    {
        std::cerr << "Failed to read record from file" << std::endl;
        return 1;
    }

    std::cout << "UserRecord{id=" << deserialized.id << ", username=" << deserialized.username
              << ", balance=" << deserialized.balance << ", is_active=" << std::boolalpha << deserialized.is_active
              << "}" << std::endl;

    return 0;
}

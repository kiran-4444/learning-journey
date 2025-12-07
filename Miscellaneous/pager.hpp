#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <ios>
#include <iostream>
#include <stdexcept>
#include <string>

const int PAGE_SIZE = 4096;

class Pager
{
  private:
    std::fstream stream;

  public:
    Pager(const char *filename)
    {
        stream = std::fstream(filename, std::ios::in | std::ios::binary);
        if (!stream.good())
        {
            stream = std::fstream(filename, std::ios::out);
            stream.close();
            stream =
                std::fstream(filename, std::ios::in | std::ios::out | std::ios::binary);
        }
        else
        {
            stream =
                std::fstream(filename, std::ios::in | std::ios::out | std::ios::binary);
            stream.seekg(0);
        }
    }

    ~Pager()
    {
        if (stream.is_open())
        {
            stream.close();
        }
    }

    void read_page(int page_id, char *buffer)
    {
        if (page_id >= length())
        {
            throw std::runtime_error("EOF reached");
        }
        const int offset = page_id * PAGE_SIZE;
        stream.seekg(offset);
        stream.read(buffer, PAGE_SIZE);

        if (stream.gcount() != PAGE_SIZE)
        {
            throw std::runtime_error("Corrupted Page!");
        }

        stream.seekg(0);
    }

    void write_page(int page_id, char *buffer)
    {
        const int offset = page_id * PAGE_SIZE;
        stream.seekg(offset);
        stream.write(buffer, PAGE_SIZE);
        stream.seekg(0);
    }

    size_t length()
    {
        stream.seekg(0);
        size_t fsize = stream.tellg();
        stream.seekg(0, std::ios::end);
        fsize =
            static_cast<size_t>(static_cast<std::streamoff>(stream.tellg())) - fsize;

        return fsize / PAGE_SIZE;
    }
};

#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

class File
{
  private:
    int fd;

  public:
    File(const char *name, int flags, int mode) { fd = open(name, flags, mode); }
    ~File()
    {
        if (fd != -1) close(fd);
    }
    ssize_t fwrite(const char *data, size_t n) { return write(fd, data, n); }
    ssize_t fread(char *buffer, size_t nbytes) { return read(fd, buffer, nbytes); }
    ssize_t fseek(off_t offset, off_t whence) { return lseek(fd, offset, whence); }
};

int main()
{
    const char *filename = "test.txt";
    File file{filename, O_CREAT | O_RDWR, S_IRWXU};
    const char *data = "Hello World!";
    // std::cout << file.fwrite(data, strlen(data)) << std::endl;

    file.fseek(0, SEEK_SET);

    char buffer[128];
    memset(buffer, 0, sizeof(buffer));

    // std::cout << file.fread(buffer, strlen(data)) << std::endl;

    std::cout << buffer << std::endl;
    return 0;
}
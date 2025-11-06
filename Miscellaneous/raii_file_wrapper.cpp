#include <cassert>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

class File
{
  private:
    int fd;

  public:
    File(const char *name, int flags, int mode = 0666)
    {
        fd = open(name, flags, mode);
        if (fd == -1)
        {
            throw std::runtime_error("Failed to open file");
        };
    }
    ~File()
    {
        if (fd != -1) close(fd);
    }

    File(const File &) = delete;
    File &operator=(const File &) = delete;

    File(File &&other) noexcept : fd(other.fd) { other.fd = -1; }
    File &operator=(File &&other)
    {

        if (this != &other)
        {
            if (fd != -1) close(fd);
            fd = other.fd;
            other.fd = -1;
        }

        return *this;
    }

    int get_fd() { return fd; }

    ssize_t fwrite(const char *data, size_t n) { return write(fd, data, n); }
    ssize_t fread(char *buffer, size_t nbytes) { return read(fd, buffer, nbytes); }
    ssize_t fseek(off_t offset, off_t whence) { return lseek(fd, offset, whence); }
};

int is_fd_valid(int fd) { return fcntl(fd, F_GETFD) != -1 || errno != EBADF; }

void check_exception()
{
    std::cout << "  Testing Exception Handling..." << std::endl;
    int tracked_fd = -1;
    try
    {
        File file("something", O_CREAT | O_RDWR);
        tracked_fd = file.get_fd();
        assert(tracked_fd != -1);
        assert(is_fd_valid(tracked_fd));
        throw std::runtime_error("Error");
    }
    catch (...)
    {
        assert(!is_fd_valid(tracked_fd));
    }
}

void check_scope_handling()
{
    std::cout << "  Testing out-of-scope destruction..." << std::endl;
    int tracked_fd = -1;
    {
        File file("testing.txt", O_CREAT | O_RDWR);
        tracked_fd = file.get_fd();
        assert(tracked_fd != -1);
        assert(is_fd_valid(tracked_fd));
    }
    assert(!is_fd_valid(tracked_fd));
}

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

    std::cout << "Running Tests..." << std::endl;
    check_exception();
    check_scope_handling();

    return 0;
}
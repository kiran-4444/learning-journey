#include <initializer_list>

class Vector
{
public:
    Vector(int sz);
    Vector(std::initializer_list<double> lst);
    ~Vector();
    double& operator[](int i);
    int size() noexcept;
    void print();

private:
    double *elem;
    int sz;
};
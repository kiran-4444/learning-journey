class Vector
{
public:
    Vector(int sz);
    double& operator[](int i);
    int size() noexcept;
    void print();

private:
    double *elem;
    int sz;
};
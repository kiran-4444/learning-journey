#include "pager.hpp"
#include "row.hpp"
#include <memory>

const

    class Table
{
  private:
    std::unique_ptr<Pager> pager;
    uint32_t num_rows;

  public:
    Table(const char *table_name)
    {
        pager = std::unique_ptr<Pager>(new Pager(table_name));
    };

    ~Table(){};

    void insert_row(const Row &row) {

    };

    Row select_row(uint32_t row_index) {

    };

    const uint32_t get_row_count() { return num_rows; }
};

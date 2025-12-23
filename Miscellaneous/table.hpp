#include "pager.hpp"
#include "row.hpp"
#include <cstdint>
#include <cstring>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <sys/types.h>

const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;

class Table
{
  private:
    std::unique_ptr<Pager> pager;
    std::shared_mutex mut;

  public:
    Table(const char *table_name)
    {
        pager = std::unique_ptr<Pager>(new Pager(table_name));
    };

    ~Table() {};

    void insert_row(const Row &row)
    {
        std::lock_guard<std::shared_mutex> lock(mut);
        char row_buffer[ROW_SIZE] = {0};
        serialize(row_buffer, row);

        uint32_t num_rows = get_row_count();
        const uint32_t page_id = num_rows / ROWS_PER_PAGE;
        const uint32_t row_offset = (num_rows % ROWS_PER_PAGE) * ROW_SIZE;

        char page_buffer[PAGE_SIZE] = {0};
        if (row_offset != 0) pager->read_page(page_id, page_buffer);

        std::memcpy(page_buffer + row_offset, row_buffer, ROW_SIZE);

        pager->write_page(page_id, page_buffer);
    };

    Row get_row(uint32_t row_index)
    {
        std::shared_lock<std::shared_mutex> lock(mut);
        const uint32_t page_id = row_index / ROWS_PER_PAGE;
        const uint32_t row_offset = (row_index % ROWS_PER_PAGE) * ROW_SIZE;

        char page_buffer[PAGE_SIZE] = {0};
        pager->read_page(page_id, page_buffer);

        char row_buffer[ROW_SIZE] = {0};

        std::memcpy(row_buffer, page_buffer + row_offset, ROW_SIZE);

        Row deserialized;
        deserialize(deserialized, row_buffer);
        return deserialized;
    }

    const uint32_t get_row_count()
    {
        char page_buffer[PAGE_SIZE] = {0};
        uint32_t rows = 0;
        uint32_t num_rows = 0;
        if (pager->length() != 0)
        {
            rows = ROWS_PER_PAGE * (pager->length() - 1);
            pager->read_page(pager->length() - 1, page_buffer);
        }

        char null_buffer[ROW_SIZE] = {0};
        for (int i = 0; i < ROWS_PER_PAGE; i += 1)
        {
            if (std::memcmp(page_buffer + i * ROW_SIZE, null_buffer, ROW_SIZE) == 0)
            {
                break;
            }
            else
            {
                num_rows++;
            }
        }
        return rows + num_rows;
    }
};

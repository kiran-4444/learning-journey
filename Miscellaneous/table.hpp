#include "pager.hpp"
#include "row.hpp"
#include <cstdint>
#include <cstring>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <sys/types.h>

// Currently we assume that we can only fix complete rows in to the page i.e. no
// fractional pages. Hence, we'll always have same number of rows in all the pages. This
// is done to reduce the complexity of handling the fractional pages.
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
        // This will return the offset of the row in the page that we'll be writing to.
        const uint32_t row_offset = (num_rows % ROWS_PER_PAGE) * ROW_SIZE;

        char page_buffer[PAGE_SIZE] = {0};
        // If this is not the first row in the page, we can copy the page into buffer,
        // append the current row and write the whole page again.
        // If this is the first row in the page, we'll directly write the row to the
        // page without copying the page into buffer. This is because, pager->write_page
        // will handle out-of-bounds writes anyways.
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

    // The logic is to first read all full pages and get row count directly (this is
    // because all pages contains same number of rows except the last page)
    const uint32_t get_row_count()
    {
        char page_buffer[PAGE_SIZE] = {0};
        uint32_t rows = 0;
        uint32_t num_rows = 0;

        // Read all the page except the last page (which might be partially filled).
        // Then count the total rows from full pages
        // ((pager->length() - 1) * ROWS_PER_PAGE).
        // Read the last page into buffer and count the rows by iterating through each
        // row in the page.
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

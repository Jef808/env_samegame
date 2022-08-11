#include "samegame.h"

#include <algorithm>
#include <cassert>
#include <deque>
#include <iostream>
#include <set>
#include <string>
#include <sstream>
#include <type_traits>
#include <vector>

namespace {

std::vector<Color> col_buffer;
std::vector<Color> row_buffer;

} // namespace

SameGame::SameGame(size_t width, size_t height)
    : m_width{width},
      m_height{height},
      m_data{width * height},
      ccount{},
      n_empty_rows{0}
{
  col_buffer = std::vector<Color>(height, Color::Empty);
  row_buffer = std::vector<Color>(width, Color::Empty);
  ccount.fill(0);
}

SameGame::~SameGame() {
  col_buffer.clear();
  col_buffer.shrink_to_fit();
  row_buffer.clear();
  row_buffer.shrink_to_fit();
}



void SameGame::load(std::istream &is) {
  m_data.reset();
  n_empty_rows = 0;

  std::string buf;
  std::string buf_ss;
  for (auto y = 0; y < m_height; ++y) {
    std::getline(is, buf);
    std::string_view sv = buf;

    bool row_empty = true;
    auto x = 0;
    for (; x < m_width; ++x) {
        size_t pspace = std::min(sv.find(' '), sv.size());
        buf_ss = std::string(pspace, '\0');
        sv.copy(&buf_ss[0], pspace, 0);
        sv.remove_prefix(pspace + 1);

        int color_i = std::stoi(buf_ss) + 1;
        ++ccount[color_i];

        Cluster& cluster = m_data[x + y * m_width];
        const Color color = cluster.color = Color(color_i);
        const int rep = cluster.rep = cluster.color == Color::Empty? -1 : x + y * m_width;
        row_empty &= rep == -1;
    }

    n_empty_rows += row_empty;
  }
  compute_clusters();
}

// TODO: It seems I can decrease y by 2 in the
// outermost for loop below.
void SameGame::compute_clusters() {
  m_data.reset();

  // Loop from the bottom row upwards
  int y = m_height - 1;
  bool row_empty;

  for (; y >= 0; --y) {
    row_empty = true;

    // Loop from the leftmost to the second to rightmost column
    int i = y * m_width;
    for (; i < (y + 1) * m_width - 1; ++i) {

      // Skip empty cells
      if (m_data[i].color == Color::Empty) {
        continue;
      }

      // Indicate that the row is non-empty if it is the case.
      row_empty = false;

      // Compare up
      if (m_data[i].color == m_data[i - m_width].color)
        m_data.unite(i, i - m_width);

      // Compare right
      if (m_data[i].color == m_data[i + 1].color)
        m_data.unite(i, i + 1);
    }

    // At rightmost column,
    // if cell is empty,
    if (m_data[i].color == Color::Empty)
    {
        // if the row is empty, update n_empty_rows
        n_empty_rows = row_empty * std::max(n_empty_rows, y);
    }

    // Otherwise compare up.
    else if (m_data[i].color == m_data[i - m_width].color) {
      m_data.unite(i, i - m_width);
    }
  }

  // On the upmost row, loop from the leftmost
  // to the second to rightmost column
  int i = 0;
  row_empty = true;
  for (; i < m_width - 1; ++i) {

    // Skip empty cells
    if (m_data[i].color == Color::Empty) {
        continue;
    }

    // Indicate that the row is non-empty if it is the case.
    row_empty = false;

    // Compare right
    if (m_data[i].color == m_data[i + 1].color)
      m_data.unite(i, i + 1);
  }

  // After traversing the topmost row, check if n_empty_rows is
  // 0 or 1.
  row_empty &= m_data[i].color == Color::Empty;
  n_empty_rows = row_empty * std::max(n_empty_rows, 1);

}

void SameGame::gravity() {
  // Loop through all columns
  for (int x = 0; x < m_width; ++x) {
    // Clear out the color buffer
    col_buffer.clear();

    // From the bottommost to the upmost cell in the column,
    for (int h = 0; h < m_height; ++h) {
      // Copy the non-empty cells into the buffer
      if (const Color color = m_data[x + (m_height - 1 - h) * m_width].color;
          color != Color::Empty) {
        col_buffer.push_back(color);
      }
    }

    int y = 0;

    // Copy back the non-empty colors at the bottom of the column.
    for (; y < col_buffer.size(); ++y) {
      m_data[x + (m_height - 1 - y) * m_width].color = col_buffer[y];
    }

    // Mark all cells above as empty.
    for (; y < m_height; ++y) {
      m_data[x + (m_height - 1 - y) * m_width].color = Color::Empty;
    }
  }
}

// NOTE: In `stack_columns`, we suppose that there are no
// horizontal gap between cells as for states following
// a call to `gravity()`.
// Therefore in this context, checking that the bottom cell of
// a column is empty is sufficient to verify that the whole
// column is empty.
void SameGame::stack_columns() {

  // Empty column predicate
  auto is_empty_column = [&](auto x) {
    return m_data[x + (m_height - 1) * m_width].color != Color::Empty;
  };

  std::deque<int> empty_cols;
  std::set<int> known_empty_cols;
  int x_left = 0, x_right = 0;

  // Find the leftmost empty column.
  while (x_left < m_width - 1 && not is_empty_column(x_left)) {
    ++x_left;
  }

  // While the leftmost empty column is not the last
  // one to the right, look for the next non-empty
  // column.
  while (x_left < m_width) {
    // Search for next non-empty column, while storing
    // any empty columns' indices.
    int x_right = [x = x_left + 1, M = m_width, &is_empty_column,
                   &empty_cols, &known_empty_cols]() mutable {
      while (x < M && is_empty_column(x)) {
        auto [col, okay] = known_empty_cols.insert(x++);
        if (okay) {
          empty_cols.push_back(*col);
        }
      }
      return x;
    }();

    // If failed to find nonempty column, we are done.
    if (x_right == m_width) {
      return;
    }

    // Otherwise, swap columns' color contents
    for (int y = 0; y < m_height; ++y) {
      std::swap(m_data[x_left + y * m_width].color,
                m_data[x_right + y * m_width].color);
    }

    // The column at x_right is now empty.
    empty_cols.push_back(x_right);

    // Keep the queue sorted to efficiently retrieve the min.
    std::sort(empty_cols.begin(), empty_cols.end());

    //std::swap(empty_cols.front(), *std::min_element(empty_cols.begin(), empty_cols.end()));

    // Update left_x to the leftmost empty column.
    x_left = empty_cols.front();
    empty_cols.pop_front();
  }

}

const Cluster& SameGame::get_cluster(int i) const {
     return m_data[m_data.find_rep(i)];
}

void SameGame::empty_cluster(int index) {
  static std::vector<int> members;
  members.clear();

  const std::vector<int>& _members = get_cluster(index).members;
  std::copy(_members.begin(), _members.end(),
            std::back_inserter(members));

  std::for_each(members.begin(), members.end(), [&](const auto i) {
    m_data[i].color = Color::Empty;
    m_data[i].rep = i;
    m_data[i].members.clear();
  });
}

void SameGame::apply(const Action &action) {

  if (not is_valid(action)) {
    std::cerr << "Invalid action: " << action.index << std::endl;
    throw std::runtime_error("Invalid action");
  }

  empty_cluster(action.index);
  gravity();
  stack_columns();
  compute_clusters();
}

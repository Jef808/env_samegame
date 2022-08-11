#include "samegame.h"

#include <algorithm>
#include <cassert>
#include <deque>
#include <iostream>
#include <string>
#include <vector>

namespace {

std::vector<Color> gravity_buffer;
std::vector<int> members_buffer;

} // namespace

SameGame::SameGame(size_t width, size_t height)
    : m_width{width}, m_height{height}, m_data{width * height}, ccount{},
      n_empty_rows{0} {
  gravity_buffer.resize(width * height, Color::Empty);
  members_buffer.reserve(width * height);
  ccount.fill(0);
}

SameGame::~SameGame() {
  gravity_buffer.clear();
  gravity_buffer.shrink_to_fit();
  members_buffer.clear();
  members_buffer.shrink_to_fit();
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

      Cluster &cluster = m_data[x + y * m_width];
      const Color color = cluster.color = Color(color_i);
      const int rep = cluster.rep =
          cluster.color == Color::Empty ? -1 : x + y * m_width;
      row_empty &= rep == -1;
    }

    n_empty_rows += row_empty;
  }
  compute_clusters();
}

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
    if (m_data[i].color == Color::Empty) {
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
  // Reset the buffer
  std::fill(gravity_buffer.begin(), gravity_buffer.end(), Color::Empty);

  // Loop through all columns
  for (int x = 0; x < m_width; ++x) {
    // Set up a fresh column for the output buffer
    auto out = gravity_buffer.begin() + x * m_height;

    // From the bottommost to the upmost cell in the column,
    for (int y = m_height - 1; y >= 0; --y) {
      // Copy the non-empty cells into the buffer
      if (Color color = m_data[x + y * m_width].color; color != Color::Empty) {
        *out++ = color;
      }
    }

    // Copy the collected nonempty colors back into the column
    int h = 0;
    size_t n = m_height;

    for (size_t h = 0; h < m_height; ++h) {
      m_data[x + (m_height - h - 1) * m_width].color =
          gravity_buffer[x * m_height + h];
    }
  }
}

// NOTE: We take advantage of the fact that #stack_columns() needs to be called
// after #gravity(). Since no horizontal gap can exist in this context, it
// suffices the check the bottom cell of a column to verify if the whole column
// is empty.
void SameGame::stack_columns() {
  // Empty column predicate
  auto is_empty_column = [&](auto x) {
    return m_data[x + (m_height - 1) * m_width].color == Color::Empty;
  };

  std::deque<size_t> empty_cols;

  // Loop from the leftmost to the rightmost column
  for (size_t col = 0; col < m_width; ++col) {

    // Store the index of empty columns as we find them
    if (is_empty_column(col)) {
      empty_cols.push_back(col);
      continue;
    }

    // In case we have empty columns available on our left,
    // swap the current (nonempty) column with the leftmost
    // empty column
    if (not empty_cols.empty()) {
      for (int y = 0; y < m_height; ++y) {
        std::swap(m_data[empty_cols.front() + y * m_width].color,
                  m_data[col + y * m_width].color);
      }
      // Remove the newly filled column from the empty column queue.
      empty_cols.pop_front();

      // The current column is now the rightmost empty column known.
      empty_cols.push_back(col);
    }
  }
}

const Cluster &SameGame::get_cluster(int i) const {
  return m_data[m_data.find_rep(i)];
}

void SameGame::empty_cluster(int index) {
  members_buffer.clear();

  const std::vector<int> &_members = get_cluster(index).members;
  std::copy(_members.begin(), _members.end(),
            std::back_inserter(members_buffer));

  std::for_each(members_buffer.begin(), members_buffer.end(),
                [&](const auto i) {
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

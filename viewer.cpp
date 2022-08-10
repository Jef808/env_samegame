#include "viewer.h"
#include "samegame.h"

#include <algorithm>
#include <iostream>
#include <sstream>

namespace {
std::ostream &fmt_cell(std::ostream &out, int idx, const SameGame &sg,
                       int highlight_cluster = -1);
} // namespace

void Viewer::print(std::ostream &out, const SameGame &sg) {
  const size_t height = sg.height();
  const size_t width = sg.width();

  out << '\n';

  // For every row
  for (size_t y = 0; y < height; ++y) {
    // First build the row labels on the
    // left of the row
    const int label_y = height - 1 - y;

    // More space after single digits
    out << label_y << ((label_y < 10) ? "  " : " ") << "| ";

    // Build cell representatives until the
    // second to cell of the row.
    for (auto x = 0; x < width - 1; ++x) {
      fmt_cell(out, x + y * width, sg) << ' ';
    }

    // Build cell representative for the last cell.
    fmt_cell(out, width - 1 + y * width, sg) << '\n';
  }

  // Print the column indices
  out << std::string(34, '_') << '\n' << std::string(5, ' ');
  for (int x = 0; x < width; ++x) {
    // More space after single digits
    out << x << ((x < 10) ? " " : "");
  }

  out << std::endl;
}

namespace {

template <typename E> auto to_integral(E e) {
  return static_cast<std::underlying_type_t<E>>(e);
}

enum class ColorCode : int {
  Black = 30,
  Red = 31,
  Green = 32,
  Yellow = 33,
  Blue = 34,
  Magenta = 35,
  Cyan = 36,
  White = 37,
  BlackBold = 90,
  RedBold = 91,
  GreenBold = 92,
  YellowBold = 93,
  BlueBold = 94,
  MagentaBold = 95,
  CyanBold = 96,
  WhiteBold = 97,
};

enum {
  NonEmpty = 1,
  NonTrivial = 2,
  Rep = 4,
  Highlighted = 8,
};

enum class Shape : int {
  EmptyCell = 0,
  NonEmptyCell = 1,                  // (1)
  NonTrivialCell = 3,                // (1 + 2)
  EmptyRepCell = 4,                  // (0 + 4)
  TrivialRepCell = 5,                // (1 + 4)
  NonTrivialRepCell = 7,             // (1 + 2 + 4)
  EmptyHighlightedCell = 8,          // (8)
  NonEmptyHighlightedCell = 9,       // (1 + 8)
  NonTrivialHighlightedCell = 11,    // (1 + 2 + 8)
  TrivialHighlightedRepCell = 13,    // (1 + 4 + 8)
  NonTrivialHighlightedRepCell = 15  // (1 + 2 + 4 + 8)
};

inline std::string shape_unicode(Shape s);
inline std::ostream &with_color_unicode(const std::string &s, Color c,
                                        std::ostream &os);

inline std::string shape_unicode(Shape s) {
  switch (s) {
  case Shape::EmptyCell:
  case Shape::EmptyRepCell:
    return "\u25A0"; // black square
  case Shape::NonEmptyCell:
  case Shape::TrivialRepCell:
    return "\u25A0"; // black square (will be colored)
  case Shape::NonTrivialCell:
    return "\u25A3"; // white square with black square inside
  case Shape::NonTrivialRepCell:
    return "\u25C9"; // white circle with black circle inside
  case Shape::EmptyHighlightedCell:
    return "\u25C6"; // black diamond
  case Shape::NonEmptyHighlightedCell:
  case Shape::TrivialHighlightedRepCell:
    return "\u25C6"; // black diamond (will be colored)
  case Shape::NonTrivialHighlightedCell:
    return "\u25C9"; // white diamond with black circle inside
  case Shape::NonTrivialHighlightedRepCell:
    return "\u25CE"; // bullseye
  default:
    std::cerr << "Shape: " << to_integral(s) << std::endl;
    throw std::runtime_error("Unknown shape");
  }
}

inline ColorCode color_code(Color c) { return ColorCode(to_integral(c) + 91); }

inline std::ostream &with_color_unicode(const std::string &s, Color c,
                                        std::ostream &os) {
  return os << "\033[1;" << to_integral(color_code(c)) << 'm' << s << "\033[0m";
}

inline Shape get_shape(const int idx, const Cluster &cluster,
                       const Cluster &cluster_hl) {

  const bool is_non_empty = cluster.color != Color::Empty;
  const bool is_nontrivial = cluster.members.size() > 1;
  const bool is_rep = idx == cluster.rep;
  const bool is_hl = std::count(cluster_hl.begin(), cluster_hl.end(), idx);

  return Shape((int)is_non_empty
               | (is_nontrivial << 1) // 0 OR 1 mod 2
               | (is_rep << 2)       // <=2 OR >=2 mod 4
               | (is_hl << 3));      // <=4 OR >=4 mod 8
}

std::ostream &fmt_cell(std::ostream &out, int idx, const SameGame &sg,
                       int highlight_cluster) {
  static int last_hl_idx = -1;
  static Cluster cluster_hl = Cluster{-1};

  if (const bool cluster_hl_dirty = highlight_cluster != last_hl_idx;
      cluster_hl_dirty)
  {
    cluster_hl = sg.get_cluster(last_hl_idx = highlight_cluster);
  }

  const Cluster &cluster = sg.get_cluster(idx);

  return with_color_unicode(
    shape_unicode(get_shape(idx, cluster, cluster_hl)),
    cluster.color,
    out);
}

} // namespace

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
    out << y << ((y < 10) ? "  " : " ") << "| ";

    // Build cell representatives until the
    // second to cell of the row.
    for (auto x = 0; x < width - 1; ++x) {
      fmt_cell(out, x + y * width, sg) << ' ';
    }

    // Build cell representative for the last cell.
    fmt_cell(out, (y + 1) * width - 1, sg) << '\n';
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

enum class ColorCode {
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

enum class Shape {
  EmptyCell = 0,
  TrivialCell = 1,
  NonTrivialCell = 2,
  RepCell = 3,
};

inline ColorCode color_code(Color c) { return ColorCode(to_integral(c) + 90); }

inline std::ostream &with_color(const Color c, const std::string &s,
                                std::ostream &os) {
  return os << "\033[1;" << to_integral(color_code(c)) << "m" << s << "\033[0m";
}

inline std::string shape_unicode(Shape s) {
  switch (s) {
  case Shape::EmptyCell:
  case Shape::TrivialCell:
    return "\u25A0"; // black square (will be non-empties)
  case Shape::NonTrivialCell:
    return "\u25A3"; // white square with black square inside
  case Shape::RepCell:
    return "\u25C9"; // white circle with black circle inside
  default:
    std::cerr << "Shape: " << to_integral(s) << std::endl;
    throw std::runtime_error("Unknown shape");
  }
}

inline Shape get_shape(const int idx, const Cluster &cluster,
                       const Cluster &cluster_hl) {

  const bool is_empty = cluster.color == Color::Empty;
  const bool is_trivial = cluster.members.size() < 2;
  const bool is_nontrivial = cluster.members.size() > 1;
  const bool is_rep = is_nontrivial && idx == cluster.rep;
  // const bool is_hl = std::count(cluster_hl.begin(), cluster_hl.end(), idx);

  Shape shape = is_rep          ? Shape::RepCell
                : is_nontrivial ? Shape::NonTrivialCell
                : is_empty      ? Shape::EmptyCell
                                : Shape::TrivialCell;

  return shape;
}

std::ostream &fmt_cell(std::ostream &out, int idx, const SameGame &sg,
                       int highlight_cluster) {
  static int last_hl_idx = -1;
  static Cluster cluster_hl = Cluster{-1};

  if (const bool cluster_hl_dirty = highlight_cluster != last_hl_idx;
      cluster_hl_dirty) {
    cluster_hl = sg.get_cluster(last_hl_idx = highlight_cluster);
  }

  const Cluster &cluster = sg.get_cluster(idx);

  return with_color(cluster.color,
                    shape_unicode(get_shape(idx, cluster, cluster_hl)), out);
}

} // namespace

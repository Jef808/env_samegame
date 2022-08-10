#include "viewer.h"
#include "samegame.h"

#include <algorithm>
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
}

namespace {

template <typename E> auto to_integral(E e) {
  return static_cast<std::underlying_type_t<E>>(e);
}

enum class Color_codes : int {
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

enum class Shape : int {
  Square,
  Diamond,
  DiamondBold,
};

std::string shape_unicode(Shape s);
std::string color_unicode(Color c);

std::string color_unicode(Color c) {
  return std::to_string(to_integral(Color_codes(to_integral(c) + 90)));
}

std::string shape_unicode(Shape s) {
  switch (s) {
  case Shape::Square:
    return "\u25A0";
  case Shape::Diamond:
    return "\u25C6";
  case Shape::DiamondBold:
    return "\u25C7";
  }
}

std::ostream &fmt_cell(std::ostream &out, int idx, const SameGame &sg,
                       int highlight_cluster) {
  static const Cluster ClusterHL =
      highlight_cluster == -1 ? Cluster{-1} : sg.get_cluster(highlight_cluster);

  const Cluster &cluster = sg.get_cluster(idx);

  const bool is_highlighted =
      std::count(ClusterHL.begin(), ClusterHL.end(), idx);
  const bool is_rep = cluster.rep == idx;

  Shape shape = is_rep           ? Shape::DiamondBold
                : is_highlighted ? Shape::Diamond
                                 : Shape::Square;

  return out << "\033[1" << color_unicode(cluster.color) << 'm'
             << shape_unicode(shape) << "\033[0m";
}

} // namespace

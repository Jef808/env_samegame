#ifndef TYPES_H_
#define TYPES_H_

#include <cstddef>

constexpr int NB_COLORS = 5;

enum class Color { Empty = 0, Nb = NB_COLORS + 1 };

constexpr size_t WIDTH = 15;
constexpr size_t HEIGHT = 15;

#endif // TYPES_H_

#ifndef SAMEGAME_H_
#define SAMEGAME_H_

#include "dsu.h"

#include <array>
#include <iosfwd>
#include <vector>

struct Action {
  int index;
};

class SameGame {
public:
  SameGame(size_t width, size_t height);

  ~SameGame();

  /**
   * Load the board from an input stream.
   */
  void load(std::istream &is);

  /**
   * Apply a move by emptying the associated cluster of cells
   * and applying downwards and leftwards gravity.
   */
  void apply(const Action &action);

  /**
   * Get valid actions.
   */
  template <typename OutputIter> void valid_actions(OutputIter out) const;

  /**
   * Get the cluster containing the cell at index i.
   */
  const Cluster &get_cluster(int i) const;

  /**
   * Check if an action is valid in the current state.
   */
  bool is_valid(const Action &action) const;

  size_t width() const { return m_width; }
  size_t height() const { return m_height; }

private:
  const size_t m_width;
  const size_t m_height;
  DSU m_data;

  std::array<int, NB_COLORS + 1> ccount;
  int n_empty_rows;

  /**
   * Organize the connected sets of cells of the same color
   * into the #Cluster array of the member #DSU `m_data`.
   */
  void compute_clusters();

  /**
   * Let downwards gravity act on the board,
   * filling gaps horizontally created by empty cells.
   */
  void gravity();

  /**
   * Stack columns towards the left,
   * filling gaps created by empty columns.
   *
   * Note:  Requires the board to have no
   * horizontal gap between its cells, e.g. after
   * a call to #gravity().
   */
  void stack_columns();

  /**
   * Empty all cells of a cluster.
   */
  void empty_cluster(int index);
};

inline bool SameGame::is_valid(const Action &action) const {
  const Cluster &cluster = m_data[action.index];
  return cluster.color != Color::Empty && cluster.rep == action.index &&
         cluster.size() > 1;
}

template <typename OutputIter>
inline void SameGame::valid_actions(OutputIter out) const {
  for (int i = 0; i < m_data.size(); ++i) {
    if (auto action = Action{i}; is_valid(action)) {
      out = action;
    }
  }
}

#endif // SAMEGAME_H_

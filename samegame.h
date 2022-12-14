
#ifndef SAMEGAME_H_
#define SAMEGAME_H_

#include "types.h"
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
   * Get the current count for each colors.
   */
  template<typename OutputIter>
  void colour_counter(OutputIter out) const;

  /**
   * Get the cluster containing the cell at index i.
   */
  const Cluster &get_cluster(int i) const;

  /**
   * Check if an action is valid in the current state.
   */
  bool is_valid(const Action &action) const;

  /**
   * Compute the score of an action.
   */
  double score(const Action &action) const;

  /**
   * Get the color count for given color.
   */
  int get_color_count(Color c) const;

  /**
   * Get the color of the cell at index i.
   */
  Color get_color(int i) const
    { return m_data[i].color; }

  size_t width() const { return m_width; }
  size_t height() const { return m_height; }


private:
  const size_t m_width;
  const size_t m_height;
  DSU m_data;

  std::array<int, NB_COLORS + 1> ccount;
  int n_empty_rows;

  std::vector<Color> gravity_buffer;
  std::vector<int> members_buffer;

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
  void clear_cluster(int index);
};

template <typename OutputIter>
inline void SameGame::valid_actions(OutputIter out) const {
  for (int i = 0; i < m_data.size(); ++i) {
    if (auto action = Action{i}; is_valid(action)) {
      out = action;
    }
  }
}

template< typename OutputIter >
inline void SameGame::colour_counter(OutputIter out) const {
  std::copy(ccount.begin(), ccount.end(), out);
}



#endif // SAMEGAME_H_

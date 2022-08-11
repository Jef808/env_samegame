#ifndef DSU_H_
#define DSU_H_

#include "types.h"

#include <cstddef>
#include <vector>

struct Cluster {

  explicit Cluster(int r) : rep{r}, members(1, r) {}

  mutable int rep;
  Color color{Color::Empty};
  std::vector<int> members;

  size_t size() const { return members.size(); }
  auto begin() { return members.begin(); }
  auto begin() const { return members.begin(); }
  auto end() { return members.end(); }
  auto end() const { return members.end(); }
};

/**
 * Disjoint set union data structure for forming clusters.
 */
class DSU {
public:
  explicit DSU(size_t size);

  /**
   * Resets datastructure so that each cells are made disjoint.
   */
  void reset();

  /**
   * Find the representative of the cluster to which a given cell belongs.
   *
   * @Param i  The index of the query cell.
   * @Return  The index of the cluster's representative.
   */
  int find_rep(int i) const;

  /**
   * Merge the two clusters to which the two given cells belong.
   *
   * @Param a  The index of the first cell.
   * @Param b  The index of the second cell.
   */
  void unite(int a, int b);

  Cluster &operator[](size_t i) { return m_clusters[i]; }
  const Cluster &operator[](size_t i) const { return m_clusters[i]; }
  auto begin() const { return m_clusters.begin(); }
  auto end() const { return m_clusters.end(); }
  size_t size() const { return m_clusters.size(); }

private:
  std::vector<Cluster> m_clusters;

  /**
   * Append a cluster at the end of another one.
   *
   * @Param a  The receiving cluster.
   * @Param b  The appended cluster.
   */
  void merge_clusters(Cluster &a, Cluster &b);
};

inline bool operator==(const Cluster &a, const Cluster &b) {
  return a.rep == b.rep;
}

#endif // DSU_H_

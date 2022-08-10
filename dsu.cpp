#include "dsu.h"

#include <algorithm>
#include <numeric>

DSU::DSU(size_t size) : m_clusters{} {
  for (auto i = 0; i < size; ++i) {
    Cluster &cluster = m_clusters.emplace_back(i);
    cluster.members.reserve(size);
  }
}

void DSU::reset() {
  std::for_each(m_clusters.begin(), m_clusters.end(),
                [n = 0](Cluster &c) mutable {
                  c.members.clear();
                  c.rep = n++;
                });
}

int DSU::find_rep(int i) {
  if (bool rep_found = m_clusters[i].rep == i; not rep_found) {
    return m_clusters[i].rep = find_rep(i);
  }
  return i;
}

void DSU::unite(int a, int b) {
  a = find_rep(a);
  b = find_rep(b);

  if (a != b) {
    if (m_clusters[a].size() < m_clusters[b].size()) {
      std::swap(a, b);
    }
    merge_clusters(m_clusters[a], m_clusters[b]);
  }
}

void DSU::merge_clusters(Cluster &a, Cluster &b) {
  b.rep = a.rep;

  a.members.insert(a.end(), b.begin(), b.end());

  b.members.clear();
}

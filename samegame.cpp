#include "samegame.h"


#include <vector>

/**
 * Disjoint set union data structure for forming clusters.
 */
class DSU {
public:
    DSU() = default;

    void reset(size_t size) {

    }

private:
    std::vector<int> m_data;
}

//
// Created by yangshu on 2018-12-21.
//

#ifndef KARGER_GRAPH_H
#define KARGER_GRAPH_H

#include <iostream>
#include <vector>
#include <set>

struct pairVertices {
    size_t i;
    size_t j;
};

class Graph {
public:
    void   set(size_t r, size_t c, float d) { _data[(r * _rc) + c] = d; };
    float get(size_t r, size_t c) const { return _data[(r * _rc) + c]; };
    void   set_size(size_t rc) { _rc = rc;   _data.resize(_rc * _rc); };
    size_t get_size() const { return _rc; };

    void calculateVerSet();
    void showVerSet() const;
    std::set<size_t> getVerSet1() { return set1; }
    std::set<size_t> getVerSet2() { return set2; }


    size_t count_vertices() const;
    size_t count_edges() const;
    float count_Weight() const;
    Graph& remove_self_loops();
    Graph& merge_vertices(size_t u, size_t v);

private:
    size_t _rc;
    std::vector<float> _data;

    std::vector<pairVertices> pairvVec;
    std::set<size_t> set1, set2;
};

#endif  // KARGER_GRAPH_H


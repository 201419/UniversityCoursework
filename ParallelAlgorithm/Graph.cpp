//
// Created by yangshu on 2018-12-21.
//

#include <algorithm>
#include "Graph.h"

void Graph::calculateVerSet() {
    std::vector<pairVertices> vec;
    reverse(pairvVec.begin(), pairvVec.end());  // reverse
    vec.assign(pairvVec.begin(), pairvVec.end());

    std::vector<pairVertices>::iterator it;
    it = vec.begin();
    set1.insert((*it).i);
    set1.insert((*it).j);
    vec.erase(it);

    while (true) {
        bool active = true;
        for(it=vec.begin(); it!=vec.end(); ) {
            if (set1.find((*it).i)!=set1.end()) {
                set1.insert((*it).j);
                active = false;
                vec.erase(it);
                continue;
            } else if (set1.find((*it).j)!=set1.end()) {
                set1.insert((*it).i);
                active = false;
                vec.erase(it);
                continue;
            }
            it++;
        }
        if(active) break;
    }

    size_t i;
    for (i=0; i<_rc; i++) {
        if (set1.find(i)==set1.end()) {
            set2.insert(i);
        }
    }
}

void Graph::showVerSet() const {
    std::set<size_t>::iterator it;
    std::cout << "**********************" << std::endl;
    std::cout << "set1:" << std::endl;
    for (it=set1.begin(); it!=set1.end(); it++) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    std::cout << "**********************" << std::endl;
    std::cout << "set2:" << std::endl;
    for (it=set2.begin(); it!=set2.end(); it++) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    std::cout << "**********************" << std::endl;
}

size_t Graph::count_vertices() const {
    size_t n = 0;
    for (size_t i = 0; i < _rc; ++i) {
        float k = 0;
        for (size_t j = 0; j < _rc; ++j) {
            k = k + get(i, j);
        }
        if (k > 0) { ++n; }
    }
    return n;
}

size_t Graph::count_edges() const {
    size_t n = 0;
    for (size_t i = 0; i < _rc; ++i) {
        for (size_t j = 0; j < _rc; ++j) {
            if (get(i, j)!=0)
            n = n + 1;
        }
    }
    return (n>>1);
}

float Graph::count_Weight() const {
    float n = 0;
    for (size_t i = 0; i < _rc; ++i) {
        for (size_t j = 0; j < _rc; ++j) {
                n = n + get(i, j);
        }
    }
    return (n/2);
}

Graph& Graph::remove_self_loops() {
    for (size_t i = 0; i < _rc; ++i) {
        set(i, i, 0);
    }
    return *this;
}

Graph& Graph::merge_vertices(size_t u, size_t v) {
    if (u < _rc && v < _rc) {
        for (size_t i = 0; i < _rc; ++i) {
            float e = get(v, i);
            set(v, i, 0);

            float n = e + get(u, i);
            set(u, i, n);

            e = get(i, v);
            set(i, v, 0);

            n = e + get(i, u);
            set(i, u, n);
        }

        pairVertices pv{u, v};
        pairvVec.push_back(pv);
    }
    return *this;
}
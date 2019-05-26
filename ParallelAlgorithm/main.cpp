//
// Created by yangshu on 2018-12-23.
//

#include <iostream>
#include <fstream>
#include <limits.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include <queue>
#include "Graph.h"
using namespace std;


#define NUM_THREADS_KARGER 22796
#define NUM_THREADS_KARGERSTEIN 44

struct GraphCut {
    Graph graph;
    float cut;
} gcKarger[NUM_THREADS_KARGER], gcKargerStein[NUM_THREADS_KARGERSTEIN];

struct FuncArg {
    Graph graph;
    size_t index;
};

// 重载 >>
istream& operator >> (istream& is, Graph& km) {
    size_t dim = 0;
    is >> dim;
    km.set_size(dim);
    for (size_t i = 0; i < dim; ++i) {
        for (size_t j = 0; j < dim; ++j) {
            float k = 0;
            is >> k;
            if (is.good()) { km.set(i, j, k); }
        }
    }
    return is;
}

// 重载 <<
ostream& operator << (ostream& os, const Graph& km) {
    size_t dim = km.get_size();
    os << dim << " " << endl;
    for (size_t i = 0; i < dim; ++i) {
        for (size_t j = 0; j < dim; ++j) {
            os << km.get(i, j) << " ";
        }
        os << endl;
    }
    return os;
}

void random_contraction_algorithm(Graph& km, size_t t) {
    km.remove_self_loops();
    while (km.count_vertices() > t) {
        /* Pick an edge. */
        size_t random_vertex_u = 0, random_vertex_v = 0;
        do {
            random_vertex_u = rand() % km.get_size();
            random_vertex_v = rand() % km.get_size();
        } while (km.get(random_vertex_u, random_vertex_v) == 0);
        //assert(random_vertex_u != random_vertex_v);

        /* Merge both vertices. */
        km.merge_vertices(random_vertex_u, random_vertex_v);
        /* Remove self-loops. */
        km.remove_self_loops();
    }
}

// 递归实现
Graph fastCutAlgoRec(Graph graph, size_t t) {
    if (t<6) {
        random_contraction_algorithm(graph, 2);
        return graph;
    } else {
        size_t s = floor(1+t/sqrt(2));
        Graph g1 = graph;
        random_contraction_algorithm(g1, s);
        Graph g2 = graph;
        random_contraction_algorithm(g2, s);
        g1 = fastCutAlgoRec(g1, g1.count_vertices());
        g2 = fastCutAlgoRec(g2, g2.count_vertices());
        if (g1.count_Weight() < g2.count_Weight()) {
            return g1;
        } else { return g2; }
    }
}

// 循环实现
Graph fastCutAlgo(Graph graph, size_t t) {
    queue<Graph> q;
    size_t s = t;
    size_t index = 1;
    Graph gReturn;

    q.push(graph);
    while (s>=6) {
        s = floor(1+s/sqrt(2));
        for (size_t i=0; i<index; i++) {
            Graph g1 = q.front();
            random_contraction_algorithm(g1, s);
            q.push(g1);
            Graph g2 = q.front();
            random_contraction_algorithm(g2, s);
            q.push(g2);
            q.pop();
        }
        index=index*2;
    }

    Graph gf = q.front();
    random_contraction_algorithm(gf, 2);
    gReturn = gf;
    q.pop();
    while (!q.empty()) {
        Graph g = q.front();
        random_contraction_algorithm(g, 2);
        if (g.count_Weight() < gReturn.count_Weight()) {
            gReturn = g;
        }
        q.pop();
    }
    return gReturn;
}

void *kargerAlgo(void *data) {
    auto param = (FuncArg *)data;
    Graph graph = param->graph;
    srand((unsigned int)time(nullptr));
    random_contraction_algorithm(graph, 2);
    float cut = graph.count_Weight();
    gcKarger[param->index].graph = graph;
    gcKarger[param->index].cut = cut;

    pthread_exit(nullptr);
}

void karger_Algo(void* data) {
    auto param = (FuncArg *)data;
    Graph graph = param->graph;
    srand((unsigned int)time(nullptr));
    random_contraction_algorithm(graph, 2);
    float cut = graph.count_Weight();
    gcKarger[param->index].graph = graph;
    gcKarger[param->index].cut = cut;
}

void *kargerSteinAlgo(void *data) {
    auto param = (FuncArg *)data;
    Graph graph = param->graph;
    srand((unsigned int)time(nullptr));
    Graph g = fastCutAlgo(graph, graph.get_size());
    float cut = g.count_Weight();
    gcKargerStein[param->index].graph = g;
    gcKargerStein[param->index].cut = cut;

    pthread_exit(nullptr);
}

void kargerStein_Algo(void *data) {
    auto param = (FuncArg *)data;
    Graph graph = param->graph;
    srand((unsigned int)time(nullptr));
    Graph g = fastCutAlgo(graph, graph.get_size());
    float cut = g.count_Weight();
    gcKargerStein[param->index].graph = g;
    gcKargerStein[param->index].cut = cut;
}

// /mincut$ g++ -pthread -o main main.cpp Graph.h Graph.cpp
// /mincut$ ./main
int main(int argc, char* argv[]) {
    Graph graph;
    graph.set_size(0);

    // read graph matrix file
    ifstream ifs;
    ifs.open("sample.txt");
    if (!ifs) { cout << "sample is empty ..." << endl; return 0; }
    ifs >> graph;
    ifs.close();  // close

    // graph information
    if (graph.get_size() > 0) {
        cout << "Input vertex count: " << graph.count_vertices() << endl;
        cout << "Input edge count: " << graph.count_edges() << endl;
    } else {
        cout << "the Graph is NULL ..." << endl << endl;
        //cout << "./main [Loop number]" << endl << endl;
        return 0;
    }

    Graph minimum_graph_K, minimum_graph_KT, minimum_graph_KS, minimum_graph_KST;
    float minimum_cut_K, minimum_cut_KT, minimum_cut_KS, minimum_cut_KST;

    minimum_graph_K.set_size(0); minimum_graph_KT.set_size(0);
    minimum_graph_KS.set_size(0); minimum_graph_KST.set_size(0);
    minimum_cut_K = minimum_cut_KT = minimum_cut_KS = minimum_cut_KST = UINT_MAX;

    pthread_t threadsKarger[NUM_THREADS_KARGER], threadsKargerStein[NUM_THREADS_KARGERSTEIN];
    size_t t;
    int rc;

    cout << "Already ..." << endl;

    struct timespec start{};
    struct timespec finish{};
    double elapsedKarger, elapsedKargerThread, elapsedKargerStein, elapsedKargerSteinThread;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(t=0; t<NUM_THREADS_KARGER; t++) {
        auto *param = new FuncArg();
        param->graph = graph;
        param->index = t;
        rc = pthread_create(&threadsKarger[t], nullptr, kargerAlgo, param);
        if (rc) {
            cout << "ERROR; return code from pthread_create() is " << rc << endl;
            exit(-1);
        }
    }
    for(t=0; t<NUM_THREADS_KARGER; t++) {
        //while (!gcKarger[t].cut) ;
        pthread_join(threadsKarger[t], NULL);
        if (gcKarger[t].cut < minimum_cut_KT) {
            minimum_graph_KT = gcKarger[t].graph;
            minimum_cut_KT = gcKarger[t].cut;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsedKargerThread = (finish.tv_sec - start.tv_sec);
    elapsedKargerThread += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    cout << "Karger Algorithm Thread: " << elapsedKargerThread << endl;
    //cout << "min cut is: " << minimum_cut_KT << endl;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(t=0; t<NUM_THREADS_KARGER; t++) {
        auto *param = new FuncArg();
        param->graph = graph;
        param->index = t;
        karger_Algo(param);
        if (gcKarger[t].cut < minimum_cut_K) {
            minimum_graph_K = gcKarger[t].graph;
            minimum_cut_K = gcKarger[t].cut;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsedKarger = (finish.tv_sec - start.tv_sec);
    elapsedKarger += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    cout << "Karger Algorithm: " << elapsedKarger << endl;
    //cout << "min cut is: " << minimum_cut_K << endl;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(t=0; t<NUM_THREADS_KARGERSTEIN/2; t++) {
        auto *param = new FuncArg();
        param->graph = graph;
        param->index = t;
        rc = pthread_create(&threadsKargerStein[t], nullptr, kargerSteinAlgo, param);
        if (rc) {
            cout << "ERROR; return code from pthread_create() is " << rc << endl;
            exit(-1);
        }
    }
    for(t=0; t<NUM_THREADS_KARGERSTEIN; t++) {
        //while (!gcKargerStein[t].cut) ;
        pthread_join(threadsKargerStein[t], NULL);
        if (gcKargerStein[t].cut < minimum_cut_KST) {
            minimum_graph_KST = gcKargerStein[t].graph;
            minimum_cut_KST = gcKargerStein[t].cut;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsedKargerSteinThread = (finish.tv_sec - start.tv_sec);
    elapsedKargerSteinThread += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    cout << "KargerStein Algorithm Thread: " << elapsedKargerSteinThread << endl;
    //cout << "min cut is: " << minimum_cut_KST << endl;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(t=0; t<NUM_THREADS_KARGERSTEIN; t++) {
        auto *param = new FuncArg();
        param->graph = graph;
        param->index = t;
        kargerStein_Algo(param);
        if (gcKargerStein[t].cut < minimum_cut_KS) {
            minimum_graph_KS = gcKargerStein[t].graph;
            minimum_cut_KS = gcKargerStein[t].cut;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsedKargerStein = (finish.tv_sec - start.tv_sec);
    elapsedKargerStein += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    cout << "KargerStein Algorithm: " << elapsedKargerStein << endl;
    //cout << "min cut is: " << minimum_cut_KS << endl;

    cout << "Done ..." << endl;
    //cout << "Output vertex count: " << minimum_graph.count_vertices() << endl;
    //cout << "Output cut: " << minimum_graph.count_Weight() << endl;


    //minimum_graph.calculateVerSet();
    //minimum_graph.showVerSet();

    // save graph matrix file
    //if (argc > 2) {
    //    ofstream ofs;
    //    ofs.open(argv[2]);
    //    ofs << minimum_graph;
    //    ofs.close();
    //}


    // Last thing that main() should do
    pthread_exit(nullptr);
}

#include "node.hpp"

int main(int argc, char** argv) {
    int n, k, alpha, beta;
    n = stoi(string(argv[2]));
    k = stoi(string(argv[3]));
    alpha = stoi(string(argv[4]));
    beta = stoi(string(argv[5]));

    assert(argc > 1);
    string id = string(argv[1]);
    Node n1(id);
    thread th1(server_thread, &n1);
    //thread th2(ready_thread, &n1);
    ready_thread(&n1);
    while (!(n1.getToStart()));
    n1.Snowball(n, k, alpha, beta);
    th1.join();

    return 0;
}
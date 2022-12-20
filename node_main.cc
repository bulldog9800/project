#include "node.hpp"

Node* returnNode(string id, string color,int is_byzantine);

int main(int argc, char** argv) {
    assert(argc > 1);
    int n, k, alpha, beta;
    n = stoi(string(argv[2]));
    k = stoi(string(argv[3]));
    alpha = stoi(string(argv[4]));
    beta = stoi(string(argv[5]));
    string id = string(argv[1]);
    string color = string(argv[6]);
    string is_byzantine = string(argv[7]);
    Node* n1 = returnNode(id, color, stoi(is_byzantine));
    thread th1(server_thread, n1);
    //thread th2(ready_thread, &n1);
    //ready_thread(n1);
    sleep(5);
    while (!(n1->checkIfClusterReady(n))) {
        sleep(1);
    }
    cout << "Cluster is ready!" << endl;
    n1->Snowball(n, k, alpha, beta);
    th1.join();
    delete n1;

    return 0;
}

Node* returnNode(string id, string color,int is_byzantine) {
    if (is_byzantine) {
        return new Byzantine(id,color,is_byzantine);
    } else {
        return new Node(id,color,is_byzantine);
    }
}

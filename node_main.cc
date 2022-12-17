#include "node.hpp"

Node* returnNode(string id);

int main(int argc, char** argv) {
    int n, k, alpha, beta;
    n = stoi(string(argv[2]));
    k = stoi(string(argv[3]));
    alpha = stoi(string(argv[4]));
    beta = stoi(string(argv[5]));

    assert(argc > 1);
    string id = string(argv[1]);
    Node* n1 = returnNode(id);
    thread th1(server_thread, n1);
    //thread th2(ready_thread, &n1);
    //ready_thread(n1);
    while (!(n1->checkIfClusterReady(n)));
    cout << "Cluster is ready!" << endl;
    n1->Snowball(n, k, alpha, beta);
    th1.join();
    delete n1;

    return 0;
}

Node* returnNode(string id) {
    string config_file_name = "config_" + id + ".txt";
    string path_file = "files/" + config_file_name;
    const char* path=path_file.c_str();

    ifstream infile(path);
    if (!infile.is_open()) {
        cout << "Couldn't open file!";
    }

    int id1, is_byzantine;
    string color;

    infile >> id1;
    infile >> color;
    infile >> is_byzantine;

    if (is_byzantine) {
        return new Byzantine(id);
    } else {
        return new Node(id);
    }
}

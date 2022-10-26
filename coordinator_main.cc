#include "node.hpp"

int main(int argc, char** argv){
    int n;
    n = stoi(string(argv[1]));
    Coordinator c1(n);
    thread th1(server_cthread, &c1);
    std::cout << "Coordinator waiting for " << n << " nodes to say ready\n";
    while (n > c1.getNumOfConnectedNodes());
    std::cout << "n = " << n << ", and vector has " << c1.getVector().size() << "elements\n";
    for(auto port1: c1.getVector()) {
        std::cout << "Coordinator is trying to contact node on port " << port1 << "\n";
        string target_str = "localhost:" + port1;
        QueryClient query(grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
        query.SayStart("start");
    }
    return 0;
}
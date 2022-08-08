#include "node.hpp"

int main(int argc, char** argv){
    int n;
    n = stoi(string(argv[1]));
    Coordinator c1(n);
    thread th1(server_cthread, &c1);
    while (n != c1.getNumOfConnectedNodes());
    for(auto port1: c1.getVector()){
        QueryClient query(grpc::CreateChannel(port1, grpc::InsecureChannelCredentials()));
        query.SayStart("start");
    }
    return 0;
}
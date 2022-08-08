
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <cassert>
#include <fcntl.h>
#include <fstream>
#include <filesystem>
#include <unistd.h>
#include <vector>
#include <iterator>
#include <algorithm>
#include <random>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "query.grpc.pb.h"
#include "coordinator.hpp"
#include "query_server.hpp"

Coordinator::Coordinator(int n): n(n),nodes_ports(n) {
    this->numOfConnectedNodes=0;
    this->port_num = "50000";
}

string Coordinator::getCPort() {
    return this->port_num;
}

int Coordinator::getNumOfConnectedNodes(){
    return numOfConnectedNodes;
}

vector<string> Coordinator::getVector(){
    return nodes_ports;
}

void Coordinator::addNum() {
    (this->numOfConnectedNodes)++;
}

void Coordinator::addToVector(string node_port){
    nodes_ports.push_back(node_port);
}

void server_thread(Coordinator* coordinator) {
    RunServer(NULL, coordinator);
}

/* int main(int argc, char** argv){
    int n;
    n = stoi(string(argv[1]));
    Coordinator c1(n);
    thread th1(server_thread, &c1);
    while (n != c1.getNumOfConnectedNodes());
    for(auto port1: c1.getVector()){
        QueryClient query(grpc::CreateChannel(port1, grpc::InsecureChannelCredentials()));
        query.SayStart("start");
    }
    return 0;
} */


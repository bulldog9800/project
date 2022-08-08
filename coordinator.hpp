#ifndef COORDINATOR_HPP
#define COORDINATOR_HPP

#include "query_client.hpp"

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
#include <ctime>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using query::Query;
using query::ColorReply;
using query::ColorRequest;
using grpc::Channel;
using grpc::ClientContext;
using std::string;
using std::thread;
using std::stoi;
using std::cout;
using std::ifstream;
using std::endl;
using std::to_string;
using std::vector;

class Coordinator {
    int n;
    int numOfConnectedNodes;
    string port_num;
    vector<string> nodes_ports;

public:
    Coordinator(int n);
    string getCPort();
    int getNumOfConnectedNodes();
    vector<string> getVector();
    void addNum();
    void addToVector(string node_port);

};

#endif
#ifndef NODE_HPP
#define NODE_HPP

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

enum Color {
    RED,
    BLUE,
    UNCOLORED
};

class Node {
    int id;
    string color;
    Color color_enum;
    string port;
    int config_fd;
    bool is_byzantine;
    time_t start_time;
public:
    Node(string id);

    int getId();
    string getColor();
    string getPort();
    bool getIsByzantine();
    void setColor(string new_color);
    void colorToString();
    void stringToColor();
    /***
     * Given n and k, picks k elements from the set {1...n}/{my_id}
     ***/
    vector<int> getSample(int n,int k);
    int* askSample(int n,int k, int* count);
    string Snowball(int n, int k, int alpha, int beta);
};


#endif
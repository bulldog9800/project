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

#include "helpers.hpp"

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
    bool is_server_ready;
    bool to_start;
    //int config_fd;
    bool is_byzantine;
    time_t start_time;
public:
    Node(string id, string color1, int is_byzantine1);
    int getId();
    string getColor();
    string getPort();
    bool getIsByzantine();
    bool getIsServerReady();
    bool getToStart();
    bool checkIfClusterReady(int num_of_nodes);
    void setColor(string new_color);
    void setReady();
    void setStart();
    void colorToString();
    void stringToColor();
    virtual string getReply(string request);

    /***
     * Given n and k, picks k elements from the set {1...n}/{my_id}
     ***/
    vector<int> getSample(int n,int k);
    int* askSample(int n,int k, int* count);
    string Snowball(int n, int k, int alpha, int beta);
};

class Byzantine : public Node {
public:
    Byzantine(string id, string color1, int is_byzantine1);
    string getReply(string request) override;
};

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

void ready_thread(Node* node);
void server_thread(Node* node);
void server_cthread(Coordinator* coordinator);

const string contact_etcd_cmd = "etcdctl --endpoints 172.20.0.5:2379 ";

#endif
//
// Created by Sari Abed on 18/06/2022.
//
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

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "query.grpc.pb.h"
#include "node.hpp"
#include "query_server.hpp"

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

Node::Node(string id){
    string config_file_name = "config_" + id + ".txt";
    string path_file = "files/" + config_file_name;
    const char* path=path_file.c_str();

    this->id = stoi(id);
    int port1 = (50000 + this->id);
    this->port = to_string(port1);

    ifstream infile(path);
    if (!infile.is_open()) {
        cout << "Couldn't open file!";
    }

    int id1, is_byzantine1;
    string color1;

    infile >> id1;
    infile >> color1;
    infile >> is_byzantine1;

    this->color=color1;
    stringToColor();
    this->is_server_ready= false;
    this->to_start= false;
    this->is_byzantine=(bool)(is_byzantine1);

}

int Node::getId() {
    return id;
}

string Node::getColor() {
    return color;
}

string Node::getPort() {
    return port;
}

bool Node::getIsByzantine() {
    return is_byzantine;
}

bool Node::getIsServerReady(){
    return is_server_ready;
}

bool Node::getToStart() {
    return to_start;
}

void Node::setColor(string new_color) {
    this->color=new_color;
    stringToColor();
}

void Node::setReady(){
    this->is_server_ready= true;
}

void Node::setStart(){
    this->to_start= true;
}

void Node::colorToString() {
    if(this->color_enum == RED) {
        this->color= 'R';
    }
    else if(this->color_enum == BLUE) {
        this->color= 'B';
    }
    else {
        this->color= 'U';
    }
}

void Node::stringToColor() {
    if(this->color == "R") {
        this->color_enum = RED;
    }
    else if(this->color == "B") {
        this->color_enum = BLUE;
    }
    else {
        this->color_enum = UNCOLORED;
    }
}

/***
 * Given n and k, picks k elements from the set {1...n}/{my_id}
 ***/
vector<int> Node::getSample(int n,int k) {
    vector<int> v1(n),v2;
    for(int i=0; i<n; i++) {
        v1[i]=i+1;
    }

    v1.erase(v1.begin()+(this->id-1));
    std::sample(v1.begin(), v1.end(), std::back_inserter(v2),k,std::mt19937{std::random_device{}()});
    v1.insert(v1.begin()+(this->id-1),this->id);

    return v2;
}

int* Node::askSample(int n,int k, int* count) {
    vector<int> sample=getSample(n,k);

    for(auto i : sample) {
        int port1 = 50000+i;
        string port_string = to_string(port1);
        string target_str = "localhost:" + port_string;

        QueryClient query(grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
        std::string reply = query.AskColor(this->color);

        //std::cout << "Asker received: " << reply << std::endl;
        if (reply == "R"){
            count[0]++;
        } else if (reply == "B") {
            count[1]++;
        }
    }
    //assuming that if the server color is 'U' , he replies with the client's color . otherwise we should add another if else
    //cout<< "the id: " << this->id << " counter Red: " << count[0]<< " counter Blue: " << count[1]<< endl;
    return count;
}

string Node::Snowball(int n, int k, int alpha, int beta) {
    int same_color_in_a_row_count = 0;
    int colors_counter1[2] = {0,0};

    Color colors[2] = {RED, BLUE};
    Color col = this->color_enum;
    Color last_color = this->color_enum;
    bool undecided = true;

    this->start_time = time(NULL);

    while(undecided){
        if (this->color_enum == UNCOLORED) {
            continue;
        }
        int colors_counter2[2]={0,0};

        askSample(n,k,colors_counter2);
        bool maj = false;

        for (auto col1: colors) {
            if(colors_counter2[col1] >= alpha) {
                maj = true;
                colors_counter1[col1]++;

                if (colors_counter1[col1] > colors_counter1[this->color_enum]) {
                    this->color_enum = col1;
                    colorToString();
                }

                if (col1 != last_color) {
                    last_color = col1;
                    same_color_in_a_row_count = 1;
                } else {
                    same_color_in_a_row_count++;
                }

                if (same_color_in_a_row_count >= beta) {
                    time_t current_time, diff_time;
                    current_time=time(NULL);
                    diff_time=current_time-start_time;
                    undecided = false;
                    this->color_enum=col1;
                    colorToString();
                    string times_file_path = "files/times.txt";
                    std::ofstream times_file;
                    times_file.open(times_file_path,std::ios_base::app);
                    times_file  << this->color <<" " << diff_time << endl;
                    times_file.close();
                }
            }
        }

        if (maj == false) { 
            same_color_in_a_row_count=0;
        }
    }
    return this->color;
}

void server_thread(Node* node) {
    RunServer(node,NULL);
}

void ready_thread(Node* node) {
    while (!(node->getIsServerReady()));
    string target_str=to_string(50000);
    QueryClient query(grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
    query.SayReady(node->getPort());
}

int main(int argc, char** argv) {
    //hello from layana
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



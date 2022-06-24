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

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "query.grpc.pb.h"

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

enum Color{RED, BLUE, UNCOLORED};
// Logic and data behind the server's behavior.

class QueryClient {
public:
    QueryClient(std::shared_ptr<Channel> channel)
            : stub_(Query::NewStub(channel)) {}

    // Assembles the client's payload, sends it and presents the response back
    // from the server.
    std::string AskColor(const std::string& color) {
        // Data we are sending to the server.
        ColorRequest request;
        request.set_color(color);

        // Container for the data we expect from the server.
        ColorReply reply;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        // The actual RPC.
        Status status = stub_->AskColor(&context, request, &reply);

        // Act upon its status.
        if (status.ok()) {
            return reply.reply();
        } else {
            std::cout << status.error_code() << ": " << status.error_message()
                      << std::endl;
            return "RPC failed";
        }
    }

private:
    std::unique_ptr<Query::Stub> stub_;
};


class Node {
    int id;
    string color;
    Color color_enum;
    string port;
    int config_fd;
    bool is_byzantine;
public:
    Node(string id){
        string config_file_name = "config_" + id + ".txt";
        //string path_file1 = "/mnt/c/Users/layana/Desktop/grpc/examples/cpp/query/" + config_file_name;
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
        //infile >> is_byzantine1;
        this->color=color1;
        stringToColor();
        this->is_byzantine=(bool)(false);

        cout << "id= "<< this->id <<" color: "<< this->color << " port: " << this->port << endl;
    }

    int getId(){
        return id;
    }
    string getColor(){
        return color;
    }
    string getPort(){
        return port;
    }
    bool getIsByzantine(){
        return is_byzantine;
    }

    void setColor(string new_color){
        this->color=new_color;
        stringToColor();
    }

    void colorToString(){
        if(this->color_enum == RED)
            this->color= 'R';
        else if(this->color_enum == BLUE)
            this->color= 'B';
        else
            this->color= 'U';
    }

    void stringToColor(){
        if(this->color == "R")
            this->color_enum=RED;
        else if(this->color == "B")
            this->color_enum=BLUE;
        else
            this->color_enum=UNCOLORED;
    }

    /***
     * Given n and k, picks k elements from the set {1...n}/{my_id}
     ***/
    vector<int> getSample(int n,int k){
        vector<int> v1(n),v2;
        for(int i=0; i<n ; i++) {
            v1[i]=i+1;
        }
        v1.erase(v1.begin()+(this->id-1));
        std::sample(v1.begin(), v1.end(), std::back_inserter(v2),k,std::mt19937{std::random_device{}()});
        v1.insert(v1.begin()+(this->id-1),this->id);
//        cout<< "sample" << endl;
//        for(int i=0; i<k ; i++) {
//            cout<< v2[i] << endl;
//        }
        return v2;
    }

    int* askSample(int n,int k, int* count){
        vector<int> sample=getSample(n,k);
        for(auto i : sample ){
            int port1 = 50000+i;
            string port_string = to_string(port1);
            string target_str = "localhost:" + port_string;
            QueryClient query(
                    grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
            std::string reply = query.AskColor(this->color);
            //std::cout << "Asker received: " << reply << std::endl;
            if(reply == "R"){
                count[0]++;
            }else if (reply == "B") {
                count[1]++;
            }
            }
            //assuming that if the server color is 'U' , he replies with the client's color . otherwise we should add another if else
        //cout<< "the id: " << this->id << " counter Red: " << count[0]<< " counter Blue: " << count[1]<< endl;
        return count;
    }

    string Snowball(int n, int k, int alpha, int beta){
        int same_color_in_a_row_count = 0, colors_counter1[2]={0,0};
        Color colors[2] = {RED, BLUE}, col= this->color_enum, last_color=this->color_enum;
        bool undecided= true;

        while(undecided){
            if(this->color_enum == UNCOLORED){
                continue;
            }
            int colors_counter2[2]={0,0};
            askSample(n,k,colors_counter2);
            bool maj = false;
            for(auto col1: colors){
                if(colors_counter2[col1] >= alpha){
                    maj = true;
                    colors_counter1[col1]++;
                    if(colors_counter1[col1] > colors_counter1[col])
                        col=col1;
                    if(col1 != last_color){
                        last_color = col1;
                        same_color_in_a_row_count = 1;
                    } else{
                        same_color_in_a_row_count++;
                    }
                    if(same_color_in_a_row_count >= beta){
                        undecided = false;
                        this->color_enum=col1;
                        colorToString();
                        cout << "Decided! the color is: " << this->color << endl;
                    }
                }
            }
            if(maj == false){
                same_color_in_a_row_count=0;
            }
        }
        return this->color;
    }
};

class QueryServiceImpl final : public Query::Service {
    Node* node;
    Status AskColor(ServerContext *context, const ColorRequest *request,
                    ColorReply *reply) override {
        if(node->getColor() == "U"){
            node->setColor(request->color());
            //cout << "the color changed" << node->getColor() << endl;
        }
        if(node->getIsByzantine()){
            if(request->color() == "R"){
                reply->set_reply("B");
            } else {
                reply->set_reply("R");
            }
            return Status::OK;
        }
        reply->set_reply(node->getColor());
        return Status::OK;
    }
public:
    QueryServiceImpl(Node* node) : node(node){
    }
};

void RunServer(Node* node) {
    std::string server_address("0.0.0.0:" + node->getPort());
    QueryServiceImpl service(node);

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr <Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

void server_thread(Node* node) {
    //hello sari
    RunServer(node);
}

int main(int argc, char** argv) {
    //hello from layana
    int n, k, alpha, beta;
    n = stoi(string(argv[2]));
    k = stoi(string(argv[3]));
    alpha = stoi(string(argv[4]));
    beta = stoi(string(argv[5]));

    if(argc == 2){
        cout << argv[1] << endl;
    }
    assert(argc > 1);
    string id = string(argv[1]);
    Node n1(id);
    thread th1(server_thread, &n1);
    n1.Snowball(n, k, alpha, beta);
    th1.join();

    return 0;
}



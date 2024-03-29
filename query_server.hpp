/*
*
* Copyright 2015 gRPC authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/
#ifndef QUERY_SERVER_HPP
#define QUERY_SERVER_HPP

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#ifdef BAZEL_BUILD
#include "examples/protos/query.grpc.pb.h"
#else
#include "query.grpc.pb.h"
#endif

#include "node.hpp"
#include "helpers.hpp"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using query::Query;
using query::ColorReply;
using query::ColorRequest;

class QueryServiceImpl final : public Query::Service {
    Node* node;
    Coordinator* coordinator;
    
    Status AskColor(ServerContext *context, const ColorRequest *request,ColorReply *reply) override {
        if(node->getColor() == "U"){
            node->setColor(request->color());
            //cout << "the color changed" << node->getColor() << endl;
        }

        string my_reply = node->getReply(request->color());
        reply->set_reply(my_reply);

        return Status::OK;
    }
    Status SayReady(ServerContext *context, const ReadyRequest *request,ReadyReply *reply)override{
        string port_num = request->port();
        coordinator->addToVector(port_num);
        std::cout << "Vector: ";
        for (auto s : coordinator->getVector()) {
            std::cout << s << ", ";
        }
        std::cout << std::endl;
        
        coordinator->addNum();
        reply->set_reply("PORT RECEIVED");
        return Status::OK;
    }

    Status SayStart(ServerContext *context, const StartRequest *request,StartReply *reply)override{
        node->setStart();
        reply->set_reply("Start");
        return Status::OK;
    }

public:
    QueryServiceImpl(Node* node,Coordinator* coordinator){
        this->node=node;
        this->coordinator=coordinator;
    }
};

void RunServer(Node* node1, Coordinator* coordinator1) {
    std::string server_address;
    string ip_addr = getIPAddress();
    if(node1){
        server_address= ip_addr + ":" + node1->getPort();
        }
    else{
        server_address= ip_addr + ":" + coordinator1->getCPort();
        }
    QueryServiceImpl service(node1,coordinator1);

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
    //std::cout << "Server listening on " << server_address << std::endl;
    if(node1) {
        node1->setReady();
        //TODO: remove next line
        cout << "Node listeing on address " + server_address << endl;
	string cmd = contact_etcd_cmd + "put ready_node" + std::to_string(node1->getId()) + " " + server_address;
    	cout << exec(cmd.c_str());
    }
    if (coordinator1) {
        std::cout << "Coordinator listening\n";
    }

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

/*
int main(int argc, char **argv) {
    std::string port_num = std::string(argv[1]);
    RunServer(port_num);

    return 0;
} */

#endif

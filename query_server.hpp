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

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using query::Query;
using query::ColorReply;
using query::ColorRequest;

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
    //std::cout << "Server listening on " << server_address << std::endl;

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

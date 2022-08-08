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

#include <grpcpp/grpcpp.h>

#ifdef BAZEL_BUILD
#include "examples/protos/query.grpc.pb.h"
#else
#include "query.grpc.pb.h"
#endif

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using query::Query;
using query::ColorReply;
using query::ColorRequest;
using query::ReadyReply;
using query::ReadyRequest;
using query::StartReply;
using query::StartRequest;
using std::string;

class QueryClient {
public:
    QueryClient(std::shared_ptr<Channel> channel): stub_(Query::NewStub(channel)) {}

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

        //CompletionQueue cq;

        // The actual RPC.
        Status status = stub_->AskColor(&context, request, &reply);
        //stub_->AsyncAskColor()

        // Act upon its status.
        if (status.ok()) {
            return reply.reply();
        } else {
            std::cout << status.error_code() << ": " << status.error_message()
                      << std::endl;
            return "RPC failed";
        }
    }

    std::string SayReady(const std::string& port){
        // Data we are sending to the server.
        ReadyRequest request;
        request.set_port(port);
        ReadyReply reply;
        ClientContext context;
        Status status = stub_->SayReady(&context, request, &reply);
        // Act upon its status.
        if (status.ok()) {
            return reply.reply();
        } else {
            std::cout << status.error_code() << ": " << status.error_message()
                      << std::endl;
            return "RPC failed";
        }
    }

    std::string SayStart(const std::string& start){
        StartRequest request;
        request.set_start(start);
        StartReply reply;
        ClientContext context;
        Status status = stub_->SayStart(&context, request, &reply);
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

/*
int main(int argc, char** argv) {
 // Instantiate the client. It requires a channel, out of which the actual RPCs
 // are created. This channel models a connection to an endpoint specified by
 // the argument "--target=" which is the only expected argument.
 // We indicate that the channel isn't authenticated (use of
 // InsecureChannelCredentials()).
 string target_str;
 string id = string(argv[1]);
 if (id.size()==1) {
     target_str = "localhost:5000" + id;
 } else if (id.size()==2) {
     target_str = "localhost:500" + id;
 } else if (id.size()==3) {
     target_str = "localhost:50" + id;
 }

 QueryClient query(
     grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
 std::string color("Blue");
 std::string reply = query.AskColor(color);
 std::cout << "Asker received: " << reply << std::endl;

 return 0;
} */

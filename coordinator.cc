#include "coordinator.hpp"

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "query.grpc.pb.h"
#include "node.hpp"
#include "query_server.hpp"

Coordinator::Coordinator(int num_of_nodes): n(num_of_nodes) {
    this->port_num = "50000";
}


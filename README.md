# Project in Distributed and Parallel Programming

## About The Project
In this repository we have implemented the Snowball consensus algorithm in C++, and used a variety of tools to help us achieve a distributed network. Please see the presentation to learn more about what we did and how we did it.

## Prerequisites
To run this project you should have a Linux Docker image with: 
1. cmake
2. The grpc library 
3. etcd v3.5.
4. python3
5. The project repo with the `node_main` target compiled.

You should also have python3 on your local machine and any unix shell.
And you should have a Docker network called snowball_cluster that can be created by the following command:

`docker network create --subnet=172.20.0.0/16 snowball_cluster`

## Compilation
Compiling our target is performed in the same manner as any other target in the gRPC library, please refer to the [gRPC docs](https://grpc.io/docs/languages/cpp/quickstart/) to see how they compile their example target.

## Info About The Files
In the final version of the project, we only run one target and that is `node_main`, found in `node_main.cc` and we use the `Node` object found in `node.cc` and `node.hpp`.

The fies `query_client.hpp` and `query_server.hpp` are the client and server functions of gRPC, you can learn more about gRPC [here](https://grpc.io/docs/languages/cpp/quickstart/).

The `coordinator.cc`, `coordinator_main.cc`, and `run.py` files are part of an older implementation of our project that wasn't fully distributed where we used a local coordinator node and relied on our localhost address and ports and local filesystem.

And finally the `runDocker.py` is the script that creates all the nodes and runs the protocol, the `config_tuple` function is where you can configure which parameters will be tested in the current run.

## How to Run The Script
To run the main script (`runDocker.py`), just use the command `python3 runDocker.py` in your terminal, you should have the Docker daemon running in order for the script to use the Docker CLI, and you should have an already running etcd server running on the aforementioned snowball_cluster network with the address `172.20.0.5:2379`.

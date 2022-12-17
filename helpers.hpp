#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <algorithm>
#include <stdio.h>      
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <random>
#include <sys/socket.h>
#include <unistd.h>

using std::string;
using std::cout;
using std::endl;
using std::mt19937;
using std::random_device;
using std::uniform_int_distribution;

string exec(const char* cmd);
string getIPAddress();
int generateRandomPort();
bool checkPort(int port);

#endif
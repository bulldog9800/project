#include <iostream>
#include <random>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

using std::random_device;
using std::mt19937;
using std::uniform_int_distribution;
using std::cout;
using std::endl;


int generate_random_port() {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> distr(1030, 65535);
	
	return distr(gen);
}

bool check_port (int port) {
	int socketdesc = socket(AF_INET, SOCK_STREAM, 0);
	if (socketdesc == -1) {
		cout << "Failed to create socket descriptor." << strerror(errno) << "\n";
		return false;
	}
	
	struct sockaddr_in addr;
    	addr.sin_family = AF_INET;
    	addr.sin_addr.s_addr = 0;
    	addr.sin_port = htons(port);

	if (bind(socketdesc, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
        	cout << "Failed to bind socket! " << strerror(errno) << "\n";
        	return false;
	}

	if (listen(socketdesc, 3) < 0)
    	{
		cout << "Failed to listen on socket! " << strerror(errno) << "\n";
		return false;
	}

	close(socketdesc);

	return true;
}



int main() {
	int port_num = generate_random_port();
	if (check_port(port_num)) {
		cout << "Port " << port_num << "is available" << endl;
	}

	if (check_port(3000) && check_port(30001)) {
		cout << "Ports 3000 and 3001 are available" << endl;
	} else {
		cout << "Ports 3000 and 3001 are available" << endl;
	}

	return 0;
}	


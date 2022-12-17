#include "helpers.hpp"

string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

string getIPAddress() {
    struct ifaddrs* addrStruct = NULL;
	struct ifaddrs* iterator = NULL;
	void *tmp = NULL;
    string ip_address;

	getifaddrs(&addrStruct);

	for (iterator = addrStruct; iterator != NULL; iterator = iterator->ifa_next) {
		if (!iterator->ifa_addr) {
           		continue;
        	}
        	if (iterator->ifa_addr->sa_family == AF_INET) { // check it is IP4
            	// is a valid IP4 Address
            	tmp=&((struct sockaddr_in *)iterator->ifa_addr)->sin_addr;
            	char addressBuffer[INET_ADDRSTRLEN];
            	inet_ntop(AF_INET, tmp, addressBuffer, INET_ADDRSTRLEN);
                
			    if (string(iterator->ifa_name) == "eth0") {
				    ip_address = string(addressBuffer);
			    }
        	}
	}
	if (addrStruct!=NULL) freeifaddrs(addrStruct);
	
	return ip_address;
}

int generateRandomPort() {
    random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> distr(1030, 65535);
	
	return distr(gen);
}

bool checkPort(int port) {
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
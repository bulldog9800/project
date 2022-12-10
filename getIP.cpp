#include <stdio.h>      
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <string>
#include <iostream>

using std::string;

int main() {
	struct ifaddrs* addrStruct = NULL;
	struct ifaddrs* iterator = NULL;
	void *tmp = NULL;

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
				string ip_address(addressBuffer);
				std::cout << "My ip address is: " << ip_address << std::endl;
			}
        	}
	}
	if (addrStruct!=NULL) freeifaddrs(addrStruct);
	
	return 0;
}


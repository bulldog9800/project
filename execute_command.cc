#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <algorithm>

using std::string;

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

int main() {
	const char* cmd = "etcdctl --endpoints=http://172.20.0.5:2379 get --prefix ready";

	string output = exec(cmd);

	std::cout << "The output is: " << output << std::endl;
	std::cout << "The number of lines in the string is: " << std::count(output.begin(), output.end(), '\n') + ( !output.empty() && output.back() != '\n' ) << std::endl;
	
	string empty = "";
	std::cout << "The number of lines in the empty string is: " << std::count(empty.begin(), empty.end(), '\n') << std::endl;


	return 0;
}

// define this to enable checkport header only mode
#define CHECK_PORT_HEADER_ONLY
#include "checkport.h"
#include <iostream>

int main(int argc, char** argv)
{
    // Constructor requires ip and port range.
    checkport::CheckPort cp("127.0.0.1", 1, 4000);

    // Set connect timeout.Consider fail if connecting port timeout.
    cp.SetConnectTimeout(1);

    // Set max concurrency.Increase this will improve check speed.
    cp.SetConcurrency(100);

    // Return 0 if no port is accessable in the port range
    // Or any number larger than 0 indicates the first accessable port in the range.
    auto port = cp.GetOne();

    if (port == 0)
        std::cout << "no port is accessable" << std::endl;
    else
        std::cout << port << " is accessable" << std::endl;

    // Return vector holds every accessable port in the range.
    auto port_list = cp.GetAll();
    for (const auto& v : port_list)
        std::cout << v << " ";
    return 0;
}
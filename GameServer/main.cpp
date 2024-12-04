#include "server.h"
#include <iostream>

int main()
{
    try
    {
        GameServer server(53000); // Port number
        server.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
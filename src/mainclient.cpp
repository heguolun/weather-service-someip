#include "mainclient.hpp"
using namespace std;

int main(int argc, char const *argv[])
{
    ClientHandle
        mainClient("Weather-Repo");
    mainClient.setLocation(argv[1]);
    if(mainClient.initApp()){
        std::cout << "Application initialized successfully\n";
        mainClient.startApp();
    } else
        std::cout << "Could not initialize the application\n";
    return 0;
}

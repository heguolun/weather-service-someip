#include <reportprovider.hpp>

int main(int argc, char const *argv[])
{
    ServiceHandle
        reportService("Report-Provider");
    if(reportService.initApp()){
        std::cout << "Application initialized successfully\n";
        reportService.startApp();
    } else
        std::cout << "Could not initialize the application\n";
    return 0;
}
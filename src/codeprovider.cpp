#include "codeprovider.hpp"

int main(int argc, char const *argv[])
{
    ServiceHandle
        locationCodeService("Code-Provider");
    if(locationCodeService.initApp()) {
        std::cout << "Location application initialized successfully\n";
        locationCodeService.startApp();
    } else std::cout << "Unable to initialize the application\n";
    return 0;
}

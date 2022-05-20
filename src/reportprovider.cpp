#include <reportprovider.hpp>
#include "json.hpp"
#include <iomanip>

using json = nlohmann::json;


static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main(int argc, char const *argv[])
{
    // CURL *curl;
    // CURLcode res;
    // std::string readBuffer;
    // const char* url = "https://api.openweathermap.org/data/2.5/weather?lat=18.52&lon=73.85&appid=c4cd41e8d84160673485c2d000d18222";
    // curl = curl_easy_init();
    // if(curl) {
    //     curl_easy_setopt(curl, CURLOPT_URL, url);
    //     curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    //     curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    //     res = curl_easy_perform(curl);
    //     curl_easy_cleanup(curl);

    //     // std::cout << readBuffer << std::endl;
    // }
    // std::cout << "-----------------------------\n";
    // json j_complete = json::parse(readBuffer);
    // std::cout << j_complete << "-----------------JSON-----------------" << "/n";
    // std::cout << j_complete.value("name","oops");
    
    ServiceHandle
        reportService("Report-Provider");
    if(reportService.initApp()){
        std::cout << "Application initialized successfully\n";
        reportService.startApp();
    } else
        std::cout << "Could not initialize the application\n";
    return 0;
}
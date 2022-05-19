#ifndef IOSTREAM
    #include <iostream>
#endif

#include <vsomeip/vsomeip.hpp>
#include "configfile.hpp"
#include <sstream>
#include <curl/curl.h>

class ServiceHandle {
    public:
        ServiceHandle(std::string &&_appName) {
            mApp = vsomeip::runtime::get()->create_application(_appName);
        }

        bool initApp() {
            if(!mApp->init()) {
                return false;
            } else {
                mApp->register_state_handler(std::bind(&ServiceHandle::onState,this,std::placeholders::_1));
                
                mApp->register_availability_handler(
                    SERVICE_ID,
                    INSTANCE1_ID,
                    std::bind(&ServiceHandle::onAvailable, this,
                        std::placeholders::_1,std::placeholders::_2,std::placeholders::_3
                    )
                );

                mApp->register_message_handler(
                    SERVICE_ID,
                    INSTANCE1_ID,
                    REPORT_GETTER_METHOD_ID,
                    std::bind(&ServiceHandle::onMessage,this,std::placeholders::_1)
                );

                mPayload = vsomeip::runtime::get()->create_payload();

                mApp->offer_service(SERVICE_ID,INSTANCE1_ID);

                return true;
            }
        }
        void startApp() {
            mApp->start();
        }
    private:
        std::shared_ptr<vsomeip::application> mApp;
        std::shared_ptr<vsomeip::payload> mPayload;
        uint32_t mLocCode;

    public:
        void onState(vsomeip::state_type_e state) {
            switch (state)
            {
            case vsomeip::state_type_e::ST_DEREGISTERED:
                std::cout << "Message: DEREGISTERED\n";
                break;
            case vsomeip::state_type_e::ST_REGISTERED:
                std::cout << "Message: REGISTERED\n";
            default:
                break;
            }
        }
        void onAvailable(vsomeip::service_t service, vsomeip::instance_t instance, bool available) {
            std::cout << (available ? "available" : "Not available") << "\n";   
        }
        void onMessage(const std::shared_ptr<vsomeip::message>& message) {
            if(message->get_message_type() == vsomeip::message_type_e::MT_REQUEST) {
                std::cout << "Received a request. . .\n";
                std::stringstream ss;
                mPayload = message->get_payload();
                auto l = mPayload->get_length();
                std::string locCode;
                for(int i{};i<l;i++) {
                    locCode += (char)mPayload->get_data()[i];
                } 
                ss << locCode;
                ss >> mLocCode;
                std::cout << mLocCode << "\n";
                
                fetchReport();

                std::shared_ptr<vsomeip::message> response = vsomeip::runtime::get()->create_response(message);
                response->set_payload(mPayload);
                // response->set_method(REPORT_GETTER_METHOD_ID);
                mApp->send(response);
                std::cout << "Report sent to mainclient. . .\n";

            }
        }

        void fetchReport() {
            uint32_t temperature{};
            if(mLocCode == 111045) {
                temperature = 32;
            } else {
                temperature = 44;
            }
            std::stringstream ss;
            ss << temperature;
            std::string temp;
            ss >> temp;
            std::vector<vsomeip::byte_t> data;
            for(auto& ch:temp) {
                data.push_back(ch);
            }
            mPayload->set_data(data);
        }
};
#include <iostream>
#include <vsomeip/vsomeip.hpp>
#include "configfile.hpp"
#include <mutex>
#include <atomic>
#include <sstream>

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
                    INSTANCE0_ID,
                    std::bind(&ServiceHandle::onAvailable, this,
                        std::placeholders::_1,std::placeholders::_2,std::placeholders::_3
                    )
                );

                mApp->register_message_handler(
                    SERVICE_ID,
                    INSTANCE0_ID,
                    CODE_GETTER_METHOD_ID,
                    std::bind(&ServiceHandle::onMessage,this,std::placeholders::_1)
                );

                //mApp->request_service(SERVICE_ID,INSTANCE_ID);
                mApp->offer_service(SERVICE_ID,INSTANCE0_ID);
                return true;
            }
        }
        void startApp() {
            mApp->start();
        }
    private:
        std::shared_ptr<vsomeip::application> mApp;
        std::shared_ptr<vsomeip::payload> mPayload;
        //std::mutex mPayloadMutex;
        //std::atomic<bool> mConnected;
        std::string mLocName;

    public:
        void onState(vsomeip::state_type_e state) {
            switch (state)
            {
            case vsomeip::state_type_e::ST_DEREGISTERED:
                std::cout << "Message: DEREGISTERED\n";
                break;
            case vsomeip::state_type_e::ST_REGISTERED:
                std::cout << "Message: REGISTERED\n";
                break;
            default:
                break;
            }
        }
        void onAvailable(vsomeip::service_t service, vsomeip::instance_t instance, bool available) {
            std::cout << (available ? "available" : "Not available") << "\n";
            //mConnected = available;
        }
        void onMessage(const std::shared_ptr<vsomeip::message>& message) {
            std::cout << "Finding out Location code for . . .\n";
            if(message->get_message_type() == vsomeip::message_type_e::MT_REQUEST) {
                mPayload = message->get_payload();
                fetchCode();
                std::shared_ptr<vsomeip::message> response = vsomeip::runtime::get()->create_response(message);
                response->set_payload(mPayload);
                mApp->send(response);
            }
        }
        private:
            void fetchCode() {
                auto length = mPayload->get_length();
                mLocName = "";
                for(vsomeip::byte_t i{};i<length;i++) {
                    mLocName+=(char)mPayload->get_data()[i];
                } std::cout << mLocName << "\n";

                std::stringstream ss;
                std::string code;
                if(mLocName == "Pune" || mLocName == "pune" || mLocName == "PUNE") {

                    ss << 111045;
                    ss >> code;
                    std::vector<vsomeip::byte_t> data;
                    for(auto& item:code) {
                        data.push_back(item);
                    }
                    mPayload->set_data(data);

                } else if(true) {

                    ss << 111111;
                    ss >> code;
                    std::vector<vsomeip::byte_t> data;
                    for(auto& item:code) {
                        data.push_back(item);
                    }
                    mPayload->set_data(data);

                }
            }
};
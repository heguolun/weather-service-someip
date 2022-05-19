#include <iostream>
#include <vsomeip/vsomeip.hpp>
#include "configfile.hpp"
#include <atomic>
#include <mutex>

class ClientHandle {
    public:
        ClientHandle(std::string &&_appName, uint32_t _cycle=1000) {
            mApp = vsomeip::runtime::get()->create_application(_appName);
            mCycle = _cycle;
        }
        void setLocation(std::string&& _locname) {
            mLocName = _locname;
            fReady = true;
        }
        void setLocation(std::string& _locname) {
            mLocName = _locname;
            fReady = true;
        }
        bool initApp() {
            if(!mApp->init()) {
                return false;
            } else {
                mApp->register_state_handler(std::bind(&ClientHandle::onState,this,std::placeholders::_1));
                
                mApp->register_availability_handler(
                    SERVICE_ID,
                    INSTANCE0_ID,
                    std::bind(&ClientHandle::onAvailable, this,
                        std::placeholders::_1,std::placeholders::_2,std::placeholders::_3
                    )
                );

                mApp->register_message_handler(
                    SERVICE_ID,
                    INSTANCE0_ID,
                    vsomeip::ANY_METHOD,
                    std::bind(&ClientHandle::onMessage,this,std::placeholders::_1)
                );

                mPayload = vsomeip::runtime::get()->create_payload();

                mApp->request_service(SERVICE_ID,INSTANCE0_ID);

                return true;
            }
        }
        void startApp() {
            mApp->start();
        }
    private:
        std::shared_ptr<vsomeip::application> mApp;
        std::shared_ptr<vsomeip::payload> mPayload;
        std::mutex mPayloadMutex;
        uint32_t mCycle;
        std::string mLocName;
        std::string mLocCode;
        std::atomic<bool> fReady;

    public:
        void onState(vsomeip::state_type_e state) {
            switch (state)
            {
            case vsomeip::state_type_e::ST_DEREGISTERED:
                std::cout << "Message: DEREGISTERED\n";
                fReady=false;
                break;
            case vsomeip::state_type_e::ST_REGISTERED:
                std::cout << "Message: REGISTERED\n";
                fReady=true;
            default:
                break;
            }
        }
        void onAvailable(vsomeip::service_t service, vsomeip::instance_t instance, bool available) {
            std::cout << (available ? "available" : "Not available") << "\n";

            if(available) {
                std::shared_ptr<vsomeip::message> request = vsomeip::runtime::get()->create_request();
                request->set_service(SERVICE_ID);
                request->set_method(CODE_GETTER_METHOD_ID);
                request->set_instance(INSTANCE0_ID);

                std::vector<vsomeip::byte_t> data;
                for(auto& ch:mLocName) {
                    data.push_back((vsomeip::byte_t)ch);
                }

                mPayload->set_data(data);
                request->set_payload(mPayload);
                mApp->send(request);

            } 
            //std::cout << "FLow\n";
        }
        void onMessage(const std::shared_ptr<vsomeip::message>& message) {

            if(message->get_message_type() == vsomeip::message_type_e::MT_RESPONSE) {

                if(message->get_method() == CODE_GETTER_METHOD_ID) {

                    std::cout << "Location Code received from Code-Service\n";
                    mPayload = message->get_payload();
                    auto length = mPayload->get_length();
                    
                    for(vsomeip::byte_t i{};i<length;i++)
                        mLocCode += (char)mPayload->get_data()[i];
                    std::cout << mLocCode << "\n";

                    // prepare a new request for the Report-Service
                    std::shared_ptr<vsomeip::message> request = vsomeip::runtime::get()->create_request();
                    request->set_service(SERVICE_ID);
                    request->set_instance(INSTANCE1_ID);
                    request->set_method(REPORT_GETTER_METHOD_ID);

                    request->set_payload(mPayload);
                    mApp->send(request);

                } else if(message->get_method() == REPORT_GETTER_METHOD_ID) {

                    std::cout << "Report reveived from Report-Service\n";

                } else {

                    std::cout << "Received ANY_RESPONSE\n";

                }

            }
        }
};
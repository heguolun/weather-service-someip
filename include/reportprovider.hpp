#ifndef IOSTREAM
    #include <iostream>
#endif

#include <vsomeip/vsomeip.hpp>
#include "configfile.hpp"

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
                    INSTANCE_ID,
                    std::bind(&ServiceHandle::onAvailable, this,
                        std::placeholders::_1,std::placeholders::_2,std::placeholders::_3
                    )
                );

                mApp->register_message_handler(
                    vsomeip::ANY_SERVICE,
                    vsomeip::ANY_INSTANCE,
                    vsomeip::ANY_METHOD,
                    std::bind(&ServiceHandle::onMessage,this,std::placeholders::_1)
                );

                mApp->request_service(SERVICE_ID,INSTANCE_ID);

                return true;
            }
        }
        void startApp() {
            mApp->start();
        }
    private:
        std::shared_ptr<vsomeip::application> mApp;
        std::shared_ptr<vsomeip::message> mPayload;

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
            std::cout << "Received a message <<<< Send a message\n";
        }
};
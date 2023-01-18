/*
 * OnvifConnector.h
 *
 *  Created on: 2016年9月12日
 *      Author: liu
 */

#pragma once

#include "remote_connector.h"
#include <thread>
#include <mutex>

class DeviceAdapter;
namespace onvif {
    class  SoapService;
    struct OnvifContext;
    class  DiscoveryService;
    class  EventLoop;
}

class OnvifConnector: public RemoteConnector {
    public:
        enum State {
            RUNING,
            STOP,
        };

        OnvifConnector(DeviceAdapter *adaptor);
        virtual ~OnvifConnector();
        virtual int Init(const InitParam &param);
        virtual int Start();
        virtual int Stop();
        int Join();

    private:
        typedef void* (ThreadFunc)(void*);
        static void *ServiceThread(OnvifConnector *connector);

    private:
        DeviceAdapter *_adaptor;
        onvif::DiscoveryService *_discoverService;
        onvif::SoapService *_soapService;
        onvif::OnvifContext *_onvifContext;
        onvif::EventLoop *_loop;

        std::thread _serviceThread;
        std::mutex _serviceStateMutex;
        State _state;
};

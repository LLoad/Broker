#ifndef __WEB_SOCKET_CLIENT_H__
#define __WEB_SOCKET_CLIENT_H__

#include <libwebsockets.h>
#include <core/private.h>
#include <Thread.h>

class WebSocketClient : public common::Thread
{
public:
   // user_data 는 callback 시 wsi->context->user_space 로 들어옴
   WebSocketClient();
   WebSocketClient(const lws_context_creation_info& creation_info, void* user_data = nullptr, const int timeout_ms = 100);
   virtual ~WebSocketClient();

public:
   lws* Connect(const std::string& address, const int port, const std::string& path, const std::string& sub_protocol_name);
   void Disconnect();

private:
   void Run();

public:
   lws* _lws;
   lws_context* _lws_context;
   int _timeout_ms;
};


#endif // __WEB_SOCKET_CLIENT_H__
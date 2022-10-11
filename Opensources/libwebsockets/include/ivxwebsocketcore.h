#pragma once

#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <libwebsockets.h>

#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <thread>
#include <map>
#include <mutex>
#include <vector>
#include <stack>
#include <iostream> 
#include <functional>

#include "../src/base32.h"

using namespace std;


// [SGLEE:20191022TUE_110900] API 키로 base64 사용 그렇지 않으면 base32가 사용됨
//#define IVX_WS_API_KEY_BASE64


namespace ivxwebsockets
{


   ////////////////////////////////////////////////////////////////
   //
   //   Enumerations
   //
   ////////////////////////////////////////////////////////////////

   enum IVX_HTTP_RET_CODE
   {
	   IVX_HTTP_RET_OK					    = 200, // 2020-03-13 Seolrok: 추가
      IVX_HTTP_RET_INVALID_METHOD_TYPE	 = 450,
      IVX_HTTP_RET_LOGIN_FAILED			 = 451,
      IVX_HTTP_RET_INVALID_API_KEY		 = 452,
      IVX_HTTP_RET_INVALID_BODY_FORM	 = 453,
      IVX_HTTP_RET_FULL_API_KEY			 = 454,
      IVX_HTTP_RET_INVALID_URI_FORM     = 455,
	  // ===========================================
	  // 2020-03-13 Seolrok: devices/alarm 메시지
	  // ===========================================
	  IVX_HTTP_RET_UNKNOWN_ERROR		    = 470,
	  IVX_HTTP_RET_EMPTY_DEVICE_ID		 = 471,
	  IVX_HTTP_RET_INVALID_DEVICE_ID	    = 472,
	  IVX_HTTP_RET_INVALID_DEVICE_PORT	 = 473,
	  IVX_HTTP_RET_INVALID_DEVICE_STATUS = 474
	  // ===========================================
   };

   enum VAMetaType
   {
      VAMetaType_Frame  = 0,
      VAMetaType_Event  = 1,
      VAMetaType_Face   = 2,
      VAMetaType_Plate  = 3,
      VAMetaType_PTZPos = 4,
      VAMetaType_EventZone = 5,  // 5GGK : 2020-06-19
   };

   const int VAMetaTypeNum = sizeof(enum IVX_VA_META_FLAG);

   enum FrmMetaSendFlag
   {
      FrmMetaSendFlag_Send    = 0x00000001,
      FrmMetaSendFlag_Color   = 0x00000002,
   };

   enum EvtMetaSendFlag
   {
      EvtMetaSendFlag_Begun      = 0x00000001,
      EvtMetaSendFlag_Ended      = 0x00000004,
      EvtMetaSendFlag_evtImg     = 0x00000040,
      EvtMetaSendFlag_FullImg    = 0x00000080,
      EvtMetaSendFlag_Color      = 0x00000100,
   };

   enum FaceMetaSendFlag
   {
      FaceMetaSendFlag_DetectBegun        = 0x00000001,
      FaceMetaSendFlag_DetectEnded        = 0x00000002,
      FaceMetaSendFlag_RecogRegistered    = 0x00000004,
      FaceMetaSendFlag_RecogUnregistered  = 0x00000008,
      FaceMetaSendFlag_AttrUpdated        = 0x00000010,
      FaceMetaSendFlag_FaceImg            = 0x00000020,
      FaceMetaSendFlag_FullImg            = 0x00000040,
   };

   enum PlateMetaSendFlag
   {
      PlateMetaSendFlag_RecogPlate = 0x00000001,
      PlateMetaSendFlag_PlateImg   = 0x00000002,
      PlateMetaSendFlag_FullTNImg  = 0x00000004,
      PlateMetaSendFlag_FullImg    = 0x00000008,
      PlateMetaSendFlag_Package    = 0x00000010,
   };

   enum EvtZoneMetaSendFlag
   {
      EvtZoneMetaSendFlag_Requested = 0x00000001,
      EvtZoneMetaSendFlag_Sent      = 0x00000002,
   };

   enum EvtZoneCntMetaSendFlag
   {
      EvtZoneCntMetaSendFlag_Send = 0x00000001,
   };

   enum VehicleQueueMetaFlag
   {
      VehicleQueueMetaSendFlag_Send       = 0x00000001,
   };

   enum CrosswalkStatisticsMetaSendFlag 
   {
      CrosswalkStatisticsMetaSendFlag_Send = 0x00000001,
   };
   enum LoginReqCallbackRet
   {
      LoginReqCallbackRet_OK   = 0,
      LoginReqCallbackRet_FAIL = 1,
   };

   enum WsOptionFlag
   {
      WsOptionFlag_BinaryInfo    = 0x00000001,
      WsOptionFlag_Gid           = 0x00000002,
   };

   enum WsFaceFilter
   {
      WsFaceFilter_Affiliation   = 1,
   };

   enum CameraStateFlag
   {
      CameraStateSendFlag_Send         = 0x00000001,
   };

   ////////////////////////////////////////////////////////////////
   //
   //   Utility Functions
   //
   ////////////////////////////////////////////////////////////////

   char* ANSIToUTF8(const char* pszCode);
   char* UTF8ToANSI(const char* pszCode);

   ///////////////////////////////////////////////////////////////////////////////
   //
   // Class: auto_signal
   //
   ///////////////////////////////////////////////////////////////////////////////

   class auto_signal {
   public:
      auto_signal()
      {

      }
      ~auto_signal()
      {

      }

      /**
      * Signal to event.
      */
      void set();

      /**
      * Wait for signal.
      */
      void wait();
      bool wait_for(uint64_t duration);

   private:
      bool _flag = false;
      std::condition_variable _condition;
      std::mutex _mutex;
   };

   ////////////////////////////////////////////////////////////////
   //
   //   struct : IVXWebAPIMessage
   //
   ////////////////////////////////////////////////////////////////

   struct IVXWebAPIMessage
   {
   public:
      uint8_t*                m_pData = nullptr;
      size_t                  m_nSize = 0;
      enum lws_write_protocol m_nProtocol;

   public:
      IVXWebAPIMessage() {};
      IVXWebAPIMessage(uint8_t* pData, size_t nSize, enum lws_write_protocol nProtocol)
      {

         // 메모리 릭이 나는 케이스
         if (m_pData != nullptr)
         {
            __debugbreak();
         }

         m_pData = new uint8_t[nSize];
         m_nSize = nSize;

         memcpy(m_pData, pData, m_nSize);

         m_nProtocol = nProtocol;
      }

      ~IVXWebAPIMessage()
      {
         if (nullptr != m_pData)
         {
            delete[] m_pData;
            m_pData = nullptr;
         }
      }
   };

   ////////////////////////////////////////////////////////////////
   //
   //   class : IVXWebsocketClient
   //
   ////////////////////////////////////////////////////////////////

   // 개별 웹소캣 연결을 관리하는 객체

   class IVXWebsocketClient
   {
   public:
      unsigned int   FrmMetaSendFlag                  = 0;
      unsigned int   EvtMetaSendFlag                  = 0;
      unsigned int   EvtAlmMetaSendFlag               = 0;
      unsigned int   FaceMetaSendFlag                 = 0;
      unsigned int   PlateMetaSendFlag                = 0;
      uint32_t       EvtZoneMetaSendFlag              = 0;
      uint32_t       EvtZoneCntMetaSendFlag           = 0;
      uint32_t       VaOptionFlags                    = 0;
      uint32_t       VaFaceFilter                     = 0;
      uint32_t       VehicleQueueMetaSendFlag         = 0;
      uint32_t       CrosswalkStatisticsMetaSendFlag  = 0;
      uint32_t       CameraStateFlag                  = 0;
      std::string    VaFaceFilterValue;

   public:
      struct lws* wsi = nullptr;
      std::mutex  mtxlws;

   private:
      std::deque<IVXWebAPIMessage*>    m_SendMessageList;
      std::mutex                       m_LockSendMessageList;
      std::thread                      m_MessageSendThread;
      int                              m_SendInterrupted = 0;
      auto_signal                      m_SignalSendMessage;

   public:
      IVXWebsocketClient();
      ~IVXWebsocketClient();

   public:
      void              AddSendMessage(uint8_t* pMessage, size_t nMessageLen, enum lws_write_protocol nProtocol);
      IVXWebAPIMessage* GetSendMessage();
      int               GetSendMessageListSize();

   private:
      int         MessageSendProc();
   };


   ////////////////////////////////////////////////////////////////
   //
   //   struct : IVXWebAPIClient
   //
   ////////////////////////////////////////////////////////////////

   // 웹 API 클라이언트 객체를 관리하고자 하는 구조체

   struct IVXWebAPIClient
   {
   public:
      std::string PeerAddress;
      std::string ID;
      std::string PW;
      char APIKey[64] = {0,};

      // API 클라이언트 별 연결유지 관리
      unsigned int KeepAliveTimeOut = 60 * 15;  // 15분
      unsigned int countFromLastKeepAlive = 0;

   public:
      std::recursive_mutex Mtx_WSClientArr;       
      std::map<struct lws*, IVXWebsocketClient*> WSClientArr_VAMetadata; // 실시간 VA Metadata 전송용 웹소켓 접속 리스트 
      std::vector<IVXWebsocketClient*> WSClientArr_VASearch;  // 미구현

      ~IVXWebAPIClient()
      {
         std::lock_guard<std::recursive_mutex> co(Mtx_WSClientArr);
         std::map<struct lws*, IVXWebsocketClient*>::iterator iter = WSClientArr_VAMetadata.begin();
         while (iter != WSClientArr_VAMetadata.end())
         {
            IVXWebsocketClient* webSocketClient = iter->second;
            if (nullptr != webSocketClient)
            {
               delete webSocketClient;
               webSocketClient = nullptr;
            }

            WSClientArr_VAMetadata.erase(iter++);
         }
      }

   public:
      IVXWebsocketClient* GetWebsocketClient(lws* wsi)
      {
         IVXWebsocketClient* pClient = nullptr;
         std::lock_guard<std::recursive_mutex> co(Mtx_WSClientArr);
         std::map<struct lws*, IVXWebsocketClient*>::iterator iter = WSClientArr_VAMetadata.find(wsi);
         if (WSClientArr_VAMetadata.end() != iter)
            pClient = iter->second;
         return pClient;
      }

      void AddSendMessage(lws* wsi, uint8_t* pMessage, size_t nMessageLen, enum lws_write_protocol nProtocol)
      {
         IVXWebsocketClient* pClient = GetWebsocketClient(wsi);
         if (nullptr != pClient)
            pClient->AddSendMessage(pMessage, nMessageLen, nProtocol);
      }

      IVXWebAPIMessage* GetSendMessage(lws* wsi)
      {
         IVXWebAPIMessage* pSendMessage = nullptr;
         IVXWebsocketClient* pClient = GetWebsocketClient(wsi);
         if (nullptr != pClient)
            pSendMessage = pClient->GetSendMessage();
         return pSendMessage;
      }

      int GetSendMessageListSize(lws* wsi)
      {
         int nSize = 0;
         IVXWebsocketClient* pClient = GetWebsocketClient(wsi);
         if (nullptr != pClient)
            nSize = pClient->GetSendMessageListSize();
         return nSize;
      }
   };

   // 사용자가 웹 API 클라이언트 정보를 추가한 커스텀 클래스의 객체를 생성할 때 사용하는 함수
   void  SetCreationFunction_WebAPIClient(IVXWebAPIClient*(*func)());


   ////////////////////////////////////////////////////////////////
   //
   //   struct : IVXWebServerInfo
   //
   ////////////////////////////////////////////////////////////////

   // 웹 서버를 초기화 하는데 사용하는 정보

   struct IVXWebServerInfo
   {
      int  WebServerPortNo = 8004;
      char UserIDHash[256] = { 'd','e','f', 0 };

      // [SGLEE:20200115WED_130600] HPPTS 지원
      bool  IsSecure = false;
      // [SGLEE:20200207FRI_133900] 인천공항
      bool  IsIcnAir = false;
   };


   ////////////////////////////////////////////////////////////////
   //
   //   struct : pss
   //
   ////////////////////////////////////////////////////////////////

   // 멀티파트 방식을 파일을 또는 파라메터들을 업로드하는 데 필요한 정보

   // Unlike ws, http is a stateless protocol.  This pss only exists for the
   // duration of a single http transaction.  With http/1.1 keep-alive and http/2,
   // that is unrelated to (shorter than) the lifetime of the network connection.
   struct pss {
      lws_callback_reasons Reason;  
      bool    Flag_ProcMultipart = 0;         // 멀티 파트로 처리해야 해야할지 여부, uri (api) 이름이 멀티파트를 통해 데이터를 업로드 해야하는 겨우 이 플래그클 On시켜야한다. 
      struct  lws_spa *spa;                   // lws helper decodes multipart form 
      char    filename[128];                  // the filename of the uploaded file 
      unsigned long long file_length;         // the amount of bytes uploaded */
      FILE*   fd;                             // fd on file being saved */
      const char * const *  param_names = 0;  // 파라메터 이름에 대한 배열
      int                   param_num = 0;    // 파라메터 수

      int      mTmpVarInt  = 0;
   };


   ////////////////////////////////////////////////////////////////
   //
   //   Class : IVXWebServerCore
   //
   ////////////////////////////////////////////////////////////////
   using namespace rapidjson;
   class IVXWebServerCore
   {

   public:
      enum OnHTTPMessageRet
      {
         OnHTTPMessageRet_WriteHttp        = LWS_WRITE_HTTP,          // 다음에 더 보낼 내요잉 있을 때
         OnHTTPMessageRet_WriteHttpFinal   = LWS_WRITE_HTTP_FINAL,    // 보낼 내용이 더 없고 마무리 할 필요가 있을 때
         OnHTTPMessageRet_MultiPartRequest = 100
      };

      enum HTTP_METHOD_TYPE
      {
         HTTP_METHOD_GET      = 0,
         HTTP_METHOD_POST     = 1,
         HTTP_METHOD_OPTIONS  = 2,
         HTTP_METHOD_PUT      = 3,
         HTTP_METHOD_DELETE   = 5,
      };

      enum WS_DISCONNECT_CODE
      {
         WS_INVALID_API_KEY = 501,
         WS_KEEP_ALIVE_TIME_OUT = 502,
      };

   public:
      using OnGetApiKeyFunctor = std::function<int(const std::string& userID, const std::string& userPW, std::string& apiKeyOut)>;
      // [SGLEE:20201027TUE_130800] LGU+ 마곡
      using OnLoginSuccessFunctor = function<void(const string& peer, const string& id, const string& pw, const string& apiKey)>;
      using OnLoginErrorFunctor = function<void(const string& peer, const string& id, const string& pw)>;
      using OnKeepAliveFunctor = function<void(const string& peer, const string& apiKey)>;
      using OnLogoutFunctor = function<void(const string& peer, const string& apiKey)>;

      using OnRegFaceFunctor = function<void(const string& peer, const string& name, int age, int gender, const string& affiliation)>;
      using OnUnregFaceFunctor = function<void(const string& peer, const string& name)>;

      using OnWsConnectedFunctor = function<void(const string& peer, const string& apiKey, const string& uri,
         const vector<string>& metas)>;
      using OnWsDisconnectedFunctor = function<void(const string& peer, const string& apiKey, const string& msg)>;

      using OnLogFunctor = function<void(const string& peer, const string& msg)>;
      struct ICallback
      {
         OnGetApiKeyFunctor         OnGetApiKey;
         OnLoginSuccessFunctor      OnLoginSuccess = nullptr;
         OnLoginErrorFunctor        OnLoginError = nullptr;
         OnKeepAliveFunctor         OnKeepAlive = nullptr;
         OnLogoutFunctor            OnLogout = nullptr;

         OnRegFaceFunctor           OnRegFace = nullptr;
         OnUnregFaceFunctor         OnUnregFace = nullptr;

         OnWsConnectedFunctor       OnWsConnected = nullptr;
         OnWsDisconnectedFunctor    OnWsDisconnected = nullptr;

         OnLogFunctor               OnLog = nullptr;
      };

   protected:
      std::thread  ServerThread;
      struct lws_context* LWSContext = nullptr;

      std::thread  ClientMgrThread;

   public:
		// 2020-03-13 Seolrok: 생성자 추가 및 소멸자를 virtual로
	   IVXWebServerCore() {}
	   IVXWebServerCore(IVXWebServerInfo* ivxWSInfo);
	   virtual ~IVXWebServerCore();

   public:
      IVXWebServerInfo WSInfo;

      int  Inititialize(IVXWebServerInfo* ivxWSInfo);
      virtual void Uninitialize(); // 2020-09-04 Seolrok: virtual로

      //  사용자는 모든 HTTP 요청에 기본해더를 받드시 추가해야한다. 
      int  AddHttpCommonHeaders(struct lws *wsi, unsigned int code, int method = 0);
      int  AddHttpAllowOptionsHeaders(struct lws *wsi, uint8_t** currentPointerOfBuffer, uint8_t* endPointerOfBuffer);
      void AddMetadataIVXClient(struct lws *wsi, int method, int type,int detail);
      //  HTTP 요청에 에러를 리턴하고 싶을 때는 이 함수를 호출해야한다. 
      //  이 함수를 요청하게 되면 body가 html_body스트링으로 채워지게 된다. 
      int  ReturnHttpStatus    (struct lws *wsi, unsigned int code, const char* html_body);

      // 웹소켓에 데이터를 Write하거나 HTTP 요청에서 body 데이터를 작성할때 사용하는 함수.
      int  LWSWrite (struct lws *wsi, unsigned char *buf, size_t len, enum lws_write_protocol protocol);

      // 로그인을 제외한 모든 요청에 대해 해당 함수를 호출하여 APIKey를 확인해야한다.
      BOOL CheckAPIKey(struct lws *wsi, const char* body);
   public:

      int(*Callback_LoginReq)    (struct lws *wsi, const char* id, const char* pw) = nullptr;
      int(*Callback_HTTPMessage) (enum lws_callback_reasons reason, struct lws *wsi, int method, char* url, char* body, char* argument, struct pss* pss) = nullptr;

   protected:
	   // 2020-05-21 Seolrok: tNVR 이벤트 통지 처리
	   virtual int OntNVREventNotify(struct lws *wsi, char* url, int method, char* body) { ReturnHttpStatus(wsi, HTTP_STATUS_OK, "OK"); return LWS_WRITE_HTTP_FINAL; }

   protected:
      static int ThreadServerProc(void* param);
      int ServerProc();

      static int ThreadClientMgrProc(void* param);
      int ClientMgrProc();

   protected:
      std::recursive_mutex Mtx_WebApiClientList;

      // 웹소켓 연결을 관리하는 배열임. 
      std::map<std::string, IVXWebAPIClient*>  WebApiClientList; 

      int OnHTTPMessage(enum lws_callback_reasons reason, struct lws *wsi, int method, char* url, char* body, char* argument, struct pss* pss);
      int OnLogin(enum lws_callback_reasons reason, struct lws *wsi, char* url, char* body);
      int OnLogout(enum lws_callback_reasons reason, struct lws *wsi, char* url, char* body);
      int OnKeepAlive(struct lws *wsi, char* body = nullptr);
      int OnOptions(struct lws *wsi, int method);
      int OnWebSocketEstablished(struct lws *wsi, char* uri);
      void  OnWebSocketClosed(struct lws *wsi);
      int   OnWebsocketWritable(struct lws *wsi);

      int SendWebsocketDisconnectCode(struct lws* wsi, WS_DISCONNECT_CODE nErrorCode);
      int OnPostDevicesChannels(lws_callback_reasons reason, lws* wsi, char* url, char* body);

   protected:
      int         ServiceThreadCount = 7;
      pthread_t*  ServiceThread = nullptr;
      int         ServiceInterrupted = 0;
      
      struct      ServiceThreadParam
      {
         lws_context* LWSContext = nullptr;
         int          ServiceThreadID = -1;
         int* ServiceInterrupted = nullptr;
      };

      // http 및 websocket 서비스의 멀티스레딩 지원을 위한 함수
      static void *ThreadService(void *param);
      static int  CallbackFileUpload(void *data, const char *name, const char *filename, char *buf, int len, enum lws_spa_fileupload_states state);
      static int  CallbackDynamicHttp(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);
      static int  CallbackWebSocket(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);


   protected:
      int  GetAPIClientCount(const char* user_id);
      std::string GetAPIKey(const char* str);
#ifdef IVX_WS_API_KEY_BASE64
      void base64Encode(char const* origSigned, unsigned origLength, std::string& strResult);
#endif
      int  GetIntfromJSON(const Value& v, std::string key);


   protected:
      int __dbgout(const char* format, ...);

      // [SGLEE:20191203TUE_203700] 이벤트 수신 채널 리스트 설정
   protected:
      std::map<std::string, std::vector<std::string>>       m_ClientMap;
      recursive_mutex                                       m_ClientMapMtx;
      int AddClient(const std::string& ClientIP, const std::vector<std::string> CamIPVec);
      std::string GetClientIPByCamIP(const std::string& CamIP);
//#define __TEST_CLIENT_IP
#ifdef __TEST_CLIENT_IP
      int __TestClientIP();
#endif

      public:
         int SetCallback(ICallback* pCallback);

      private:
         static int LWS_OnClientAdded_static(void* wsi, void* userPtr, wchar_t* ipAddr, wchar_t* url);

         static int LWS_OnGetMethod_static(void* wsi, void* userPtr, wchar_t* ipAddr, wchar_t* url, int isSendingHdr);
         static int LWS_OnPostMethod_static(void* wsi, void* userPtr, wchar_t* ipAddr, wchar_t* url, int isSendingHdr);

         static int LWS_OnWsConnected_static(void* wsi, void* userPtr, wchar_t* ipAddr);
         static int LWS_OnWsDisconnected_static(void* wsi, void* userPtr, wchar_t* ipAddr);

      protected:
         virtual int OnClientAdded(void* wsi, const wchar_t* ipAddr, const wchar_t* url) = 0;

         virtual int OnPostFacesRegister(void* wsi, const wchar_t* name, const wchar_t* fileName, int fileSize) = 0;
         virtual int OnPostFacesUnregister(void* wsi, const wchar_t* name) = 0;

         virtual int _OnPostDevicesChannels(void* wsi) = 0;

         virtual int OnWsConnected(void* wsi, const wchar_t* ipAddr) = 0;
         virtual int OnWsDisconnected(void* wsi, const wchar_t* ipAddr) = 0;

      public:
         ICallback      mCallback;

      public:
         BOOL           m_bAlwaysVehiQueueMetaFlag = FALSE;

   };

};
#ifndef Pt_Net_HttpClient_h
#define Pt_Net_HttpClient_h

#include <Pt/Text/String.h>
#include <Pt/IO/Url.h>
#include <Pt/Net/TcpIp4Socket.h>
#include <Pt/Net/HttpHeader.h>

namespace Pt {

namespace Net {

	//! HTTP Request
	class PT_API HttpRequest {
		public:
			//! HTTP Request methods
			enum Method {
				HEAD   = 0,
				GET    = 1,
				POST   = 2,
				PUT    = 3,
				DELETE = 4
			};

			HttpRequest();

			HttpRequest(Method method, const String& url);

			~HttpRequest();

			void clear() {
				_url.clear();
				_header.clear();
			}

			Method method() const throw();
			void setMethod(Method m) throw();

			const String& url() const throw();
			void setUrl(const String& url);

			void setProtocol(const String& protocol);
			const String& protocol() const;

			const HttpRequestHeader& header() const throw();

			HttpRequestHeader& header() throw();

		private:
			Method _method;

			String _url;
			String _protocol;
			HttpRequestHeader _header;
	};

	//! HTTP Status codes
	/*!
		Response status codes used by the Hypertext-Transfer-Protocol.
	*/
	enum HttpStatusCode {
		HTTP_100_CONTINUE           = 100,
		HTTP_101_SWITCHING_PROTOCOL = 101,
		HTTP_200_OK                 = 200,
		HTTP_201_CREATED            = 201,
		HTTP_202_ACCEPTED           = 202,
		HTTP_203_NON_AUTH_INFO      = 203,
		HTTP_204_NO_CONTENT         = 204,
		HTTP_205_RESET_CONTENT      = 205,
		HTTP_206_PARTIAL_CONTENT    = 206,
		HTTP_300_MULTIPLE_CHOICES   = 300,
		HTTP_301_MOVED_PERMANENTLY  = 301,
		HTTP_302_FOUND              = 302,
		HTTP_303_SEE_OTHER          = 303,
		HTTP_304_NOT_MODIFIED       = 304,
		HTTP_305_USE_PROXY          = 305,
		HTTP_307_TEMPORARY_REDIRECT = 307,
		HTTP_400_BAD_REQUEST        = 400,
		HTTP_401_UNAUTHORIZED       = 401,
		HTTP_402_PAYMENT_REQUIRED   = 402,
		HTTP_403_FORBIDDEN          = 403,
		HTTP_404_NOT_FOUND          = 404,
		HTTP_405_METHOD_NOT_ALLOWED = 405,
		HTTP_406_NOT_ACCEPTABLE     = 406,
		HTTP_407_PROXY_AUTH_REQUIRED= 407,
		HTTP_408_REQUEST_TIMEOUT    = 408,
		HTTP_409_CONFLICT           = 409,
		HTTP_410_GONE               = 410,
		HTTP_411_LENGTH_REQUIRED    = 411,
		HTTP_412_PRECONDITION_FAILED= 412,
		HTTP_413_REQUEST_TOO_LARGE  = 413,
		HTTP_414_REQ_URI_TOO_LONG   = 414,
		HTTP_415_UNSUPP_MEDIA_TYPE  = 415,
		HTTP_416_REQ_RANGE_INVALID  = 416,
		HTTP_417_EXPECTATION_FAILED = 417,
		HTTP_500_INTERNAL_ERROR     = 500,
		HTTP_501_NOT_IMPLEMENTED    = 501,
		HTTP_502_BAD_GATEWAY        = 502,
		HTTP_503_SERVICE_UNAVAILABLE= 503,
		HTTP_504_GATEWAY_TIMEOUT    = 504,
		HTTP_505_VERSION_UNSUPPORTED= 505
	};


	class PT_API HttpResponse {
		public:
			HttpResponse();
			HttpResponse(const std::string& protoVer, int code, const std::string& reason);
	
			~HttpResponse() throw();
	
			void clear() {
				_protoVer.clear();
				_reason.clear();
				_bytesRead = 0;
				_contentLength = 0;
				_chunkedEncoding = false;
				_header.clear();
			}

			const HttpResponseHeader& header() const throw();
			HttpResponseHeader& header() throw();

			const std::string& protocolVersion() const throw();

			int statusCode() const throw();

			const std::string& reason() const throw();

		private:
			HttpResponseHeader _header;
			std::string  _protoVer;
			int          _statusCode;
			std::string  _reason;
			size_t       _bytesRead;
			size_t       _contentLength;
			bool         _chunkedEncoding;
	};


	class PT_API HttpSocket : public TcpIp4Socket {
		public:
			HttpSocket(const Ip4Address& address, port_t port);

			HttpSocket(TcpIp4Socket& socket);

			HttpSocket(TcpIp4ServerSocket& socket);

			virtual ~HttpSocket() throw();

			void sendRequest(HttpRequest& request);

			void readRequest(HttpRequest& request) throw(IO::IOError);

			void readResponse(HttpResponse& response);

			void sendResponse(HttpResponse& response);

			void readContent(std::string& content, int contentLength);

			void sendContent(const std::string& content, int contentLength);

			void readHttpHeader(HttpHeader& header);
	};

} // !namespace Net

} // !namespace Pt

#endif

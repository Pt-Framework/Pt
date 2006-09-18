#include <netinet/in.h>

#ifdef PT_HAVE_APPLETALK
#  include <netatalk/at.h>
#endif

#ifdef PT_HAVE_IPX
#  include <netipx/ipx.h>
#endif

#include "Pt/Pt-config.h"
#include "Pt/IO/IOStream.h"
#include "Pt/Net/HttpClient.h"

#include <sstream>
using namespace std;


namespace Pt {

namespace Net {

HttpRequest::HttpRequest()
{
}

HttpRequest::HttpRequest(Method method, const String& url)
: _method(method), _url(url)
{
}

HttpRequest::~HttpRequest()
{
}

HttpRequest::Method HttpRequest::method() const throw()
{
	return _method;
}

void HttpRequest::setMethod(Method m) throw()
{
	_method = m;
}

const String& HttpRequest::url() const throw()
{
	return _url;
}

void HttpRequest::setUrl(const String& url) 
{
	_url = url;
}

void HttpRequest::setProtocol(const String& protocol) {
	_protocol = protocol;
}

const String& HttpRequest::protocol() const {
	return _protocol;
}

const HttpRequestHeader& HttpRequest::header() const throw()
{
	return _header;
}

HttpRequestHeader& HttpRequest::header() throw()
{
	return _header;
}





HttpResponse::HttpResponse()
: _bytesRead(0), _contentLength(0), _chunkedEncoding(false)
{
}


HttpResponse::HttpResponse(const std::string& protoVer, int code, const std::string& reason)
: _protoVer(protoVer), _statusCode(code), _reason(reason),
  _bytesRead(0), _contentLength(0), _chunkedEncoding(false)
{
}

HttpResponse::~HttpResponse() throw()
{
}

const HttpResponseHeader& HttpResponse::header() const throw()
{
	return _header;
}

HttpResponseHeader& HttpResponse::header() throw()
{
	return _header;
}

const std::string& HttpResponse::protocolVersion() const throw()
{
	return _protoVer;
}

int HttpResponse::statusCode() const throw()
{
	return _statusCode;
}

const std::string& HttpResponse::reason() const throw()
{
	return _reason;
}





HttpSocket::HttpSocket(const Ip4Address& address, port_t port)
: TcpIp4Socket()
{
	//cerr << "Http: Connecting to  " << address.str() << " " << port << endl;
	TcpIp4Socket::connect(address, port);
	//cerr << "Http: done." << endl;
}


HttpSocket::HttpSocket(TcpIp4Socket& socket)
: TcpIp4Socket(socket)
{
}


HttpSocket::HttpSocket(TcpIp4ServerSocket& socket)
: TcpIp4Socket(socket)
{
}


HttpSocket::~HttpSocket() throw()
{
}


void HttpSocket::readHttpHeader(HttpHeader& header) {
	string line;
	while(1) {
		line = this->readLine();
		//cerr << "HttpSocket: " << line;

		if( line.empty() || line == "\r" || line == "\r\n" || line == "\n" || this->eof() )
			break;

		size_t pos = line.find(':');
		if(pos != string::npos) {
			string fieldName = line.substr(0, pos);
			string fieldVal  = line.substr(pos+2, line.size()-(pos+4));
			header.setField(fieldName, fieldVal);
		}
	}
}


void HttpSocket::sendRequest(HttpRequest& req) {
	IO::IOStream reqs(*this);

	switch(req.method())
	{
		case HttpRequest::GET:
		reqs << "GET ";
		break;

		case HttpRequest::HEAD:
		reqs << "HEAD ";
		break;

		case HttpRequest::POST:
		reqs << "POST ";
		break;

		case HttpRequest::PUT:
		reqs << "PUT ";
		break;

		case HttpRequest::DELETE:
		reqs << "DELETE ";
		break;

		default:
		throw IO::IOError("HTTP request not implemented", PT_SOURCEINFO);
	}

	// send request line ..
	reqs << req.url() << " HTTP/1.1\r\n";

	// send header lines ...
	HttpHeader::ConstFieldIterator i = req.header().begin();
	bool hostFound = false;
	while(i != req.header().end())
	{
		if(i->first == "Host")
			hostFound = true;

		reqs << i->first << ": " << i->second << "\r\n";
		//cerr << i->first << ": " << i->second << "\r\n";
		++i;
	}

	// "Host:" header is required in HTTP/1.1!
	//if(!hostFound)
		//reqs << "Host: " << req.url() << "\r\n";

	// terminate request and flush stream
	reqs << "\r\n";
	reqs.sync();
}


void HttpSocket::readRequest(HttpRequest& request) throw(IO::IOError) {
	//cerr << "HttpSocket: Reading request" << endl;
	string method, url, protocol;
	
	string str = readLine();
	//cerr << "HttpSocket: HTTP Primer: " << str << endl;

	istringstream is(str);

	is >> method;
	is >> url;
	is >> protocol;

	request.clear();
	request.setUrl(url);
	request.setProtocol(protocol);

	if(method == "GET") {
		request.setMethod(HttpRequest::GET);
	}
	else if(method == "HEAD") {
		request.setMethod(HttpRequest::HEAD);
	}
	else if(method == "POST") {
		request.setMethod(HttpRequest::POST);
	}
	else if(method == "PUT") {
		request.setMethod(HttpRequest::PUT);
	}
	else if(method == "DELETE") {
		request.setMethod(HttpRequest::DELETE);
	}
	else {
		throw IO::IOError("HTTP request not implemented", PT_SOURCEINFO);
	}

	this->readHttpHeader( request.header() );
}


void HttpSocket::readResponse(HttpResponse& response) {
	//cerr << "HttpSocket: Reading response" << endl;

	std::string line1 = readLine();
	//cerr << "HttpSocket: HTTP Primer:" << line1;

	std::string httpProto, httpResponse;
	int httpResponseCode;
	std::string tmp;
	std::istringstream is(line1);

	is >> httpProto;
	is >> httpResponseCode;
	std::getline(is, httpResponse);

	HttpResponse resp(httpProto, httpResponseCode, httpResponse);
	response = resp;

	this->readHttpHeader( response.header() );
}


void HttpSocket::sendResponse(HttpResponse& response) {
	std::stringstream data;
	data << response.protocolVersion() << " " << response.statusCode() << " " << response.reason() <<"\r\n";

	for(HttpHeader::FieldIterator it = response.header().begin(); it != response.header().end(); ++it) {
		data << it->first << ": " << it->second << "\r\n"; 
	}
	data << "\r\n";
	
	this->write( data.str().c_str(), data.str().size() );
}


void HttpSocket::readContent(std::string& content, int contentLength) {
	//cerr << "Reading HTTP content." << endl;
	char buffer[contentLength];
	//content.resize(contentLength+10);
	
	while(content.size() < contentLength) {
		int ret = this->read(buffer, contentLength);
		//cerr << "Read " << ret << " bytes." << endl;
		//cerr << '\'' << string(buffer, contentLength) << '\'' << endl;
		if(ret <= 0) {
			break;
		}

		content.append(buffer, buffer + ret);
	}
}


void HttpSocket::sendContent(const std::string& content, int contentLength) {
	if(content.size() != contentLength) {
		// throw?
	}

	size_t n = this->write(content.c_str(), contentLength);
	//cerr << "HttpSocket: wrote " << n << " of " << contentLength << " bytes." << endl;
}


} // !namespace Net

} // !namespace Pt

#ifndef Pt_Net_HttpHeader_h
#define Pt_Net_HttpHeader_h

#include <Pt/Text/String.h>

#include <map>
#include <sstream>


namespace Pt {

namespace Net {

//! Http Header class
/*!
  The base class for a Http-Request or -Response.
*/
class PT_EXPORT HttpHeader {
	public:
		typedef std::map<String, String> FieldMap;
		typedef std::map<String, String>::iterator FieldIterator;
		typedef std::map<String, String>::const_iterator ConstFieldIterator;

	public:
		HttpHeader();
		~HttpHeader();

		inline void clear() {
			_fieldMap.clear();
		}
		inline ConstFieldIterator begin() const
			{ return _fieldMap.begin(); }

		inline ConstFieldIterator end() const
			{ return _fieldMap.end(); }

		inline FieldIterator begin()
			{ return _fieldMap.begin(); }

		inline FieldIterator end()
			{ return _fieldMap.end(); }

		void set(const String& field, const String& val) { 
			_fieldMap[field] = val;
		}

		template <typename T>
		void setField(const String& field, const T& val) {
			std::stringstream ss;
			ss << val;
			_fieldMap[field]= ss.str();
		}

		const String& field(const String& fieldName) const {
			static const String empty;

			FieldMap::const_iterator it = _fieldMap.find(fieldName);
			if( it != _fieldMap.end() )
				return it->second;

			return empty;
		}

		inline void setCacheControl(const String& val)
		{ setField("Cache-Control", val); }

		inline void setConnection(const String& val)
		{ setField("Connection", val); }

		inline const String& connection() const
		{ return field("Connection"); }

		inline void setContentLegth(const String& val)
		{ setField("Content-Length", val); }

		inline const String& contentLength() const
		{ if(_fieldMap.find("Content-Length")!=_fieldMap.end()) return field("Content-Length"); else return field("Content-length"); }

		inline void setDate(const String& val)
		{ setField("Date", val); }

		inline void setPragma(const String& val)
		{ setField("Pragma", val); }

		inline void setTrailer(const String& val)
		{ setField("Trailer", val); }

		inline void setTransferEncoding(const String& val)
		{ setField("Transfer-Encoding", val); }

		inline const String& transferEncoding() const
		{ return field("Transfer-Encoding"); }

		inline void setUpgrade(const String& val)
		{ setField("Upgrade", val); }

		inline void setVia(const String& val)
		{ setField("Via", val); }

		inline void setWarning(const String& val)
		{ setField("Warning", val); }

	private:
		FieldMap _fieldMap;
};

//! Http Request-Header
/*!
  A class used to store Http Request-Header informations
*/
class PT_EXPORT HttpRequestHeader: public HttpHeader {
	public:
		HttpRequestHeader();
		~HttpRequestHeader();
	
		inline void setAccept(const String& val)
		{ setField("Accept", val); }
	
		inline void setAcceptCharset(const String& val)
		{ setField("Accept-Charset", val); }
	
		inline void setAcceptEncoding(const String& val)
		{ setField("Accept-Encoding", val); }
	
		inline void setAcceptLanguage(const String& val)
		{ setField("Accept-Language", val); }
	
		inline void setAuthorization(const String& val)
		{ setField("Authorization", val); }
	
		inline void setExpect(const String& val)
		{ setField("Expect", val); }
	
		inline void setFrom(const String& val)
		{ setField("From", val); }
	
		inline void setHost(const String& val)
		{ setField("Host", val); }
	
		inline void setIfMatch(const String& val)
		{ setField("If-Match", val); }
	
		inline void setIfModifiedSince(const String& val)
		{ setField("If-Modified-Since", val); }
	
		inline void setIfNoneMatch(const String& val)
		{ setField("If-None-Match", val); }
	
		inline void setIfRange(const String& val)
		{ setField("If-Range", val); }
	
		inline void setIfUnmodifiedSince(const String& val)
		{ setField("If-Unmodified-Since", val); }
	
		inline void setMaxForwards(const String& val)
		{ setField("Max-Forwards", val); }
	
		inline void setProxyAuthorization(const String& val)
		{ setField("Proxy-Authorization", val); }
	
		inline void setRange(const String& val)
		{ setField("Range", val); }
	
		inline void setReferer(const String& val)
		{ setField("Referer", val); }
	
		inline void setTE(const String& val)
		{ setField("TE", val); }
	
		inline void setUserAgent(const String& val)
		{ setField("User-Agent", val); }
};

//! Http Response-Header
/*!
  A class used to store Http Response-Header informations
*/
class PT_EXPORT HttpResponseHeader: public HttpHeader {
	public:
		HttpResponseHeader();
		~HttpResponseHeader();
	
		inline void setAcceptRanges(const String& val)
		{ setField("Accept-Ranges", val); }
	
		inline void setAge(const String& val)
		{ setField("Age", val); }
	
		inline void setETag(const String& val)
		{ setField("ETag", val); }
	
		inline void setLocation(const String& val)
		{ setField("Location", val); }
	
		inline void setProxyAuthenticate(const String& val)
		{ setField("Proxy-Authenticate", val); }
	
		inline void setRetryAfter(const String& val)
		{ setField("Retry-After", val); }
	
		inline void setServer(const String& val)
		{ setField("Server", val); }
	
		inline void setVary(const String& val)
		{ setField("Vary", val); }
	
		inline void setWWWAuthenticate(const String& val)
		{ setField("WWW-Authenticate", val); }
	
		inline void setAllow(const String& val)
		{ setField("Allow", val); }
	
		inline void setContentEncoding(const String& val)
		{ setField("Content-Encoding", val); }
	
		inline void setContentLanguage(const String& val)
		{ setField("Content-Language", val); }
	
		inline void setContentLocation(const String& val)
		{ setField("Content-Location", val); }
	
		inline void setContentMD5(const String& val)
		{ setField("Content-MD5", val); }
	
		inline void setContentRange(const String& val)
		{ setField("Content-Range", val); }
	
		inline void setContentType(const String& val)
		{ setField("Content-Type", val); }
	
		inline void setExpires(const String& val)
		{ setField("Expires", val); }
	
		inline void setLastModified(const String& val)
		{ setField("Last-Modified", val); }
};


} // !namespace Net

} // !namespace Pt

#endif

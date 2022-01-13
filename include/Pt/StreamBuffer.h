/*
 * Copyright (C) 2012 Marc Boris Duerner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef Pt_StreamBuffer_h
#define Pt_StreamBuffer_h

#include <Pt/Api.h>
#include <Pt/String.h>
#include <streambuf>
#include <cstddef>

#if(0)
namespace std {

template <>
class basic_streambuf<Pt::Char, std::char_traits<Pt::Char> >
{
    public:
        typedef Pt::Char char_type;
        typedef std::char_traits<Pt::Char> traits_type;
        typedef typename std::char_traits<Pt::Char>::int_type int_type;
        typedef typename std::char_traits<Pt::Char>::pos_type pos_type;
        typedef typename std::char_traits<Pt::Char>::off_type off_type;

    public:
        /** @brief Destructor.
        */
        virtual ~basic_streambuf()
        {
          delete _Plocale;
        }

        pos_type pubseekoff(off_type _Off, ios_base::seekdir _Way,
		                        ios_base::openmode _Mode = ios_base::in | ios_base::out)
		    {
		      return (seekoff(_Off, _Way, _Mode));
		    }

  	    pos_type pubseekpos(pos_type _Pos, ios_base::openmode _Mode = ios_base::in | ios_base::out)
		    {
		      return (seekpos(_Pos, _Mode));
		    }

	      basic_streambuf* pubsetbuf(Pt::Char* _Buffer, streamsize _Count)
		    {
		      return (setbuf(_Buffer, _Count));
		    }

	      locale pubimbue(const locale& /*_Newlocale*/)
		    {
		      //locale _Oldlocale = *_Plocale;
		      //imbue(_Newlocale);
		      //*_Plocale = _Newlocale;
		      //return (_Oldlocale);
          return 
		    }

	      locale getloc() const
		    {
  		    return (*_Plocale);
		    }

	      streamsize in_avail()
	      {
		      streamsize _Res = _Gnavail();
		      return (0 < _Res ? _Res : showmanyc());
	      }

	      int pubsync()
	      {
		      return (sync());
	      }

	      int_type sbumpc()
	      {
		      return (0 < _Gnavail()
			      ? traits_type::to_int_type(*_Gninc()) : uflow());
	      }

	      int_type sgetc()
	      {
		      return (0 < _Gnavail()
			      ? traits_type::to_int_type(*gptr()) : underflow());
	      }

	      streamsize sgetn(char_type*_Ptr, streamsize _Count)
	      {
		      return (xsgetn(_Ptr, _Count));
	      }

	      int_type snextc()
	      {
		      return (1 < _Gnavail()
			      ? traits_type::to_int_type(*_Gnpreinc())
			      : traits_type::eq_int_type(traits_type::eof(), sbumpc())
				      ? traits_type::eof() : sgetc());
	      }

      	int_type sputc(char_type _Ch)
		    {
		      return (0 < _Pnavail()
			      ? traits_type::to_int_type(*_Pninc() = _Ch)
			      : overflow(traits_type::to_int_type(_Ch)));
		    }

        streamsize sputn(const char_type* _Ptr, streamsize _Count)
		    {
  		    return (xsputn(_Ptr, _Count));
		    }

    public:
      	virtual int sync()
	    	{
		      return (0);
		    }


    protected:
        /** @brief Default Constructor.
        */
        basic_streambuf()
        : _Plocale(0)
        {
          _Init();
        }

        /** @brief Returns the number of characters buffered for output.
        */
        virtual std::streamsize showfull()
        { return 0; }

	      char_type* eback() const
		    {	// return beginning of read buffer
		      return (*_IGfirst);
		    }

	      char_type* gptr() const
		    {	// return current position in read buffer
		      return (*_IGnext);
		    }

	      char_type* pbase() const
		    {	// return beginning of write buffer
		      return (*_IPfirst);
		    }

	      char_type* pptr() const
		    {	// return current position in write buffer
		      return (*_IPnext);
		    }

	      char_type* egptr() const
		    {	// return end of read buffer
		      return (*_IGnext + *_IGcount);
		    }

	      void gbump(int _Off)
		    {	// alter current position in read buffer by _Off
		      *_IGcount -= _Off;
		      *_IGnext += _Off;
		    }

	      void setg(char_type*_First, char_type*_Next, char_type*_Last)
		    {	// set pointers for read buffer
		      *_IGfirst = _First;
		      *_IGnext = _Next;
		      *_IGcount = (int)(_Last - _Next);
		    }

	      char_type* epptr() const
		    {	// return end of write buffer
		      return (*_IPnext + *_IPcount);
		    }

	      char_type* _Gndec()
		    {	// decrement current position in read buffer
		      ++*_IGcount;
		      return (--*_IGnext);
		    }

	      char_type* _Gninc()
		    {	// increment current position in read buffer
	  	    --*_IGcount;
  		    return ((*_IGnext)++);
		    }

	      char_type* _Gnpreinc()
		    {	// preincrement current position in read buffer
		      --*_IGcount;
		      return (++(*_IGnext));
		    }

	      std::streamsize _Gnavail() const
		    {	// count number of available elements in read buffer
  		    return (*_IGnext != 0 ? *_IGcount : 0);
		    }

	      void pbump(int _Off)
		    {	// alter current position in write buffer by _Off
		      *_IPcount -= _Off;
		      *_IPnext += _Off;
		    }

	      void setp(char_type*_First, char_type*_Last)
		    {	// set pointers for write buffer
		      *_IPfirst = _First;
		      *_IPnext = _First;
		      *_IPcount = (int)(_Last - _First);
		    }

	      void setp(char_type*_First, char_type*_Next, char_type*_Last)
		    {	// set pointers for write buffer, extended version
		      *_IPfirst = _First;
		      *_IPnext = _Next;
		      *_IPcount = (int)(_Last - _Next);
		    }

	      char_type* _Pninc()
		    {	// increment current position in write buffer
		      --*_IPcount;
		      return ((*_IPnext)++);
		    }

	      std::streamsize _Pnavail() const
		    {	// count number of available positions in write buffer
		      return (*_IPnext != 0 ? *_IPcount : 0);
		    }

	      void _Init()
        {	// initialize buffer parameters for no buffers
		      _IGfirst = &_Gfirst;
		      _IPfirst = &_Pfirst;
		      _IGnext = &_Gnext;
		      _IPnext = &_Pnext;
		      _IGcount = &_Gcount;
		      _IPcount = &_Pcount;
		      setp(0, 0);
		      setg(0, 0, 0);
	      }

    protected:
	      virtual pos_type seekoff(off_type, ios_base::seekdir,
		                           ios_base::openmode = ios_base::in | ios_base::out)
		    {
		      return streampos(-1);
		    }

	      virtual pos_type seekpos(pos_type, ios_base::openmode = ios_base::in | ios_base::out)
		    {
		      return streampos(-1);
		    }

        virtual basic_streambuf* setbuf(Pt::Char*, streamsize)
		    {
		      return (this);
        }

	      virtual int_type overflow(int_type = traits_type::eof())
	      {	// put a character to stream (always fail)
		      return (traits_type::eof());
	      }

	      virtual int_type pbackfail(int_type = traits_type::eof())
	      {	// put a character back to stream (always fail)
		      return (traits_type::eof());
	      }

	      virtual std::streamsize showmanyc()
	      {	// return count of input characters
		      return (0);
	      }

	      virtual int_type underflow()
	      {	// get a character from stream, but don't point past it
		      return (traits_type::eof());
	      }

	      virtual int_type uflow()
	      {	// get a character from stream, point past it
		      return (traits_type::eq_int_type(traits_type::eof(), underflow())
			      ? traits_type::eof() : traits_type::to_int_type(*_Gninc()));
	      }

        virtual streamsize xsgetn(char_type* _Ptr, streamsize _Count)
		    {
		      int_type _Meta;
		      streamsize _Size, _Copied;

		      for (_Copied = 0; 0 < _Count; )
          {
			      if (0 < (_Size = _Gnavail()))
				    {
				      if (_Count < _Size)
					      _Size = _Count;
				      traits_type::copy(_Ptr, gptr(), (size_t)_Size);
				      _Ptr += _Size;
				      _Copied += _Size;
				      _Count -= _Size;
				      gbump((int)_Size);
				    }
			      else if (traits_type::eq_int_type(traits_type::eof(), _Meta = uflow()))
				      break;	// end of file, quit
			      else
				    {	// get a single character
				      *_Ptr++ = traits_type::to_char_type(_Meta);
				      ++_Copied;
				      --_Count;
				    }
          }

		      return (_Copied);
		    }

  	    virtual streamsize xsputn(const char_type* _Ptr, streamsize _Count)
		    {
		      streamsize _Size = 0;
          streamsize _Copied = 0;

		      for (_Copied = 0; 0 < _Count; )
			      if (0 < (_Size = _Pnavail()))
				      {	// copy to write buffer
				      if (_Count < _Size)
					      _Size = _Count;
				      traits_type::copy(pptr(), _Ptr, (size_t)_Size);
				      _Ptr += _Size;
				      _Copied += _Size;
				      _Count -= _Size;
				      pbump((int)_Size);
				      }
			      else if (traits_type::eq_int_type(traits_type::eof(),
				      overflow(traits_type::to_int_type(*_Ptr))))
				      break;	// single character put failed, quit
			      else
				      {	// count character successfully put
				      ++_Ptr;
				      ++_Copied;
				      --_Count;
				      }

		      return (_Copied);
		    }

  private:
	  char_type*   _Gfirst;	  ///< beginning of read buffer
	  char_type*   _Pfirst;	  ///< beginning of write buffer
	  char_type**  _IGfirst;  ///< pointer to beginning of read buffer
	  char_type**  _IPfirst;  ///< pointer to beginning of write buffer
	  char_type*   _Gnext;    ///< current position in read buffer
	  char_type*   _Pnext;    ///< current position in write buffer
	  char_type**  _IGnext;   ///< pointer to current position in read buffer
	  char_type**  _IPnext;   ///< pointer to current position in write buffer

	  int   _Gcount;	  ///< length of read buffer
	  int   _Pcount;	  ///< length of write buffer
	  int*  _IGcount;	  ///< pointer to length of read buffer
	  int*  _IPcount;	  ///< pointer to length of write buffer

  protected:
	  locale* _Plocale;	///< pointer to imbued locale object

};

} // namespace
#endif

#if defined(_MSC_VER) && defined(_WIN32_WCE) 
    // alternatively compile with /FORCE:multiple
    template class PT_EXPORT std::basic_streambuf<char>; 
#endif

#if defined(_MSC_VER)
	template class PT_EXPORT std::basic_streambuf<Pt::Char>;
#endif


namespace Pt {

/** @brief Buffer for input and output streams.
*/
template <typename CharT, typename TraitsT = std::char_traits<CharT> >
class BasicStreamBuffer : public std::basic_streambuf<CharT, TraitsT>
{
    public:
        typedef CharT char_type;
        typedef TraitsT traits_type;
        typedef typename TraitsT::int_type int_type;
        typedef typename TraitsT::pos_type pos_type;
        typedef typename TraitsT::off_type off_type;

    public:
        /** @brief Destructor.
        */
        ~BasicStreamBuffer()
        { }

        /** @brief Peek data in stream.
        */
        std::streamsize speekn(CharT* buffer, std::streamsize size)
        {
            if(size <= 0)
                return 0;

            int_type next = 0;
            if( ! this->gptr() || this->gptr() == this->egptr() )
            {
                next = this->underflow();
                
                if( traits_type::eof() == next)
                    return 0;
            }

            std::size_t avail = this->gptr() ? this->egptr() - this->gptr() : 0;

            // unbuffered streambufs
            if(avail == 0)
            {
                *buffer = traits_type::to_char_type(next);
                return 1;
            }

            std::size_t n = static_cast<std::size_t>(size);
            if(avail < n) 
                n = avail;

            traits_type::copy(buffer, this->gptr(), n);
            return size;
        }

        /** @brief Returns the number of characters buffered for output.
        */
        std::streamsize out_avail()
        {
            if( this->pptr() )
                return this->pptr() - this->pbase();

            return showfull();
        }

    protected:
        /** @brief Default Constructor.
        */
        BasicStreamBuffer()
        { }

        /** @brief Returns the number of characters buffered for output.
        */
        virtual std::streamsize showfull()
        { return 0; }
};

} // namespace Pt

#endif

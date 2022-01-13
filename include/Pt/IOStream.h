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

#ifndef Pt_IOStream_h
#define Pt_IOStream_h

#include <Pt/Api.h>
#include <Pt/StreamBuffer.h>
#include <iostream>
#include <algorithm>

#if(0)

namespace std {

template<>
class basic_ios< Pt::Char, std::char_traits<Pt::Char> >
{
    public:
        typedef basic_ios< Pt::Char, std::char_traits<Pt::Char> > _Myt;
        typedef basic_ostream<Pt::Char, std::char_traits<Pt::Char> > _Myos;
        typedef Pt::Char char_type;
        typedef std::char_traits<Pt::Char> traits_type;
        typedef typename traits_type::int_type int_type;
        typedef typename traits_type::pos_type pos_type;
        typedef typename traits_type::off_type off_type;
        typedef std::basic_streambuf<char_type> buffer_type;
        
        // For compatibility with non specialized class!
        typedef buffer_type _Mysb;

        typedef ios_base::iostate iostate;
        typedef ios_base::openmode openmode;
        typedef ios_base::fmtflags fmtflags;
        typedef ios_base::seekdir seekdir;

    public:
      static constexpr ios_base::_Fmtflags skipws = ios_base::skipws;
	    static constexpr ios_base::_Fmtflags unitbuf = ios_base::unitbuf;
	    static constexpr ios_base::_Fmtflags uppercase = ios_base::uppercase;
	    static constexpr ios_base::_Fmtflags showbase = ios_base::showbase;
	    static constexpr ios_base::_Fmtflags showpoint = ios_base::showpoint;
	    static constexpr ios_base::_Fmtflags showpos = ios_base::showpos;
	    static constexpr ios_base::_Fmtflags left = ios_base::left;
	    static constexpr ios_base::_Fmtflags right = ios_base::right;
	    static constexpr ios_base::_Fmtflags internal = ios_base::internal;
	    static constexpr ios_base::_Fmtflags dec = ios_base::dec;
	    static constexpr ios_base::_Fmtflags oct = ios_base::oct;
	    static constexpr ios_base::_Fmtflags hex = ios_base::hex;
	    static constexpr ios_base::_Fmtflags scientific = ios_base::scientific;
	    static constexpr ios_base::_Fmtflags fixed = ios_base::fixed;
	    static constexpr ios_base::_Fmtflags hexfloat = ios_base::hexfloat;
	    static constexpr ios_base::_Fmtflags boolalpha = ios_base::boolalpha;
	    static constexpr ios_base::_Fmtflags _Stdio = ios_base::_Stdio;
	    static constexpr ios_base::_Fmtflags adjustfield = ios_base::adjustfield;
	    static constexpr ios_base::_Fmtflags basefield = ios_base::basefield;
	    static constexpr ios_base::_Fmtflags floatfield = ios_base::floatfield;

      static constexpr ios_base::_Iostate goodbit = ios_base::goodbit;
	    static constexpr ios_base::_Iostate eofbit = ios_base::eofbit;
	    static constexpr ios_base::_Iostate failbit = ios_base::failbit;
	    static constexpr ios_base::_Iostate badbit = ios_base::badbit;


    public:
        basic_ios()
		    {
		    }

	      explicit basic_ios(buffer_type* sb)
		    {
		        init(sb);
		    }

	      virtual ~basic_ios() throw()
		    {
		    }

	      _Myos*  tie() const
		    {
		      return (_Tiestr);
		    }

	      _Myos*  tie(_Myos *_Newtie)
		    {
		      _Myos *_Oldtie = _Tiestr;
		      _Tiestr = _Newtie;
		       return (_Oldtie);
		    }

        buffer_type* rdbuf()
        {
            return _buffer;
        }

        buffer_type* rdbuf(buffer_type* sb)
        { 
            buffer_type* buf = _buffer;
            _buffer = sb; 
            return buf;
        }

	      std::ios_base::iostate rdstate() const
		    {
		        return _Mystate;
		    }

       	char_type fill() const
		    {
  		    return (_Fillch);
		    }

      	char_type fill(char_type _Newfill)
		    {
		      char_type _Oldfill = _Fillch;
		      _Fillch = _Newfill;
		      return (_Oldfill);
		    }

       	char narrow(Pt::Char _Ch, char _Dflt = '\0') const
		    {
		      std::ctype<Pt::Char> _ctype;
		      return _ctype.narrow(_Ch, _Dflt);
		    }

	      Pt::Char widen(char _Byte) const
		    {
		      std::ctype<Pt::Char> _ctype;
		      return _ctype.widen(_Byte);
		    }

        void move(_Myt& _Right)
		    {
		      if (this != &_Right)
			    {
			      _buffer = 0;
            _Tiestr = 0;
			      this->swap(_Right);
			    }
		    }

	      void move(_Myt&& _Right)
		    {
		      if (this != &_Right)
			    {
			      _buffer = 0;
            _Tiestr = 0;
			      this->swap(_Right);
			    }
		    }

        void swap(_Myt& _Right)
		    {
		      if (this != &_Right)
		      {
			      std::swap(_Mystate, _Right._Mystate);
			      std::swap(_Except, _Right._Except);
			      std::swap(_Fmtfl, _Right._Fmtfl);
			      std::swap(_Prec, _Right._Prec);
			      std::swap(_Wide, _Right._Wide);
		      }
  		    std::swap(_Fillch, _Right._Fillch);
          std::swap(_Tiestr, _Right._Tiestr);
		    }

	      void setstate(std::ios_base::iostate _State, bool _Reraise = false)
		    {
		        clear(rdstate() | _State, _Reraise);
		    }

	      void clear(std::ios_base::iostate _State = std::ios_base::goodbit, 
                   bool _Reraise = false)
		    {
            _State = _buffer == 0 ? (_State | std::ios_base::badbit) 
                                  : _State;

		        _Mystate = _State;
		        
            const int _Filtered = _State & _Except;
		        if(_Filtered)
			      {
			          if(_Reraise)
				        {
				            throw;
				        }

			          const char* _Msg;
			          if (_Filtered & std::ios_base::badbit)
				        {
				            _Msg = "ios_base::badbit set";
				        }
			          else if(_Filtered & std::ios_base::failbit)
				        {
				            _Msg = "ios_base::failbit set";
				        }
			          else
				        {
				            _Msg = "ios_base::eofbit set";
				        }

			          throw std::ios_base::failure(_Msg);
			      }
		    }

	      bool good() const
		    {
		        return rdstate() == std::ios_base::goodbit;
		    }

	      bool eof() const
		    {
		        return rdstate() & std::ios_base::eofbit;
		    }

	      bool fail() const
		    {
		        return rdstate() & (std::ios_base::badbit | std::ios_base::failbit);
		    }

	      bool bad() const
		    {
		        return rdstate() & std::ios_base::badbit;
		    }

	      explicit operator bool() const
		    {
		        return ! fail();
		    }

	      bool operator!() const
		    {
		        return fail();
		    }

	      std::ios_base::iostate exceptions() const
		    {
		        return _Except;
		    }

	      void exceptions(std::ios_base::iostate e)
		    {
		        _Except = e;
		        clear( rdstate() );
		    }


	      ios_base::fmtflags flags() const
		    {
		      return (_Fmtfl);
		    }

	      ios_base::fmtflags flags(ios_base::fmtflags _Newfmtflags)
		    {
		      ios_base::fmtflags _Oldfmtflags = _Fmtfl;
		      _Fmtfl = (ios_base::fmtflags)((int)_Newfmtflags & (int)ios_base::_Fmtmask);
		      return (_Oldfmtflags);
		    }

   	    ios_base::fmtflags setf(ios_base::fmtflags _Newfmtflags)
		    {	// merge in format flags argument
  		    ios_base::fmtflags _Oldfmtflags = _Fmtfl;
	  	    _Fmtfl = (ios_base::fmtflags)((int)_Fmtfl
		        	    | ((int)_Newfmtflags & (int)ios_base::_Fmtmask));
		      return (_Oldfmtflags);
		    }

	      ios_base::fmtflags setf(ios_base::fmtflags _Newfmtflags, ios_base::fmtflags _Mask)
		    {	// merge in format flags argument under mask argument
		      ios_base::fmtflags _Oldfmtflags = _Fmtfl;
		      _Fmtfl = (ios_base::fmtflags)(((int)_Fmtfl & (int)~_Mask)
			            | ((int)_Newfmtflags & (int)_Mask & (int)ios_base::_Fmtmask));
		      return (_Oldfmtflags);
		    }

	      void unsetf(ios_base::fmtflags _Mask)
		    {
		      _Fmtfl = (ios_base::fmtflags)((int)_Fmtfl & (int)~_Mask);
		    }

	      streamsize precision() const
		    {
		      return (_Prec);
		    }

	      streamsize precision(streamsize _Newprecision)
		    {
		      streamsize _Oldprecision = _Prec;
		      _Prec = _Newprecision;
		      return (_Oldprecision);
		    }

	      streamsize width() const
		    {
		      return (_Wide);
		    }

	      streamsize width(streamsize _Newwidth)
		    {
		      streamsize _Oldwidth = _Wide;
		      _Wide = _Newwidth;
		      return (_Oldwidth);
        }
		
    protected:
	      void init(buffer_type* buffer = 0, bool _Isstd = false)
		    {	// initialize with stream buffer pointer
		      _Init();	// initialize ios_base
		      _buffer = buffer;
          _Tiestr = 0;
		      _Fillch = Pt::Char(' ');

		      if ( ! _buffer )
			      setstate(badbit);


          // Microsoft specific!
		      //if (_Isstd)
			     // _Addstd(this);  // special handling for standard streams
  		  }

    private:
    	  void _Init()
		    {	// initialize a new ios_base
		      _Except = goodbit;
		      _Fmtfl = (std::ios_base::fmtflags)(skipws | dec);
		      _Prec = 6;
		      _Wide = 0;
		      clear(goodbit);
	      }

        basic_ios(const _Myt&) = delete;
	      _Myt& operator=(const _Myt&) = delete;


    private:
        buffer_type*            _buffer;
        _Myos*                  _Tiestr;
        char_type               _Fillch;
        std::ios_base::iostate  _Mystate;
        std::ios_base::iostate  _Except;
        std::ios_base::fmtflags _Fmtfl;
	      std::streamsize         _Prec;
	      std::streamsize         _Wide;

};

/*
    MANIPULATORS

*/
inline basic_ios<Pt::Char>& boolalpha(basic_ios<Pt::Char>& _Iosbase)
{
  _Iosbase.setf(ios_base::boolalpha);
  return (_Iosbase);
}

inline basic_ios<Pt::Char>& dec(basic_ios<Pt::Char>& _Iosbase)
{
  _Iosbase.setf(ios_base::dec, ios_base::basefield);
  return (_Iosbase);
}

inline basic_ios<Pt::Char>& defaultfloat(basic_ios<Pt::Char>& _Iosbase)
{
  _Iosbase.unsetf(ios_base::floatfield);
  return (_Iosbase);
}

inline basic_ios<Pt::Char>& fixed(basic_ios<Pt::Char>& _Iosbase)
{
  _Iosbase.setf(ios_base::fixed, ios_base::floatfield);
  return (_Iosbase);
}

inline basic_ios<Pt::Char>& hex(basic_ios<Pt::Char>& _Iosbase)
{
  _Iosbase.setf(ios_base::hex, ios_base::basefield);
  return (_Iosbase);
}

inline basic_ios<Pt::Char>& hexfloat(basic_ios<Pt::Char>& _Iosbase)
{
  _Iosbase.setf(ios_base::hexfloat, ios_base::floatfield);
  return (_Iosbase);
}

/** @brief Output stream.
*/
template<>
class basic_ostream<Pt::Char, std::char_traits<Pt::Char> >
: virtual public basic_ios<Pt::Char, std::char_traits<Pt::Char> >
{
  public:
    typedef basic_ostream<Pt::Char, std::char_traits<Pt::Char> > _Myt;
    typedef basic_streambuf<Pt::Char> buffer_type;

	  class sentry
    {
	    public:
		    explicit sentry(_Myt& _Ostr)
        : _Myostr(_Ostr)
			  {
			    if (_Ostr.good() && _Ostr.tie() != 0 && _Ostr.tie() != &_Ostr)
				    _Ostr.tie()->flush();
			    _Ok = _Ostr.good();	// store test only after flushing tie
			  }

		    ~sentry() throw()
			  {
          if( _Myostr.flags() & ios_base::unitbuf && ! std::uncaught_exception() )
          {
            std::basic_streambuf<Pt::Char>* buf = _Myostr.rdbuf();
			      if( buf && buf->pubsync() == -1)
				      _Myostr.setstate(ios_base::badbit);
          }
			  }

		    explicit operator bool() const
			  {
			    return (_Ok);
			  }

		    sentry(const sentry&) = delete;
		    sentry& operator=(const sentry&) = delete;

	    private:
        _Myt& _Myostr;
		    bool _Ok;
		};

  public:
    explicit basic_ostream(buffer_type* buffer, bool isstd = false)
    {
      init(buffer, isstd);
    }

    basic_ostream(_Uninitialized /*, bool _Addit = true*/)
    {
      // Microsoft specific!
      // @see C:\src\microsoft\STL.git\trunk\stl\src\ios.cpp
      // if (_Addit)
			  //this->_Addstd(this);	// suppress for basic_iostream
		}

    virtual ~basic_ostream() throw()
    {
    }

  public:
  	_Myt& put(char_type ch)
		{
      ios_base::iostate state = ios_base::goodbit;
      
      const sentry sen(*this);
      if (sen)
	    {
	      try
	      {
	        const int_type c = this->rdbuf()->sputc(ch);
	        if (traits_type::eq_int_type(c, traits_type::eof()))
		        state |= ios_base::badbit;
	      }
	      catch(...)
	      {
          setstate(ios_base::badbit, true);
        }
	    }
      else
      {
        state |= ios_base::badbit;
      }

      if( state != ios_base::goodbit )
        setstate(state);

      return *this;
		}

  	_Myt& write(const char_type* _Str, streamsize _Count)
		{
		  ios_base::iostate state = ios_base::goodbit;
		  
      const sentry sen(*this);
		  if (sen)
			{
			  try
        {
          if( rdbuf()->sputn(_Str, _Count) != _Count )
            state |= ios_base::badbit;
        }
        catch(...)
        {
          setstate(ios_base::badbit, true);
        }
			}
      else
      {
        state |= ios_base::badbit;
      }

      if( state != ios_base::goodbit )
		    setstate(state);

		  return (*this);
		}

	  _Myt& flush()
		{
      std::basic_streambuf<Pt::Char>* buf = basic_ios::rdbuf();
		  if( buf )
			{
			  const sentry _Ok(*this);

			  if( _Ok && buf->pubsync() == -1 )
				  basic_ios::setstate(ios_base::badbit);
			}
		  
      return (*this);
		}


  public:

	  _Myt& operator<<(_Myt& (__cdecl *_Pfn)(_Myt&))
		{
		  return ((*_Pfn)(*this));
		}

	  _Myt& operator<<(basic_ios& (__cdecl *_Pfn)(basic_ios&))
		{
  		(*_Pfn)(*(basic_ios*)this);
	  	return (*this);
		}

	 // _Myt& operator<<(ios_base& (__cdecl *_Pfn)(ios_base&))
		//{
  //		(*_Pfn)(*(ios_base *)this);
		//  return (*this);
		//}

	  _Myt& operator<<(bool _Val)
		{
		  ios_base::iostate _State = ios_base::goodbit;
		  const sentry _Ok(*this);

		  if (_Ok)
		  {
			  try
        {
          std::num_put<Pt::Char> np;
          ostreambuf_iterator<Pt::Char> it(rdbuf());
			    if (np.put(it, *this, fill(), _Val).failed())
				    _State |= ios_base::badbit;
        }
        catch(...)
        {
          setstate(ios_base::badbit, true);
        }
			}

		  setstate(_State);
		  return (*this);
		}

	  _Myt& operator<<(short _Val)
		{
		  ios_base::iostate _State = ios_base::goodbit;
		  const sentry _Ok(*this);

		  if (_Ok)
			{
        try
        {
          std::num_put<Pt::Char> np;
          ostreambuf_iterator<Pt::Char> it(rdbuf());
          ios_base::fmtflags _Bfl = flags() & ios_base::basefield;
			    long _Tmp = (_Bfl == ios_base::oct || _Bfl == ios_base::hex)
				              ? (long)(unsigned short)_Val : (long)_Val;

			    if (np.put(it, *this, fill(), _Tmp).failed())
				    _State |= ios_base::badbit;
        }
        catch(...)
        {
          setstate(ios_base::badbit, true);
        }
		  }

		  setstate(_State);
		  return (*this);
		}

	  _Myt&operator<<(unsigned short _Val)
		{
  		ios_base::iostate _State = ios_base::goodbit;
	  	const sentry _Ok(*this);

		  if (_Ok)
			{
        try
        {
          std::num_put<Pt::Char> np;
          ostreambuf_iterator<Pt::Char> it(rdbuf());

  			  if( np.put(it, *this, fill(), (unsigned long)_Val).failed() )
				    _State |= ios_base::badbit;
        }
        catch(...)
        {
          setstate(ios_base::badbit, true);
        }
			}

		  setstate(_State);
		  return (*this);
		}


  	_Myt& operator<<(int _Val)
		{
		  ios_base::iostate _State = ios_base::goodbit;
		  const sentry _Ok(*this);
		  if (_Ok)
			{
			  try
        {
			    ios_base::fmtflags _Bfl = flags() & ios_base::basefield;
			    long _Tmp = (_Bfl == ios_base::oct || _Bfl == ios_base::hex)
				            ? (long)(unsigned int)_Val : (long)_Val;

          std::num_put<Pt::Char> np;
          ostreambuf_iterator<Pt::Char> it(rdbuf());

			    if (np.put(it, *this, fill(), _Tmp).failed())
				    _State |= ios_base::badbit;
        }
        catch(...)
        {
          setstate(ios_base::badbit, true);
        }
			}

		  setstate(_State);
		  return (*this);
		}

	  _Myt& operator<<(unsigned int _Val)
		{
		  ios_base::iostate _State = ios_base::goodbit;
		  const sentry _Ok(*this);

  		if (_Ok)
			{
			  try
        {
          std::num_put<Pt::Char> np;
          ostreambuf_iterator<Pt::Char> it(rdbuf());

			    if (np.put(it, *this, fill(), (unsigned long)_Val).failed())
				    _State |= ios_base::badbit;
        }
        catch(...)
        {
          setstate(ios_base::badbit, true);
        }
			}

		  setstate(_State);
		  return (*this);
		}

	  _Myt& operator<<(long _Val)
		{
		  ios_base::iostate _State = ios_base::goodbit;
		  const sentry _Ok(*this);

  		if (_Ok)
			{
			  try
        {
          std::num_put<Pt::Char> np;
          ostreambuf_iterator<Pt::Char> it(rdbuf());

			    if (np.put(it, *this, fill(), _Val).failed())
				    _State |= ios_base::badbit;
        }
        catch(...)
        {
          setstate(ios_base::badbit, true);
        }
			}

		  setstate(_State);
		  return (*this);
		}

	  _Myt& operator<<(unsigned long _Val)
		{
		  ios_base::iostate _State = ios_base::goodbit;
		  const sentry _Ok(*this);

		  if (_Ok)
			{
			  try
        {
          std::num_put<Pt::Char> np;
          ostreambuf_iterator<Pt::Char> it(rdbuf());

			    if (np.put(it, *this, fill(), _Val).failed())
				    _State |= ios_base::badbit;
        }
        catch(...)
        {
          setstate(ios_base::badbit, true);
        }
			}

		  setstate(_State);
		  return (*this);
		}

	  _Myt& operator<<(long long _Val)
		{
		  ios_base::iostate _State = ios_base::goodbit;
		  const sentry _Ok(*this);

		  if (_Ok)
			{
			  try
        {
          std::num_put<Pt::Char> np;
          ostreambuf_iterator<Pt::Char> it(rdbuf());

			    if (np.put(it, *this, fill(), _Val).failed())
				    _State |= ios_base::badbit;
        }
        catch(...)
        {
          setstate(ios_base::badbit, true);
        }
			}

		  setstate(_State);
		  return (*this);
		}

	  _Myt& operator<<(unsigned long long _Val)
		{
		  ios_base::iostate _State = ios_base::goodbit;
		  const sentry _Ok(*this);

		  if (_Ok)
			{
			  try
        {
          std::num_put<Pt::Char> np;
          ostreambuf_iterator<Pt::Char> it(rdbuf());

			    if (np.put(it, *this, fill(), _Val).failed())
				    _State |= ios_base::badbit;
        }
        catch(...)
        {
          setstate(ios_base::badbit, true);
        }
      }

		  setstate(_State);
		  return (*this);
		}

   	_Myt& operator<<(float _Val)
		{
		  ios_base::iostate _State = ios_base::goodbit;
		  const sentry _Ok(*this);

		  if (_Ok)
			{
			  try
        {
          std::num_put<Pt::Char> np;
          ostreambuf_iterator<Pt::Char> it(rdbuf());

			    if(np.put(it, *this, fill(), _Val).failed())
				    _State |= ios_base::badbit;
        }
        catch(...)
        {
			    setstate(ios_base::badbit, true);
        }
			}

		  setstate(_State);
		  return (*this);
		}

	  _Myt& operator<<(double _Val)
		{
		  ios_base::iostate _State = ios_base::goodbit;
		  const sentry _Ok(*this);

		  if (_Ok)
			{
			  try
        {
          std::num_put<Pt::Char> np;
          ostreambuf_iterator<Pt::Char> it(rdbuf());

			    if (np.put(it, *this, fill(), _Val).failed())
				    _State |= ios_base::badbit;
        }
        catch(...)
        {
          setstate(ios_base::badbit, true);
        }
			}

		  setstate(_State);
		  return (*this);
		}

	  _Myt& operator<<(long double _Val)
		{
		  ios_base::iostate _State = ios_base::goodbit;
		  const sentry _Ok(*this);

		  if (_Ok)
			{
			  try
        {
          std::num_put<Pt::Char> np;
          ostreambuf_iterator<Pt::Char> it(rdbuf());

			    if (np.put(it, *this, fill(), _Val).failed())
				    _State |= ios_base::badbit;
        }
        catch(...)
        {
          setstate(ios_base::badbit, true);
        }
			}

		  setstate(_State);
		  return (*this);
		}

	  _Myt& operator<<(const void *_Val)
		{
		  ios_base::iostate _State = ios_base::goodbit;
		  const sentry _Ok(*this);

		  if (_Ok)
			{
			  try
        {
          std::num_put<Pt::Char> np;
          ostreambuf_iterator<Pt::Char> it(rdbuf());

			    if (np.put(it, *this, fill(), _Val).failed())
				    _State |= ios_base::badbit;
        }
        catch(...)
        {
          setstate(ios_base::badbit, true);
        }
			}

		  setstate(_State);
		  return (*this);
		}

	  _Myt& __CLR_OR_THIS_CALL operator<<(_Mysb *_Strbuf)
		{      
		  ios_base::iostate _State = ios_base::goodbit;
		  bool _Copied = false;
		  const sentry _Ok(*this);

		  if (_Ok && _Strbuf != 0)
      {
			  for (int_type _Meta = traits_type::eof(); ; _Copied = true)
			  {
				  try
          {
				    _Meta = traits_type::eq_int_type(traits_type::eof(), _Meta)
					        ? _Strbuf->sgetc() : _Strbuf->snextc();
          }
				  catch(...)
          {
					  setstate(ios_base::failbit);
					  throw;
          }

				  if (traits_type::eq_int_type(traits_type::eof(), _Meta))
					  break;

          try
          {
					  if (traits_type::eq_int_type(traits_type::eof(), rdbuf()->sputc(
							  traits_type::to_char_type(_Meta))))
					  {
						  _State |= ios_base::badbit;
						  break;
					  }
          }
          catch(...)
          {
            setstate(ios_base::badbit, true);
				  }
			  }
      }

		  this->width(0);
		  setstate(_Strbuf == 0 ? ios_base::badbit
			          : !_Copied ? _State | ios_base::failbit : _State);
		  return *this;
		}

  protected:
	  basic_ostream(_Myt&& _Right)
		{
		  basic_ios::init();
		  basic_ios::move(std::move(_Right));
		}

	  _Myt& operator=(_Myt&& _Right)
		{
		  this->swap(_Right);
		  return (*this);
		}

	  void swap(_Myt& _Right)
		{
		  if (this != &_Right)
			  basic_ios::swap(_Right);
		}

  public:
	  basic_ostream(const _Myt&) = delete;
	  _Myt& operator=(const _Myt&) = delete;
};
//
//
//inline
//basic_ostream<Pt::Char>& operator<<(basic_ostream<Pt::Char>& _Ostr, char _Ch)
//{
//  typedef basic_ostream<Pt::Char>::traits_type traits_type;
//
//  ios_base::iostate _State = ios_base::goodbit;
//  const basic_ostream<Pt::Char>::sentry _Ok(_Ostr);
//
//  if (_Ok)
//	{	// state okay, insert
//    std::ctype<Pt::Char> ct;
//	  streamsize _Pad = _Ostr.width() <= 1 ? 0 : _Ostr.width() - 1;
//
//	  try
//    {
//	    if ((_Ostr.flags() & ios_base::adjustfield) != ios_base::left)
//		    for (; _State == ios_base::goodbit && 0 < _Pad;
//			    --_Pad)	// pad on left
//			    if (traits_type::eq_int_type(traits_type::eof(),
//				    _Ostr.rdbuf()->sputc(_Ostr.fill())))
//				    _State |= ios_base::badbit;
//
//	    if (_State == ios_base::goodbit
//		    && traits_type::eq_int_type(traits_type::eof(),
//			    _Ostr.rdbuf()->sputc(ct.widen(_Ch))))
//		    _State |= ios_base::badbit;
//
//	    for (; _State == ios_base::goodbit && 0 < _Pad;
//		    --_Pad)	// pad on right
//		    if (traits_type::eq_int_type(traits_type::eof(),
//			    _Ostr.rdbuf()->sputc(_Ostr.fill())))
//			    _State |= ios_base::badbit;
//    }
//    catch(...)
//    {
//	  _CATCH_IO_(_Ostr)
//    }
//	}
//
//  _Ostr.width(0);
//  _Ostr.setstate(_State);
//  return (_Ostr);
//}

inline
basic_ostream<Pt::Char>& operator<<(basic_ostream<Pt::Char>& _Ostr, Pt::Char _Ch)
{
	typedef basic_ostream<Pt::Char> _Myos;
  typedef basic_ostream<Pt::Char>::traits_type traits_type;

	ios_base::iostate _State = ios_base::goodbit;
	const typename _Myos::sentry _Ok(_Ostr);

	if (_Ok)
	{
		streamsize _Pad = _Ostr.width() <= 1 ? 0 : _Ostr.width() - 1;

    try
    {
  		if ((_Ostr.flags() & ios_base::adjustfield) != ios_base::left)
	  		for (; _State == ios_base::goodbit && 0 < _Pad;
				  --_Pad)	// pad on left
				  if (traits_type::eq_int_type(traits_type::eof(),
					  _Ostr.rdbuf()->sputc(_Ostr.fill())))
					  _State |= ios_base::badbit;

		  if (_State == ios_base::goodbit
			  && traits_type::eq_int_type(traits_type::eof(),
				  _Ostr.rdbuf()->sputc(_Ch)))
			  _State |= ios_base::badbit;

		  for (; _State == ios_base::goodbit && 0 < _Pad;
			  --_Pad)	// pad on right
			  if (traits_type::eq_int_type(traits_type::eof(),
				  _Ostr.rdbuf()->sputc(_Ostr.fill())))
				  _State |= ios_base::badbit;
    }
    catch(...)
    {
      _Ostr.setstate(ios_base::badbit, true);
    }
  }

	_Ostr.width(0);
	_Ostr.setstate(_State);
	return (_Ostr);
}

inline
basic_ostream<Pt::Char>& operator<<(basic_ostream<Pt::Char>& _Ostr, char _Ch)
{
  return _Ostr << Pt::Char(_Ch);
}

/** @brief Input stream.
*/
template <>
class basic_istream< Pt::Char, std::char_traits<Pt::Char> >
: virtual public basic_ios<Pt::Char, std::char_traits<Pt::Char> >
{
    public:
        typedef Pt::Char char_type;
        typedef basic_istream< Pt::Char, std::char_traits<Pt::Char> > _Myt;
        typedef basic_ios<Pt::Char> ios_type;
        typedef std::char_traits<Pt::Char> traits_type;
        typedef typename traits_type::int_type int_type;
        typedef typename traits_type::pos_type pos_type;
        typedef typename traits_type::off_type off_type;
        typedef std::ios_base::iostate iostate;
        typedef basic_streambuf<Pt::Char> buffer_type;

	      static const std::ios_base::iostate goodbit = std::ios_base::goodbit;
	      static const std::ios_base::iostate eofbit = std::ios_base::eofbit;
	      static const std::ios_base::iostate failbit = std::ios_base::failbit;
	      static const std::ios_base::iostate badbit = std::ios_base::badbit;

    public:
	      class sentry
		    {	
	          public:
		            explicit sentry(basic_istream& is, bool _Noskip = false)
                : _Myistr(is)
                , _Ok( is.ipfx(_Noskip) )
			          {
			          }

		            explicit operator bool() const
			          {
			              return _Ok;
			          }

            private:
		          sentry(const sentry&);

		          sentry& operator=(const sentry&);

	          private:
                basic_istream& _Myistr;
		            bool _Ok;
		    };

    public:
          //! @brief Constructor.
        explicit basic_istream(buffer_type* sb = 0, bool isstd = false)
        : _gcount(0)
        {
          ios_type::init(sb, isstd);
        }
        
        //! @brief Destructor.
        ~basic_istream()
        {}

        basic_istream(const _Myt&) = delete;
        _Myt& operator=(const _Myt&) = delete;

    protected:
	      basic_istream(_Myt&& _Right)
		    : _gcount(_Right._gcount)
		    {
		      basic_ios::init();
		      basic_ios::move(std::move(_Right));
		      _Right._gcount = 0;
		    }

	      _Myt& operator=(_Myt&& _Right)
		    {
		      this->swap(_Right);
		      return (*this);
		    }

	      void swap(_Myt& _Right)
		    {
		      basic_ios::swap(_Right);
		      std::swap(_gcount, _Right._gcount);
		    }

    public:
	      int_type peek()
		    {
		        std::ios_base::iostate _State = std::ios_base::goodbit;
		        _gcount = 0;
		        int_type _Meta = 0;
		        
            const sentry _Ok(*this, true);

		        if( ! _Ok )
            {
			          _Meta = traits_type::eof();
            }
		        else
			      {
			          try
                {
                    _Meta = rdbuf()->sgetc();
			              if( traits_type::eq_int_type(traits_type::eof(), _Meta) )
				              _State |= std::ios_base::eofbit;
                }
			          catch(...)
                {
                  setstate(std::ios_base::badbit, true);
                }
			      }

		        setstate(_State);
		        return (_Meta);
		    }

        int_type get() 
        {
            int_type _Meta           = 0;
            ios_base::iostate _State = ios_base::goodbit;
            _gcount                 = 0;
            
            const sentry _Ok(*this, true);

            if( ! _Ok )
            {
                _Meta = traits_type::eof();
            } 
            else
            {
                try 
                {
                    _Meta = rdbuf()->sgetc();

                    if (traits_type::eq_int_type(traits_type::eof(), _Meta))
                    {
                        _State |= ios_base::eofbit | ios_base::failbit;
                    } 
                    else 
                    { 
                        rdbuf()->sbumpc();
                        _gcount = 1;
                    }
                }
                catch(...)
                {
                    setstate(ios_base::badbit, true);
                }
            }

            setstate(_State);
            return _Meta;
        }

        basic_istream& getline(Pt::Char* _Str, streamsize _Count) 
        {
            std::ctype<Pt::Char> ctype;
            return getline( _Str, _Count, ctype.widen('\n') );
        }

        basic_istream& getline(Pt::Char* _Str, streamsize _Count, Pt::Char _Delim) 
        {
            ios_base::iostate _State = ios_base::goodbit;
            _gcount                  = 0;
            
            const sentry _Ok(*this, true);

            if (_Ok && 0 < _Count) 
            {
                int_type _Metadelim = traits_type::to_int_type(_Delim);

                try
                {
                    int_type _Meta = rdbuf()->sgetc();

                    for(;; _Meta = rdbuf()->snextc() ) 
                    {
                        if (traits_type::eq_int_type(traits_type::eof(), _Meta)) 
                        {
                            _State |= ios_base::eofbit;
                            break;
                        } 
                        else if (_Meta == _Metadelim) 
                        {
                            gcountInc();
                            rdbuf()->sbumpc();
                            break;
                        } 
                        else if (--_Count <= 0) 
                        {
                            _State |= ios_base::failbit;
                            break;
                        } 
                        else 
                        {
                            *_Str++ = traits_type::to_char_type(_Meta);
                            gcountInc();
                        }
                    }
                }
                catch(...)
                {
                    setstate(ios_base::badbit, true);
                }
            }

            *_Str = Pt::Char();
            setstate(_gcount == 0 ? _State | ios_base::failbit : _State);
            return *this;
        }

        basic_istream& seekg(pos_type _Pos) 
        {
            std::ios_base::iostate _State = ios_base::goodbit;
            std::ios_base::iostate _Oldstate = rdstate();
        
            clear(_Oldstate & ~ios_base::eofbit);
        
            const sentry _Ok(*this, true);

            if( ! this->fail() && static_cast<off_type>(rdbuf()->pubseekpos(_Pos, ios_base::in)) == -1) {
                setstate(_State | ios_base::failbit);
            }

            return *this;
        }

        basic_istream& operator>>(int& n) 
        {
            long v = n;
            extractNumber(v);
            n = v;
            return *this;
        }

        basic_istream& operator>>(float& n) 
        {
            return extractNumber(n);
        }

    private:
        template <class T>
        basic_istream& extractNumber(T& n) 
        {
            ios_base::iostate _Err = ios_base::goodbit;
        
            const sentry _Ok(*this);

            if (_Ok) 
            {
                try
                {
                    std::num_get<Pt::Char> numGet;

                    istreambuf_iterator<Pt::Char> it(*this);
                    istreambuf_iterator<Pt::Char> end;
                    numGet.get(it, end, *this, _Err, n);
                }
                catch(...)
                {
                    setstate(ios_base::badbit, true);
                }
            }

            setstate(_Err);
            return *this;
        }

        bool ipfx(bool _Noskip = false) 
        {
            if( ! this->good() ) 
            {
                ios_type::setstate(ios_base::failbit);
                return false;
            }

            //const auto _Tied = ios_type::tie();
            //if (_Tied) {
            //    _Tied->flush();
            //}

            bool _Eof = false;
            if( ! _Noskip && this->flags() & ios_base::skipws) 
            { 
                std::ctype<Pt::Char> ct; // use_facet
                
                try 
                {
                  int_type _Meta = ios_type::rdbuf()->sgetc();

                  for (;; _Meta = ios_type::rdbuf()->snextc()) 
                  {
                      if (traits_type::eq_int_type(traits_type::eof(), _Meta)) 
                      {
                          _Eof = true;
                          break;
                      } 
                      else if ( ! ct.is(std::ctype<Pt::Char>::space, traits_type::to_char_type(_Meta))) 
                      {
                          break;
                      }
                  }
                }
                catch(...)
                {
                    setstate(ios_base::badbit, true);
                }
            }

            if (_Eof) {
                ios_type::setstate(ios_base::eofbit | ios_base::failbit);
            }

            return this->good();
        }

        template <class = void>
        void _Increment_gcount() {
            if (_Chcount != (numeric_limits<streamsize>::max)()) {
                ++_Chcount;
            }
        }

          void gcountInc() 
          {
              if( _gcount != numeric_limits<streamsize>::max() )
                  ++_gcount;

          }

    private:
        std::streamsize _gcount;
};

/** @brief Input/Ouput stream.
*/
template <>
class basic_iostream< Pt::Char, std::char_traits<Pt::Char> >
    : public basic_istream< Pt::Char, std::char_traits<Pt::Char> >
    , public basic_ostream< Pt::Char, std::char_traits<Pt::Char> >
{
    public:
        typedef Pt::Char char_type;
        typedef basic_iostream< Pt::Char, std::char_traits<Pt::Char> > _Myt;
        typedef std::char_traits<Pt::Char> traits_type;
        typedef typename traits_type::int_type int_type;
        typedef typename traits_type::pos_type pos_type;
        typedef typename traits_type::off_type off_type;
        typedef basic_streambuf<Pt::Char> buffer_type;

        explicit basic_iostream(std::basic_streambuf<char_type>* sb)
        : basic_istream(sb)
        , basic_ostream(_Noinit)
        {}

    protected:
	      basic_iostream(_Myt&& _Right)
		    : basic_istream(_Right.rdbuf())
			  , basic_ostream(_Noinit)
		    {
		      basic_ios::init();
		      basic_ios::move(  std::forward<_Myt>(_Right) );
		    }

	      _Myt& operator=(_Myt&& _Right)
		    {
		      this->swap(_Right);
		      return (*this);
		    }

	      void swap(_Myt& _Right)
		    {
		      if (this != &_Right)
			    basic_ios::swap(_Right);
		    }

    public:
        basic_iostream(const basic_iostream&) = delete;

        basic_iostream& operator=(const basic_iostream&) = delete;

        virtual ~basic_iostream() noexcept {}
};

} // namespace
#endif

#if defined(_MSC_VER) && defined(_WIN32_WCE) 
    // alternatively compile with /FORCE:multiple
    template class PT_EXPORT std::basic_ios<char>;
    template class PT_EXPORT std::basic_istream<char>;
    template class PT_EXPORT std::basic_ostream<char>;
    template class PT_EXPORT std::basic_iostream<char>; 
#endif

#if defined(_MSC_VER)
    template class PT_EXPORT std::basic_ios<Pt::Char>;
    template class PT_EXPORT std::basic_istream<Pt::Char>;
    template class PT_EXPORT std::basic_ostream<Pt::Char>;
    template class PT_EXPORT std::basic_iostream<Pt::Char>; 
#endif

namespace Pt {

/** @brief Input stream.
*/
template <typename CharT, typename TraitsT = std::char_traits<CharT> >
class BasicIStream : public std::basic_istream<CharT, TraitsT>
{
    public:
        typedef CharT char_type;
        typedef TraitsT traits_type;
        typedef typename TraitsT::int_type int_type;
        typedef typename TraitsT::pos_type pos_type;
        typedef typename TraitsT::off_type off_type;

    public:
        //! @brief Constructor.
        explicit BasicIStream(BasicStreamBuffer<CharT>* sb = 0);
        
        //! @brief Destructor.
        ~BasicIStream()
        {}

        /** @brief Peeks bytes in the stream buffer.
        
            The number of bytes that can be peeked depends on the current
            stream buffer get area and maybe less than requested,
            similar to istream::readsome().
        */
        std::streamsize peeksome(CharT* buffer, std::streamsize n);

        //! @brief Returns the buffer.
        BasicStreamBuffer<CharT>* buffer()
        { return _buffer; }

        //! @brief Sets the buffer.
        void setBuffer(BasicStreamBuffer<CharT>* sb)
        { 
            _buffer = sb; 
            this->rdbuf(sb);
        }

    private:
        BasicStreamBuffer<CharT>* _buffer;
};


/** @brief Output stream.
*/
template <typename CharT, typename TraitsT = std::char_traits<CharT> >
class BasicOStream : public std::basic_ostream<CharT, TraitsT>
{
    public:
        typedef CharT char_type;
        typedef TraitsT traits_type;
        typedef typename TraitsT::int_type int_type;
        typedef typename TraitsT::pos_type pos_type;
        typedef typename TraitsT::off_type off_type;
        typedef BasicStreamBuffer<CharT> buffer_type;

    public:
        //! @brief Constructor.
        explicit BasicOStream(buffer_type* sb = 0);

        //! @brief Destructor.
        ~BasicOStream()
        {}

        /** @brief Write as much data as fits in buffer
        */
        std::streamsize writesome(CharT* buffer, std::streamsize n);
    
        //! @brief Returns the buffer.
        buffer_type* buffer()
        { return _buffer; }

        //! @brief Sets the buffer.
        void setBuffer(buffer_type* sb)
        { 
            _buffer = sb; 
            this->rdbuf(sb);
        }

    private:
        buffer_type* _buffer;
};


/** @brief Input/Output stream.
*/
template <typename CharT, typename TraitsT = std::char_traits<CharT> >
class BasicIOStream : public std::basic_iostream<CharT, TraitsT>
{
    public:
        typedef CharT char_type;
        typedef TraitsT traits_type;
        typedef typename TraitsT::int_type int_type;
        typedef typename TraitsT::pos_type pos_type;
        typedef typename TraitsT::off_type off_type;
        typedef BasicStreamBuffer<CharT> buffer_type;

    public:
        //! @brief Constructor.
        explicit BasicIOStream(buffer_type* sb = 0);

        //! @brief Destructor.
        ~BasicIOStream()
        {}

        /** @brief Peeks bytes in the stream buffer.
        
            The number of bytes that can be peeked depends on the current
            stream buffer get area and maybe less than requested,
            similar to istream::readsome().
        */
        std::streamsize peeksome(CharT* buffer, std::streamsize n);
    
        /** @brief Write as much data as fits in buffer
        */
        std::streamsize writesome(CharT* buffer, std::streamsize n);
    
        //! @brief Returns the buffer.
        buffer_type* buffer()
        { return _buffer; }

        //! @brief Sets the buffer.
        void setBuffer(buffer_type* sb)
        { 
            _buffer = sb; 
            this->rdbuf(sb);
        }

    private:
        buffer_type* _buffer;
};


template <typename CharT, typename TraitsT>
inline BasicIStream<CharT, TraitsT>::BasicIStream(BasicStreamBuffer<CharT>* sb)
: std::basic_istream<CharT>(sb)
, _buffer(sb)
{
}


template <typename CharT, typename TraitsT>
inline std::streamsize BasicIStream<CharT, TraitsT>::peeksome(CharT* buffer, std::streamsize n)
{
    if(_buffer && this->rdbuf() == _buffer)
        return _buffer->speekn(buffer, n);

    if(n > 0)
    {
        buffer[0] = this->peek();
        return 1;
    }

    return 0;
}


template <typename CharT, typename TraitsT>
inline BasicOStream<CharT, TraitsT>::BasicOStream(buffer_type* sb)
: std::basic_ostream<CharT>(sb)
, _buffer(sb)
{
}


template <typename CharT, typename TraitsT>
inline std::streamsize BasicOStream<CharT, TraitsT>::writesome(CharT* buffer, std::streamsize n)
{
    if( ! _buffer || this->rdbuf() != _buffer )
        return 0;

    std::streamsize avail = _buffer->out_avail();
    if(avail == 0)
    {
        return 0;
    }

    n = std::min(avail, n);
    return _buffer->sputn(buffer, n);
}


template <typename CharT, typename TraitsT>
inline BasicIOStream<CharT, TraitsT>::BasicIOStream(buffer_type* sb)
: std::basic_iostream<CharT>(sb)
, _buffer(sb)
{
}


template <typename CharT, typename TraitsT>
inline std::streamsize BasicIOStream<CharT, TraitsT>::peeksome(CharT* buffer, std::streamsize n)
{
    if(_buffer && this->rdbuf() == _buffer)
        return _buffer->speekn(buffer, n);

    if(n > 0)
    {
        buffer[0] = this->peek();
        return 1;
    }

    return 0;
}


template <typename CharT, typename TraitsT>
inline std::streamsize BasicIOStream<CharT, TraitsT>::writesome(CharT* buffer, std::streamsize n)
{
    if( ! _buffer || this->rdbuf() != _buffer )
        return 0;

    std::streamsize avail = _buffer->out_avail();
    if(avail == 0)
    {
        return 0;
    }

    n = std::min(avail, n);
    return _buffer->sputn(buffer, n);
}

} // namespace Pt

#endif

#ifndef Pt_TextCodec_h
#define Pt_TextCodec_h

#include <Pt/Api.h>
#include <Pt/Locale.h>
#include <Pt/String.h>

namespace Pt {

/** @brief Converts between character encodings.

    A Pt::TextCodec is used by text converters to encode and decode external
    byte sequences, hence the name codec. It implements the std::codecvt facet
    interface, on systems provide the std::locale facilities.
    Codecs are stateless, which means that one codec can be used with multiple
    text converters. A %TextCodec is constructed with a reference counter that
    indicates whether the converter or locale manages the lifetime of the codec. 
    If that value is 0, as it is the case if the %TextCodec is default
    constructed, the text converter or locale will delete the codec. 
    
    @code
    Pt::TextOStream tos(new Pt::Utf8Codec);
    @endcode
    
    Therefore, a default constructed %TextCodec has to be cretaed with new,
    as it is the rule for all localization facets. This can be avoided by
    passing a value different from 0 to the codecs constructor, in which
    case the codec must exist at least as long as the stream that uses it:

    @code
    Pt::Utf8Codec codec(1);
    Pt::TextOStream tos(&codec);
    @endcode

    @ingroup Unicode
*/
template <typename CharT, typename ByteT>
class TextCodec : public std::codecvt<T, B, Pt::MBState>
{   
    public:
        //! @brief Internal character type.
        typedef CharT intern_type;

        //! @brief External character type.
        typedef ByteT extern_type;

        //! @brief Conversion state type.
        typedef MBState state_type; 

    public:
        /** @brief Constructs with reference count.
        
            If @a ref is 0 the stream, stream buffer or locale deletes the
            facet.
         */
        TextCodec(std::size_t ref = 0);

        //! @brief Decodes a character sequence.
        result in(MBState& s, 
                  const ByteT* fbeg, const ByteT* fend, const ByteT*& fnext,
                  CharT* tbeg, CharT* tend, CharT*& tnext) const;

        //! @brief Encodes a character sequence.
        result out(MBState& state, 
                   const CharT* fbeg, const CharT* fend, const CharT*& fnext,
                   ByteT* tbeg, ByteT* tend, ByteT*& tnext) const;

        //! @brief Unshifts a character sequence.
        result unshift(MBState& state, 
                       ByteT* to, ByteT* to_end, ByteT*& to_next) const;

        //! @brief Returns the encoding rate.
        int encoding() const;

        //! @brief Returns true if no conversion is required.
        bool always_noconv() const;

        //! @brief Returns the number of the decoded characters.
        int length(MBState& state, 
                   const ByteT* from, const ByteT* end, std::size_t max) const;

        //! @brief Maximum length of an external sequence if one character is encoded.
        int max_length() const;

    public:
        //! Desctructor.
        virtual ~TextCodec()
        {}

        //! @internal
        std::size_t refs() const
        { return _refs; }

    private:
        std::size_t _refs;
};

}

#endif

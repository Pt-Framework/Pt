#ifndef Pt_TextCodec_h
#define Pt_TextCodec_h

#include <Pt/Api.h>
#include <Pt/Locale.h>
#include <Pt/String.h>

namespace Pt {

/** @brief Converts between character encodings.

    The %TextCodec is used by the text streams and buffers to convert to
    and from external encodings. Since it implements the std::codecvt facet
    interface, it can be used as this type of facet on all systems which
    support facets and the std::locale.

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

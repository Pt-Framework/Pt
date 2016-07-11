#ifndef Pt_Hmi_PictureImpl_h
#define Pt_Hmi_PictureImpl_h

#include <Pt/Gfx/Image.h>
#include <vector>

namespace Pt{
namespace Hmi{

class PictureImpl
{
    public:
        PictureImpl(const Gfx::Image& image);

        virtual ~PictureImpl();

        const std::vector<Pt::uint8_t>& andMask() const
        {
          return _andMask;
        }

        std::vector<Pt::uint8_t>& andMask()
        {
          return _andMask;
        }

        const std::vector<Pt::uint8_t>& xorMask() const
        {
            return _xorMask;
        }

        std::vector<Pt::uint8_t>& xorMask()
        {
            return _xorMask;
        }

        size_t width() const
        {
            return _width;
        }
        
        size_t height() const
        {
            return _height;
        }


    private:
        std::vector<Pt::uint8_t> _andMask;
        std::vector<Pt::uint8_t> _xorMask;
        size_t _width;
        size_t _height;
};

}}
#endif
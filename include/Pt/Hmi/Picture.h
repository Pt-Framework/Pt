#ifndef Pt_Hmi_Picture_h
#define Pt_Hmi_Picture_h

#include <Pt/Hmi/Api.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Painter.h>


namespace Pt{
namespace Hmi{

class PictureImpl;

class PT_HMI_API Picture
{
    public:
        Picture(const Gfx::Image& image);
        
        Picture();
        
        virtual ~Picture();

        void set(const Gfx::Image& image);

        bool empty() const;

        PictureImpl* impl()
        {
            return _impl;
        }

        const PictureImpl* impl() const
        {
            return _impl;
        }

        size_t width() const;

        size_t  height() const;


        
    private:

        PictureImpl* _impl;
};

}}
#endif
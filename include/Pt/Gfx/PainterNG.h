
namespace Pt {

namespace Gfx {

class Transform 
{
    public:
        // contruct with identity matrix
        Transform();
        
        ~Transform();

        // returns true if the matrix is the identity matrix
        bool isIdentity() const;

        // reset the matrix to the identity matrix
        void reset();

        // set the translation factors in the _matrix 
        void translate(double x, double y);
        
        // set the sacling factors in the _matrix
        void scale(double x, double y);

        // set the rotaion factors in the _matrix 
        // This uses an angle in degree.
        //
        // angle = 30.0;
        // radians = angle * PI/180;
        void rotateDeg(double angle);

        // set the rotaion factors in the _matrix 
        // This uses an angle in radians.
        void rotateRad(double angle)

        // set the horizontal and vertical shear factors in the _matrix 
        void shear(double sh, double sv)
        
        double m11() const;
        double m12() const;
        double m21() const;
        double m22() const;
        double dx() const;
        double dy() const;

        // sets the matrix fields
        void set(double m11, double m12,
                 double m21, double m22,
                 double dx, double dy);


        Transform operator*(const Transform& t) const;
        PointF operator*(const PointF& p) const;
        SizeF operator*(const SizeF& p) const;

        Transform& operator*=(const Transform& t);

        bool operator==(const Transform& t) const;
        bool operator!=(const Transform& t) const;

    private:
        // _matrix could either be a 3x3 matrix or a 2x2 matrix with extra
        // _dx and _dy for the translation. It seems to be cheaper if a 2x2
        // is used and always add the offset for the translation separately
        // afterwards
        
        IMPL_DEFINED _matrix;
};



class Path 
{
    // IMPL DEFINED
    class Element;

    public:
        Path();

        Path(const Path& p);

        ~Path();

        const Path& operator=(const Path& p);

        std::size_t size();

        const Element& at(std::size_t n) const;

        bool isEmpty() const;

        void clear();

        // adds a closed subpath
        void addPath(const Path& p);

        // adds the elements to this path, connecting the last point with the 
        // first point of the inserted path
        void insertPath(const Path& p);

        // ads a LineTo to the begin of the subpath and adds a moveTo(0,0)
        void closeSubpath();

        RectF boundingRect() const;

        const PointF& currentPosition() const;

        void moveTo(const PointF& p);

        void lineTo(const PointF& p);

        void arcTo(const PointF& p, double r);

        void quadraticBezierTo(const PointF &c, const PointF& to);

        void cubicBezierTo(const PointF &c1, const PointF &c2, const PointF& to);

        void bezierTo(const PointF* controlPoints, size_t n, const PointF& to);

        // Apply the tranformation to the points in the path
        void transform(const Transform& transform);
};


class ImagePainter2 : public Painter
{
    public:
        ImagePainter2(Image& image);

        virtual ~ImagePainter2();


        void setImage(Image& image);

        virtual const ImageFormat& format() const;


        virtual bool isAntialiasing() const;

        virtual void setAntialiasing(bool on);

        virtual const CompositionMode& compositionMode() const;

        virtual void setCompositionMode(const CompositionMode& mode);

        
        virtual const Gfx::RectF& clip() const;

        virtual void setClip(const RectF& clip);

        // Apply transformations to all draw and fill functions except
        // drawImage. Also setPen, setBrush, setFont, setClip do not depend on
        // the current transform
        virtual const Gfx::Transform& transform() const;

        virtual void setTransform(const const Gfx::Transform& t);


        virtual const Pen& pen() const;
        
        virtual void setPen(const Pen& pen);
        
        virtual const Brush& brush() const;
        
        virtual void setBrush(const Brush& brush);

        virtual const Font& font() const;
        
        virtual void setFont(const Font& font);


        virtual FontMetrics fontMetrics(const Pt::String& text) const;


        // DO NOT APPLY TRANSFORMATIONS TO IMAGES
        virtual void drawImage(const PointF& to, const Image& image);

        // DO NOT APPLY TRANSFORMATIONS TO IMAGES
        virtual void drawImage(const PointF& to, const Image& image, const RectF& imageRect);

        // TRANSFORM: use 2x2 FT_Matrix with freetype and add dy, dy separatly
        virtual void drawText(const PointF& to, const Pt::String& text);

        virtual void drawLine(const PointF& from, const PointF& to);

        virtual void drawRect(const RectF& rect);

        virtual void drawRoundedRect(const RectF& rect, float radius);

        // NOTE: The points must move in counter-clockwise (CCW) direction or something wrong may be drawn!
        virtual void drawPolyline(const PointF* points, const size_t pointCount);


        // NOTE: The begin and end angle must move in counter-clockwise (CCW) direction or something wrong may be drawn!
        virtual void drawArc(const PointF& topLeft, const SizeF& size, 
                             float degBegin, float degEnd);

        // NOTE: The begin and end angle must move in counter-clockwise (CCW) direction or something wrong may be drawn!
        virtual void drawChord(const PointF& topLeft, const SizeF& size, 
                               float degBegin, float degEnd);

        // NOTE: The begin and end angle must move in counter-clockwise (CCW) direction or something wrong may be drawn!
        virtual void drawPie(const PointF& topLeft, const SizeF& size, 
                             float degBegin, float degEnd);

        virtual void drawEllipse(const PointF& topLeft, const SizeF& size);


        void drawQuadraticBezier(const PointF& from, const PointF& to, 
                                 const PointF& c);

        void drawCubicBezier(const PointF& from, const PointF& to, 
                            const PointF &c1, const PointF &c2);

        virtual void drawQuadraticPolybezier(const PointF& from, const PointF& to, 
                                             const PointF* controls, const size_t n);

        
        // maybe better have ImagePainter::setSmoothness
        virtual void drawPath(const Path& path, float smoothness = 1.0f);


        virtual void fillRect(const RectF& rect);

        virtual void fillRoundRect(const RectF& rect, float radius);

        virtual void fillPolygon(const PointF* points, const size_t pointCount);

        virtual void fillEllipse(const PointF& topLeft, const SizeF& size);

        virtual void fillPie(const PointF& topLeft, const SizeF& size, 
                             float degBegin, float degEnd);

        virtual void fillChord(const PointF& topLeft, const SizeF& size, 
                              float degBegin, float degEnd);

        // maybe better have a separate ImagePainter::setSmoothness
        virtual void fillPath(const Path& path, float smoothness = 1.0f);

    public:
        static void setFontDir(const System::Path& path);
        static void setDefaultFont(const std::string& name);
        static std::string defaultFont();
        static std::vector<std::string> fontNames();
        static FontMetrics fontMetrics(const Font& font, const Pt::String& text);
};

} // namespace

} // namespace

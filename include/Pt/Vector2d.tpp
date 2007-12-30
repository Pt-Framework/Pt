
template <typename T, typename Allocator>
Vector2d<T, Allocator>::Vector2d()
: m_height(0)
, m_array()
{
}


template <typename T, typename Allocator>
Vector2d<T, Allocator>::Vector2d(const Pt::size_t width, const Pt::size_t height)
: m_height(height)
, m_array(width * height)
{
}


template <typename T, typename Allocator>
Vector2d<T, Allocator>::Vector2d(const Vector2d<T, Allocator>& rhs)
: m_height(rhs.height)
, m_array(rhs.m_array)
{
}


template <typename T, typename Allocator>
Vector2d<T, Allocator>::~Vector2d()
{
}


template <typename T, typename Allocator>
Vector2d<T, Allocator>& Vector2d<T, Allocator>::operator=(const Vector2d<T, Allocator>& rhs)
{
    m_array = rhs.m_array;
    m_height = rhs.m_height;
    return *this;
}


template <typename T, typename Allocator>
const T& Vector2d<T, Allocator>::operator()(const Pt::size_t x, const Pt::size_t y) const
{
    return m_array[index(x, y)];
}


template <typename T, typename Allocator>
T& Vector2d<T, Allocator>::operator()(const Pt::size_t x, const Pt::size_t y)
{
    return m_array[index(x, y)];
}


template <typename T, typename Allocator>
const T& Vector2d<T, Allocator>::at(const Pt::size_t x, const Pt::size_t y) const
{
    if (x >= width())
    {
        throw std::out_of_range();
    }
    if (y >= height())
    {
        throw std::out_of_range();
    }
    return m_array.at(index(x, y));
}


template <typename T, typename Allocator>
T& Vector2d<T, Allocator>::at(const Pt::size_t x, const Pt::size_t y)
{
    if (x >= width())
    {
        throw std::out_of_range();
    }
    if (y >= height())
    {
        throw std::out_of_range();
    }
    return m_array.at(index(x, y));
}


template <typename T, typename Allocator>
bool Vector2d<T, Allocator>::empty() const
{
    return m_array.empty();
}


template <typename T, typename Allocator>
void Vector2d<T, Allocator>::clear()
{
    m_height = 0;
    m_array.clear();
}


template <typename T, typename Allocator>
void Vector2d<T, Allocator>::resize(const Pt::size_t width, const Pt::size_t height)
{
    m_array.resize(width * height);
    m_height = height;
}


template <typename T, typename Allocator>
void Vector2d<T, Allocator>::swap(Vector2d<T, Allocator>& other)
{
    m_array.swap(other.m_array);
    swap(m_height, other.m_height);
}


template <typename T, typename Allocator>
inline Pt::size_t Vector2d<T, Allocator>::height() const
{
    return m_height;
}


template <typename T, typename Allocator>
inline Pt::size_t Vector2d<T, Allocator>::width() const
{
    if (m_height == 0)
    {
        return 0;
    }
    return m_array.size() / m_height;
}


template <typename T, typename Allocator>
inline Pt::size_t Vector2d<T, Allocator>::index(const Pt::size_t x, const Pt::size_t y) const
{
    return x * m_height + y;
}


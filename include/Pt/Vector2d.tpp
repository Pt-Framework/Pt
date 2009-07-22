
template <typename T, typename Container>
Vector2d<T, Container>::Vector2d()
: m_height(0)
, m_array()
{
}


template <typename T, typename Container>
Vector2d<T, Container>::Vector2d(const Pt::size_t width,
								 const Pt::size_t height,
								 const T& default)
: m_height(height)
, m_array(width * height, default)
{
}


template <typename T, typename Container>
const T& Vector2d<T, Container>::operator()(const Pt::size_t x, const Pt::size_t y) const
{
    return m_array[index(x, y)];
}


template <typename T, typename Container>
T& Vector2d<T, Container>::operator()(const Pt::size_t x, const Pt::size_t y)
{
    return m_array[index(x, y)];
}


template <typename T, typename Container>
const T& Vector2d<T, Container>::at(const Pt::size_t x, const Pt::size_t y) const
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


template <typename T, typename Container>
T& Vector2d<T, Container>::at(const Pt::size_t x, const Pt::size_t y)
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


template <typename T, typename Container>
bool Vector2d<T, Container>::empty() const
{
    return m_array.empty();
}


template <typename T, typename Container>
void Vector2d<T, Container>::clear()
{
    m_height = 0;
    m_array.clear();
}


template <typename T, typename Container>
void Vector2d<T, Container>::resize(const Pt::size_t width,
								    const Pt::size_t height,
								    const T& default)
{
    m_array.resize(width * height, default);
    m_height = height;
}


template <typename T, typename Container>
void Vector2d<T, Container>::swap(Vector2d<T, Container>& other)
{
    m_array.swap(other.m_array);
    swap(m_height, other.m_height);
}


template <typename T, typename Container>
inline Pt::size_t Vector2d<T, Container>::height() const
{
    return m_height;
}


template <typename T, typename Container>
inline Pt::size_t Vector2d<T, Container>::width() const
{
    if (m_height == 0)
    {
        return 0;
    }
    return m_array.size() / m_height;
}


template <typename T, typename Container>
inline Pt::size_t Vector2d<T, Container>::index(const Pt::size_t x, const Pt::size_t y) const
{
    return x * m_height + y;
}


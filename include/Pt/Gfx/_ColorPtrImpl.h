//
// This is just a backup of the older algorithm
//


        /** @brief Implementation of a pointer to a color in planar images
         *  @internal
         *
         *  This class derives as many times recursively from itself, as
         *  the color model has planes. Each derivation handles a pointer
         *  to an element in a plane. ComponentT is the value type of
         *  an element in a plane. NumPlanes is the total number of planes.
         *  N indexes the plane this class handles.
         */
        template <typename ComponentT, size_t NumPlanes, size_t N = NumPlanes-1>
        struct ColorPtrImpl : public ColorPtrImpl<ComponentT, NumPlanes, N-1>
        {
            inline ColorPtrImpl()
            {}

            inline ColorPtrImpl(ComponentT** c)
            : ColorPtrImpl<ComponentT, NumPlanes, N-1>(c)
            { this->_components[N] = c[N]; }

            // Set the component pointers to the appropriate offsets
            inline void set(ComponentT* data, size_t xpos, size_t ypos, size_t width)
            {
                const size_t planeOffset = (N * width);
                const size_t elemOffset  = (xpos + (ypos * width));
                this->_components[N]     = data + planeOffset + elemOffset;
                ColorPtrImpl<ComponentT, NumPlanes, N-1>::set(data, xpos, ypos, width);
            }

            inline void inc()
            {
                ++this->_components[N];
                ColorPtrImpl<ComponentT, NumPlanes, N-1>::inc();
            }

            inline void dec()
            {
                --this->_components[N];
                ColorPtrImpl<ComponentT, NumPlanes, N-1>::inc();
            }

            inline void advance(size_t val)
            {
                this->_components[N] += val;
                ColorPtrImpl<ComponentT, NumPlanes, N-1>::advance(val);
            }

            inline void rewind(size_t val)
            {
                this->_components[N] -= val;
                ColorPtrImpl<ComponentT, NumPlanes, N-1>::rewind(val);
            }

            inline bool equals(const ColorPtrImpl& other) const
            {
                if(this->_components[N] != other._components[N]) return false;
                return ColorPtrImpl<ComponentT, NumPlanes, N-1>::equals(other);
            }

            inline bool notEquals(const ColorPtrImpl& other) const
            {
                if(this->_components[N] != other._components[N]) return true;
                return ColorPtrImpl<ComponentT, NumPlanes, N-1>::notEquals(other);
            }
        };

        /** @brief Terminates recursive derivation
         *  @internal
         *
         *  This class has the actual array of pointers that point to
         *  the component values in the planes of a planar image. It
         *  also handles the 0th plane.
         */
        template <typename ComponentT, size_t NumPlanes>
        struct ColorPtrImpl<ComponentT, NumPlanes, 0>
        {
            inline ColorPtrImpl()
            {}

            inline ColorPtrImpl(ComponentT** c)
            { _components[0] = c[0]; }

            inline ComponentT** components()
            { return _components; }

            inline ComponentT* const* components() const
            { return _components; }

            inline void set(ComponentT* data, size_t xpos, size_t ypos, size_t width)
            {
                const size_t offset = (ypos * width) + xpos;
                this->_components[0] = data + offset;
            }

            inline void inc()
            { ++_components[0]; }

            inline void dec()
            { --_components[0]; }

            inline void advance(size_t val)
            { _components[0] += val; }

            inline void rewind(size_t val)
            { _components[0] -= val; }

            inline bool equals(const ColorPtrImpl& other) const
            { return _components[0] == other._components[0]; }

            inline bool notEquals(const ColorPtrImpl& other) const
            { return _components[0] != other._components[0]; }

            ComponentT* _components[NumPlanes];
        };

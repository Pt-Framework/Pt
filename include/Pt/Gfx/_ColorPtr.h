        //
        // ColorPtrT
        //
        template<typename ColorProxyT_>
        class PlanarImageModel<ColorProxyT_, 1, 1>::ColorPtrT
        {
            friend class PlanarImage<PlanarImageModel>;

            public:
                inline ColorPtrT(void*)
                : _imgW(0), _imgH(0), _beginPlanes(0)
                {}

                inline ColorPtrT(ComponentT* data,
                                 size_t      imageWidth,
                                 size_t      imageHeight,
                                 size_t      posX,
                                 size_t      posY)
                : _imgW(imageWidth), _imgH(imageHeight), _beginPlanes(data)
                { _impl.set(data, posX, posY, imageWidth); }

                inline ColorProxyT operator*()
                { return ColorProxyT( _impl.components() ); }

                inline ColorPtrT& operator++()
                { _impl.inc();  return *this; }

                inline ColorPtrT& operator--()
                { _impl.dec(); return *this; }

                inline ColorPtrT& operator+=(size_t n)
                { _impl.advance(n); return *this; }

                inline ColorPtrT& operator-=(size_t n)
                { _impl.rewind(n); return *this; }

                inline bool operator==(const ColorPtrT& c) const
                { return _impl.equals( c._impl ); }

                inline bool operator!=(const ColorPtrT& c) const
                { return _impl.notEquals( c._impl ); }

            private:
                inline Math::Point currentXYPosition() const
                {
                    ComponentT* const* components = _impl.components();
                    const size_t pos = components[0] - _beginPlanes;
                    return Math::Point(pos/_imgH, pos/_imgW);
                }

            private:
                //! @brief Width of the image
                size_t _imgW;

                //! @brief Height of the image
                size_t _imgH;

                //! @brief Pointer to the start of data of the master channel
                ComponentT* _beginPlanes;

                //! @brief Implementation of the pointer to color
                ColorPtrImpl<ComponentT, NumberOfChannels> _impl;
        };


        //
        // ConstColorPtrT
        //
        template<typename ColorProxyT_>
        class PlanarImageModel<ColorProxyT_, 1, 1>::ConstColorPtrT
        {
            public:
                inline ConstColorPtrT(const void*)
                : _imgW(0), _imgH(0), _chnStart(0)
                {}

                inline ConstColorPtrT(ConstPixelData& chanPtr,
                                      size_t               imageWidth,
                                      size_t               imageHeight,
                                      size_t               posX,
                                      size_t               posY)
                : _imgW(imageWidth), _imgH(imageHeight), _chnStart(0)
                {
                    _chnStart = chanPtr[0]; // Channel #0 is always the master channel

                    const size_t pos = posY*imageWidth + posX;
                    addElements< NumberOfChannels >(_chnCur, chanPtr, pos);
                }

                inline const ValueT operator*() const
                {
                    return ValueT( _impl.components() );
                    //return ValueT(_chnCur);
                }

                inline ConstColorPtrT& operator++()
                {
                    //incrementElements<NumberOfChannels>(_chnCur);
                    _impl.inc();
                    return *this;
                }

                inline ConstColorPtrT& operator--()
                {
                    //decrementElements< NumberOfChannels >(_chnCur);
                    _impl.dec();
                    return *this;
                }

                inline ConstColorPtrT& operator+=(size_t n)
                {
                    //addAssignElements< NumberOfChannels >(_chnCur, n);
                    _impl.advance(n);
                    return *this;
                }

                inline ConstColorPtrT& operator-=(size_t n)
                {
                    //subAssignElements< NumberOfChannels >(_chnCur, n);
                    _impl.rewind(n);
                    return *this;
                }


                bool operator==(const ConstColorPtrT& c) const
                {
                    //return equalElements< NumberOfChannels >(_chnCur, c._chnCur);
                    return _impl.equals( c._impl );
                }

                bool operator!=(const ConstColorPtrT& c) const
                {
                    //return notEqualElements< NumberOfChannels >(_chnCur, c._chnCur);
                    return _impl.notEquals( c._impl );
                }

                friend class PlanarImage<PlanarImageModel>;

            private:
                inline Math::Point currentXYPosition() const
                {
                    const size_t pos = _chnCur[0] - _chnStart;
                    return Math::Point(pos/_imgH, pos/_imgW);
                }

            private:
                size_t _imgW; // Image's width

                size_t _imgH; // Image's height

                const ComponentT*   _chnStart; // Start pointer of the master channel

                ConstPixelData _chnCur;   // Pointer to current positions in the channes

                //! @brief Implementation of the pointer to color
                ColorPtrImpl<const ComponentT, NumberOfChannels> _impl;
        };

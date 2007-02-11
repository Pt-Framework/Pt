/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PTV_MATH_MATRIX_H
#define PTV_MATH_MATRIX_H

#include <Pt/Types.h>
#include <Pt/Math/Api.h>
#include <Pt/Math/Math.h>
#include <Pt/Math/MathUtils.h>
#include <vector>
#include <assert.h>
#include <iostream>
#include <fstream>


namespace Pt
{
    namespace Math
    {

    //--------------------------------------------------------------------------------------------
    // special methods to type-dependent compare matrix elements.
    // float data needs a epsilon to deal with rounding issues
    //--------------------------------------------------------------------------------------------

    template<typename T>
    bool isEqual(const size_t row, const size_t col, const T* leftData, const T* rightData)
    {
        return !memcmp(leftData, rightData, row * col * sizeof(T));
    };


    template<>
    bool isEqual<double>(const size_t row, const size_t col, const double* leftData, const double* rightData);

    template<>
    bool isEqual<float>(const size_t row, const size_t col, const float* leftData, const float* rightData);



    /** \brief BasicMatrix class
        Matrix elements are stored in an array. The matrix holds an pointer to this array.
        The data elements in the array are sorted according to row. This means that
        the matrix consist of row vectors.
    */
    template <typename T, size_t rowDim, size_t colDim>
    class BasicMatrix {

        public:

            /** \brief Construct a BasicMatrix.
                All values of the matrix are set to 0.
                \param row The No. of rows.
                \param col The No. of columns.
            */
            BasicMatrix()
            {
                this->setToNull();
            }

            /** \brief Construct a BasicMatrix with another BasicMatrix.
                \param matrix Reference to another matrix.
            */
              BasicMatrix(const BasicMatrix<T, rowDim, colDim>& matrix)
            {
                 memcpy(this->m_matrixData, matrix.m_matrixData, rowDim * colDim * sizeof(T));
            }


            /** \brief Construct a BasicMatrix with a std::vector.
                The data vector must have a size of col * row.
                \param data Reference to a vector with data.
                \param row The No. of rows.
                \param col The No. of columns.
            */
            BasicMatrix(const std::vector<T>& data)
            {
                assert(data.size() == rowDim * colDim  );

                //TODO ggf. durch memcpy ersetzen?
                for(size_t r = 0; r < rowDim; ++r)
                {
                    for(size_t c = 0; c < colDim; ++c)
                    {
                        this->m_matrixData[r][c] = data[r * colDim+c];
                    }
                }
            }


            /** \brief Set all elements of the matrix to 0.
            */
            inline void setToNull()
            {
                memset(this->m_matrixData, 0, rowDim * colDim * sizeof(T));
            }


            /** \brief Create a identity matrix. The elements in the
                diagonal of the matrix have the value 1 all other
                elments are 0.
            */
            inline void setToIdentity()
            {
                // has to be a sqare matrix
                assert(rowDim == colDim);
                this->setToNull();
                for(size_t i = 0; i < colDim; ++i)
                {
                    this->m_matrixData[i][i] = (T)(1);
                }
            }


            /** \brief Create a identity matrix. The elements in the
                diagonal of the matrix have the value 1 all other
                elments are 0.
            */
           inline  bool isIdentity()
            {
                // has to be a sqare matrix
                assert(rowDim == colDim);
                BasicMatrix<T, rowDim, colDim> tmpMat;
                tmpMat.setToIdentity();
                return *this == tmpMat;
            }

            /** \brief Destructor for a BasicMatrix object.
            */
            virtual ~BasicMatrix()
            {
            }


            /** \brief Get the No. of columns for the matrix.
                \return The No. of columns.
            */
            inline size_t colCount() const
            {
                return colDim;
            }

             /** \brief Get the contents of matrix.
                \return The contents of matrix.
            */
            inline const T* data() const
            {
                return *this->m_matrixData;
            }

            /** \brief Get the No. of rows for the matrix.
                \return The No. of rows.
            */
            inline size_t rowCount() const
            {
                return rowDim;
            }


            /** \brief Get a certain element of the matrix.
                \param col Index of the column.
                \param row Index of the row.
                \return The value of a matrix member.
            */
            inline const T  getValue(const size_t& row, const size_t& col) const
            {
                return this->m_matrixData[row][col];
            }


            /** \brief Set a value for a certain element of the matrix.
                \param val Value to set.
                \param row Index of the row.
                \param col Index of the column.
            */
            inline void setValue(const T& val, const size_t& row, const size_t& col)
            {
                this->m_matrixData[row][col] = val;
            }


            /** \brief Add a value to a certain matrix element.
                \param val Value to add to the element.
                \param row Index of the row.
                \param col Index of the column.
            */
            inline void addValue(const T& val, const size_t& row, const size_t& col)
            {
                this->m_matrixData[row][col] += val;
            }


            /** \brief Subtract a value from a certain matrix element.
                \param val Value to subtract from the element.
                \param row Index of the row.
                \param col Index of the column.
            */
            inline void subValue(const T& val, const size_t& row, const size_t& col)
            {
                this->m_matrixData[row][col] -= val;
            }


            /** \brief Multiply a value to a certain matrix element.
                \param val Value to multiply to the element.
                \param row Index of the row.
                \param col Index of the column.
            */
            inline void mulValue(const T& val, const size_t& row, const size_t& col)
            {
                this->m_matrixData[row][col] *= val;
            }


            /** \brief Divide a certain matrix element by a value.
                \param val Value to divide an element.
                \param row Index of the row.
                \param col Index of the column.
            */
            inline void divValue(const T& val, const size_t& row, const size_t& col)
            {
               this->m_matrixData[row][col] /= val;
            }


            /** \brief Assignment of a BasicMatrix to another.
                \param matrix BasicMatrix to assign.
                \return A Reference to this matrix.
            */
            template <typename T1, size_t rowDimIn, size_t colDimIn>
            inline const BasicMatrix<T, rowDim, colDim>& operator=(const BasicMatrix<T1, rowDimIn, colDimIn>& matrix)
            {
                memcpy(this->m_matrixData, matrix.m_matrixData, this->m_elemCount * sizeof(T));
                return *this;
            }


            /** \brief Comparison of a BasicMatrix with another.
                \param matrix BasicMatrix to compare with.
                \return Whether the matrices are equal.
            */

            inline bool operator== (const BasicMatrix<T, rowDim, colDim>& matrix) const
            {
                // the c-style cast is neccessary here since:
                // the memory for m_matrixdata is allocated at compile time, so m_matrix has
                // a specific type, f.e. T[3][3] which differs from a T*
                return isEqual<T>(rowDim, colDim, (T*)this->m_matrixData, (T*)matrix.m_matrixData);
            }


            /** \brief Check a BasicMatrix with another for inequality.
                \param matrix BasicMatrix to compare with.
                \return Whether the matrices are different.
            */
            inline bool operator!=(const BasicMatrix<T, rowDim, colDim>& matrix) const
            {
                // the c-style cast is neccessary here since:
                // the memory for m_matrixdata is allocated at compile time, so m_matrix has
                // a specific type, f.e. T[3][3] which differs from a T*
                return !isEqual<T>(rowDim, colDim, (T*)this->m_matrixData, (T*)matrix.m_matrixData);
            }


            /** \brief Add a BasicMatrix to this one.
                \param matrix BasicMatrix for the Addition.
                \return A Reference to this matrix.
            */
            inline const BasicMatrix<T, rowDim, colDim>& operator+=(const BasicMatrix<T, rowDim, colDim>& matrix)
            {
                for(size_t r = 0; r < rowDim; ++r)
                {
                    for(size_t c = 0; c < colDim; ++c)
                    {
                        this->m_matrixData[r][c] += matrix.m_matrixData[r][c];
                    }
                }
                return *this;
            }


            /** \brief Sum up a BasicMatrix with this one.
                \param matrix BasicMatrix for the Addition.
                \return The resulting BasicMatrix.
            */
            inline BasicMatrix<T, rowDim, colDim> operator+(const BasicMatrix<T, rowDim, colDim>& matrix)
            {
                BasicMatrix<T, rowDim, colDim> resMatrix(*this);
                resMatrix += matrix;
                return resMatrix;
            }


            /** \brief Subtract a BasicMatrix from this one.
                \param matrix BasicMatrix for the Subtraction.
                \return A Reference to this matrix.
            */
            inline const BasicMatrix<T, rowDim, colDim>& operator-=(const BasicMatrix<T, rowDim, colDim>& matrix)
            {
                for(size_t r = 0; r < rowDim; ++r)
                {
                    for(size_t c = 0; c < colDim; ++c)
                    {
                        this->m_matrixData[r][c] -= matrix.m_matrixData[r][c];
                    }
                }
                return *this;
            }


            /** \brief Subtract a BasicMatrix from this one.
                \param matrix BasicMatrix for the Subtraction.
                \return The resulting BasicMatrix.
            */
            inline BasicMatrix<T, rowDim, colDim> operator-(const BasicMatrix<T, rowDim, colDim>& matrix)
            {

                BasicMatrix<T, rowDim, colDim> resMatrix(*this);
                resMatrix -= matrix;
                return resMatrix;
            }


            /** \brief Multiply BasicMatrix to this one.
                The left matrix must have as many columns, like the right one rows.
                \param matrix BasicMatrix for the Multiplication.
                \return The resulting BasicMatrix.
            */
            template <typename T2, size_t rightRowDim, size_t rightColDim>
            inline BasicMatrix<T2, rowDim, rightColDim> operator*(const BasicMatrix<T2, rightRowDim, rightColDim>& matrix)
            {

                BasicMatrix<T2, rowDim, rightColDim> resMatrix;

                for(size_t r = 0; r < /*rightRowDim*/rowDim; ++r)
                {
                    for(size_t c = 0; c < rightColDim; ++c)
                    {
                        for(size_t k = 0; k < colDim; ++k)
                        {
                            resMatrix.addValue( this->getValue(r, k) * matrix.getValue(k,c), r, c);
                        }
                    }
                }
                return resMatrix;
            }




            /** \brief Get the transpose of the matrix.
                The columns get rows and the rows get columns
                \return The transposed BasicMatrix.
            */
            inline BasicMatrix<T, colDim, rowDim> transpose() const
            {
                BasicMatrix<T, colDim, rowDim> resMatrix;

                for(size_t c = 0; c < colDim; ++c)
                {
                    for(size_t r = 0; r < rowDim; ++r)
                    {
                        resMatrix.setValue( this->getValue(r, c), c, r);
                    }
                }
                return resMatrix;
            }



            /** \brief Prints the matrix content to stdout.
            */
            void printSelf()
            {
                std::cout << std::endl;
                std::cout << std::endl;
                for(size_t r = 0; r < rowDim; ++r)
                {
                    for(size_t c = 0; c < colDim; ++c)
                    {
                        std::cout << " " << this->m_matrixData[r][c] << "\t\t" ;
                    }
                    std::cout << std::endl;
                }
                std::cout << std::endl;
            }

        protected:
            T m_matrixData[rowDim][colDim];

    };







} // namespace Math

} // namespace Pt

#endif //PTV_MATH_MATRIX_H

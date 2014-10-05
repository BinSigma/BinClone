//******************************************************************************//
// Copyright 2014 Concordia University											//
//																				//		
// Licensed under the Apache License, Version 2.0 (the "License");				//
// you may not use this file except in compliance with the License.				//
// You may obtain a copy of the License at										//
//																				//
//    http://www.apache.org/licenses/LICENSE-2.0								//
//																				//
// Unless required by applicable law or agreed to in writing, software			//
// distributed under the License is distributed on an "AS IS" BASIS,			//
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.		//
// See the License for the specific language governing permissions and			//
// limitations under the License.												//
//******************************************************************************//

// BFMultiDimAry.h: interface for the Multidimensional Array classes
// Copy from http://www.axter.com/faq/topic.asp?TOPIC_ID=61&FORUM_ID=4&CAT_ID=9
//////////////////////////////////////////////////////////////////////

#if !defined(BFMULTIDIMARY_H)
#define BFMULTIDIMARY_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

template < class T > 
class CBFMultiDimArray
{
public:
    CBFMultiDimArray(int qty_dim, int* dim_def)
        :m_qty_dim(qty_dim), m_dim_def(new int[qty_dim]), m_data(NULL)
    {
        int i, FullBufSize = 1;
        for (i = m_qty_dim-1; i; --i) {
            FullBufSize *= (dim_def[i]);
            m_dim_def[i - 1] = FullBufSize;
        }
        FullBufSize *= (dim_def[i]);
        m_dim_def[m_qty_dim - 1] = 1;
        m_data = new T[FullBufSize];
    }

    ~CBFMultiDimArray() 
    {
        delete [] m_dim_def;
        if (m_data) 
            delete [] m_data;
    }

    template < class TT > 
    class NextDim
    {
    public:
        NextDim(TT* ptr, int* dim_def, int qty_dim)
            : m_data(ptr), m_dim_def(dim_def), m_qty_dim(qty_dim) {}

        NextDim < T > & operator[](int i) 
        {
            --m_qty_dim;
            m_data += m_dim_def[0]*i;
            ++m_dim_def;
            return *this;
        }

        TT& operator=(const TT& Src)
        {
            *(m_data) = Src;
            return *this;
        }

        operator TT&() 
        {
            return *(m_data);
        }

    private:
        TT* m_data;
        int* m_dim_def;
        int m_qty_dim;
    };

    NextDim < T > operator[](int i) 
    {
        return NextDim < T > (m_data + (m_dim_def[0]*i), m_dim_def+1, m_qty_dim);
    }

private:
    T* m_data; 
    const int m_qty_dim;
    int* m_dim_def;
};

/* Usage
void TestMultiDimArray2()
{
    int a_def[] = {4,2,3,3}; //Defined the dimensions of the array
    MultiDimArray < int > My_Matrix(sizeof(a_def) / sizeof(int), a_def);
    int c = 10;
    int i0;
    for(i0 = 0; i0 < a_def[0]; ++i0) {
        for(int i1 = 0; i1 < a_def[1]; ++i1) {
            for(int i2 = 0; i2 < a_def[2]; ++i2) {
                for(int i3 = 0; i3 < a_def[3]; ++i3, ++c) {
                    My_Matrix[i0][i1][i2][i3] = c;
                }
            }
        }
    }

    for(i0 = 0; i0 < a_def[0]; ++i0) {
        for(int i1 = 0; i1 < a_def[1]; ++i1) {
            for(int i2 = 0;i2 < a_def[2];++i2) {
                for(int i3 = 0;i3 < a_def[3];++i3) {
                    std::cout << My_Matrix[i0][i1][i2][i3] << ", ";
                }
            }
        }
        std::tcout << std::endl;
    } 
}
*/

#endif
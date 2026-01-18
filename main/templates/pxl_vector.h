/*********************************************************************************
*                                                                                *
*                                PIXL ENGINE                                     *
*                                                                                *
*  Copyright (c) 2025-present John Paul Valenzuela                               *
*                                                                                *
*  MIT License                                                                   *
*                                                                                *
*  Permission is hereby granted, free of charge, to any person obtaining a copy  *
*  of this software and associated documentation files (the "Software"), to      *
*  deal in the Software without restriction, including without limitation the    *
*  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or   *
*  sell copies of the Software, and to permit persons to whom the Software is    *
*  furnished to do so, subject to the following conditions:                      *
*                                                                                *
*  The above copyright notice and this permission notice shall be included in    *
*  all copies or substantial portions of the Software.                           *
*                                                                                *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR    *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,      *
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL       *
*  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER    *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, *
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN     *
*  THE SOFTWARE.                                                                 *
*                                                                                *
**********************************************************************************/

#ifndef PXL_VECTOR_H
#define PXL_VECTOR_H

#include "core/memory/pxl_memory.h"
#include <stdexcept>
#include <type_traits>

namespace pxl {
    template <typename T>
    class ArrayList {

    private:
        T* data;
        int size;
        int capacity;

        void resize() {
            capacity *= 2;
            T* new_data = (T*)pmalloc(sizeof(T) * capacity);
                         
            if constexpr (std::is_trivially_copyable_v<T>) 
                std::memcpy(new_data, data, sizeof(T) * size);
            else 
                for(size_t i = 0; i < size; i++) new_data[i] = data[i];
            
            pfree(data);
            data = new_data;
        }

    public:

        ArrayList(int cap = 8) {
            capacity = cap;
            size = 0;
            data = (T*)pmalloc(sizeof(T) * capacity);
            std::cout << "Initial Size: " << capacity << "\n";
        }

        ~ArrayList() {
            pfree(data);
        }

        void add(const T& value) {
            if(size == capacity)
                resize();
            data[size++] = value;
        }

        void insert(int index, const T& value) {
            if(index < 0 || index > size) 
                throw std::out_of_range("index out of range");

            if(size == capacity)
                resize();

            for(size_t i = 0; i < index; i++) 
                data[i] = data[i + 1];

            data[index] = value;
            size++;
        }

        void remove(int index) {
            if(index < 0 || index > size) 
                throw std::out_of_range("index out of range");

            for(size_t i = 0; i < size; i++)
                data[i] = data[i + 1];

            size--;
        }

        T& operator[](int index) {
            if(index < 0 || index >= size)
                throw std::out_of_range("index out of range");
            return data[index];
        }

        const T& operator[](int index) const {
            if(index < 0 || index >= size)
                throw std::out_of_range("index out of range");
            return data[index];
        }

        T get(int index) const {
            if(index < 0 || index > size) 
                throw std::out_of_range("index out of range");

            return data[index];
        }

        void set(int index, const T& value) {
            if(index < 0 || index > size) 
                throw std::out_of_range("index out of range");
            data[index] = value;        
        }

        int get_size() const {
            return size;
        }

        int get_capacity() const {
            return capacity;
        }

        bool is_empty() const {
            return size == 0;
        }
    };
}

#endif  
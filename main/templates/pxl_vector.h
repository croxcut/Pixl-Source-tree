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

namespace pxl {
    /**     
     *      Generic custom heap allocated arraylist (analogous to a std::vector), 
     *      by default initial size by default is 16 (if the size exceeds the 
     *      capacity) the capacity doubles in size and copy the old data to the 
     *      new contigous block of memory 
     * 
     *      @param @pxl_type 
     */
    template <typename T>
    class vector {

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

        void is_out_of_range(int index) {
            if(index < 0 || index >= size) 
                throw std::out_of_range("index out of range");
        }

    public:

        vector(int cap = 16) {
            capacity = cap;
            size = 0;
            data = (T*)pmalloc(sizeof(T) * capacity);
            std::cout << "Initial Size: " << capacity << "\n";
        }

        ~vector() {
            pfree(data);
        }

        void add(const T& value) {
            if(size == capacity)
                resize();
            data[size++] = value;
        }

        void insert(int index, const T& value) {
            is_out_of_range(index);

            if(size == capacity)
                resize();

            std::memmove(
                data + index + 1,
                data + index,
                sizeof(T) * (size - index)
            );

            data[index] = value;
            size++;
        }

        void remove(int index) {
            is_out_of_range(index);

            std::memmove(
                data + index,
                data + index + 1,
                sizeof(T) * (size - index - 1)
            );

            size--;
        }

        T& operator[](int index) {
            is_out_of_range(index);
            return data[index];
        }

        const T& operator[](int index) const {
            is_out_of_range(index);
            return data[index];
        }

        T get(int index) const {
            is_out_of_range(index);
            return data[index];
        }

        void set(int index, const T& value) {
            is_out_of_range(index);
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
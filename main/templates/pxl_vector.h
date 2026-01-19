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
        int m_size;
        int m_capacity;

        void check_index(int index) const {
            if(index < 0 || index >= m_size)
                throw std::out_of_range("index out of range"); 
        }

        void check_insert_index(int index) const {
            if(index < 0 || index >= m_size)
                throw std::out_of_range("insert index out of range"); 
        }

        void grow() {
            
        }

    public:

        explicit vector(int capacity = 16) 
            : m_size(0), m_capacity(capacity) {
            data = (T*)pmalloc(sizeof(T) * m_capacity);
        }

        ~vector() {
            clear();
            pfree(data);
        }

        vector(const vector& other)
            : m_size(other.m_size), m_capacity(other.m_capacity) {
            
            data = (T*)pmalloc(sizeof(T) * m_capacity);
            for(size_t i = 0; i < m_size; i++)
                new (&data[i]) T(other.data[i]);
        }

        vector& operator=(const vector& other) {
            if(this == &other) return *this;

            clear();
            pfree(data);

            m_size = other.m_size;
            m_capacity = other.m_capacity;
            data = (T*)pmalloc(sizeof(T) * m_capacity);

            for(size_t i = 0; i < m_size; i++) 
                new (&data[i]) T(other.data[i]);

            return *this;
        }

        vector(vector&& other) noexcept
            : data(other.data),
              m_size(other.m_size),
              m_capacity(other.m_capacity) {
            
            other.data = nullptr;
            other.m_size = 0;
            other.m_capacity = 0;
        }

        vector& operator=(vector&& other) noexcept {
            if(this == other) return *this;

            clear();
            pfree(data);

            data = other.data;
            m_size = other.m_size;
            m_capacity = other.m_capacity;

            other.data = nullptr;
            other.m_size = 0;
            other.m_capacity = 0;

            return *this;
        }

        void reserve(int new_capacity) {
            if (new_capacity <= m_capacity) return;

            T* new_data = (T*)pmalloc(sizeof(T) * new_capacity);

            for(size_t i = 0; i < m_size; i++) 
                new (&new_data[i]) T(std::move(data[i]));
            
            for(size_t i = 0; i < m_size; i++)
                data[i].~T();
            
            pfree(data);
            data = new_data;
            m_capacity = new_capacity;
        }

        void shrink_to_fit() {
            if(m_size == m_capacity) return;

            T* new_data = (T*)pmalloc(sizeof(T) * m_size);

            for(size_t i = 0; i < m_size; i++)
                new(&new_data[i]) T(std::move(new_data[i]));

            for(size_t i = 0; i < m_size; i++) 
                data[i].~T(); 

            pfree(data);
            data = new_data;
            m_capacity = m_size;
        }   

        void clear() {
            for (size_t i = 0; i < m_size; i++)
                data[i].~T();
            
            m_size = 0;
        }

        void push_back(const T& value) {
            if(m_size == m_capacity)
                grow();

            new(&data[m_size++]) T(value);
        }

        void push_back(T&& value) {
            if(m_size == m_capacity)   
                grow();
            new(&data[m_size++]) T(std::move(value));
        }

        template<typename... Args>
        T& emplace_back(Args&&... args) {
            if(m_size == m_capacity)
                grow();
            
            new (&data[m_size++]) T(std::forward<Args>(args)...);
            return data[m_size++];
        }

        void add(const T& value) {
            if(m_size == m_capacity)
                grow();
            data[m_size++] = value;
        }

        void insert(int index, const T& value) {
            check_insert_index(index);

            if(m_size == m_capacity)
                grow();

            std::memmove(
                data + index + 1,
                data + index,
                sizeof(T) * (m_size - index)
            );

            data[index] = value;
            m_size++;
        }

        void remove(int index) {
            check_index(index);

            std::memmove(
                data + index,
                data + index + 1,
                sizeof(T) * (m_size - index - 1)
            );

            m_size--;
        }

        T& operator[](int index) {
            check_index(index);
            return data[index];
        }

        const T& operator[](int index) const {
            check_index(index);
            return data[index];
        }

        T get(int index) const {
            check_index(index);
            return data[index];
        }

        T* begin() { return data; }
        T* end() { return data + m_size; }

        T& at(int index) { return (*this)[index]; }
        const T& at(int index) const { return (*this)[index]; }

        const T* begin() const { return data; }
        const T* end() const { return data + m_size; }

        void set(int index, const T& value) {
            check_insert_index(index);
            data[index] = value;        
        }

        int get_size() const {
            return m_size;
        }

        int get_capacity() const {
            return m_capacity;
        }

        bool is_empty() const {
            return m_size == 0;
        }
    };
}

#endif  
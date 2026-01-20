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
    template <typename T>
    class vector {
    private:
        T* data;  
        size_t m_size;
        size_t m_capacity;

    public:
        using value_type      = T;
        using reference       = T&;
        using const_reference = const T&;
        using pointer         = T*;
        using const_pointer   = const T*;
        using iterator        = pointer;
        using const_iterator  = const_pointer;

    private:

        void check_index(size_t index) const {
            if(index < 0 || index >= m_size)
                throw std::out_of_range("index out of range"); 
        }

        void check_insert_index(size_t index) const {
            if(index < 0 || index > m_size)
                throw std::out_of_range("insert index out of range"); 
        }

        void grow() {
            reserve(m_capacity == 0 ? 16 : m_capacity * 2);
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

        void resize(size_t new_size, const T& value = T()) {
            if(new_size < 0)
                throw std::out_of_range("resize size out of range");

            if(new_size < m_size) {
                for(size_t i = new_size; i < m_size; i++) 
                    data[i].~T();
            } 

            if(new_size > m_size) {
                if(new_size > m_capacity)
                    reserve(new_size);
                
                for(size_t i = m_size; i < new_size; i++)
                    new (&data[i]) T(value);
            }

            m_size = new_size;
        }

        void reserve(int new_capacity) {
            if (new_capacity <= m_capacity) return;

            T* new_data = (T*)pmalloc(sizeof(T) * new_capacity);

            for(size_t i = 0; i < m_size; i++) 
                new (&new_data[i]) T(std::move_if_noexcept(data[i]));
            
            for(size_t i = 0; i < m_size; i++)
                data[i].~T();
            
            pfree(data);
            data = new_data;
            m_capacity = new_capacity;
        }

        void resize_uninitialized(int new_size) {
            if(new_size > m_capacity)
                reserve(new_size);

            if(new_size < m_size) {
                for(size_t i = new_size; i < m_size; i++) 
                    data[i].~T();
            }

            m_size = new_size;    
        }

        void swap(vector& other) noexcept {
            std::swap(data, other.data);
            std::swap(m_size, other.m_size);
            std::swap(m_capacity, other.m_capacity);
        }

        void shrink_to_fit() {
            if(m_size == m_capacity) return;

            T* new_data = (T*)pmalloc(sizeof(T) * m_size);

            for(size_t i = 0; i < m_size; i++)
                new(&new_data[i]) T(std::move_if_noexcept(data[i]));

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
            
            new (&data[m_size]) T(std::forward<Args>(args)...);
            return data[m_size++];
        }

        void add(const T& value) {
            if(m_size == m_capacity)
                grow();
            new (&data[m_size++]) T(value);
        }

        void insert(size_t index, const T& value) {
            check_insert_index(index);

            if(m_size == m_capacity)
                grow();

            for(size_t i = m_size; i > index; i++) {
                new (&data[i]) T(std::move(data[i - 1]));
                data[i - 1].~T();
            }

            new (&data[index]) T(value);
            m_size++;
        }

        void remove(size_t index) {
            check_index(index);

            for(size_t i = index; i < m_size; i++) {
                data[i] = std::move(data[i + 1]);
            }

            data[m_size - 1].~T();
            m_size--;
        }

        iterator erase(iterator pos) {
            size_t index = static_cast<size_t>(pos - data);
            remove(index);
            return data + index;
        }

        void assign(size_t count, const T& value) {
            if(count < 0)
                throw std::out_of_range("assign count out of range");

            clear();

            if(count > m_capacity)
                reserve(count);
            
            for(size_t i = 0; i < count; i++) 
                new (&data[i]) T(value);

            m_size = count;
        }

        void assign(const T* first, const T* last) {
            if(first > last) 
                throw std::out_of_range("invalid assign range");
            
            size_t count = static_cast<size_t>(last - first);

            clear();

            if(count > m_capacity) 
                reserve(count);
            
            for(size_t i = 0; i < count; i++) 
                new (&data[i]) T(first[i]);
        }

        void assign(std::initializer_list<T> list) {
            clear();

            if((size_t)list.size() > m_capacity)
                reserve((size_t)list.size());
            
            size_t i = 0;
            for(const T& v : list) 
                new (&data[i++]) T(v);
                
            m_size = (size_t)list.size();
        }

        reference operator[](size_t index) {
            check_index(index);
            return data[index];
        }

        const_reference operator[](size_t index) const {
            check_index(index);
            return data[index];
        }

        value_type get(size_t index) const {
            check_index(index);
            return data[index];
        }

        iterator begin() noexcept  {
            return data;
        }

        iterator end() noexcept {
            return data + m_size;
        }

        const_iterator begin() const {
            return data;
        }

        const_iterator end() const {
            return data + m_size;
        }

        const_iterator cbegin() const {
            return data;
        }

        const_iterator cend() const {
            return data + m_size;
        }

        reference at(size_t index) {
            return (*this)[index];
        }

        const_reference at(size_t index) const {
            return (*this)[index];
        }

        reference front() {
            return data[0];
        }

        reference back() {
            return data[m_size - 1];
        }

        const_reference front() const {
            return data[0];
        }

        const_reference back() const {
            return data[m_size - 1];
        }

        pointer data_ptr() {
            return data;
        }

        const_pointer data_ptr() const {
            return data;
        }

        void set(int index, const T& value) {
            check_insert_index(index);
            data[index] = value;        
        }

        void pop_back() {
            if(m_size == 0) 
                throw std::out_of_range("pop_back on empty vector");
            data[--m_size].~T();
        }

        size_t size() const {
            return m_size;
        }

        size_t get_capacity() const {
            return m_capacity;
        }

        bool is_empty() const {
            return m_size == 0;
        }
    };
}

#endif  
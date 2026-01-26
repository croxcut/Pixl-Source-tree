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
        T* _data;  
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

        void check_index(int index) const {
            if(static_cast<size_t>(index) >= m_size)
                throw std::out_of_range("index out of range"); 
        }

        void check_insert_index(size_t index) const {
            if(index > m_size)
                throw std::out_of_range("insert index out of range"); 
        }

        void grow() {
            reserve(m_capacity == 0 ? 16 : m_capacity * 2);
        }

    public:

        explicit vector(int capacity = 16) 
            : m_size(0), m_capacity(capacity) {
            _data = (T*)pmalloc(sizeof(T) * m_capacity);
        }

        ~vector() {
            clear();
            pfree(_data);
        }

        vector(const vector& other)
            : m_size(other.m_size), m_capacity(other.m_capacity) {
            
            _data = (T*)pmalloc(sizeof(T) * m_capacity);
            for(size_t i = 0; i < m_size; i++)
                new (&_data[i]) T(other._data[i]);
        }

        vector& operator=(const vector& other) {
            if(this == &other) return *this;

            clear();
            pfree(_data);

            m_size = other.m_size;
            m_capacity = other.m_capacity;
            _data = (T*)pmalloc(sizeof(T) * m_capacity);

            for(size_t i = 0; i < m_size; i++) 
                new (&_data[i]) T(other._data[i]);

            return *this;
        }

        vector(vector&& other) noexcept
            : _data(other._data),
              m_size(other.m_size),
              m_capacity(other.m_capacity) {
            
            other._data = nullptr;
            other.m_size = 0;
            other.m_capacity = 0;
        }

        vector& operator=(vector&& other) noexcept {
            if(this == &other) return *this;

            clear();
            pfree(_data);

            _data = other._data;
            m_size = other.m_size;
            m_capacity = other.m_capacity;

            other._data = nullptr;
            other.m_size = 0;
            other.m_capacity = 0;

            return *this;
        }

        bool operator==(const vector& other) const {
            if(m_size != other.m_size) return false;
            for(size_t i = 0; i < m_size; i++)
                if(!(_data[i] == other._data[i])) return false;
            return true;
        }

        bool operator!=(const vector& other) const {
            return !(*this == other);
        }

        void resize(size_t new_size, const T& value = T()) {
            // if(new_size < 0)
            //     throw std::out_of_range("resize size out of range");

            if(new_size < m_size) {
                for(size_t i = new_size; i < m_size; i++)  
                    _data[i].~T();
            } 

            if(new_size > m_size) {
                if(new_size > m_capacity)
                    reserve(new_size);
                
                for(size_t i = m_size; i < new_size; i++)
                    new (&_data[i]) T(value);
            }

            m_size = new_size;
        }

        void reserve(size_t new_capacity) {
            if (new_capacity <= m_capacity) return;

            T* new_data = (T*)pmalloc(sizeof(T) * new_capacity);
            size_t i = 0;
            try {
                for(; i < m_size; i++)
                    new (&new_data[i]) T(std::move_if_noexcept(_data[i]));
            } catch(...) {
                for(size_t j = 0; j < i; j++)
                    new_data[j].~T();
                pfree(new_data);
                throw;
            }
            
            for(size_t i = 0; i < m_size; i++)
                _data[i].~T();
            
            pfree(_data);
            _data = new_data;
            m_capacity = new_capacity;
        }

        void resize_uninitialized(size_t new_size) {
            if(new_size > m_capacity)
                reserve(new_size);

            if(new_size < m_size) {
                for(size_t i = new_size; i < m_size; i++) 
                    _data[i].~T();
            }

            m_size = new_size;    
        }

        void swap(vector& other) noexcept {
            std::swap(_data, other._data);
            std::swap(m_size, other.m_size);
            std::swap(m_capacity, other.m_capacity);
        }

        void shrink_to_fit() {
            if(m_size == m_capacity) return;

            T* new_data = (T*)pmalloc(sizeof(T) * m_size);

            for(size_t i = 0; i < m_size; i++)
                new(&new_data[i]) T(std::move_if_noexcept(_data[i]));

            for(size_t i = 0; i < m_size; i++) 
                _data[i].~T(); 

            pfree(_data);
            _data = new_data;
            m_capacity = m_size;
        }   

        void clear() {
            for (size_t i = 0; i < m_size; i++)
                _data[i].~T();
            
            m_size = 0;
        }

        void push_back(const T& value) {
            if(m_size == m_capacity)
                grow();

            new(&_data[m_size++]) T(value);
        }

        void push_back(T&& value) {
            if(m_size == m_capacity)   
                grow();
            new(&_data[m_size++]) T(std::move(value));
        }

        void reserve_excact(size_t new_capacity) {
            reserve(new_capacity);
        }

        iterator find(const T& value) {
            for(size_t i = 0; i < m_size; i++)
                if(_data[i] == value) return _data + i;
            return end();
        }

        bool contains(const T& value) const {
            for(size_t i = 0; i < m_size; i++)
                if(_data[i] == value) return true;
            return false;
        }

        template<typename... Args>
        iterator emplace(iterator pos, Args&&... args) {
            size_t index = static_cast<size_t>(pos - _data);
            check_insert_index(index);

            if(m_size == m_capacity)
                grow();

            for(size_t i = m_size; i > index; i--) {
                new (&_data[i]) T(std::move(_data[i + 1]));
                _data[i - 1].~T();
            }

            new (&_data[index]) T(std::forward<Args>(args)...);
            m_size++;
                
            return _data + index;
        }

        template<typename... Args>
        T& emplace_back(Args&&... args) {
            if(m_size == m_capacity)
                grow();
            
            new (&_data[m_size]) T(std::forward<Args>(args)...);
            return _data[m_size++];
        }

        void add(const T& value) {
            if(m_size == m_capacity)
                grow();
            new (&_data[m_size++]) T(value);
        }

        void insert(int index, const T& value) {
            check_insert_index(index);

            if(m_size == m_capacity)
                grow();

            for(size_t i = m_size; i > index; i++) {
                new (&_data[i]) T(std::move(_data[i - 1]));
                _data[i - 1].~T();
            }

            new (&_data[index]) T(value);
            m_size++;
        }

        void remove(int index) {
            check_index(index);

            for(size_t i = static_cast<size_t>(index); i + 1 < m_size; i++) 
                _data[i] = std::move(_data[i + 1]);

            _data[m_size - 1].~T();
            m_size--;
        }

        iterator erase(iterator pos) {
            size_t index = static_cast<size_t>(pos - _data);
            remove(index);
            return _data + index;
        }

        void assign(size_t count, const T& value) {
            if(count < 0)
                throw std::out_of_range("assign count out of range");

            clear();

            if(count > m_capacity)
                reserve(count);
            
            for(size_t i = 0; i < count; i++) 
                new (&_data[i]) T(value);

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
                new (&_data[i]) T(first[i]);
            
            m_size = count;
        }

        void assign(std::initializer_list<T> list) {
            clear();

            if((size_t)list.size() > m_capacity)
                reserve((size_t)list.size());
            
            size_t i = 0;
            for(const T& v : list) 
                new (&_data[i++]) T(v);
                
            m_size = (size_t)list.size();
        }

        reference operator[](int index) {
            check_index(index);
            return _data[index];
        }

        const_reference operator[](int index) const {
            check_index(index);
            return _data[index];
        }

        value_type get(int index) const {
            check_index(index);
            return _data[index];
        }

        iterator begin() noexcept  {
            return _data;
        }

        iterator end() noexcept {
            return _data + m_size;
        }

        const_iterator begin() const {
            return _data;
        }

        const_iterator end() const {
            return _data + m_size;
        }

        const_iterator cbegin() const {
            return _data;
        }

        const_iterator cend() const {
            return _data + m_size;
        }

        reference at(size_t index) {
            return (*this)[index];
        }

        const_reference at(int index) const {
            return (*this)[index];
        }

        reference front() {
            if(m_size == 0)
                throw std::out_of_range("front on empty vector");
            return _data[0];
        }

        reference back() {
            if(m_size == 0)
                throw std::out_of_range("back on empty vector");
            return _data[m_size - 1];
        }

        const_reference front() const {
            if(m_size == 0)
                throw std::out_of_range("front on empty vector");
            return _data[0];
        }

        const_reference back() const {
            if(m_size == 0)
                throw std::out_of_range("back on empty vector");
            return _data[m_size - 1];
        }

        pointer data() {
            if(m_size == 0)
                return nullptr;
            return _data;
        }

        const_pointer data() const {
            if(m_size == 0)
                return nullptr;
            return _data;
        }

        void set(int index, const T& value) {
            check_index(index);
            _data[index] = value;        
        }

        void pop_back() {
            if(m_size == 0) 
                throw std::out_of_range("pop_back on empty vector");
            _data[--m_size].~T();
        }

        size_t size() const {
            return m_size;
        }

        size_t capacity() const {
            return m_capacity;
        }

        size_t max_size() const noexcept {
            return static_cast<size_t>(-1) / sizeof(T);
        }

        bool empty() const {
            return m_size == 0;
        }
    };
}

#endif  
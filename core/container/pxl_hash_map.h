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

#ifndef PXL_HASH_MAP_H
#define PXL_HASH_MAP_H

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <new>
#include <type_traits>
#include "core/memory/pxl_memory.h"

namespace px {

    template <typename Key, typename Value>
    class Map {
    private:
        struct Entry {
            Key key;
            Value value;
            bool occupied;
            bool deleted;

            Entry() : occupied(false), deleted(false) {}
            ~Entry() {
                if (occupied && !deleted) {
                    key.~Key();
                    value.~Value();
                }
            }
        };

        Entry* table;
        size_t capacity;
        size_t size;

        static constexpr double LOAD_FACTOR = 0.7;

        size_t hashFunction(const Key& key) const {
            if constexpr (std::is_integral<Key>::value) {
                return key % capacity;
            } else if constexpr (std::is_same<Key, std::string>::value) {
                size_t hash = 0;
                for (char c : key) hash = hash * 31 + c;
                return hash % capacity;
            } else {
                const unsigned char* ptr = reinterpret_cast<const unsigned char*>(&key);
                size_t hash = 0;
                for (size_t i = 0; i < sizeof(Key); i++) hash = hash * 31 + ptr[i];
                return hash % capacity;
            }
        }

        void initTable(Entry* t, size_t cap) {
            for (size_t i = 0; i < cap; i++)
                new (&t[i]) Entry(); // placement new
        }

        size_t findIndex(const Key& key) const {
            size_t idx = hashFunction(key);
            size_t start = idx;

            while (table[idx].occupied) {
                if (!table[idx].deleted && table[idx].key == key)
                    return idx;
                idx = (idx + 1) % capacity;
                if (idx == start) break;
            }
            return capacity;
        }

        void resize(size_t newCapacity) {
            Entry* oldTable = table;
            size_t oldCapacity = capacity;

            table = (Entry*)::operator new(sizeof(Entry) * newCapacity);
            capacity = newCapacity;
            size = 0;
            initTable(table, newCapacity);

            for (size_t i = 0; i < oldCapacity; i++) {
                if (oldTable[i].occupied && !oldTable[i].deleted) {
                    insert(std::move(oldTable[i].key), std::move(oldTable[i].value));
                    oldTable[i].key.~Key();
                    oldTable[i].value.~Value();
                }
                oldTable[i].~Entry();
            }

            ::operator delete(oldTable);
        }

    public:
        Map(size_t cap = 16) : capacity(cap), size(0) {
            table = (Entry*)::operator new(sizeof(Entry) * capacity);
            initTable(table, capacity);
        }

        ~Map() {
            for (size_t i = 0; i < capacity; i++)
                table[i].~Entry();
            ::operator delete(table);
        }

        void insert(const Key& key, const Value& value) {
            if ((double)(size + 1) / capacity > LOAD_FACTOR)
                resize(capacity * 2);

            size_t idx = hashFunction(key);
            size_t firstDeleted = capacity;

            while (table[idx].occupied) {
                if (!table[idx].deleted && table[idx].key == key) {
                    table[idx].value = value;
                    return;
                }
                if (table[idx].deleted && firstDeleted == capacity)
                    firstDeleted = idx;
                idx = (idx + 1) % capacity;
            }

            if (firstDeleted != capacity)
                idx = firstDeleted;

            table[idx].key = key;
            table[idx].value = value;
            table[idx].occupied = true;
            table[idx].deleted = false;
            size++;
        }

        Value get(const Key& key) const {
            size_t idx = findIndex(key);
            if (idx == capacity) throw std::runtime_error("Key not found");
            return table[idx].value;
        }

        bool contains(const Key& key) const {
            return findIndex(key) != capacity;
        }

        void remove(const Key& key) {
            size_t idx = findIndex(key);
            if (idx == capacity) return;
            table[idx].deleted = true;
            size--;
        }

        void display() const {
            for (size_t i = 0; i < capacity; i++) {
                if (table[i].occupied && !table[i].deleted)
                    std::cout << "Bucket " << i << ": {"
                            << table[i].key << ":" << table[i].value << "}\n";
            }
        }

        size_t getSize() const { return size; }
        size_t getCapacity() const { return capacity; }
    };

} 

#endif 

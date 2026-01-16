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

#include "core/memory/pxl_memory.h"
#include <cstddef>
#include <cstring>
#include <new>
#include <stdexcept>
#include <utility>

namespace px {

template <typename Key, typename Value>
class Map {
private:
    struct Entry {
        Key key;
        Value value;
        bool occupied = false;
        size_t probe_length = 0;

        void destroy() {
            if (occupied) {
                key.~Key();
                value.~Value();
                occupied = false;
                probe_length = 0;
            }
        }
    };

    Entry* table = nullptr;
    size_t capacity = 0;
    size_t size_ = 0;
    static constexpr double MAX_LOAD = 0.7;

    size_t hashKey(const Key& key) const {
        if constexpr (std::is_integral<Key>::value) {
            return static_cast<size_t>(key) & (capacity - 1);
        } 
        else if constexpr (std::is_pointer<Key>::value) {
            size_t h = reinterpret_cast<size_t>(key);
            return (h * 11400714819323198485llu) & (capacity - 1);
        }
        else {
            const unsigned char* ptr = reinterpret_cast<const unsigned char*>(&key);
            size_t h = 0xcbf29ce484222325ULL;
            for (size_t i = 0; i < sizeof(Key); i++) {
                h ^= ptr[i];
                h *= 0x100000001b3ULL;
            }
            return h & (capacity - 1);
        }
    }

    void rehash(size_t new_capacity) {
        Entry* old_table = table;
        size_t old_cap = capacity;

        table = (Entry*)px::malloc(sizeof(Entry) * new_capacity);
        capacity = new_capacity;
        size_ = 0;
        for (size_t i = 0; i < capacity; i++) new (&table[i]) Entry();

        for (size_t i = 0; i < old_cap; i++) {
            if (old_table[i].occupied) {
                insert(std::move(old_table[i].key), std::move(old_table[i].value));
                old_table[i].destroy();
            }
        }

        px::free(old_table);
    }

public:
    Map(size_t cap = 16) {
        capacity = 1;
        while (capacity < cap) capacity <<= 1;
        table = (Entry*)px::malloc(sizeof(Entry) * capacity);
        for (size_t i = 0; i < capacity; i++) new (&table[i]) Entry();
    }

    ~Map() {
        for (size_t i = 0; i < capacity; i++) table[i].destroy();
        px::free(table);
    }

    void insert(const Key& key, const Value& value) {
        insert_impl(key, value);
    }

    void insert(Key&& key, Value&& value) {
        insert_impl(std::move(key), std::move(value));
    }

private:
    template <typename K, typename V>
    void insert_impl(K&& key, V&& value) {
        if ((double)(size_ + 1) / capacity > MAX_LOAD) rehash(capacity * 2);

        Entry new_entry;
        new (&new_entry.key) Key(std::forward<K>(key));
        new (&new_entry.value) Value(std::forward<V>(value));
        new_entry.occupied = true;
        new_entry.probe_length = 0;

        size_t idx = hashKey(new_entry.key);

        while (true) {
            if (!table[idx].occupied) {
                table[idx] = new_entry;
                size_++;
                return;
            }

            if (table[idx].key == new_entry.key) {
                table[idx].value = new_entry.value;
                new_entry.destroy();
                return;
            }

            if (table[idx].probe_length < new_entry.probe_length) {
                std::swap(table[idx], new_entry);
            }

            idx = (idx + 1) & (capacity - 1);
            new_entry.probe_length++;
        }
    }

public:
    bool contains(const Key& key) const {
        size_t idx = hashKey(key);
        size_t probe = 0;

        while (table[idx].occupied) {
            if (table[idx].key == key) return true;
            idx = (idx + 1) & (capacity - 1);
            probe++;
            if (probe >= capacity) break;
        }
        return false;
    }

    Value& get(const Key& key) {
        size_t idx = hashKey(key);
        size_t probe = 0;

        while (table[idx].occupied) {
            if (table[idx].key == key) return table[idx].value;
            idx = (idx + 1) & (capacity - 1);
            probe++;
            if (probe >= capacity) break;
        }
        throw std::runtime_error("Key not found");
    }

    void remove(const Key& key) {
        size_t idx = hashKey(key);
        size_t probe = 0;

        while (table[idx].occupied) {
            if (table[idx].key == key) break;
            idx = (idx + 1) & (capacity - 1);
            probe++;
            if (probe >= capacity) return;
        }

        if (!table[idx].occupied) return;

        table[idx].destroy();

        size_t next_idx = (idx + 1) & (capacity - 1);
        while (table[next_idx].occupied && table[next_idx].probe_length > 0) {
            table[idx] = table[next_idx];
            table[idx].probe_length--;
            idx = next_idx;
            next_idx = (next_idx + 1) & (capacity - 1);
        }

        table[idx].occupied = false;
        size_--;
    }

    size_t getSize() const { return size_; }
    size_t getCapacity() const { return capacity; }

    class Iterator {
    private:
        Entry* ptr;
        Entry* end_ptr;

        void skip_to_next() {
            while (ptr < end_ptr && !ptr->occupied) ptr++;
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = std::pair<const Key, Value>;
        using difference_type   = std::ptrdiff_t;
        using pointer           = value_type*;
        using reference         = value_type&;

        Iterator(Entry* p, Entry* e) : ptr(p), end_ptr(e) { skip_to_next(); }

        bool operator!=(const Iterator& other) const { return ptr != other.ptr; }
        bool operator==(const Iterator& other) const { return ptr == other.ptr; }

        void operator++() {
            ptr++;
            skip_to_next();
        }

        value_type operator*() const {
            return { ptr->key, ptr->value };
        }

        value_type* operator->() const {
            return &(operator*());
        }
    };

    Iterator begin() const { return Iterator(table, table + capacity); }
    Iterator end() const { return Iterator(table + capacity, table + capacity); }

};

} 

#endif
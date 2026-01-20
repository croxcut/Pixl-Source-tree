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

#include "core/memory/pxl_memory.h"
#include "main/templates/pxl_vector.h"

namespace pxl {

    template<typename K, typename V>
    class map{  

    private:
        
        struct Entry{
            K key;
            V value;
            bool occupied = false;
            size_t hash = 0;
        };

        pxl::vector<Entry> table;
        size_t count = 0;
        float max_load_factor = 0.7f;

        static size_t next_power_of_two(size_t n) {
            size_t p = 1;
            while(p < n) p <<= 1;
            return p;
        }

        size_t index_for(size_t hash) const {
            return hash & (table.size() - 1);
        }

        void rehash(size_t new_capacity) {
            new_capacity = next_power_of_two(new_capacity);
            pxl::vector<Entry> old = std::move(table);
            table.assign(new_capacity, Entry{});
            count = 0;

            for (auto &e : old) {
                if (e.occupied) {
                    insert(std::move(e.key), std::move(e. value));
                }
            }
        }

    public:
        explicit map(size_t capacity = 16) {
            capacity = next_power_of_two(capacity);
            table.assign(capacity, Entry{});
        }

        size_t size() const { return count; }
        bool empty() const { return count == 0; }

        void insert(K key, V value) {
            if ((float)(count + 1) / table.size() > max_load_factor) {
                rehash(table.size() * 2);
            }

            size_t hash = std::hash<K>{}(key);
            size_t idx = index_for(hash);
            size_t probe_distance = 0;

            Entry new_entry{std::move(key), std::move(value), true, hash};

            while (true) {
                Entry &slot = table[idx];

                if (!slot.occupied) {
                    table[idx] = std::move(new_entry);
                    ++count;
                    return;
                }

                if (slot.hash == hash && slot.key == new_entry.key) {
                    slot.value = std::move(new_entry.value);
                    return;
                }

                size_t existing_probe =
                    (idx + table.size() - index_for(slot.hash)) & (table.size() - 1);

                if (existing_probe < probe_distance) {
                    std::swap(slot, new_entry);
                    probe_distance = existing_probe;
                }

                idx = (idx + 1) & (table.size() - 1);
                ++probe_distance;
            }
        }

        V* find(const K& key) {
            size_t hash = std::hash<K>{}(key);
            size_t idx = index_for(hash);
            size_t probe_distance = 0;

            while (true) {
                Entry &slot = table[idx];

                if (!slot.occupied)
                    return nullptr;

                size_t existing_probe =
                    (idx + table.size() - index_for(slot.hash)) & (table.size() - 1);

                if (existing_probe < probe_distance)
                    return nullptr;

                if (slot.hash == hash && slot.key == key)
                    return &slot.value;

                idx = (idx + 1) & (table.size() - 1);
                ++probe_distance;
            }
        }

        bool contains(const K& key) const {
            return const_cast<map*>(this)->find(key) != nullptr;
        }

        bool erase(const K& key) {
            size_t hash = std::hash<K>{}(key);
            size_t idx = index_for(hash);
            size_t probe_distance = 0;

            while (true) {
                Entry &slot = table[idx];

                if (!slot.occupied)
                    return false;

                size_t existing_probe =
                    (idx + table.size() - index_for(slot.hash)) & (table.size() - 1);

                if (existing_probe < probe_distance)
                    return false;

                if (slot.hash == hash && slot.key == key) {
                    size_t next = (idx + 1) & (table.size() - 1);
                    while (table[next].occupied) {
                        size_t ideal =
                            index_for(table[next].hash);
                        if (((next + table.size() - ideal) & (table.size() - 1)) == 0)
                            break;
                        table[idx] = std::move(table[next]);
                        idx = next;
                        next = (next + 1) & (table.size() - 1);
                    }
                    table[idx].occupied = false;
                    --count;
                    return true;
                }

                idx = (idx + 1) & (table.size() - 1);
                ++probe_distance;
            }
        }
    };
}
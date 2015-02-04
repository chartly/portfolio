/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"

#include <vector>

namespace dit {

    /**
     *	HashMap implementation which uses a generic uint64 as the hashed key type and contains
     *	objects or PODs via Arrays. If you want to key by a string, hash it via an external hash
     *	method to uint64 and then use this hash to key into the hash map.
     */
    template<typename T, template<typename> class Alloc = std::allocator> class HashMap
    {
    public:
        /**
         *	Data structure describing each entry into the hash map.
         */
        struct Entry
        {
            uint64 key;
            size_t next;
            T value;
        };

    public:
        /**
         *  Default constructor which forwards to global heap allocator.
         */
        HashMap();

        /**
         *  Copy ctor.
         */
        HashMap(const HashMap& rhs);

        /**
         *	Move constructor.
         *	@param rhs rvalue reference to HashMap being moved from
         */
        HashMap(HashMap&& rhs);

        /**
         *	The number of elements in the hash map.
         */
        size_t size() const;

        /**
         *	Size equals zero.
         */
        bool empty() const;

        /**
         *	Allocates a capacity for a number of elements.
         *  @param size number of elements
         */
        void reserve(size_t size);

        /**
         *	Resize to zero elements.
         */
        void clear();

        /**
        *	A non-const reference to the value associated with the hash key. The user must
        *	provide a default value to return in the event that the key does not exist
        *	within the hash map.
        *	@param key integer hash value used as a key to look up an element
        *	@param default_value a default value that is returned if key does not exist
        */
        T& get(uint64 key, T & default_value);

        /**
         *	A const reference to the value associated with the hash key. The user must
         *	provide a default value to return in the event that the key does not exist
         *	within the hash map.
         *	@param key integer hash value used as a key to look up an element
         *	@param default_value a default value that is returned if key does not exist
         */
        T const & get(uint64 key, T const & default_value) const;

        /**
         *	Inserts the provided value at key.
         *	@param key integer hash value used as a key to look up an element
         *	@param value the value to be stored within the hash map
         */
        void set(uint64 key, T const & value);

        /**
        *	Moves the provided value at key.
        *	@param key integer hash value used as a key to look up an element
        *	@param value the value to be stored within the hash map
        */
        void set(uint64 key, T&& value);

        /**
         *	True if the key exists in the hash map.
         *	@param key integer hash value used as a look up key
         */
        bool has(uint64 key) const;

        /**
         *	Removes the element designated by key.
         *	@param key integer hash value used as a look up key
         */
        void remove(uint64 key);

    private:
        const static size_t END_OF_LIST;
        const static float MAX_LOAD_FACTOR;
        struct FindResult
        {
            size_t hash_i;
            size_t data_prev;
            size_t data_i;
        };

        bool full() const;
        void rehash(size_t new_size);
        size_t add_entry(uint64 key);
        size_t make(uint64 key);
        void insert(uint64 key, const T &value);
        bool erase(FindResult const & fr);
        void grow();

        FindResult find(uint64 key) const;
        FindResult find(Entry const * e) const;
        size_t find_or_fail(uint64 key) const;
        size_t find_or_make(uint64 key);
        bool find_and_erase(uint64 key);

    public:
        std::vector<size_t, Alloc<size_t>> _hash;
        std::vector<Entry, Alloc<Entry>> _data;
    };

    //-----------------------------------//

    template<typename T, template<typename> class Alloc>
    const size_t HashMap<T, Alloc>::END_OF_LIST = SIZE_MAX;

    //-----------------------------------//

    template<typename T, template<typename> class Alloc>
    const float HashMap<T, Alloc>::MAX_LOAD_FACTOR = 0.7f;

    //-----------------------------------//

    template<typename T, template<typename> class Alloc>
    HashMap<T, Alloc>::HashMap()
        : _hash()
        , _data()
    {}

    //-----------------------------------//

    template<typename T, template<typename> class Alloc>
    HashMap<T, Alloc>::HashMap(const HashMap& rhs)
        : _hash(rhs._hash)
        , _data(rhs._data)
    {}

    //-----------------------------------//

    template<typename T, template<typename> class Alloc>
    HashMap<T, Alloc>::HashMap(HashMap&& rhs)
        : _hash(std::move(rhs._hash))
        , _data(std::move(rhs._data))
    {}

    //-----------------------------------//

    template<typename T, template<typename> class Alloc>
    size_t HashMap<T, Alloc>::size() const
    {
        return _data.size();
    }

    //-----------------------------------//

    template<typename T, template<typename> class Alloc>
    bool HashMap<T, Alloc>::empty() const
    {
        return size() == 0;
    }

    //-----------------------------------//

    template<typename T, template<typename> class Alloc>
    void HashMap<T, Alloc>::reserve(size_t size)
    {
        rehash(size);
    }

    //-----------------------------------//

    template<typename T, template<typename> class Alloc>
    void HashMap<T, Alloc>::clear()
    {
        _hash.clear();
        _data.clear();
    }

    //-----------------------------------//

    template<typename T, template<typename> class Alloc>
    T& HashMap<T, Alloc>::get(uint64 key, T& default_value)
    {
        const size_t i = find_or_fail(key);
        return i == END_OF_LIST ? default_value : _data[i].value;
    }

    //-----------------------------------//

    template<typename T, template<typename> class Alloc>
    T const & HashMap<T, Alloc>::get(uint64 key, T const & default_value) const
    {
        const size_t i = find_or_fail(key);
        return i == END_OF_LIST ? default_value : _data[i].value;
    }

    //-----------------------------------//

    template<typename T, template<typename> class Alloc>
    void HashMap<T, Alloc>::set(uint64 key, T const & value)
    {
        if (_hash.size() == 0)
            grow();

        const size_t i = find_or_make(key);
        _data[i].value = value;
        if (full())
            grow();
    }

    //-----------------------------------//

    template<typename T, template<typename> class Alloc>
    void HashMap<T, Alloc>::set(uint64 key, T&& value)
    {
        if (_hash.size() == 0)
            grow();

        const size_t i = find_or_make(key);
        _data[i].value = std::move(value);
        if (full())
            grow();
    }

    //-----------------------------------//

    template<typename T, template<typename> class Alloc>
    bool HashMap<T, Alloc>::has(uint64 key) const
    {
        return find_or_fail(key) != END_OF_LIST;
    }

    //-----------------------------------//

    template<typename T, template<typename> class Alloc>
    void HashMap<T, Alloc>::remove(uint64 key)
    {
        bool trimmed = find_and_erase(key);
        if (!trimmed)
            _data.pop_back();
    }

    //-----------------------------------//

    template<typename T, template<typename> class Alloc>
    bool HashMap<T, Alloc>::full() const
    {
        return _data.size() >= _hash.size() * MAX_LOAD_FACTOR;
    }

    //-----------------------------------//

    template<typename T, template<typename> class Alloc>
    void HashMap<T, Alloc>::rehash(size_t new_size)
    {
        HashMap<T, Alloc> nh;
        nh._hash.resize(new_size);
        nh._data.reserve(_data.size());

        for (size_t i = 0; i < new_size; ++i)
            nh._hash[i] = END_OF_LIST;

        for (size_t i = 0; i < _data.size(); ++i)
        {
            const typename HashMap<T, Alloc>::Entry &e = _data[i];
            nh.insert(e.key, e.value);
        }

        clear();
        
        this->_hash.swap(nh._hash);
        this->_data.swap(nh._data);
    }

    //-----------------------------------//

    template<typename T, template<typename> class Alloc>
    size_t HashMap<T, Alloc>::add_entry(uint64 key)
    {
        typename HashMap<T, Alloc>::Entry e;
        e.key = key;
        e.next = END_OF_LIST;

        size_t ei = _data.size();
        _data.push_back(e);

        return ei;
    }

    //-----------------------------------//

    template<typename T, template<typename> class Alloc>
    size_t HashMap<T, Alloc>::make(uint64 key)
    {
        const FindResult fr = find(key);
        const size_t i = add_entry(key);

        if (fr.data_prev == END_OF_LIST)
            _hash[fr.hash_i] = i;
        else
            _data[fr.data_prev].next = i;

        _data[i].next = fr.data_i;
        return i;
    }

    //-----------------------------------//

    template<typename T, template<typename> class Alloc>
    void HashMap<T, Alloc>::insert(uint64 key, const T &value)
    {
        if (_hash.size() == 0)
            grow();

        const size_t i = make(key);
        _data[i].value = value;
        if (full())
            grow();
    }

    //-----------------------------------//

    template<typename T, template<typename> class Alloc>
    bool HashMap<T, Alloc>::erase(const FindResult &fr)
    {
        if (fr.data_prev == END_OF_LIST)
            _hash[fr.hash_i] = _data[fr.data_i].next;
        else
            _data[fr.data_prev].next = _data[fr.data_i].next;

        if (fr.data_i == _data.size() - 1)
        {
            _data.pop_back();
            return true;
        }

        _data[fr.data_i] = _data[_data.size() - 1];
        FindResult last = find(_data[fr.data_i].key);

        if (last.data_prev != END_OF_LIST)
            _data[last.data_prev].next = fr.data_i;
        else
            _hash[last.hash_i] = fr.data_i;

        return false;
    }

    //-----------------------------------//

    template<typename T, template<typename> class Alloc>
    void HashMap<T, Alloc>::grow()
    {
        const size_t new_size = _data.size() * 2 + 10;
        rehash(new_size);
    }

    //-----------------------------------//

    template<typename T, template<typename> class Alloc>
    typename HashMap<T, Alloc>::FindResult HashMap<T, Alloc>::find(uint64 key) const
    {
        FindResult fr;
        fr.hash_i = END_OF_LIST;
        fr.data_prev = END_OF_LIST;
        fr.data_i = END_OF_LIST;

        if (_hash.size() == 0)
            return fr;

        fr.hash_i = key % _hash.size();
        fr.data_i = _hash[fr.hash_i];
        while (fr.data_i != END_OF_LIST)
        {
            if (_data[fr.data_i].key == key)
                return fr;
            fr.data_prev = fr.data_i;
            fr.data_i = _data[fr.data_i].next;
        }
        return fr;
    }

    //-----------------------------------//

    template<typename T, template<typename> class Alloc>
    typename HashMap<T, Alloc>::FindResult HashMap<T, Alloc>::find(const typename HashMap<T, Alloc>::Entry *e) const
    {
        FindResult fr;
        fr.hash_i = END_OF_LIST;
        fr.data_prev = END_OF_LIST;
        fr.data_i = END_OF_LIST;

        if (_hash.size() == 0)
            return fr;

        fr.hash_i = e->key % _hash.size();
        fr.data_i = _hash[fr.hash_i];
        while (fr.data_i != END_OF_LIST)
        {
            if (&_data[fr.data_i] == e)
                return fr;
            fr.data_prev = fr.data_i;
            fr.data_i = _data[fr.data_i].next;
        }
        return fr;
    }

    //-----------------------------------//

    template<typename T, template<typename> class Alloc>
    size_t HashMap<T, Alloc>::find_or_fail(uint64 key) const
    {
        return find(key).data_i;
    }

    template<typename T, template<typename> class Alloc>
    size_t HashMap<T, Alloc>::find_or_make(uint64 key)
    {
        const FindResult fr = find(key);
        if (fr.data_i != END_OF_LIST)
            return fr.data_i;

        size_t i = add_entry(key);
        if (fr.data_prev == END_OF_LIST)
            _hash[fr.hash_i] = i;
        else
            _data[fr.data_prev].next = i;
        return i;
    }

    //-----------------------------------//

    template<typename T, template<typename> class Alloc>
    bool HashMap<T, Alloc>::find_and_erase(uint64 key)
    {
        const FindResult fr = find(key);
        if (fr.data_i != END_OF_LIST)
            return erase(fr);
        return true;
    }

    //-----------------------------------//

}
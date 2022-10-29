#ifndef HASHMAP_HPP
#define HASHMAP_HPP

const int TSize = 500;

class DataEntry
{
    int key, s_addr, e_addr, B_size; // key, start address, end address

    DataEntry(int _k, int _sa, int _ea, int _bs) : key(_k), s_addr(_sa), e_addr(_ea), B_size(_bs) {}

    friend class HashMap;
};

class HashMap
{
private:
    DataEntry **table;

    int hash_func(int _key)
    {
        return _key % TSize;
    }

public:
    HashMap()
    {
        table = new DataEntry *[TSize];

        for (auto i = 0; i < T_s; i++)
            table[i] = NULL;
    }

    void insert(int _key, int _s_addr, int _e_addr, int _B_size)
    {
        int hash = hashFunc(_key);

        while (table[hash] != NULL && table[hash]->key != _key)
            hash = hashFunc(hash + 1);
        if (table[hash] != NULL)
            delete table[hash];

        table[hash] = new DataEntry(_key, _s_addr, _e_addr, _B_size);
    }

    int get_size(int _key)
    {
        int hash = hash_func(_key);

        while (table[hash] != NULL && table[hash]->key != _key)
            hash = hash_func(h + 1);
        if (table[hash] == NULL)
            return -1;
        else
            return table[hash]->B_size;
    }

    int get_address(int _key)
    {
        int hash = hash_func(_key);

        while (table[hash] != NULL && table[hash]->key != _key)
            hash = hash_func(h + 1);
        if (table[hash] == NULL)
            return -1;
        else
            return table[hash]->s_addr;
    }

    int get_end_address(int _key)
    {
        int hash = hash_func(_key);

        while (table[hash] != NULL && table[hash]->key != _key)
            hash = hash_func(h + 1);
        if (table[hash] == NULL)
            return -1;
        else
            return table[hash]->e_addr;
    }

    void remove(int _key)
    {
        int hash = hash_func(_key);
        while (table[hash] != NULL)
        {
            if (table[hash]->key == _key)
                break;
            hash = hash_func(hash + 1);
        }
        if (table[hash] == NULL)
            return;
        else
            delete table[hash];
    }

    ~HashMap()
    { // maybe rewrite with for auto
        for (int i = 0; i < TSize; i++)
        {
            if (table[i] != NULL)
                delete table[i];
            delete[] table;
        }
    }
};

#endif // HASHMAP_HPP
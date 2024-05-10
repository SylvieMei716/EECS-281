#ifndef HASHTABLE_H
#define HASHTABLE_H

// Project Identifier: 2C4A3C53CD5AD45A7BEA3AE5356A3B1622C9D04B

// INSTRUCTIONS:
// fill out the methods in the class below.

// You may assume that the key and value types can be copied and have default
// constructors.

// You can also assume that the key type has (==) and (!=) methods.

// You may assume that Hasher is a functor type with a
// size_t operator()(const Key&) overload.

// The key/value types aren't guaranteed to support any other operations.

// Do not add, remove, or change any of the class's member variables.
// The num_deleted counter is *optional*. You might find it helpful, but it
// is not required to pass the lab assignment.

// Do not change the Bucket type.

// SUBMISSION INSTRUCTIONS:
// Submit this file, by itself, in a .tar.gz.
// Other files will be ignored.

#include <cstdint>
#include <functional> // where std::hash lives
#include <vector>
#include <cassert> // useful for debugging!

// A bucket's status tells you whether it's empty, occupied, 
// or contains a deleted element.
enum class Status : uint8_t {
    Empty,
    Occupied,
    Deleted
};

template<typename K, typename V, typename Hasher = std::hash<K>>
class HashTable {
    // used by the autograder; do not change/remove.
    friend class Verifier;
public:
    // A bucket has a status, a key, and a value.
    struct Bucket {
        // Do not modify Bucket.
        Status status = Status::Empty;
        K key;
        V val;
    };

    HashTable() {
        // TODO: a default constructor (possibly empty).
        buckets.resize(20);
        // You can use the following to avoid implementing rehash_and_grow().
        // However, you will only pass the AG test cases ending in _C.
        // To pass the rest, start with at most 20 buckets and implement rehash_and_grow().
        //    buckets.resize(10000);
    }

    size_t size() const {
        return num_elements;
    }

    // returns a reference to the value in the bucket with the key, if it
    // already exists. Otherwise, insert it with a default value, and return
    // a reference to the resulting bucket.
    V& operator[](const K& key) {
        // TODO
        rehash_and_grow();
        Hasher hasher;
        size_t code = hasher(key) % buckets.size();

        bool deletedFound = false;
        bool found = false;
        size_t deletedKey;

        for (size_t i = 0; i < buckets.size(); i++){
            switch (buckets[code].status){
                case Status::Empty : {
                    if(deletedFound){
                        code = deletedKey;
                    }
                    found = true;
                    break;
                }
                case Status::Occupied : {
                    if(buckets[code].key == key){
                        return buckets[code].val;
                    }
                    break;
                }
                case Status::Deleted : {
                    if(!deletedFound){
                        deletedKey = code;
                        deletedFound = true;
                    }
                    break;
                }
            }
            if (found){
                break;
            } else {
                code = (code+1) % buckets.size();
            }
        }
        if (found) {
            buckets[code].status = Status::Occupied;
            buckets[code].key = key;
            buckets[code].val = V();
            num_elements++;
            return buckets[code].val;
        } else {
            buckets[deletedKey].status = Status::Occupied;
            buckets[deletedKey].key = key;
            buckets[deletedKey].val = V();
            num_elements++;
            return buckets[deletedKey].val;  
        }

    }

    // insert returns whether inserted successfully
    // (if the key already exists in the table, do nothing and return false).
    bool insert(const K& key, const V& val) {
        // TODO
        rehash_and_grow();
        Hasher hasher;
        size_t code = hasher(key) % buckets.size();

        bool deletedFound = false;
        bool found = false;
        size_t deletedKey;

        for(size_t i = 0; i < buckets.size(); i++){
            switch(buckets[code].status){
                case Status::Empty: {
                    if(deletedFound){
                        code = deletedKey;
                    }
                    found = true;
                    break;
                }
                case Status::Occupied:{
                    if(buckets[code].key == key){
                        return false;
                    }
                    break;
                }
                case Status::Deleted: {
                    if(!deletedFound){
                        deletedKey = code;
                        deletedFound = true;
                    }
                    break;
                }
            }
            if (found) {
                break;
            } else {
                code = (code+1) % buckets.size();
            }
        }
        if (found) {
            buckets[code].status = Status::Occupied;
            buckets[code].key = key;
            buckets[code].val = val;
        } else {
            buckets[deletedKey].status = Status::Occupied;
            buckets[deletedKey].key = key;
            buckets[deletedKey].val = val;
        }

        num_elements++;
        return true;  
    }
    // erase returns the number of items remove (0 or 1)
    size_t erase(const K& key) {
        // TODO
        size_t num = 0;
        
        Hasher hasher;
        size_t code = hasher(key) % buckets.size();
        bool found = false;
        bool breakout = false;

        for(size_t i = 0; i < buckets.size(); i++){
            switch (buckets[code].status){
                case Status::Empty : {
                    breakout = true;
                    break;
                }
                case Status::Occupied : {
                    if(buckets[code].key == key){
                        found = true;
                    }
                    break;
                }
                case Status::Deleted : {
                    break;
                }
            }
            if (found) {
                buckets[code].status = Status::Deleted;
                num_elements--;
                num = 1;
                break;
            } else if (breakout) {
                break;
            } else {
                code = (code+1) % buckets.size();
            }
        }
        return num;
    }

private:
    size_t num_elements = 0;
    size_t num_deleted = 0; // OPTIONAL: you don't need to use num_deleted to pass
    std::vector<Bucket> buckets;

    void rehash_and_grow() {
        // You can avoid implementing rehash_and_grow() by calling
        //    buckets.resize(10000);
        // in the constructor. However, you will only pass the AG test cases ending in _C.
        // To pass the rest, start with at most 20 buckets and implement rehash_and_grow().
        if(num_elements * 2 > buckets.size()){
            //std::cout << "rehashed" << std::endl;
            std::vector<Bucket> copy = buckets;
            buckets.clear();
            buckets.resize(copy.size() * 2);
            for(size_t i = 0; i < copy.size(); i++){
                if(copy[i].status == Status::Occupied){
                    insert(copy[i].key, copy[i].val);
                    num_elements--;
                }
            }
        }
    }

    // You can add methods here if you like.
    // TODO
};

#endif // HASHTABLE_H
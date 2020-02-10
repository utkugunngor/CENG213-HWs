#ifndef __HASHTABLE__
#define __HASHTABLE__

#include "HashUtils.h"

// Do not modify the public interface of this class.
// Otherwise, your code will note compile!
template <class T>
class HashTable {
    struct Entry {
        std::string Key;             // the key of the entry
        T Value;   // the value of the entry
        bool Deleted;        // flag indicating whether this entry is deleted
        bool Active;         // flag indicating whether this item is currently used

        Entry() : Key(), Value(), Deleted(false), Active(false) {}
    };

    struct Bucket {
        Entry entries[3];
    };

    int _capacity; // INDICATES THE SIZE OF THE TABLE
    int _size; // INDICATES THE NUMBER OF ITEMS IN THE TABLE

    Bucket* _table; // HASH TABLE
    
    // You can define private methods and variables
    bool find_key(std::string key){
        for(int a = 0 ; a < _capacity ; a++){
            for(int b = 0 ; b < 3 ; b++){
                if(_table[a].entries[b].Key == key) return true;
            }
        }
        return false;
    }
    int suc_probe_helper(std::string key){
        int search = 0;
        int hash_value = Hash(key);
        for(int j = 0; ; hash_value = Hash(key) + (j*j)){    
            for(int i = 0 ; i < 3 ; i++ ){
                if( _table[hash_value%_capacity].entries[i].Key != key ) continue;
                else return ++search;
            }
            search++;
            j++;
        }
    }
    int unsuc_probe_helper(int bucket_index){
        int search = 0;
        int j = 0;
        for(int i = bucket_index ; i < _capacity ; i++){
            if(_table[i].entries[j].Active == false || _table[i].entries[j+1].Active == false || _table[i].entries[j+2].Active == false ) return ++search;
            else {
                search++;
                continue;
            }
        }
    }
   
public:
    // TODO: IMPLEMENT THESE FUNCTIONS.
    // CONSTRUCTORS, ASSIGNMENT OPERATOR, AND THE DESTRUCTOR
    HashTable();
    HashTable(const HashTable<T>& rhs);
    HashTable<T>& operator=(const HashTable<T>& rhs);
    ~HashTable();

    // TODO: IMPLEMENT THIS FUNCTION.
    // INSERT THE ENTRY IN THE HASH TABLE WITH THE GIVEN KEY & VALUE
    // IF THE GIVEN KEY ALREADY EXISTS, THE NEW VALUE OVERWRITES
    // THE ALREADY EXISTING ONE. 
    // IF LOAD FACTOR OF THE TABLE IS BIGGER THAN 0.5,
    // RESIZE THE TABLE WITH THE NEXT PRIME NUMBER.
    void Insert(std::string key, const T& value);

    // TODO: IMPLEMENT THIS FUNCTION.
    // DELETE THE ENTRY WITH THE GIVEN KEY FROM THE TABLE
    // IF THE GIVEN KEY DOES NOT EXIST IN THE TABLE, JUST RETURN FROM THE FUNCTION
    // HINT: YOU SHOULD UPDATE ACTIVE & DELETED FIELDS OF THE DELETED ENTRY.
    void Delete(std::string key);

    // TODO: IMPLEMENT THIS FUNCTION.
    // IT SHOULD RETURN THE VALUE THAT CORRESPONDS TO THE GIVEN KEY.
    // IF THE KEY DOES NOT EXIST, THIS FUNCTION MUST RETURN T()
    T Get(std::string key) const;

    // TODO: IMPLEMENT THIS FUNCTION.
    // AFTER THIS FUNCTION IS EXECUTED THE TABLE CAPACITY MUST BE
    // EQUAL TO newCapacity AND ALL THE EXISTING ITEMS MUST BE REHASHED
    // ACCORDING TO THIS NEW CAPACITY.
    // WHEN CHANGING THE SIZE, YOU MUST REHASH ALL OF THE ENTRIES FROM 0TH ENTRY TO LAST ENTRY
    void Resize(int newCapacity);
    
    // TODO: IMPLEMENT THIS FUNCTION.
    // RETURNS THE AVERAGE NUMBER OF PROBES FOR SUCCESSFUL SEARCH
    double getAvgSuccessfulProbe();
    
    // TODO: IMPLEMENT THIS FUNCTION.
    // RETURNS THE AVERAGE NUMBER OF PROBES FOR UNSUCCESSFUL SEARCH
    double getAvgUnsuccessfulProbe();

    // THE IMPLEMENTATION OF THESE FUNCTIONS ARE GIVEN TO YOU
    // DO NOT MODIFY!
    int Capacity() const;
    int Size() const;
};



template <class T>
HashTable<T>::HashTable() {
// TODO: CONSTRUCTOR
    _table = NULL;
    _capacity = 0;
    _size = 0;
}

template <class T>
HashTable<T>::HashTable(const HashTable<T>& rhs) {
// TODO: COPY CONSTRUCTOR
    _table = NULL;
    _table = new Bucket[rhs._capacity];
    for(int i = 0 ; i < rhs._capacity ; i++){
        for(int j = 0 ; j < 3 ; j++){
            _table[i].entries[j].Key = rhs._table[i].entries[j].Key;
            _table[i].entries[j].Value = rhs._table[i].entries[j].Value;
            _table[i].entries[j].Active = rhs._table[i].entries[j].Active;
            _table[i].entries[j].Deleted = rhs._table[i].entries[j].Deleted;
        }
    }
    _size = rhs._size;
    _capacity = rhs._capacity;
    
}

template <class T>
HashTable<T>& HashTable<T>::operator=(const HashTable<T>& rhs) {
// TODO: OPERATOR=
    if(_table != NULL) delete [] _table;
    _table = NULL;
    _table = new Bucket[rhs._capacity];
    for(int i = 0 ; i < rhs._capacity ; i++){
        for(int j = 0 ; j < 3 ; j++){
            _table[i].entries[j].Key = rhs._table[i].entries[j].Key;
            _table[i].entries[j].Value = rhs._table[i].entries[j].Value;
            _table[i].entries[j].Active = rhs._table[i].entries[j].Active;
            _table[i].entries[j].Deleted = rhs._table[i].entries[j].Deleted;
        }
    }
    _size = rhs._size;
    _capacity = rhs._capacity;
    return *this;
}

template <class T>
HashTable<T>::~HashTable() {
// TODO: DESTRUCTOR
    delete [] _table;
    _capacity = 0;
    _size = 0;
}

template <class T>
void HashTable<T>::Insert(std::string key, const T& value) {
// TODO: IMPLEMENT THIS FUNCTION.
// INSERT THE ENTRY IN THE HASH TABLE WITH THE GIVEN KEY & VALUE
// IF THE GIVEN KEY ALREADY EXISTS, THE NEW VALUE OVERWRITES
// THE ALREADY EXISTING ONE. IF LOAD FACTOR OF THE TABLE IS BIGGER THAN 0.5,
// RESIZE THE TABLE WITH THE NEXT PRIME NUMBER.
    int hash_value = Hash(key);
    if(_table == NULL){
        _capacity = NextCapacity(_capacity);
        _table = new Bucket[_capacity];
    }
    if(find_key(key) == true){
        for(int a = 0 ; a < _capacity ; a++){
            for(int b = 0 ; b < 3 ; b++){
                if(_table[a].entries[b].Key == key){
                    _table[a].entries[b].Value = value;
                    return;
                    }
                }
            }
        } 
    for(int j = 0; ; hash_value = Hash(key) + (j*j)){    
        for(int i = 0 ; i < 3 ; i++ ){
            if( _table[hash_value%_capacity].entries[i].Active == false ){
                _table[hash_value%_capacity].entries[i].Key = key;
                _table[hash_value%_capacity].entries[i].Value = value;
                _table[hash_value%_capacity].entries[i].Active = true;
                _size++;
                if((float)_size/(3*_capacity) > 0.5)  Resize(NextCapacity(_capacity));
                return;
            }
        }
        j++;
    }
    
   
}

template <class T>
void HashTable<T>::Delete(std::string key) {
// TODO: IMPLEMENT THIS FUNCTION.
// DELETE THE ENTRY WITH THE GIVEN KEY FROM THE TABLE
// IF THE GIVEN KEY DOES NOT EXIST IN THE TABLE, JUST RETURN FROM THE FUNCTION
// HINT: YOU SHOULD UPDATE ACTIVE & DELETED FIELDS OF THE DELETED ENTRY.
    if(_table == NULL) return;
    if(find_key(key) == true){
        for(int i = 0 ; i < _capacity ; i++){
            for(int j = 0 ; j < 3 ; j++){
                    if(_table[i].entries[j].Key == key){
                        _table[i].entries[j] = Entry();
                        _table[i].entries[j].Active = false;
                        _table[i].entries[j].Deleted = true;
                        _size--;
                        return;
                }
            }
        }
    }
    else return;
}

template <class T>
T HashTable<T>::Get(std::string key) const {
// TODO: IMPLEMENT THIS FUNCTION. IT SHOULD RETURN THE VALUE THAT
// IT SHOULD RETURN THE VALUE THAT CORRESPONDS TO THE GIVEN KEY.
// IF THE KEY DOES NOT EXIST, THIS FUNCTION MUST RETURN T()
    if(_table == NULL) return T();
    for(int i = 0 ; i < _capacity ; i++){
        for(int j = 0 ; j < 3 ; j++){
                if(_table[i].entries[j].Key == key) return _table[i].entries[j].Value;
            }
        }
    return T();
}

template <class T>
void HashTable<T>::Resize(int newCapacity) {
// TODO: IMPLEMENT THIS FUNCTION. AFTER THIS FUNCTION IS EXECUTED
// THE TABLE CAPACITY MUST BE EQUAL TO newCapacity AND ALL THE
// EXISTING ITEMS MUST BE REHASHED ACCORDING TO THIS NEW CAPACITY.
// WHEN CHANGING THE SIZE, YOU MUST REHASH ALL OF THE ENTRIES FROM 0TH ENTRY TO LAST ENTRY
    T *arr = new T[_size];
    int k = 0;
    int prev_size = _size;
    while( k < prev_size){
        for(int j = 0 ; j < 3 ; j++){
            for(int i = 0 ; i < _capacity ; i++){
                if(_table[i].entries[j].Active == true){
                    arr[k] = _table[i].entries[j].Value;
                    k++;
                } 
            }
        }
    }
    delete [] _table;
    _table = NULL;
    _size = 0;
    _table = new Bucket[newCapacity];
    _capacity = newCapacity;
    for(int m = 0 ; m < prev_size ; m++){
        Insert(arr[m].getIsbn(),arr[m]);
    }
    delete [] arr;
}


template <class T>
double HashTable<T>::getAvgSuccessfulProbe() {
// TODO: IMPLEMENT THIS FUNCTION.
// RETURNS THE AVERAGE NUMBER OF PROBES FOR SUCCESSFUL SEARCH
    if(_table != NULL){
        int number = 0;
        double result;
        for(int i = 0 ; i < _capacity ; i++){
            for(int j = 0 ; j < 3 ; j++){
                    if(_table[i].entries[j].Active == true){
                        number += suc_probe_helper(_table[i].entries[j].Key);
                }    
            }
        }
        result = (double)number/_size;
        return result;
    }
    
}

template <class T>
double HashTable<T>::getAvgUnsuccessfulProbe() {
// TODO: IMPLEMENT THIS FUNCTION.
// RETURNS THE AVERAGE NUMBER OF PROBES FOR UNSUCCESSFUL SEARCH
    if(_table != NULL){
        int number = 0;
        double result;
        for(int i = 0 ; i < _capacity ; i++){
           number += unsuc_probe_helper(i);
        }
        result = (double)number/_capacity;
        return result;
    }
}

template <class T>
int HashTable<T>::Capacity() const {
    return _capacity;
}

template <class T>
int HashTable<T>::Size() const {
    return _size;
}

#endif

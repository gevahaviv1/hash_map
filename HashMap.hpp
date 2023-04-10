#include <vector>
#include <list>
#include <stdexcept>
#include <iostream>
#include <complex>
#include <algorithm>
#ifndef _HASHMAP_HPP_
#define _HASHMAP_HPP_
#define START_CAPACITY 16
#define TOP_THRESHOLD (3.0 / 4.0)
#define LOW_THRESHOLD (1.0 / 4.0)

template<typename KeyT, typename ValueT>
class HashMap
{
  template<class T>
  class iterator_t;
  class bucket;

 public:
  typedef iterator_t<const std::pair<KeyT, ValueT>> const_iterator;

  HashMap<KeyT, ValueT> () : _bucket_list (new bucket[(size_t)START_CAPACITY]),
                _capacity (START_CAPACITY), _size (0), _exponent (4) {}

  HashMap<KeyT, ValueT> (const std::vector<KeyT> &keys_vector, const
  std::vector<ValueT> &values_vector)
  : HashMap<KeyT, ValueT> ()
  {
    if (keys_vector.size () != values_vector.size ())
    { throw std::length_error ("The size of the vectors is unmatched."); }
    for (std::size_t i = 0; i < keys_vector.size (); ++i)
    {
      if (this->contains_key (keys_vector[i]))
      { this->at(keys_vector[i]) = values_vector[i]; }
      else
      { this->insert (keys_vector[i], values_vector[i]); }
    }
  }

  HashMap<KeyT, ValueT> (const HashMap<KeyT, ValueT> &other)
  {
    this->_bucket_list = new bucket[other.capacity ()];
    this->_capacity = other.capacity ();
    this->_size = 0;
    this->_exponent = other._exponent;

    for (int i = 0; i < other.capacity (); ++i)
    {
      bucket *other_bucket_ptr = &other._bucket_list[i];
      auto pair = other_bucket_ptr->get_bucket ().begin ();
      while (pair != other_bucket_ptr->get_bucket ().end ())
      {
        this->insert (pair->first, pair->second);
        ++pair;
      }
    }
  }

  virtual ~HashMap<KeyT, ValueT> ()
  {
    this->clear ();
    this->_capacity = 0;
    this->_exponent = 0;
    delete[] this->_bucket_list;
  }

  /**
   * Size of elements inside the HashMap.
   * @return Int value.
   */
  int size () const
  { return this->_size; }

  /**
   * Size of capacity for the Hashmap elements.
   * @return Int value.
   */
  int capacity () const
  { return this->_capacity; }

  /**
   * Check if the Hashmap is empty.
   * @return Boolean value.
   */
  bool empty () const
  { return this->_size == 0; }

  /**
   * Insert new pair<Key, Value> into the HashMap.
   * If key already exists, do nothing.
   * Otherwise calculate the bucket index and insert the pair.
   * @param key Generic type value.
   * @param value Generic type value.
   * @return Boolean value.
   */
  bool insert (const KeyT &key, const ValueT &value)
  {
    if (this->contains_key (key))
    { return false; }
    ++this->_size;
    if (this->get_load_factor () > TOP_THRESHOLD)
    { this->re_hashing ("increase"); }
    std::size_t index = std::hash<KeyT>{} (key) & (this->_capacity - 1);
    bucket *bucket_ptr = &this->_bucket_list[index];
    bucket_ptr->update_bucket (key, value);
    return true;
  }

  /**
   * Check if given key is already in the HashMap.
   * @param key Generic value.
   * @return Boolean Value.
   */
  bool contains_key (const KeyT &key) const
  {
    std::size_t index = std::hash<KeyT>{} (key) & (this->_capacity - 1);
    bucket *bucket_ptr = &this->_bucket_list[index];
    return this->is_in_bucket (key, bucket_ptr);
  }

  /**
   * Check if given key is already in the HashMap.
   * @param key Generic value.
   * @return Boolean Value.
   */
  bool contains_key (const KeyT &key)
  {
    std::size_t index = std::hash<KeyT>{} (key) & (this->_capacity - 1);
    bucket *bucket_ptr = &this->_bucket_list[index];
    return this->is_in_bucket (key, bucket_ptr);
  }

  /**
   * Given reference to value by key.
   * If key doesnt exists throw error.
   * @param key Generic type.
   * @return Reference to generic type variable named value.
   */
  const ValueT &at (const KeyT &key) const
  {
    std::size_t index = std::hash<KeyT>{} (key) & (this->_capacity - 1);
    bucket *bucket_ptr = &this->_bucket_list[index];
    if (!this->is_in_bucket (key, bucket_ptr))
    { throw std::invalid_argument ("Key doesn't exists."); }
    for (auto it = bucket_ptr->get_bucket ().begin (); it <
                                                       bucket_ptr->get_bucket ().end (); ++it)
    {
      if (it->first == key)
      { return it->second; }
    }
  }

  /**
   * Given reference to value by key.
   * If key doesnt exists throw error.
   * @param key Generic type.
   * @return Reference to generic type variable named value.
   */
  ValueT &at (const KeyT &key)
  {
    std::size_t index = std::hash<KeyT>{} (key) & (this->_capacity - 1);
    bucket *bucket_ptr = &this->_bucket_list[index];
    if (this->is_in_bucket (key, bucket_ptr))
    {
      for (auto it = bucket_ptr->get_bucket ().begin (); it !=
                                       bucket_ptr->get_bucket ().end (); ++it)
      {
        if (it->first == key)
        { return it->second; }
      }
    }
    throw std::invalid_argument ("Key doesn't exists.");
  }

  /**
   *
   * @param key
   * @return
   */
  virtual bool erase (const KeyT &key)
  {
    if (this->get_load_factor () < LOW_THRESHOLD)
    { this->re_hashing ("decrease"); }

    if (!this->contains_key (key))
    { return false; }
    std::size_t index = std::hash<KeyT>{} (key) & (this->_capacity - 1);
    bucket *bucket_ptr = &this->_bucket_list[index];
    for (auto it = bucket_ptr->get_bucket ().begin (); it !=
                                       bucket_ptr->get_bucket ().end (); ++it)
    {
      if (it->first == key)
      {
        bucket_ptr->get_bucket ().remove (*it);
        --this->_size;
        if (this->get_load_factor () < LOW_THRESHOLD)
        { this->re_hashing ("decrease"); }
        return true;
      }
    }
    return false;
  }

  /**
   *
   * @return
   */
  double get_load_factor ()
  { return (double) this->_size / (double) this->_capacity; }

  /**
   *
   * @param key
   * @return
   */
  int bucket_size (const KeyT &key)
  {
    std::size_t index = std::hash<KeyT>{} (key) & (this->_capacity - 1);
    bucket *bucket_ptr = &this->_bucket_list[index];
    if (this->is_in_bucket (key, bucket_ptr))
    { return bucket_ptr->get_bucket ().size (); }
    throw std::invalid_argument ("Key doesn't exists.");
  }

  /**
   *
   * @param key
   * @return
   */
  int bucket_index (const KeyT &key)
  {
    std::size_t index = std::hash<KeyT>{} (key) & (this->_capacity - 1);
    bucket *bucket_ptr = &this->_bucket_list[index];
    if (this->is_in_bucket (key, bucket_ptr))
    { return (int) index; }

    throw std::invalid_argument ("Key doesn't exists.");
  }

  /**
   *
   */
  void clear ()
  {
    for (int i = 0; i < this->_capacity; ++i)
    {
      bucket *bucket_ptr = &this->_bucket_list[i];
      if (!bucket_ptr->get_bucket ().empty ())
      { bucket_ptr->get_bucket ().clear (); }
    }
    this->_size = 0;
  }

  const_iterator begin () 
  { return cbegin(); }

  const_iterator begin () const
  { return const_iterator (*this, 0, 0); }

  const_iterator cbegin () const
  { return const_iterator (*this, 0, 0); }

  const_iterator end ()
  { return cend(); }

  const_iterator end () const
  { return const_iterator (*this, this->_capacity, 0); }

  const_iterator cend () const
  { return const_iterator (*this, this->_capacity, 0); }
	
	friend void swap (HashMap<KeyT, ValueT> &src, HashMap<KeyT, ValueT> &dst)
	{
		std::swap(src._size, dst._size);
		std::swap(src._capacity, dst._capacity);
		std::swap(src._exponent, dst._exponent);
		std::swap(src._bucket_list, dst._bucket_list);
	}

	HashMap<KeyT, ValueT> &operator= (HashMap<KeyT, ValueT> rhs)
	{
		swap(*this, rhs);
		return *this;
	}

/*
  HashMap<KeyT, ValueT> &operator= (const HashMap<KeyT, ValueT> &rhs)
  {
    delete[] this->_bucket_list;

    this->_bucket_list = new bucket[rhs._capacity];
    this->_capacity = rhs.capacity ();
    this->_size = 0;
    this->_exponent = rhs._exponent;
    for (int i = 0; i < rhs.capacity (); ++i)
    {
      bucket *other_bucket_ptr = &rhs._bucket_list[i];
      auto pair = other_bucket_ptr->get_bucket ().begin ();
      while (pair != other_bucket_ptr->get_bucket ().end ())
      {
        this->insert (pair->first, pair->second);
        ++pair;
      }
    }
    return *this;
  }
*/

  ValueT &operator[] (const KeyT &key)
  {
    if (!this->contains_key (key))
    { this->insert (key, ValueT ()); }
    return this->at (key);
  }

  ValueT operator[] (const KeyT &key) const
  { return this->at (key); }

  bool operator== (const HashMap<KeyT, ValueT> &rhs) const
  {
    if (this->_size != rhs._size)
    { return false; }
    for (auto i = 0; i < rhs._capacity; ++i)
    {
      bucket &bucket_ref = rhs._bucket_list[i];
      if (!bucket_ref.get_bucket ().empty ())
      {
        for (const auto &pair: bucket_ref.get_bucket ())
        {
          if (!this->contains_key (pair.first))
          { return false; }
          if (this->at (pair.first) != pair.second)
          { return false; }
        }
      }
    }
    return true;
  }

  bool operator== (const HashMap<KeyT, ValueT> &rhs)
  {
    if (this->_size != rhs._size)
    { return false; }
    for (auto i = 0; i < rhs._capacity; ++i)
    {
      bucket &bucket_ref = rhs._bucket_list[i];
      if (!bucket_ref.get_bucket ().empty ())
      {
        for (const auto &pair: bucket_ref.get_bucket ())
        {
          if (!this->contains_key (pair.first))
          { return false; }
          if (this->at (pair.first) != pair.second)
          { return false; }
        }
      }
    }
    return true;
  }

  bool operator!= (const HashMap<KeyT, ValueT> &rhs) const
  { return !this->operator== (rhs); }

  bool operator!= (const HashMap<KeyT, ValueT> &rhs)
  { return !this->operator== (rhs); }

 protected:
  bucket *_bucket_list;
  int _capacity;
  int _size;
  int _exponent;

  /**
   * Check if the give key is exists in the given bucket.
   * @param key Generic type variable.
   * @param bucket_ptr Pointer to bucket object.
   * @return Boolean type.
   */
  bool is_in_bucket (const KeyT &key, bucket *bucket_ptr)
  {
    bucket_data &cur_bucket = bucket_ptr->get_bucket ();
    for (auto it = cur_bucket.begin (); it != cur_bucket.end (); it++)
    {
      if (it->first == key)
      { return true; }
    }
    return false;
  }

  /**
   * Increase or decrease capacity (memory) for buckets in HashMap.
   * After the capacity change, calculate new bucket index for each pair,
   * and insert them again.
   * @param operation String type variable.
   */
  void re_hashing (const std::string &operation)
  {
    int old_capacity = this->_capacity;
    int old_size = this->_size;
    if (operation == "increase")
    { ++this->_exponent; }
    else if (operation == "decrease")
    { --this->_exponent; }
    int new_capacity = std::pow (2, this->_exponent);
    auto *temp = new bucket[new_capacity];
    for (int i = 0; i < old_capacity; ++i)
    {
      bucket *old_bucket_ptr = &this->_bucket_list[i];
      auto pair = old_bucket_ptr->get_bucket ().begin ();
      while (pair != old_bucket_ptr->get_bucket ().end ())
      {
        std::size_t index =
            std::hash<KeyT>{} (pair->first) & (new_capacity - 1);
        bucket *new_bucket_ptr = &temp[index];
        new_bucket_ptr->update_bucket (pair->first, pair->second);
        ++pair;
      }
    }

    this->clear ();
    delete[] this->_bucket_list;
    this->_capacity = new_capacity;
    this->_size = old_size;
    this->_bucket_list = temp;
  }

 private:
  /**
   * New private inner class for HashMap.
   * The class represent bucket structure, which the HashMap holding.
   * Each bucket has member variable from type bucket_data.
   */
  typedef std::list<std::pair<KeyT, ValueT>> bucket_data;
  class bucket
  {
   private:
    bucket_data _bucket;

   public:
    /**
     * Empty ctor.
     */
    bucket ()
    {}

    /**
     * Return reference to the member variable bucket from the bucket_data.
     */
    bucket_data &get_bucket ()
    { return this->_bucket; }

    /**
     * Return reference to the member variable bucket from the bucket_data.
     */
    bucket_data get_bucket () const
    { return this->_bucket; }

    /**
     * Insert pair into bucket.
     * @param key Generic type variable.
     * @param value Generic type variable.
     */
    void update_bucket (const KeyT &key, const ValueT &value)
    {
      this->_bucket.push_back (std::make_pair (key, value));
    }

    /**
     *
     * @param rhs
     * @return
     */
    bool operator== (const bucket &rhs) const
    {
      if (this->_bucket.size () != rhs._bucket.size ())
      { return false; }
      return this->_bucket == rhs._bucket;
    }

    /**
     *
     * @param rhs
     * @return
     */
    bool operator== (const bucket &rhs)
    {
      if (this->_bucket.size () != rhs._bucket.size ())
      { return false; }
      return this->_bucket == rhs._bucket;
    }

    /**
     *
     * @param rhs
     * @return
     */
    bool operator!= (const bucket &rhs) const
    { return !(this->operator== (rhs)); }

    /**
     *
     * @param rhs
     * @return
     */
    bool operator!= (const bucket &rhs)
    { return !(this->operator== (rhs)); }

  };

  template<typename T>
  class iterator_t
  {
    friend class HashMap<KeyT, ValueT>;
   private:
    const HashMap<KeyT, ValueT> &_map_container;
    int _bucket_index;
    int _pair_index;

   public:
    typedef std::forward_iterator_tag iterator_category;
    typedef T value_type;
    typedef T &reference;
    typedef T *pointer;
    typedef std::ptrdiff_t difference_type;

    explicit iterator_t (const HashMap<KeyT, ValueT> &map_container, int
    bucket_index, int pair_index) :
        _map_container (map_container),
        _bucket_index (bucket_index),
        _pair_index (pair_index)
    {
      bucket *cur_bucket = &this->_map_container
          ._bucket_list[this->_bucket_index];
      while (this->_bucket_index != this->_map_container.capacity() && cur_bucket->get_bucket ().empty ())
      {
        ++this->_bucket_index;
        cur_bucket = &this->_map_container._bucket_list[this->_bucket_index];
      }
    }

    reference operator* ()
    {
      bucket *cur_bucket = &this->_map_container
          ._bucket_list[this->_bucket_index];
      auto cur_pair = cur_bucket->get_bucket ().begin ();
      std::advance (cur_pair, this->_pair_index);
//      while (cur_pair.operator-> () == nullptr)
//      {
//        ++this->_pair_index;
//        ++cur_pair;
//      }
      return *cur_pair;
    }

    value_type operator* () const
    {
      const bucket &cur_bucket = this->_map_container[this->_bucket_index];
      auto cur_pair = cur_bucket.get_bucket ().begin ();
      std::advance (cur_pair, this->_pair_index);
      return cur_pair.operator* ();
    }

    iterator_t &operator++ ()
    {
      if (this->_bucket_index < this->_map_container.capacity ())
      {
        bucket *cur_bucket = &this->_map_container.
            _bucket_list[this->_bucket_index];
        ++this->_pair_index;
        if (this->_pair_index >= (int) cur_bucket->get_bucket ().size ())
        {
          this->_pair_index = 0;
          do
          {
            ++this->_bucket_index;
            if (this->_bucket_index >= this->_map_container.capacity ())
            { return *this; }
            cur_bucket = &this->_map_container
                ._bucket_list[this->_bucket_index];
          }
          while (cur_bucket->get_bucket ().empty ());
        }
      }
      return *this;
    }

    iterator_t operator++ (int)
    {
      iterator_t<T> it (*this);
      this->operator++ ();
      return it;
    }

    pointer operator-> ()
    { return &(this->operator* ()); }

    pointer operator-> () const
    { return &(this->operator* ()); }

    bool operator== (const iterator_t &rhs) const
    {
      return (&this->_map_container == &rhs._map_container)
             && (this->_bucket_index == rhs._bucket_index)
             && (this->_pair_index == rhs._pair_index);
    }

    bool operator!= (const iterator_t &rhs) const
    { return !this->operator== (rhs); }
  };
};

#endif //_HASHMAP_HPP_

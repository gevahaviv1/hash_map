#include "HashMap.hpp"

#ifndef _DICTIONARY_HPP_
#define _DICTIONARY_HPP_
class InvalidKey : public std::invalid_argument
{
 public:
  explicit InvalidKey (const std::string &arg) : invalid_argument (arg)
  {}

  ~InvalidKey () noexcept override = default;

  const char *what () const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override
  { return std::invalid_argument::what (); }
};

class Dictionary : public HashMap<std::string, std::string>
{
 public:
  using HashMap<std::string, std::string>::HashMap;
  bool erase (const std::string &key) override
  {
    if (!HashMap<std::string, std::string>::contains_key (key))
    { throw InvalidKey ("Key doesn't exists."); }
    return HashMap<std::string, std::string>::erase (key);
  }

  template<typename V>
  void update (const V &begin, const V &end)
  {
    if (begin != end)
    {
      for (V it = begin; it != end; ++it)
      { this->operator[] (it->first) = it->second; }
    }
  }
};
#endif //_DICTIONARY_HPP_

#ifndef PRESUBMIT_H
#define PRESUBMIT_H
#include "HashMap.hpp"
#include "Helpers.h"
#include "Dictionary.hpp"
#include <map>
#include <iostream>

#ifndef __DISABLE_PRESUBMISSION_TESTS
#define PRESUBMISSION_ASSERT(testCase) __ASSERT_TEST("Presubmission", testCase)
#else
#define PRESUBMISSION_ASSERT(testCase)
#endif

//-------------------------------------------------------
// Tests
//-------------------------------------------------------

int __presubmit_testCreateHashMaps ()
{
  HashMap<int, int> intMap{};
  HashMap<std::string, std::string> stringMap{};
  HashMap<double, double> doubleMap{};

  ASSERT_MAP_PROPERTIES(intMap, 0, 16, 0);
  ASSERT_MAP_PROPERTIES(stringMap, 0, 16, 0);
  ASSERT_MAP_PROPERTIES(doubleMap, 0, 16, 0);

  auto dynamicallyAllocated = new HashMap<int, std::string> ();
  delete dynamicallyAllocated;

  return 1;
}

int __presubmit_testInsert ()
{
  HashMap<int, int> map;
  map.insert (5, 1);
  map.insert (1, 5);

  return 1;
}

int __presubmit_testSize ()
{
  HashMap<int, int> map;
  RETURN_ASSERT_TRUE(map.size () == 0);
}

int __presubmit_testCapacity ()
{
  HashMap<int, int> map;
  RETURN_ASSERT_TRUE(map.capacity () == 16); // Default capacity is 16.
}

int __presubmit_testEmpty ()
{
  HashMap<int, int> map;
  RETURN_ASSERT_TRUE(map.empty ());
}

int __presubmit_testClear ()
{
  HashMap<int, int> map;
  if (!map.empty ())
  {
    return 0;
  }

  map.insert (1, 5);
  map.clear ();

  RETURN_ASSERT_TRUE(map.empty ());
}

int __presubmit_testGetElement ()
{
  HashMap<int, int> map;
  map.insert (5, 1);

  ASSERT_TRUE(map.at (5) == 1);  // using HashMap::at
  RETURN_ASSERT_TRUE(map[5] == 1); // using HashMap::operator[]
}

int __presubmit_testContainsKey ()
{
  HashMap<int, int> map;
  map.insert (5, 1);
  RETURN_ASSERT_TRUE(map.contains_key (5));
}

int __presubmit_testAssignment ()
{
  HashMap<int, int> m1{};
  m1.insert (1, 2);

  HashMap<int, int> m2 = m1;
  RETURN_ASSERT_TRUE(m2.contains_key (1));
}

int __presubmit_testComparison ()
{
  HashMap<int, int> m1{};
  HashMap<int, int> m2{};

  RETURN_ASSERT_TRUE(m1 == m2);
}

int __presubmit_testIterator ()
{
  HashMap<int, int> map;

  map.begin ();
  map.end ();
  map.cbegin ();
  map.cend ();

  return 1;
}

int __presubmit_testVectorsCtor ()
{
  /* Create the keys & values */
  std::vector<int> keys;
  std::vector<int> values;
  keys.push_back (1);
  values.push_back (1);

  /* Create the map */
  HashMap<int, int> map (keys, values);
  RETURN_ASSERT_TRUE(map.size () == 1 && map.contains_key (1));
}

int __presubmit_testCopyCtor ()
{
  HashMap<int, int> m1{};
  m1.insert (1, 2);

  HashMap<int, int> m2 (m1);
  RETURN_ASSERT_TRUE(m2.contains_key (1));
}

int __presubmit_testBasicDataInsert ()
{
  /******************** Int Map ********************/
  HashMap<int, int> intMap;

  /* Insert */
  for (int i = 0; i < 100; i++)
  {
    intMap.insert (i, i);
  }

  /* Make sure that we got the right size & capacity */
  ASSERT_MAP_PROPERTIES(intMap, 0.390625, 256, 100);

  /* Look for these entries */
  for (int i = 0; i < 100; i++)
  {
    ASSERT_TRUE(intMap.contains_key (i));
  }

  /* Make sure we can't find some extra entries */
  for (int i = 101; i < 112; i++)
  {
    ASSERT_TRUE(!intMap.contains_key (i));
  }

  /******************** Chars Map ********************/
  HashMap<char, int> charMap;
  for (int i = 0; i < 26; i++)
  {
    charMap.insert ('A' + i, i);
  }

  /* Make sure that we got the right size & capacity */
  ASSERT_MAP_PROPERTIES(charMap, 0.40625, 64, 26);

  /* Look for these entries */
  for (int i = 0; i < 26; i++)
  {
    ASSERT_TRUE(charMap.contains_key ('A' + i));
  }

  /* Make sure we can't find some extra entries */
  for (int i = 0; i < 11; i++)
  {
    ASSERT_TRUE(!charMap.contains_key ('a' + i));
  }

  /******************** Strings Map ********************/
  HashMap<std::string, int> stringMap;
  for (int i = 0; i < 50; i++)
  {
    stringMap.insert (std::to_string (i), i);
  }

  /* Make sure that we got the right size & capacity */
  ASSERT_MAP_PROPERTIES(stringMap, 0.390625, 128, 50);

  /* Look for these entries */
  for (int i = 0; i < 50; i++)
  {
    ASSERT_TRUE(stringMap.contains_key (std::to_string (i)));
  }

  /* Make sure we can't find some extra entries */
  for (int i = 51; i < 62; i++)
  {
    ASSERT_TRUE(!stringMap.contains_key (std::to_string (i)));
  }

  return true;
}

int __presubmit_testDataInteraction ()
{
  // region Insertion using operator[]

  {
    HashMap<int, int> map;
    for (int i = 0; i < 95; ++i)
    {
      map[i] = 95 - i;
    }

    // That's quite simple.
    ASSERT_MAP_PROPERTIES(map, 0.7421875, 128, 95);

    for (int i = 0; i < 95; ++i)
    {
      ASSERT_TRUE(map.contains_key (i));
    }

    // Lets modify some elements
    map[0] = 34;
    ASSERT_TRUE(map.at (0) == 34);

    map.at (0) = 43;
    ASSERT_TRUE(map[0] == 43);
  }

  // endregion

  // region Insertion using at() should fail, as the key does not exists.

  {
    ASSERT_THROWING(
        HashMap<int, int> map;
        map.at (0) = 1;
    );
  }

  // endregion

  // region Editing using at() should be allowed, as well as using operator[]

  {
    ASSERT_NOT_THROWING(
        HashMap<std::string, std::string> map;
        map["Hello"] = "world!";
        map["from"] = "C++";
        ASSERT_TRUE (map.contains_key ("Hello"));
        ASSERT_TRUE (!map.contains_key ("hello"));
        ASSERT_TRUE (map.contains_key ("from"));
        ASSERT_TRUE (map.at ("Hello") == "world!");
        ASSERT_TRUE (map.at ("from") == "C++");

        // We evolve!
        map.at ("from") = "C#";
        ASSERT_TRUE (map["from"] == "C#");
    );
  }

  // endregion

  // region Remvoing values

  {
    HashMap<char, int> map;
    for (char c = 'A'; c <= 'Z'; ++c)
    {
      map.insert (c, (int) c);
      ASSERT_TRUE(map[c] == (int) c);
    }

    /* Remove some chars */
    ASSERT_MAP_PROPERTIES(map, 0.40625, 64, 26);
    for (char c = 'A'; c <= 'F'; ++c)
    {
      map.erase (c);
      ASSERT_TRUE(!map.contains_key (c));
      ASSERT_THROWING(
          map.at (c); // Accessing undefined element with at should throw.
      )
    }
    ASSERT_MAP_PROPERTIES(map, 0.3125, 64, 20);
  }

  // endregion

  return 1;
}

int __presubmit_testFullTestVectorsCtor ()
{
  // region Initialization of a map with empty vectors
  ASSERT_NOT_THROWING (
      std::vector<int> keys;
      std::vector<int> values;

      HashMap<int, int> map (keys, values);
  );

  ASSERT_NOT_THROWING (
      std::vector<std::string> keys;
      std::vector<int> values;

      HashMap<std::string, int> map (keys, values);
  );
  // endregion

  // region Exceptions in case of vectors with different sizes (a.k.a, assert(keys.size() == values.size())

  /* 0 keys and 1 values */
  ASSERT_THROWING (
      std::vector<int> keys{};
      std::vector<int> values{1};
      HashMap<int, int> map (keys, values);
  );

  /* 1 keys and 0 values */
  ASSERT_THROWING (
      std::vector<int> keys{1};
      std::vector<int> values{};
      HashMap<int, int> map (keys, values);
  );

  /* Mismatch number (but not empty) of keys and values */
  ASSERT_THROWING (
      std::vector<char> keys{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
                             'k', 'l', 'm'};
      std::vector<int> values{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

      HashMap<char, int> map (keys, values);
  );
  // endregion

  // region Seed and check if we got the right values

  {
    std::vector<char> keys{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
                           'k', 'l', 'm'};
    std::vector<int> values{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    HashMap<char, int> map (keys, values);

    ASSERT_MAP_PROPERTIES(map, 0.40625, 32, 13);
    ASSERT_NOT_THROWING(
        for (char c = 'a'; c <= 'm'; ++c)
        {
          ASSERT_TRUE (map.at (c) == (int) c - 96);
        }
    );
  }

  // endregion

  // region Handle duplicate keys

  {
    std::vector<char> keys{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
                           'k', 'l', 'm',
                           'c'};
    std::vector<int> values{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 2};
    HashMap<char, int> myMap2 (keys, values);
    ASSERT_TRUE(myMap2['c'] == 2);
  }

  // endregion

  return 1;
}

int __presubmit_testBasicIterator ()
{
  HashMap<char, int> map;
  for (char c = 'A'; c <= 'Z'; ++c)
  {
    map.insert (c, (int) c);
  }

  // region Basic Iteration

  {
    ASSERT_NOT_THROWING(
        for (auto it = map.cbegin (); it != map.cend (); ++it)
        {
          ASSERT_TRUE ((int) it->first == it->second);
        }
    );
  }

  // endregion

  return 1;
}

int __presubmit_testClearAndOperatorEquals ()
{
  // region Clear

  {
    // Setup
    HashMap<int, int> map;
    auto copy1 = map;
    ASSERT_MAP_PROPERTIES(map, 0, 16, 0);
    ASSERT_MAP_PROPERTIES(copy1, 0, 16, 0);

    // Add some entries
    for (int i = 0; i < 13; ++i)
    {
      map.insert (i, i);
    }
    ASSERT_MAP_PROPERTIES(map, 0.40625, 32, 13);

    // Create a nice copy
    auto copy2 = map;
    ASSERT_MAP_PROPERTIES(copy2, 0.40625, 32, 13);

    // Clear
    map.clear ();

    // Check that we really cleared this map (size = 0, capacity stays the same)
    ASSERT_MAP_PROPERTIES(map, 0, 32, 0);
    for (int i = 0; i < 13; ++i)
    {
      ASSERT_TRUE(!map.contains_key (i));
    }

    // Make sure that the copies didn't got modified
    ASSERT_MAP_PROPERTIES(copy1, 0, 16, 0);
    ASSERT_MAP_PROPERTIES(copy2, 0.40625, 32, 13);
    ASSERT_THROWING(
        ASSERT_TRUE (copy1.at (2) == 2); // No such element in copy1
    );
    ASSERT_NOT_THROWING(
        ASSERT_TRUE (copy2.at (2) == 2); // But it does exists in copy2
    );
  }

  // endregion

  // region operator==, operator!=

  {
    HashMap<int, int> map1;
    auto map2 = map1;
    ASSERT_TRUE(map1 == map2);
    ASSERT_TRUE(!(map1 != map2)); // Eww... but oh well, need to test it.

    // Add some data
    for (int i = 0; i < 5; ++i)
    {
      map1.insert (i, i);
    }

    ASSERT_TRUE(map1 != map2);
    ASSERT_TRUE(!(map1 == map2));

    // Add almost every entry to map2
    for (int i = 0; i < 4; ++i)
    {
      map2.insert (i, i);
    }

    ASSERT_TRUE(map1 != map2);
    ASSERT_TRUE(!(map1 == map2));

    // Even it
    map2[4] = 4;
    ASSERT_TRUE(map1 == map2);
    ASSERT_TRUE(!(map1 != map2));

    // Make map2 bigger
    map2[5] = 5;
    ASSERT_TRUE(map1 != map2);
    ASSERT_TRUE(!(map1 == map2));

    // Erase from map2
    map2.erase (5);
    ASSERT_TRUE(map1 == map2);
    ASSERT_TRUE(!(map1 != map2));

    // Erase from map1
    map1.erase (3);
    ASSERT_TRUE(map1 != map2);
    ASSERT_TRUE(!(map1 == map2));

    // Erase 3 from map2, which make it equal, but in addition modify a key in map1 to make it not equal.
    map2.erase (3);
    ASSERT_TRUE(map1 == map2);
    ASSERT_TRUE(!(map1 != map2));
    map1[0] = 5;
    ASSERT_TRUE(map1 != map2);
    ASSERT_TRUE(!(map1 == map2));
  }

  // endregion

  return 1;
}

int __presubmit_testBucketSize ()
{
  // Setup a nice map (a bullet-proof one :) that'll map the key to the right bucket index + size).
  auto wordsToBuckets = std::map<std::string, std::pair<int, int>> ();
  wordsToBuckets["Hello"] = std::make_pair (21, 1);
  wordsToBuckets["hello"] = std::make_pair (8, 2);
  wordsToBuckets["World"] = std::make_pair (9, 2);
  wordsToBuckets["WORLD"] = std::make_pair (29, 1);
  wordsToBuckets["wOrLd"] = std::make_pair (15, 2);
  wordsToBuckets["FROM"] = std::make_pair (3, 1);
  wordsToBuckets["from"] = std::make_pair (30, 1);
  wordsToBuckets["C"] = std::make_pair (2, 1);
  wordsToBuckets["C++"] = std::make_pair (8, 2);
  wordsToBuckets["Java"] = std::make_pair (4, 2);
  wordsToBuckets["C#"] = std::make_pair (15, 2);
  wordsToBuckets["F#"] = std::make_pair (9, 2);
  wordsToBuckets["PHP"] = std::make_pair (4, 2);
  wordsToBuckets["NodeJS"] = std::make_pair (5, 1);
  wordsToBuckets["Ruby"] = std::make_pair (13, 1);
  wordsToBuckets["Python"] = std::make_pair (1, 1);

  /* Create some random strings from buckets that we don't have in the list above
   * Note: No. I didn't found them by hand. I ran a code that get the unused buckets
   * and then generates a string that hashes to these buckets. */
  std::map<int, std::string> unusedBuckets;
  unusedBuckets[1] = "kHFswuzleh";
  unusedBuckets[5] = "iivhOmebpb";
  unusedBuckets[6] = "YDeWl5noes";
  unusedBuckets[7] = "WO8AK6OoG9";
  unusedBuckets[10] = "51lu6AcNnL";
  unusedBuckets[11] = "9mx1urN3Gj";
  unusedBuckets[12] = "AvkRy0oZhR";
  unusedBuckets[13] = "ataikWBMkU";
  unusedBuckets[14] = "FPGcyeyBge";
  unusedBuckets[18] = "0wUkYn6cnB";
  unusedBuckets[19] = "hKZnCCT9LV";
  unusedBuckets[21] = "rYbXyiO0bw";
  unusedBuckets[22] = "fEwpd8KSnb";
  unusedBuckets[23] = "uZ2kLiJRaO";
  unusedBuckets[25] = "3W0X6Q9be9";
  unusedBuckets[26] = "FwqIBMum8H";
  unusedBuckets[29] = "WRtAfdhEWn";
  unusedBuckets[30] = "RX8KMEWZWi";

  // Setup a map and seed it
  HashMap<std::string, int> map;
  for (auto word: wordsToBuckets)
  {
    map.insert (word.first, rand ());
  }

  ASSERT_MAP_PROPERTIES(map, 0.5, 32, 16);

  // Assert that we got the right buckets
  for (auto pair: wordsToBuckets)
  {
    ASSERT_TRUE(map.contains_key (pair.first));
    ASSERT_TRUE(map.bucket_index (pair.first) == pair.second.first);
    ASSERT_TRUE(map.bucket_size (pair.first) == pair.second.second);
  }

  // Assert that we're throwing exception for each unused bucket
  for (auto pair: unusedBuckets)
  {
    ASSERT_TRUE(!map.contains_key (pair.second));
    ASSERT_THROWING(map.bucket_index (pair.second););
    ASSERT_THROWING(map.bucket_size (pair.second););
  }

  return 1;
}

int __presubmit_testDictionaryUpdate ()
{

  // adds values to use for the update function
  Dictionary d_1;
  std::vector<std::pair<std::string, std::string>> v1;

  // checks that nothing was added
  ASSERT_TRUE(d_1.size () == 0);

  // adds values to vector
  v1.push_back (std::make_pair ("21", "1"));
  v1.push_back (std::make_pair ("8", "2"));
  v1.push_back (std::make_pair ("29", "1"));
  v1.push_back (std::make_pair ("15", "2"));
  v1.push_back (std::make_pair ("3", "1"));
  v1.push_back (std::make_pair ("30", "1"));
  v1.push_back (std::make_pair ("2", "1"));
  v1.push_back (std::make_pair ("4", "2"));
  v1.push_back (std::make_pair ("15", "2"));
  v1.push_back (std::make_pair ("9", "2"));
  v1.push_back (std::make_pair ("5", "1"));
  v1.push_back (std::make_pair ("13", "1"));
  v1.push_back (std::make_pair ("1", "1"));

  d_1.update (v1.begin (), v1.end ());

  // checks that items were put into the vector
  ASSERT_TRUE(d_1.size () == 12);

  // trys adding items already in the Dictionary and check that they get updated
  std::vector<std::pair<std::string, std::string>> v2;

  v2.push_back (std::make_pair ("21", "Java"));
  v2.push_back (std::make_pair ("8", "Python"));
  v2.push_back (std::make_pair ("29", "C#"));
  d_1.update (v2.begin (), v2.end ());

  // checks that the size stays the same
  ASSERT_TRUE(d_1.size () == 12);

  // checks that the values were changed
  ASSERT_TRUE(d_1.at ("21") == "Java");
  ASSERT_TRUE(d_1.at ("8") == "Python");
  ASSERT_TRUE(d_1.at ("29") == "C#");

  // keys from the same vector are the same
  std::vector<std::pair<std::string, std::string>> v3;

  v3.push_back (std::make_pair ("key", "Java"));
  v3.push_back (std::make_pair ("key", "Python"));
  v3.push_back (std::make_pair ("key", "C++"));
  v3.push_back (std::make_pair ("key", "C"));
  d_1.update (v3.begin (), v3.end ());

  // only a single key has been added
  ASSERT_TRUE(d_1.size () == 13);

  // the value is the most recent insert
  ASSERT_TRUE(d_1.at ("key") == "C");
  return 1;
}

int __presubmit_testDictionaryErase ()
{

  // adds values to use for the update function
  Dictionary d_1;
  std::vector<std::pair<std::string, std::string>> v1;

  // adds values to vector
  v1.push_back (std::make_pair ("21", "1"));
  v1.push_back (std::make_pair ("8", "2"));
  v1.push_back (std::make_pair ("29", "1"));
  v1.push_back (std::make_pair ("15", "2"));
  v1.push_back (std::make_pair ("3", "1"));
  v1.push_back (std::make_pair ("30", "1"));
  v1.push_back (std::make_pair ("2", "1"));
  v1.push_back (std::make_pair ("4", "2"));
  v1.push_back (std::make_pair ("15", "2"));
  v1.push_back (std::make_pair ("9", "2"));
  v1.push_back (std::make_pair ("5", "1"));
  v1.push_back (std::make_pair ("13", "1"));
  v1.push_back (std::make_pair ("1", "1"));

  d_1.update (v1.begin (), v1.end ());

  // erases a key
  d_1.erase ("21");

  // checks that we actually erased the key
  ASSERT_TRUE(d_1.contains_key ("21") == false);

  // tries removing a key that does not exist an exception should be raised
  ASSERT_THROWING(d_1.erase ("21"););

  // tests the name of our exception
  try
  {
    d_1.erase ("21");
  }
  catch (const InvalidKey &e)
  {

  }
  catch (const std::exception &e)
  {
    ASSERT_TRUE(true);
  }

  // checks that our inlvaid key inherits
  try
  {
    d_1.erase ("21");
  }
  catch (const std::invalid_argument &e)
  {

  }
  catch (const std::exception &e)
  {
    ASSERT_TRUE(true);
  }

  return 1;
}

//-------------------------------------------------------
//  The main entry point
//-------------------------------------------------------
int runPreSubmissionChecks ()
{
  PRESUBMISSION_ASSERT(__presubmit_testCreateHashMaps);
  PRESUBMISSION_ASSERT(__presubmit_testInsert);
  PRESUBMISSION_ASSERT(__presubmit_testSize);
  PRESUBMISSION_ASSERT(__presubmit_testCapacity);
  PRESUBMISSION_ASSERT(__presubmit_testEmpty);
  PRESUBMISSION_ASSERT(__presubmit_testClear);
  PRESUBMISSION_ASSERT(__presubmit_testGetElement);
  PRESUBMISSION_ASSERT(__presubmit_testContainsKey);
  PRESUBMISSION_ASSERT(__presubmit_testAssignment);
  PRESUBMISSION_ASSERT(__presubmit_testComparison);
  PRESUBMISSION_ASSERT(__presubmit_testIterator);
  PRESUBMISSION_ASSERT(__presubmit_testVectorsCtor);
  PRESUBMISSION_ASSERT(__presubmit_testCopyCtor);
  PRESUBMISSION_ASSERT(__presubmit_testBasicDataInsert);
  PRESUBMISSION_ASSERT(__presubmit_testDataInteraction);
  PRESUBMISSION_ASSERT(__presubmit_testFullTestVectorsCtor);
  PRESUBMISSION_ASSERT(__presubmit_testBasicIterator);
  PRESUBMISSION_ASSERT(__presubmit_testClearAndOperatorEquals);
  PRESUBMISSION_ASSERT(__presubmit_testBucketSize);
  PRESUBMISSION_ASSERT(__presubmit_testDictionaryUpdate);
  PRESUBMISSION_ASSERT(__presubmit_testDictionaryErase);
  return 1;
}

#endif
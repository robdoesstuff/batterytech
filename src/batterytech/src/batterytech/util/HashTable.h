/*
 * BatteryTech
 * Copyright (c) 2010 Battery Powered Games LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
 */

//============================================================================
// Name        : HashTable.h
// Description : A simple generic and string-key-specific HashTable implementation
//============================================================================

#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <stdlib.h>
#include <string.h>

namespace BatteryTech {

// Currently automatic growing is disabled.
// Initialize this hashtable at the approximate capacity you believe you will need or performance will suffer as you lose sparsity.
template<typename K, typename V>
class HashTable {
protected:
	class Bucket {
	public:
		K key;
		V data;
		Bucket *next;
		Bucket(const K &key, const V &data, Bucket *next = NULL);
	}**buckets;

	// The size of the hash array (see DEFAULT_HASH_ARRAY_SIZE)
	int hashArraySize;

	// The total number of actual hashed entries (key/value pairs) in the table
	int entries;

	// The default (prime) number of elements in the hash array (LL starting buckets)
	const static int DEFAULT_HASH_ARRAY_SIZE = 89;

	// The threshold (entries / buckets) at which we grow the hash array so we don't have too many entries in one hash array element.
	const static double THRESHOLD;

	// Finds the bucket matching key.
	// Buckets are linked lists so the list will need to be traversed with a comparison check to find the actual entry.
	Bucket* findBucket(const K &key, bool remove) const;
	// hash function
	int hash(const K &key) const;
	/*
	 Doubles hash array size but currently is broken as it does not rehash (which is required for entries to match hash function)
	 */
	void grow();

	HashTable<K, V>* deleteOrNop(V const&) { return this; }
	HashTable<K, V>* deleteOrNop(V* const& p) { delete p;  return this; }
	V nullReturnVal;

public:
	HashTable<K, V> (int hashArraySize = DEFAULT_HASH_ARRAY_SIZE);
	~HashTable<K, V> ();

	HashTable<K, V>* operator=(HashTable<K, V> *table);
	bool contains(const K &key) const;
	HashTable<K, V>* put(const K &key, const V &data = 0);
	V get(const K &key);
	V remove(const K &key);
	void setNullReturnVal(V val) { nullReturnVal = val; }
	HashTable<K, V>* clear();
	HashTable<K, V>* deleteElements();

	int size() const {
		return entries;
	}

	// Iterator state
	struct Iterator {
		int index, entriesVisited;
		bool hasNext;
		Bucket *bucket;
		K key;
	};


	// Get the initial iterator state used for iterating the hash table with getNext().
	Iterator getIterator() const {
		Iterator i;
		i.index = -1;
		i.entriesVisited = 0;
		i.bucket = NULL;
		i.hasNext = entries > 0;
		// keys aren't always assignable to NULL and we start with an assignment anyway
		// i.key = NULL;
		return i;
	}

	V getNext(Iterator &iterator) const;
};

// The entries / hash array threshold for growing, currently disabled until rehashing is added.
template<typename K, typename V>
const double HashTable<K, V>::THRESHOLD = 0.75;

template<typename K, typename V>
HashTable<K, V>* HashTable<K, V>::operator=(HashTable<K, V> *table) {
	Bucket *bucket = NULL;
	this->clear();
	// Iterate over all the buckets in <table>, copying over each entry.
	for (int i = 0; i < table->hashArraySize; i++) {
		bucket = table->buckets[i];
		while (bucket != NULL) {
			put(bucket->key, bucket->data);
			bucket = bucket->next;
		}
	}
	return this;
}

template<typename K, typename V>
int HashTable<K, V>::hash(const K &key) const {
	unsigned int hash = 0;
	// just use first 4 bytes or less of key for now (this is terrible for string hashes).
	memcpy(&hash, &key, sizeof(K) < sizeof(unsigned int) ? sizeof(K) : sizeof(unsigned int));
	return (int) (hash % hashArraySize);
}

template<typename K, typename V>
void HashTable<K, V>::grow() {
	//hashArraySize = 2 * hashArraySize + 1;
	//buckets = (Bucket **) realloc(buckets, (sizeof(Bucket *) * hashArraySize));
	// TODO rehash - this will fail without it since we use modulus!
}

template<typename K, typename V>
HashTable<K, V>::~HashTable<K, V>() {
	Bucket *currentBucket = NULL;
	Bucket *nextBucket = NULL;
	for (int i = 0; i < hashArraySize; i++) {
		currentBucket = buckets[i];
		while (currentBucket != NULL) {
			nextBucket = currentBucket->next;
			// buckets are allocated with new
			delete currentBucket;
			currentBucket = nextBucket;
		}
	}
	free(buckets);
}

template<typename K, typename V>
HashTable<K, V>::HashTable(int hashArraySize) {
	this->hashArraySize = hashArraySize;
	entries = 0;
	buckets = (Bucket **) calloc(sizeof(Bucket *), hashArraySize);
	nullReturnVal = 0;
}

template<typename K, typename V>
HashTable<K, V>* HashTable<K, V>::put(const K &key, const V &data) {
	if (((double) entries / hashArraySize) >= THRESHOLD) {
		// needs to rehash all entries in order for grow to work correctly.
		//grow();
	}
	int hashValue = hash(key);
	entries++;
	Bucket *bucket = new Bucket(key, data);
	if (buckets[hashValue] != NULL) {
		bucket->next = buckets[hashValue];
	}
	buckets[hashValue] = bucket;
	return this;
}

template<typename K, typename V>
bool HashTable<K, V>::contains(const K &key) const {
	return findBucket(key, false) != NULL;
}

template<typename K, typename V>
typename HashTable<K, V>::Bucket* HashTable<K, V>::findBucket(const K &key, bool remove) const {
	int hashValue = hash(key);
	Bucket *bucket = buckets[hashValue];
	Bucket *lastBucket = NULL;
	while (bucket != NULL) {
		if (bucket->key == key) {
			if (remove) {
				//relink
				if (bucket->next) {
					if (lastBucket) {
						lastBucket->next = bucket->next;
					} else {
						buckets[hashValue] = bucket->next;
					}
				} else if (lastBucket) {
					lastBucket->next = NULL;
				} else {
					// no next, no last, is only bucket in hash array element
					buckets[hashValue] = NULL;
				}
			}
			return bucket;
		}
		lastBucket = bucket;
		bucket = bucket->next;
	}
	return NULL;
}

template<typename K, typename V>
V HashTable<K, V>::getNext(Iterator &iterator) const {
	// Visit next in current bucket, if any
	if (iterator.bucket && iterator.bucket->next) {
		iterator.bucket = iterator.bucket->next;
		iterator.entriesVisited++;
		iterator.hasNext = iterator.entriesVisited < entries;
		iterator.key = iterator.bucket->key;

		return iterator.bucket->data;
	}

	// Visit the remaining buckets
	for (int i = iterator.index + 1; i < hashArraySize; ++i) {
		Bucket *bucket = buckets[i];
		iterator.index = i;
		iterator.bucket = bucket;
		if (!bucket)
			continue;

		iterator.entriesVisited++;
		iterator.hasNext = iterator.entriesVisited < entries;
		iterator.key = iterator.bucket->key;
		return iterator.bucket->data;
	}

	iterator.hasNext = false;
	return nullReturnVal;
}

template<typename K, typename V>
HashTable<K, V>* HashTable<K, V>::clear() {
	for (int i = 0; i < hashArraySize; i++) {
		Bucket *bucket = buckets[i];
		while (bucket != NULL) {
			Bucket *next_bucket = bucket->next;
			delete bucket;
			bucket = next_bucket;
		}
		buckets[i] = NULL;
	}
	entries = 0;
	// FIXME: This should probably use the original size that was passed in constructor, cache it?
	hashArraySize = DEFAULT_HASH_ARRAY_SIZE;
	// realloc back to original hash array size
	buckets = (Bucket **) realloc(buckets, (sizeof(Bucket *) * hashArraySize));
	memset(buckets, 0, sizeof(Bucket *) * hashArraySize);
	return this;
}

template<typename K, typename V>
HashTable<K, V>* HashTable<K, V>::deleteElements() {
	for (int i = 0; i < hashArraySize; i++) {
		Bucket *bucket = buckets[i];
		while (bucket != NULL) {
			Bucket *next_bucket = bucket->next;
			deleteOrNop(bucket->data);
			delete bucket;
			bucket = next_bucket;
		}
		buckets[i] = NULL;
	}
	entries = 0;
	// FIXME: This should probably use the original size that was passed in constructor, cache it?
	hashArraySize = DEFAULT_HASH_ARRAY_SIZE;
	// realloc back to original hash array size
	buckets = (Bucket **) realloc(buckets, (sizeof(Bucket *) * hashArraySize));
	memset(buckets, 0, sizeof(Bucket *) * hashArraySize);
	return this;
}

template<typename K, typename V>
V HashTable<K, V>::get(const K &key) {
	Bucket *bucket = findBucket(key, false);
	if (!bucket) {
		return nullReturnVal;
	}
	return bucket->data;
}

template<typename K, typename V>
V HashTable<K, V>::remove(const K &key) {
	Bucket *bucket = findBucket(key, true);
	if (!bucket) {
		return nullReturnVal;
	}
	V data = bucket->data;
	// bucket allocated with new
	delete bucket;
	entries--;

	return data;
}

template<typename K, typename V>
HashTable<K, V>::Bucket::Bucket(const K &key, const V &data, Bucket *next) {
	this->key = key;
	this->data = data;
	this->next = next;
}

/* ---------------------------- char* specialized hashtable support ------------------------------- */
template<typename V>
class StrHashTable : public HashTable<char*,V> {
public:
	StrHashTable<V> (int hashArraySize = 89);
	bool contains(const char* key) const;
	StrHashTable<V>* put(const char* key, const V &data = 0);
	V get(const char* key);
	V remove(const char* key);
protected:
	typename StrHashTable<V>::Bucket* findBucket(const char* key, bool remove) const;
	int hash(const char* key) const;
};

template<typename V>
StrHashTable<V>::StrHashTable(int hashArraySize) : HashTable<char*, V>(hashArraySize) {}

template<typename V>
typename StrHashTable<V>::Bucket* StrHashTable<V>::findBucket(const char* key, bool remove) const {
	int hashValue = hash(key);
	typename HashTable<char*, V>::Bucket *bucket = HashTable<char*, V>::buckets[hashValue];
	typename HashTable<char*, V>::Bucket *lastBucket = NULL;
	while (bucket != NULL) {
		if (strcmp(bucket->key,key) == 0) {
			if (remove) {
				//relink
				if (bucket->next) {
					if (lastBucket) {
						lastBucket->next = bucket->next;
					} else {
						HashTable<char*, V>::buckets[hashValue] = bucket->next;
					}
				} else if (lastBucket) {
					lastBucket->next = NULL;
				} else {
					// no next, no last, is only bucket in hash array element
					HashTable<char*, V>::buckets[hashValue] = NULL;
				}
			}
			return bucket;
		}
		lastBucket = bucket;
		bucket = bucket->next;
	}
	return NULL;
}

template<typename V>
int StrHashTable<V>::hash(const char* key) const {
	unsigned int hash, i, len;
	len = strlen(key);
	for(hash = i = 0; i < len; ++i) {
		hash += key[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return (int) (hash % HashTable<char*, V>::hashArraySize);
}

template<typename V>
StrHashTable<V>* StrHashTable<V>::put(const char* key, const V &data) {
	if (((double) HashTable<char*, V>::entries / HashTable<char*, V>::hashArraySize) >= HashTable<char*, V>::THRESHOLD) {
		HashTable<char*, V>::grow();
	}
	int hashValue = hash(key);
	HashTable<char*, V>::entries++;
	char *safeKey = new char[strlen(key)+1];
	strcpy(safeKey, key);
	typename HashTable<char*, V>::Bucket *bucket = new typename HashTable<char*, V>::Bucket((char*)safeKey, data);
	/*
	 If there are already buckets in the bucket list for the given hash
	 value, prepend the new one to the existing list.  Regardless,
	 update the Bucket pointer in the hash array.
	 */
	if (HashTable<char*, V>::buckets[hashValue] != NULL) {
		bucket->next = HashTable<char*, V>::buckets[hashValue];
	}
	HashTable<char*, V>::buckets[hashValue] = bucket;
	return this;
}

template<typename V>
bool StrHashTable<V>::contains(const char* key) const {
	return findBucket(key, false) != NULL;
}

template<typename V>
V StrHashTable<V>::get(const char* key) {
	typename HashTable<char*, V>::Bucket *bucket = findBucket(key, false);
	if (!bucket) {
		return HashTable<char*, V>::nullReturnVal;
	}
	return bucket->data;
}

template<typename V>
V StrHashTable<V>::remove(const char* key) {
	typename HashTable<char*, V>::Bucket *bucket = findBucket(key, true);
	if (!bucket) {
		return HashTable<char*, V>::nullReturnVal;
	}
	V data = bucket->data;
	// bucket key is string we allocated with new
	delete [] bucket->key;
	// bucket allocated with new
	delete bucket;
	HashTable<char*, V>::entries--;
	return data;
}

} /* end namespace BatteryTech */
#endif /* HASHTABLE_H_ */

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
// Name        : ManagedArray.h
// Description : An array-wrapping template that tracks capacity and allows removing from random locations in the array
//============================================================================

#ifndef MANAGEDARRAY_H_
#define MANAGEDARRAY_H_
#include "../primitives.h"

namespace BatteryTech {

	template <class T>
	class ManagedArray {
	public:
		/**
		 * A pointer to the array of pointers of data.  Use directly for fast iteration access, but do not modify directly.
		 */
		T **array;
		/**
		 * The capacity of this array
		 */
		S32 capacity;
		/**
		 * The index of the last item in the array
		 */
		S32 lastItemIndex;
		/**
		 * Creates a new managed array of (size) capacity.
		 * ManagedArrays to not grow automatically.  Once they are at capacity, they will not accept more items.
		 * Fortunately each item's slot only takes up a pointer's size (usually 4 bytes), so if large numbers of items
		 * may be needed, it isn't extremely memory intensive to instantiate this class with a much larger capacity then needed.
		 * For instance, on most systems the array itself would only be 4000 bytes for 1000 items.
		 * Each object, however, will consume its own memory on the heap, as this only holds pointers to the objects, not the objects themselves.
		 */
		ManagedArray(S32 size);
		/**
		 * Adds an element pointer to the array
		 */
		S32 add(T *element);
		/**
		 * Gets the size of the array (number of items currently pointed to, not capacity)
		 */
		S32 getSize();
		/**
		 * Removes an element from the array at an index (0 is first)
		 */
		T* remove(S32 which);
		/**
		 * Gets the index of a specific element in the array (returns -1 if not in array)
		 */
		S32 getIndex(T *element);
		/**
		 * Removes the element from the array, returns false if element not in array
		 */
		BOOL32 remove (T *element);
		/**
		 * Swaps elements at indices a and b (useful for sorts)
		 */
		void swap(S32 a, S32 b);
		/**
		 * Clears all pointers in the array - does not delete objects
		 */
		void clear();
		/**
		 * Deletes all objects in the array and clears the array.
		 */
		void deleteElements();
		virtual ~ManagedArray();
	};

	template<class T>
	ManagedArray<T>::ManagedArray(S32 size) {
		array = new T*[size];
		lastItemIndex = -1;
		capacity = size;
	}

	template<class T>
	S32 ManagedArray<T>::add(T *element) {
		if (lastItemIndex + 1 >= capacity) {
			return -1;
		} else {
			array[++lastItemIndex] = element;
		}
		return lastItemIndex;
	}

	template<class T>
	T* ManagedArray<T>::remove(S32 which) {
		int lastItem = lastItemIndex;
		T* element = array[which];
		if (lastItem != which && lastItem > 0) {
			// move the last item into this spot
			array[which] = array[lastItem];
		}
		// clear off the last item.
		array[lastItem] = NULL;
		lastItemIndex--;
		return element;
	}

	template<class T>
	void ManagedArray<T>::swap(S32 a, S32 b) {
		T* temp = array[a];
		array[a] = array[b];
		array[b] = temp;
	}

	template<class T>
	S32 ManagedArray<T>::getIndex(T* element) {
		S32 index;
		for (index = 0; index <= lastItemIndex; index++) {
			if (array[index] == element) {
				return index;
			}
		}
		return -1;
	}

	template<class T>
	BOOL32 ManagedArray<T>::remove(T* element) {
		S32 index = getIndex(element);
		if (index != -1) {
			remove(index);
			return true;
		}
		return false;
	}

	template<class T>
	void ManagedArray<T>::deleteElements() {
		S32 i;
		S32 size = getSize();
		// intentionally set size to 0 so that nothing tries to concurrently use this list while being deleted
		lastItemIndex = -1;
		for (i = 0; i < size; i++) {
			delete array[i];
			array[i] = NULL;
		}
	}

	template<class T>
	void ManagedArray<T>::clear() {
		S32 i;
		for (i = 0; i < getSize(); i++) {
			array[i] = NULL;
		}
		lastItemIndex = -1;
	}

	template<class T>
	S32 ManagedArray<T>::getSize() {
		return lastItemIndex + 1;
	}

	template<class T>
	ManagedArray<T>::~ManagedArray() {
		delete [] array;
	}

}
#endif /* MANAGEDARRAY_H_ */

/*
 * ManagedArray.h
 *
 *  Created on: Oct 7, 2010
 *      Author: rgreen
 */

#ifndef MANAGEDARRAY_H_
#define MANAGEDARRAY_H_
#include "../primitives.h"

template <class T>
class ManagedArray {
public:
	T **array;
	S32 capacity;
	S32 lastItemIndex;
	ManagedArray(S32 size);
	S32 add(T *element);
	S32 getSize();
	void remove(S32 which);
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
void ManagedArray<T>::remove(S32 which) {
	int lastItem = lastItemIndex;
	if (lastItem != which && lastItem > 0) {
		// move the last item into this spot
		array[which] = array[lastItem];
	}
	// clear off the last item.
	array[lastItem] = NULL;
	lastItemIndex--;
}

template<class T>
S32 ManagedArray<T>::getSize() {
	return lastItemIndex + 1;
}

template<class T>
ManagedArray<T>::~ManagedArray() {
	delete [] array;
}

#endif /* MANAGEDARRAY_H_ */

/*
 * BatteryTech
 * Copyright (c) 2012 Battery Powered Games LLC.
 *
 * This code is a component of BatteryTech and is subject to the 'BatteryTech
 * End User License Agreement'.  Among other important provisions, this
 * license prohibits the distribution of source code to anyone other than
 * authorized parties.  If you have any questions or would like an additional
 * copy of the license, please contact: support@batterypoweredgames.com
 */

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

namespace BatteryTech {
	template <class T>
	class RingBuffer {
    private: 
        T** buffer;
        
	public:
        RingBuffer(S32 size);
        virtual ~RingBuffer();
        
        void write(T& element);
        T* read();
        T* getCurrent();
        void setAt(int i,T* newItem);
        int fillCount;
        int oldest;
        int fillPoint;
        int bufferSize;
    };
    
    template<class T>
    RingBuffer<T>::RingBuffer(S32 size) {
        fillCount = 0;
        fillPoint = 0;
        oldest = 0;
        bufferSize = size;
        buffer = new T*[size];
    }            
    
    template<class T>
    RingBuffer<T>::~RingBuffer() {
        for( int i = 0; i < bufferSize; ++i )
        {
            delete buffer[i];
        }
        delete [] buffer;
    }

    template<class T>
    void RingBuffer<T>::setAt(int i,T* newItem) {
        buffer[i] = newItem;
    }
    
    template<class T>
    void RingBuffer<T>::write(T& element) {
        buffer[fillPoint] = element;
        ++fillPoint %= bufferSize;
        if( fillPoint == oldest )
        {
            ++oldest %= bufferSize;
            //oh hey, the buffer filled, wahhh etc.
        }
        fillCount++;
    }

    template<class T>
    T* RingBuffer<T>::getCurrent() {
        T* current = buffer[fillPoint];
        ++fillPoint %= bufferSize;
        if( fillPoint == oldest )
        {
            ++oldest %= bufferSize;
            //oh hey, the buffer filled, wahhh etc.
        }
        fillCount++;
        return current;
    }
    
    template<class T>
    T* RingBuffer<T>::read() {
    	if (fillCount == 0) {
    		return 0;
    	}
        T* fetched = buffer[oldest];
        ++oldest %= bufferSize;
        fillCount--;
        return fetched;
    }    
}

#endif

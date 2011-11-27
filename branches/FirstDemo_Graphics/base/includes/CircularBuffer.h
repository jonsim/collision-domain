/**
 * @file	CircularBuffer.h
 * @author  Jonathan Simmonds
 * @brief 	Very fast implementation of a Circular Buffer.
 *          NB: To maximise speed a minimum amount of bound-checking is done, please
 *          ensure you understand how to use the functions beforehand.
 *          A circular buffer is used to implement a queue when the queue has a fixed
 *          maximum size. Once space in the buffer is always left empty for technical
 *          reasons, so ensure that this is accounted for when the bufferSize is 
 *          decided upon. All operations on the Circular Buffer can be performed in 
 *          constant time.
 *          For further information on circular buffers, see:
 *          http://en.wikipedia.org/wiki/Circular_buffer
 */
#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"



/*-------------------- CLASS DEFINITIONS --------------------*/
template <class T>
class CircularBuffer
{
public:
    CircularBuffer (uint8_t bs);
    ~CircularBuffer (void);
    inline bool isFull (void);
    inline bool isEmpty (void);
    T* get(const uint8_t index);
    void add (const T item);
    void remove ();

private:
    inline uint8_t sMod (const uint8_t x);

    const uint8_t bufferSize;           ///< The buffer size.
    const uint8_t bufferSizeMinusTwo;
    T* buffer;
    uint8_t head;
    uint8_t tail;
};

#endif // #ifndef CIRCULARBUFFER_H

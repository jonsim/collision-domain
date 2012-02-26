/**
 * @file    CircularBuffer.cpp
 * @author  Jonathan Simmonds
 * @brief     Very fast implementation of a Circular Buffer.
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "SharedIncludes.h"




/*-------------------- METHOD DEFINITIONS --------------------*/

/// @brief  Constructor, setting the CircularBuffer's variables.
/// @param  bs  The buffer size. Once initialised this is not changeable.
template <class T>
CircularBuffer<T>::CircularBuffer (uint8_t bs = 32) : bufferSize(bs), bufferSizeMinusTwo(bs - 2)
{
    buffer = calloc(bufferSize, sizeof(T));
}


/// @brief  Deconstructor.
template <class T>
CircularBuffer<T>::~CircularBuffer (void)
{
    free(buffer);
}


/// @brief  sMod performs the calculation (x % bufferSize) where 0 <= x < 2*bufferSize.
/**         The function performs the calculation much faster than the modulo function, however
            it is important that the two restrictions on x are observed. */
/// @param  x   x in the calculation (x % bufferSize).
/// @return The solution to the calculation (x % bufferSize).
template <class T>
inline uint8_t CircularBuffer<T>::sMod (const uint8_t x)
{
    return x < bufferSize ? x : x - bufferSize;
}


/// @brief  Returns whether or not the buffer is full.
///         When full the buffer will insert new elements over the oldest.
/// @return The state of the buffer.
template <class T>
inline bool CircularBuffer<T>::isFull (void)
{
    return (sMod(head + 1) == tail);
}


/// @brief  Returns whether or not the buffer is empty.
/// @return The state of the buffer.
template <class T>
inline bool CircularBuffer<T>::isEmpty (void)
{
    return (head == tail);
}


/// @brief  Treats the buffer like an array (elements 0, 1, ..., bufferSize-1) and returns the item from
///         the buffer located at the given index.
///         Please note, the index MUST BE VALID (i.e. index >= 0,   index < bufferSize-2 (as the buffer
///         always has 1 empty space for technical reasons)).
/// @param  index   The index of the target element in the virtual 'array'.
/// @return A pointer to the target element.
template <class T>
inline T* CircularBuffer<T>::get (const uint8_t index)
{
    return buffer + (sMod(tail + (bufferSizeMinusTwo - index)));
}


/// @brief  Adds an item to the buffer, overwriting the oldest item if the buffer is full.
/// @param  item  The item to add to the buffer.
template <class T>
void CircularBuffer<T>::add (const T item)
{
    buffer[head] = item;
    head = sMod(head + 1);
    if (head == tail)
        tail = sMod(tail + 1);
}


/// @brief  Removes the oldest item (if one exists) from the buffer.
template <class T>
void CircularBuffer<T>::remove (void)
{
    if (!isEmpty())
        tail = sMod(tail + 1);
}


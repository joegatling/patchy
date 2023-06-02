#ifndef RING_BUFFER_H
#define RING_BUFFER_H

//#include <type_traits>
#include <stdlib.h> 

template <typename T>
class CircularBuffer
{
public:
    CircularBuffer(int capacity);

    void Add(const T& value);
    void UpdateLast(const T& value);
    T Get(int index);
    T Last();

    T Dequeue();
    void Enqueue(T& value);

    void Clear();

    int GetCount();
    int GetStartIndex();

    void OutputToSerial();

    T& operator[](int index);

    //Helper functions for comparability checks
    //template <typename U = T>
    // typename std::enable_if<std::is_arithmetic<U>::value, U>::type Min();
    
    // template <typename U = T>
    // typename std::enable_if<std::is_arithmetic<U>::value, U>::type Max();

    // template <typename U = T>
    // typename std::enable_if<std::is_arithmetic<U>::value, U>::type Average();    

private:
    T* _values = 0;
    int _startIndex = 0;
    int _count = 0;
    int _capacity = 0;

    int TransformIndex(int index);
};

   
template <typename T>
CircularBuffer<T>::CircularBuffer(int capacity)
{
    _capacity = capacity;
    _values = new T[capacity];
}

template <typename T>
void CircularBuffer<T>::Add(const T& value)
{
    if (_count < _capacity)
    {
        _count++;
    }

    _values[_startIndex] = value;
    _startIndex = TransformIndex(1);
}

template <typename T>
void CircularBuffer<T>::UpdateLast(const T& value)
{
    if (_count > 0)
    {
        _values[TransformIndex(_count - 1)] = value;
    }
}

template <typename T>
T CircularBuffer<T>::Get(int index)
{
    if (index < 0 || index >= _count)
    {
        return 0;
    }

    return _values[TransformIndex(index)];
}

template <typename T>
T CircularBuffer<T>::Last()
{
    if (_count == 0)
    {
      return 0;
    }

    return _values[TransformIndex(_count - 1)];
}

template <typename T>
inline T CircularBuffer<T>::Dequeue()
{
    if(_count == 0)
    {
        return 0;
    }

    T val = Get(0);
    _startIndex = TransformIndex(1);
    _count--;

    return val;
}

template <typename T>
inline void CircularBuffer<T>::Enqueue(T &value)
{
    Add(value);
}

template <typename T>
void CircularBuffer<T>::Clear()
{
    _count = 0;
    _startIndex = 0;
}

template <typename T>
int CircularBuffer<T>::GetCount()
{
    return _count;
}

template <typename T>
int CircularBuffer<T>::GetStartIndex()
{
    return _startIndex;
}

template <typename T>
void CircularBuffer<T>::OutputToSerial()
{
    for (int i = 0; i < _count; i++)
    {
        int index = TransformIndex(i);
        // Output _values[index] to serial
    }
}

template <typename T>
T& CircularBuffer<T>::operator[](int index)
{
    if (index < 0 || index >= _count)
    {
        return 0;
    }

    return _values[TransformIndex(_startIndex + index)];
}

template <typename T>
int CircularBuffer<T>::TransformIndex(int index)
{
    return (index + _startIndex) % _capacity;
}


// template <typename T>
// template <typename U>
// typename std::enable_if<std::is_arithmetic<U>::value, U>::type CircularBuffer<T>::Min()
// {
//     if (_count == 0)
//     {
//         throw std::out_of_range("Buffer is empty");
//     }

//     U minVal = _values[TransformIndex(0)];
//     for (int i = 1; i < _count; i++)
//     {
//         int index = TransformIndex(i);
//         if (_values[index] < minVal)
//         {
//             minVal = _values[index];
//         }
//     }
//     return minVal;
// }

// template <typename T>
// template <typename U>
// typename std::enable_if<std::is_arithmetic<U>::value, U>::type CircularBuffer<T>::Max()
// {
//     if (_count == 0)
//     {
//       throw std::out_of_range("Buffer is empty");
//     }

//     U maxVal = _values[_startIndex];
//     for (int i = 1; i < _count; i++)
//     {
//         int index = TransformIndex(i);
//         if (_values[index] > maxVal)
//         {
//             maxVal = _values[index];
//         }
//     }
//     return maxVal;
// }

// template <typename T>
// template <typename U>
// typename std::enable_if<std::is_arithmetic<U>::value, U>::type CircularBuffer<T>::Average()
// {
//     if (_count == 0)
//     {
//         throw std::out_of_range("Buffer is empty");
//     }

//     U sum = _values[_startIndex];
    
//     for (int i = 1; i < _count; i++)
//     {
//         int index = TransformIndex(i);
//         sum += _values[index];
//     }
//     return sum / _count;
// }

#endif //RING_BUFFER_H

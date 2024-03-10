#pragma once
#include <string>
#include <thread>

// ReadWriteType is a wrapper for a datatype T that allows asynchronous read/write from two different threads. 
// This is a typical use case in real-time processing where the real-time thread is reading the data, and must not be locked or interrupted, and the 
// lower-priority message thread is writing and allocating/freeing memory.
//
// author : Kristian Timm Andersen
template<typename T>
class ReadWriteType
{
public:
    ReadWriteType(T& data) 
    {
        startList = new LinkedData(data);
        readList = startList;
        endList = startList;
        numElements = 1;
    }

    ~ReadWriteType()
    {
        while(startList != endList)
        {
            readList = startList->next;
            delete startList;
            startList = readList;
        }
        delete startList;
    }

    // NOTE: This method must be called from real-time thread
    T& get() 
    { 
        // update readList
        readList = endList;
        return readList->data;
    }

    // NOTE: This method must be called from same thread as set()
    T getConst() const
    {
        return endList->data;
    }

    // set data in message thread
    void set(T& data)
    {
        // update endList
        endList->next = new LinkedData(data);
        endList = endList->next;
        numElements++;

        // update startList
        while(startList != readList)
        {
            LinkedData* iterList = startList->next;
            delete startList;
            startList = iterList;
            numElements--;
        }
    }

    int getNumElements() const { return numElements; }

private:
    struct LinkedData
    {
        LinkedData(int& d) : data(d){ }
        T data;
        LinkedData *next = nullptr; // ensure pointer is not uninitialized
    };

    LinkedData* startList;
    LinkedData* readList;
    LinkedData* endList;
    int numElements;
};

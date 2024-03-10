#pragma once
#include <string>
#include <thread>

template<typename T>
class ReadWriteType
{
public:
    ReadWriteType(T& data) 
    {
        startList = new LinkedData(data);
        readList = startList;
        endList = startList;
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

    T& get() 
    { 
        // update readList
        while (readList->next != nullptr)
        {
            readList = readList->next;
        }
        return readList->data;
    }

    void set(T& data)
    {
        // update endList
        endList->next = new LinkedData(data);
        endList = endList->next;

        // update startList
        while(startList != readList)
        {
            LinkedData* iterList = startList->next;
            delete startList;
            startList = iterList;
        }
    }

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
};

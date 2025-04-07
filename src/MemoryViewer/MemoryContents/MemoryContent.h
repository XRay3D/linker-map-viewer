#pragma once
#include <QDebug>
using namespace Qt::Literals;

class MemoryContent {
public:
    // Constants
    static constexpr long DEFAULT_ADDRESS = -1;
    static constexpr long DEFAULT_SIZE = -1;

private:
    // Attributes
    long address{DEFAULT_ADDRESS};
    long size{DEFAULT_SIZE};

    // Attributes - protection
    bool isSetAddress = false;
    bool isSetSize = false;

public:
    // Constructors
    MemoryContent(long address, long size) {
        setAddress(address);
        setSize(size);
    }

    MemoryContent() = default;

    // Getters
    long getAddress() { return address; }
    long getSize() { return size; }

    // Setters
    void setAddress(long newAddress) {
        if(!isSetAddress) {
            address = newAddress;
            isSetAddress = true;
        } else
            qCritical("The address of the memory content can only be set once!");
    }

    void setSize(long newSize) {
        if(!isSetSize) {
            size = newSize;
            isSetSize = true;
        } else
            qCritical("The size of the memory content can only be set once!");
    }
};

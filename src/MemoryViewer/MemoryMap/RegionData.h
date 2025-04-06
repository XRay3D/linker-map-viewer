#pragma once
#include <QDebug>

class RegionData {
public:
    // Constants
    static constexpr long DEFAULT_ADDRESS = -1;
    static constexpr long DEFAULT_SIZE = -1;

private:
    // Attributes
    long address{DEFAULT_ADDRESS};
    long size{DEFAULT_SIZE};
    QString info;

    // Attributes - protection
    bool isSetAddress = false;
    bool isSetSize = false;
    bool isSetInfo = false;

public:
    // Constructors
    RegionData(long address, long size, QString info) {
        setAddress(address);
        setSize(size);
        setInfo(info);
    }

    RegionData() = default;

    // Getters
    long getAddress() { return address; }
    long getSize() { return size; }
    QString getInfo() { return info; }

    // Setters
    void setAddress(long newAddress) {
        if(!isSetAddress) {
            address = newAddress;
            isSetAddress = true;
        } else
            qCritical("The address of the region data can only be set once!");
    }

    void setSize(long newSize) {
        if(!isSetSize) {
            size = newSize;
            isSetSize = true;
        } else
            qCritical("The size of the region data can only be set once!");
    }

    void setInfo(QString newInfo) {
        if(!isSetInfo) {
            info = newInfo;
            isSetInfo = true;
        } else
            qCritical("The info of the region data can only be set once!");
    }
};

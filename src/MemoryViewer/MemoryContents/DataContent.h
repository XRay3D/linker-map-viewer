#pragma once
#include "MemoryContents/MemoryContent.h"
#include <QDebug>

class DataContent : public MemoryContent {
    // Constants
    static constexpr QStringView DEFAULT_DATA = u"?";
    // Attributes
    QString data{DEFAULT_DATA};
    // Attributes - protection
    bool isSetData = false;

public:
    // Constructors
    DataContent(long address, long size, QString data)
        : MemoryContent{address, size} {
        setData(data);
    }

    DataContent() = default;

    // Getters
    QString getData() { return data; }

    // Setters
    void setData(QString newData) {
        if(!isSetData) {
            data = newData;
            isSetData = true;
        } else
            qCritical("The data of the data content can only be set once!");
    }
};

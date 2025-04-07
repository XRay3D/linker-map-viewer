#pragma once
#include <QDebug>
using namespace Qt::Literals;

class SpaceAttributes {
    // Attributes
    bool erasable{}; // DEFAULT false,
    bool readable{}; // DEFAULT false,
    bool writable{}; // DEFAULT false,

    // Attributes - protection
    bool isSetErasable = false;
    bool isSetReadable = false;
    bool isSetWritable = false;

public:
    // Constructors
    SpaceAttributes(bool erasable, bool readable, bool writable) {
        setErasable(erasable);
        setReadable(readable);
        setWritable(writable);
    }

    SpaceAttributes() = default;

    // Getters
    bool isErasable() const { return erasable; }
    bool isReadable() const { return readable; }
    bool isWritable() const { return writable; }

    // Setters
    void setErasable(bool newErasable) {
        if(!isSetErasable) {
            erasable = newErasable;
            isSetErasable = true;
        } else qCritical("The attribute erasable of the space attributes can only be set once!");
    }

    void setReadable(bool newReadable) {
        if(!isSetReadable) {
            readable = newReadable;
            isSetReadable = true;
        } else qCritical("The attribute readable of the space attributes can only be set once!");
    }

    void setWritable(bool newWritable) {
        if(!isSetWritable) {
            writable = newWritable;
            isSetWritable = true;
        } else qCritical("The attribute writable of the space attributes can only be set once!");
    }
};

#pragma once
#include "MemoryConfiguration/SpaceAttributes.h"
#include <QDebug>

class Space {
public:
    // Constants
    static constexpr long DEFAULT_ORIGIN = -1;
    static constexpr long DEFAULT_LENGTH = -1;

private:
    // Attributes
    QString name;
    long origin{DEFAULT_ORIGIN};
    long length{DEFAULT_LENGTH};
    SpaceAttributes attributes;

    // Attributes - protection
    bool isSetName{};
    bool isSetOrigin{};
    bool isSetLength{};
    bool isSetAttributes{};

public:
    // Constructors
    Space() = default;
    Space(QString name, long origin, long length, SpaceAttributes attributes) {
        setName(name);
        setOrigin(origin);
        setLength(length);
        setAttributes(attributes);
    }

    // Getters
    QString getName() { return name; }
    long getOrigin() { return origin; }
    long getLength() { return length; }
    SpaceAttributes getAttributes() { return attributes; }

    // Setters
    void setName(QString newName) {
        if(!isSetName) {
            name = newName;
            isSetName = true;
        } else
            qCritical("The name of the space can only be set once!");
    }

    void setOrigin(long newOrigin) {
        if(!isSetOrigin) {
            origin = newOrigin;
            isSetOrigin = true;
        } else
            qCritical("The origin of the space can only be set once!");
    }

    void setLength(long newLength) {
        if(!isSetLength) {
            length = newLength;
            isSetLength = true;
        } else
            qCritical("The length of the space can only be set once!");
    }

    void setAttributes(SpaceAttributes newAttributes) {
        if(!isSetAttributes) {
            attributes = newAttributes;
            isSetAttributes = true;
        } else
            qCritical("The attributes of the space can only be set once!");
    }
};

Q_DECLARE_METATYPE(const Space&)

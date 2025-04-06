#pragma once
#include "MemoryContents/MemoryContent.h"
#include <QDebug>

class IdentifierContent final : public MemoryContent {
public:
    // Constants
    static constexpr QStringView UNKNOWN_IDENTIFIER = u">? ";
    static constexpr QStringView DEFAULT_IDENTIFIER = u"?";
    static constexpr QStringView DEFAULT_FILE = u"?";

private:
    // Attributes
    QString identifier{DEFAULT_IDENTIFIER};
    QString file{DEFAULT_FILE};

    // Attributes - protection
    bool isSetIdentifier = false;
    bool isSetFile = false;

public:
    // Constructors
    IdentifierContent(long address, long size, QString identifier, QString file)
        : MemoryContent{address, size} {
        setIdentifier(identifier);
        setFile(file);
    }

    IdentifierContent() = default;

    // Getters
    QString getIdentifier() const { return identifier; }
    QString getFile() const { return file; }

    // Setters
    void setIdentifier(QString newIdentifier) {
        if(!isSetIdentifier) {
            identifier = newIdentifier;
            isSetIdentifier = true;
        } else
            qCritical("The identifier of the identifier content can only be set once!");
    }

    void setFile(QString newFile) {
        if(!isSetFile) {
            file = newFile;
            isSetFile = true;
        } else
            qCritical("The file of the identifier content can only be set once!");
    }
};

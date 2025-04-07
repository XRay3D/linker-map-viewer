#pragma once
#include "MemoryContents/MemoryContents.h"
#include <QDebug>
#include <QStringBuilder>

class MemoryContentsLoader {
    // Basic patterns
    static constexpr QStringView fillContent = u"*fill*";

    // Attributes
    static inline MemoryContents memoryContents;

    // Helper functions - Data
    static int getFill(QString fill) {
        // e.g.: u"ff"_s returns 255
        if(fill.isEmpty())
            return FillContent::DEFAULT_FILL;
        else if(fill.isEmpty())
            return FillContent::DEFAULT_FILL;
        else
            return fill.toInt(nullptr, 16);
    }

public:
    // Helper functions - Loader
    static void clear() { memoryContents = {}; }

    static void addIdentifierContent(long address, long size, QString identifier, QString file) {
        memoryContents.addIdentifierContent(IdentifierContent{address, size, identifier, file});
    }

    static void addDataContent(long address, long size, QString data) {
        memoryContents.addDataContent(DataContent{address, size, data});
    }

    static void addFillContent(long address, long size, int fill) {
        memoryContents.addFillContent(FillContent{address, size, fill});
    }

    static void addOtherContent(long address, long size, QString info, QString subRegionName) {
        // Determine type of content (identifier, data or fill)
        if((info.contains(u"/"_s) || info.contains(u"\\"_s))) {
            addIdentifierContent(address, size, IdentifierContent::UNKNOWN_IDENTIFIER % subRegionName, info);
        } else if(subRegionName == fillContent) {
            addFillContent(address, size, getFill(info));
        } else {
            addDataContent(address, size, info);
        }
    }

    // Wrapper - Loader
    static MemoryContents load() { return memoryContents; }
};

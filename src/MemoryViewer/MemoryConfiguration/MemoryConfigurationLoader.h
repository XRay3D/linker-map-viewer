#pragma once
#include "MemoryConfiguration/MemoryConfiguration.h"
#include "MemoryConfiguration/Space.h"
#include <QDebug>
#include <QFile>
#include <QRegularExpression>
#include <QStringBuilder>

class MemoryConfigurationLoader {
    // Basic patterns
    static constexpr QStringView startConfiguration = u"Memory Configuration";
    static constexpr QStringView endConfiguration = u"Linker script and memory map";
    static constexpr QStringView defaultSpace = u"*default*";
    static constexpr QStringView regexOfName = u"(\\S+)";
    static constexpr QStringView regexOfOrigin = u"(0x[a-fA-F0-9]{8})";
    static constexpr QStringView regexOfLength = u"(0x[a-fA-F0-9]+)";
    static constexpr QStringView regexOfAttributes = u"(x?)(r?)(w?)";

    // Compiled patterns
    static inline const QRegularExpression patternOfSpace{regexOfName % u"\\s+"_s % regexOfOrigin % u"\\s+"_s % regexOfLength % u"\\s*"_s % regexOfAttributes};

    // Helper function - Data
    static long getOrigin(QString origin) {
        if(origin.isEmpty())
            return Space::DEFAULT_ORIGIN;
        else
            return origin.mid(2).toLong(nullptr, 16);
    }

    static long getLength(QString length) {
        if(length == nullptr)
            return Space::DEFAULT_LENGTH;
        else
            return length.mid(2).toLong(nullptr, 16);
    }

    // Helper functions - Reader
    static inline QTextStream* reader;
    static inline QString readerLine;

    static QString getNextLine() {
        // Read next line (skip blank lines) and store it AND return it
        try {
            while(!reader->atEnd()) {
                readerLine = reader->readLine();
                if(readerLine.length()) return readerLine;
            }
        } catch(std::exception e) {
            qCritical("Error reading next line: %s", e.what());
        }

        return readerLine;
    }

    // Helper functions - Loader
    static bool isDefaultSpace(Space space) {
        return ((space.getName() == defaultSpace)
            && (space.getOrigin() == 0)
            && (space.getLength() > 0)
            && (!space.getAttributes().isErasable())
            && (!space.getAttributes().isReadable())
            && (!space.getAttributes().isWritable()));
    }

    static MemoryConfiguration loadConfiguration() {
        MemoryConfiguration todoConfiguration;

        // Find the start of the memory configuration
        while(!readerLine.startsWith(startConfiguration))
            getNextLine();

        // Parse and store the spaces of the memory configuration
        while(!readerLine.startsWith(endConfiguration)) {
            auto matcher = patternOfSpace.match(readerLine);
            qInfo() << matcher.capturedTexts();
            if(matcher.hasMatch()) {
                // Create a new space with the name, origin, length and attributes found
                SpaceAttributes todoAttributes{
                    static_cast<bool>(matcher.captured(4).size()),
                    static_cast<bool>(matcher.captured(5).size()),
                    static_cast<bool>(matcher.captured(6).size()),
                };

                Space todoSpace{
                    matcher.captured(1),
                    getOrigin(matcher.captured(2)),
                    getLength(matcher.captured(3)),
                    todoAttributes,
                };

                // Check for the space defining the addressable area
                if(isDefaultSpace(todoSpace))
                    todoConfiguration.setTotalSize(todoSpace.getLength());
                else
                    todoConfiguration.addSpace(todoSpace);
            }
            getNextLine();
        }

        return todoConfiguration;
    }

public:
    // Wrapper - Loader
    static MemoryConfiguration load(QTextStream* bufferedReader) {
        // Wrap to other loaders, using one buffered reader
        reader = bufferedReader;
        readerLine.clear();
        return loadConfiguration();
    }
};

#pragma once
#include "MemoryMap/MemoryMap.h"
#include "MemoryMap/Region.h"
#include "MemoryMap/RegionData.h"
#include <QDebug>
#include <QRegularExpression>
#include <QTextStream>

class MemoryMapLoader {
    // Basic patterns
    static constexpr QStringView startMap = u"Linker script and memory map";
    static constexpr QStringView endMap = u"OUTPUT(";
    static constexpr QStringView regexOfRegion = u"(\\.[a-zA-Z_][\\.a-zA-Z0-9_]*)";
    static constexpr QStringView regexOfSubRegion = u" ([C\\.\\*]{1}\\S+( \\.\\S+)?)";
    static constexpr QStringView regexOfAddress = u"(0x[a-fA-F0-9]{8})";
    static constexpr QStringView regexOfSize = u"(0x[a-fA-F0-9]+)";
    static constexpr QStringView regexOfFill = u" FILL mask (0x[a-fA-F0-9]+)";

    // Compiled patters
    static inline const QRegularExpression patternOfFill{regexOfFill.toString()};
    static inline const QRegularExpression patternFindRegion{regexOfRegion % "(.*)"};
    static inline const QRegularExpression patternEndOfRegion = patternFindRegion;
    static inline const QRegularExpression patternFindSubRegion{regexOfSubRegion % "(.*)"};
    static inline const QRegularExpression patternEndOfSubRegion{" ?[C\\.\\*]{1}\\S+.*"};
    static inline const QRegularExpression patternRegionData{"\\s+" % regexOfAddress % "\\s+" % regexOfSize % "\\s*(.*)?$"};
    static inline const QRegularExpression patternSubRegionData{"\\s+" % regexOfAddress % "\\s+" % regexOfSize % "?\\s*(.*)$"};

    // Helper functions - Data
    static long getAddress(QString address) {
        // e.g.: "0x00000314" returns 788
        if(address == nullptr)
            return RegionData::DEFAULT_ADDRESS;
        else
            return address.mid(2).toLong(nullptr, 16);
    }

    static long getSize(QString size) {
        // e.g.: "0x4" returns 4
        if(size == nullptr)
            return RegionData::DEFAULT_SIZE;
        else
            return size.mid(2).toLong(nullptr, 16);
    }

    static int getFill(QString fill) {
        // e.g.: "0xff" returns 255
        if(fill == nullptr)
            return Region::DEFAULT_FILL;
        else
            return fill.mid(2).toInt(nullptr, 16);
    }

    // Helper functions - Reader
    static inline QTextStream* reader;
    static inline QString readerLine;

    static QString getNextLine() {
        // Read next line (skip blank lines) and store it AND return it
        try {
            while(!reader->atEnd()) {
                readerLine = reader->readLine();
                if(readerLine.length())
                    return readerLine;
            }
        } catch(std::exception e) {
            qCritical("Error reading next line: %s", e.what());
        }

        return readerLine;
    }

    // Helper functions - Loader
    static MemoryMap loadMap() {
        MemoryMap todoMap;

        // Find the start of the memory map
        while(!readerLine.startsWith(startMap))
            getNextLine();

        auto matcher = patternFindRegion.match(readerLine);
        while(!matcher.hasMatch() && !readerLine.startsWith(endMap)) {
            getNextLine();
            matcher = patternFindRegion.match(readerLine);
        }

        // Parse and store the regions of the memory map
        while(matcher.hasMatch() && !readerLine.startsWith(endMap)) {
            todoMap.addRegion(loadRegion());
            matcher = patternFindRegion.match(readerLine);
        }

        return todoMap;
    }

    static Region loadRegion() {
        Region todoRegion;

        // Find name, data, fill and subregions
        auto matcher = patternFindRegion.match(readerLine);
        if(matcher.hasMatch()) {
            // Get name and data
            todoRegion.setName(matcher.captured(1));
            if(matcher.captured(2).length() != 0) {
                // Data on the same line
                readerLine = matcher.captured(2);
                todoRegion.setData(loadRegionData());
            } else {
                // Data on the next line
                getNextLine();
                todoRegion.setData(loadRegionData());
            }

            // Check next line for fill
            getNextLine();
            matcher = patternOfFill.match(readerLine);
            if(matcher.hasMatch()) {
                todoRegion.setFill(getFill(matcher.captured(1)));
                getNextLine();
            }

            // Add subregions
            matcher = patternEndOfRegion.match(readerLine);
            while(!matcher.hasMatch() && !readerLine.startsWith(endMap)) {
                todoRegion.addSubRegion(loadSubRegion());
                matcher = patternEndOfRegion.match(readerLine);
            }
        }

        return todoRegion;
    }

    static SubRegion loadSubRegion() {
        SubRegion todoSubRegion;

        // Find name and data
        auto matcher = patternFindSubRegion.match(readerLine);
        if(matcher.hasMatch()) {
            todoSubRegion.setName(matcher.captured(1));
            if(matcher.captured(3).length() != 0) {
                // First data on the same line
                readerLine = matcher.captured(3);
                todoSubRegion.addData(loadSubRegionData());
            }
        } else {
            // Nameless subregion, just only add data
            todoSubRegion.addData(loadSubRegionData());
        }

        // More data on next line or no more data for this subregion
        getNextLine();
        matcher = patternEndOfSubRegion.match(readerLine);
        while(!matcher.hasMatch() && !readerLine.startsWith(endMap)) {
            todoSubRegion.addData(loadSubRegionData());
            getNextLine();
            matcher = patternEndOfSubRegion.match(readerLine);
        }

        return todoSubRegion;
    }

    static RegionData loadRegionData() {
        RegionData todoRegionData;

        // Find address, size and info (optional!)
        auto matcher = patternRegionData.match(readerLine);
        if(matcher.hasMatch()) {
            todoRegionData.setAddress(getAddress(matcher.captured(1)));
            todoRegionData.setSize(getSize(matcher.captured(2)));
            if(matcher.lastCapturedIndex() == 3)
                todoRegionData.setInfo(matcher.captured(3));
        }

        return todoRegionData;
    }

    static RegionData loadSubRegionData() {
        RegionData todoRegionData;

        // Find address, size (optional!) and info
        auto matcher = patternSubRegionData.match(readerLine);
        if(matcher.hasMatch()) {
            todoRegionData.setAddress(getAddress(matcher.captured(1)));
            todoRegionData.setInfo(matcher.captured(matcher.lastCapturedIndex()));
            if(matcher.lastCapturedIndex() == 3)
                todoRegionData.setSize(getSize(matcher.captured(2)));
        }

        return todoRegionData;
    }

    // Wrapper - Loader
public:
    static MemoryMap load(QTextStream* bufferedReader) {
        // Wrap to other loaders, using one buffered reader
        reader = bufferedReader;
        readerLine.clear();
        return loadMap();
    }
};

#pragma once
#include <QDebug>

#include "MemoryConfiguration/MemoryConfiguration.h"
#include "MemoryConfiguration/MemoryConfigurationLoader.h"
#include "MemoryContents/MemoryContents.h"
#include "MemoryContents/MemoryContentsLoader.h"
#include "MemoryMap/MemoryMap.h"
#include "MemoryMap/MemoryMapCompressor.h"
#include "MemoryMap/MemoryMapFilter.h"
#include "MemoryMap/MemoryMapLoader.h"

class MemoryInfoFactory {
    // Attributes

    MemoryConfiguration memoryConfiguration;
    MemoryContents memoryContents;
    MemoryMap rawMemoryMap;
    MemoryMap filteredMemoryMap;
    MemoryMap compressedMemoryMap;

    // Constructor
public:
    MemoryInfoFactory() = default;

    // Memory info creator

    void load(QFile* file) {
        try {
            // Load the configuration
            QTextStream fileReader{file};
            memoryConfiguration = MemoryConfigurationLoader::load(&fileReader);
            fileReader.seek(0);

            // Load, filter and compress the map
            rawMemoryMap = MemoryMapLoader::load(&fileReader);
            filteredMemoryMap = MemoryMapFilter::filter(rawMemoryMap);
            compressedMemoryMap = MemoryMapCompressor::compress(filteredMemoryMap);
            fileReader.seek(0);

            // Load the contents (actually created by the compressor!)
            memoryContents = MemoryContentsLoader::load();
        } catch(std::exception e) {
            qCritical("%s", e.what());
        }
    }

    // Getters

    MemoryConfiguration getMemoryConfiguration() { return memoryConfiguration; }
    MemoryContents getMemoryContents() { return memoryContents; }
    MemoryMap getRawMemoryMap() { return rawMemoryMap; }
    MemoryMap getFilteredMemoryMap() { return filteredMemoryMap; }
    MemoryMap getCompressedMemoryMap() { return compressedMemoryMap; }
};

#pragma once
#include "MemoryContents/MemoryContentsLoader.h"
#include "MemoryMap/MemoryMap.h"
#include "MemoryMap/RegionData.h"
#include <QDebug>
#include <QStringBuilder>
#include <ranges>

class MemoryMapCompressor {
    // Attributes - temporary storage of regiondata
    static inline long address;
    static inline long size;
    static inline QString info;
    static inline bool used;

    // Helper functions - temporary storage of regiondata
    static bool isStored() { return (size > 0); }
    static void clearStorage() { size = RegionData::DEFAULT_SIZE; }
    static void setUsed() { used = true; }
    static long getEndAddress() {
        if(!isStored())
            return RegionData::DEFAULT_ADDRESS;
        else
            return address + size;
    }

    static bool isToBeStored(RegionData regionData) {
        return ((regionData.getAddress() != RegionData::DEFAULT_ADDRESS) && (regionData.getSize() > 0));
    }

    static void store(RegionData regionData) {
        address = regionData.getAddress();
        size = regionData.getSize();
        info = regionData.getInfo();
        used = false;
    }

    static bool isLeftToBeAdded() { return (isStored() && !used); }

    static RegionData add() { return RegionData{address, size, info}; }

    static bool isToBeMerged(RegionData regionData) {
        return (isStored() && (regionData.getAddress() < getEndAddress()) && (regionData.getSize() == RegionData::DEFAULT_SIZE));
    }

    static RegionData merge(RegionData regionData, long mergeSize) {
        return RegionData{regionData.getAddress(), mergeSize, regionData.getInfo() % u" ("_s % info % u")"_s};
    }

    // Method to compress maps
public:
    static MemoryMap compress(MemoryMap map) {
        // Start gathering new memory contents
        MemoryContentsLoader::clear();

        MemoryMap compressedMap;
        for(int i = 0; i < map.getNumberOfRegionSize(); i++) {
            Region region = map.getRegion(i);
            Region compressedRegion{region.getName(), region.getData(), region.getFill(), {}};
            for(int j = 0; j < region.getNumberOfSubRegionSize(); j++) {
                SubRegion subRegion = region.getSubRegion(j);
                SubRegion compressedSubRegion;
                compressedSubRegion.setName(subRegion.getName());

                // Start of compression
                clearStorage();
                for(int k = 0; k < subRegion.getNumberOfRegionDataSize(); k++) {
                    RegionData regionData = subRegion.getRegionData(k);

                    if(isToBeStored(regionData)) {
                        // Store it, but first save anything left in storage not already used
                        if(isLeftToBeAdded()) {
                            compressedSubRegion.addData(add());
                            MemoryContentsLoader::addOtherContent(address, size, info, subRegion.getName());
                        }
                        store(regionData);
                    } else if(isToBeMerged(regionData)) {
                        // Add the merged data, keep in storage, but note that it has been used
                        long mergeSize = RegionData::DEFAULT_SIZE;
                        if((k + 1) < subRegion.getNumberOfRegionDataSize()) {
                            RegionData nextRegionData = subRegion.getRegionData(k + 1);
                            if(nextRegionData.getAddress() <= getEndAddress())
                                mergeSize = nextRegionData.getAddress() - regionData.getAddress();
                            else
                                mergeSize = getEndAddress() - regionData.getAddress();
                        } else {
                            mergeSize = getEndAddress() - regionData.getAddress();
                        }

                        setUsed();
                        compressedSubRegion.addData(merge(regionData, mergeSize));
                        MemoryContentsLoader::addIdentifierContent(regionData.getAddress(), mergeSize, regionData.getInfo(), info);
                    }
                    // else
                    //{
                    //	System.out.println(u"Unexpected data format while compressing..."_s);
                    // }
                }
                if(isLeftToBeAdded()) {
                    compressedSubRegion.addData(add());
                    MemoryContentsLoader::addOtherContent(address, size, info, subRegion.getName());
                }
                // End of compression

                if(compressedSubRegion.getNumberOfRegionDataSize() > 0)
                    compressedRegion.addSubRegion(compressedSubRegion);
            }

            if(compressedRegion.getNumberOfSubRegionSize() > 0)
                compressedMap.addRegion(compressedRegion);
        }

        // Memory contents have been gathered
        return compressedMap;
    }
};

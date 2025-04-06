#pragma once
#include "MemoryContents/DataContent.h"
#include "MemoryContents/FillContent.h"
#include "MemoryContents/IdentifierContent.h"
#include <QDebug>
#include <vector>

class MemoryContents {
    // Attributes
    std::vector<IdentifierContent> identifierContents;
    std::vector<DataContent> dataContents;
    std::vector<FillContent> fillContents;

public:
    // Constructors
    MemoryContents(std::vector<IdentifierContent> identifierContents, std::vector<DataContent> dataContents, std::vector<FillContent> fillContents)
        : identifierContents{std::move(identifierContents)}
        , dataContents{std::move(dataContents)}
        , fillContents{std::move(fillContents)} {
    }

    MemoryContents() = default;

    // Getters
    int getNumberOfIdentifierContents() { return identifierContents.size(); }
    IdentifierContent getIdentifierContent(int identifierContent) { return identifierContents.at(identifierContent); }
    int getNumberOfDataContents() { return dataContents.size(); }
    DataContent getDataContent(int dataContent) { return dataContents.at(dataContent); }
    int getNumberOfFillContents() { return fillContents.size(); }
    FillContent getFillContent(int fillContent) { return fillContents.at(fillContent); }

    // Setters
    void addIdentifierContent(IdentifierContent&& identifierContent) {
        identifierContents.emplace_back(std::move(identifierContent));
    }
    void addDataContent(DataContent&& dataContent) {
        dataContents.emplace_back(std::move(dataContent));
    }
    void addFillContent(FillContent&& fillContent) {
        fillContents.emplace_back(std::move(fillContent));
    }
};

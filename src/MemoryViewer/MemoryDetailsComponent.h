#pragma once
#include "MemoryConfiguration/MemoryConfiguration.h"
#include "MemoryConfiguration/Space.h"
#include "MemoryConfiguration/SpaceAttributes.h"
#include "MemoryContents/DataContent.h"
#include "MemoryContents/FillContent.h"
#include "MemoryContents/IdentifierContent.h"
#include "MemoryContents/MemoryContents.h"
#include "MemoryDetailsListener.h"
#include "MemoryMap/MemoryMap.h"
#include "MemoryMap/Region.h"
#include "MemoryMap/RegionData.h"
#include "MemoryMap/SubRegion.h"
#include <QDebug>
#include <QtWidgets>

class DetailsTableModel final : public QAbstractTableModel {
    // Attributes
    struct Property {
        QString property;
        QVariant value;
    };
    std::vector<Property> properties;

public:
    // Constructor
    DetailsTableModel(QObject* parent = nullptr);
    ~DetailsTableModel() override = default;

    // QAbstractItemModel interface
    int rowCount(const QModelIndex& = {}) const override;
    int columnCount(const QModelIndex& = {}) const override;

    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    // Make it possible to set a new details object
    void setDetails(QVariant detailsObject);

private:
    // Helpers
    QString toLargeHexString(long number);
    // Different details loaders
    void updateRow(int oldRows, int newRows);
    void load(DataContent dataContent);
    void load(FillContent fillContent);
    void load(IdentifierContent identifierContent);
    void load(MemoryConfiguration memoryConfiguration);
    void load(MemoryContents memoryContents);
    void load(MemoryMap memoryMap);
    void load(QString string);
    void load(Region region);
    void load(RegionData regionData);
    void load(Space space);
    void load(SpaceAttributes spaceAttributes);
    void load(SubRegion subRegion);
};

class MemoryDetailsComponent final : public QWidget, public MemoryDetailsListener {
    // Details table
    DetailsTableModel* detailsTableModel;
    QTableView* detailsTable;
    QGroupBox* memoryDetailsPanel;

public:
    // Constructor
    MemoryDetailsComponent(QWidget* parent = nullptr);

    void showDetails(QVariant object) override;
};

#include "MemoryDetailsComponent.h"

/// \brief DetailsTableModel::DetailsTableModel
/// \param parent
DetailsTableModel::DetailsTableModel(QObject* parent)
    : QAbstractTableModel{parent} {
}

int DetailsTableModel::rowCount(const QModelIndex&) const { return properties.size(); }

int DetailsTableModel::columnCount(const QModelIndex&) const { return 2; }

QVariant DetailsTableModel::data(const QModelIndex& index, int role) const {
    if(role == Qt::DisplayRole) {
        if(!index.column()) return properties[index.row()].property;
        else return properties[index.row()].value;
    }
    return {};
}

QVariant DetailsTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if(!section) return u"Property"_s;
        else return u"Value"_s;
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

void DetailsTableModel::setDetails(QVariant detailsObject) {
    if(detailsObject.isValid()) {
        auto tryLoad = [this]<typename... Ts>(QVariant& detailsObject) {
            return ([this]<typename T>(QVariant& detailsObject) {
                static const auto id = QMetaType::fromType<T>().id();
                if(detailsObject.userType() == id) {
                    load(detailsObject.value<T>());
                    return true;
                } else
                    return false;
            }.template operator()<Ts>(detailsObject)
                || ...);
        };

        if(tryLoad.operator()<MemoryMap,
               Region,
               SubRegion,
               RegionData,
               MemoryConfiguration,
               Space,
               SpaceAttributes,
               MemoryContents,
               IdentifierContent,
               DataContent,
               FillContent>(detailsObject)) {
        } else {
            load(detailsObject.toString());
        }
    }
}

QString DetailsTableModel::toLargeHexString(long number) {
    number &= 0xFFFFFFFF;
    return u"0x"_s % QString::number(0x100000000L | number, 16).mid(1).toUpper();
}

void DetailsTableModel::updateRow(int oldRows, int newRows) {
    if(oldRows > newRows) {
        beginRemoveRows({}, newRows, oldRows - 1);
        endRemoveRows();
    } else if(oldRows < newRows) {
        beginInsertRows({}, oldRows, newRows - 1);
        endInsertRows();
    } else {
        dataChanged(index(0, 0), index(newRows - 1, 1), {Qt::DisplayRole});
    }
}

void DetailsTableModel::load(DataContent dataContent) {
    int oldRows = properties.size();
    properties.clear();
    properties.emplace_back(u"Type"_s, u"DataContent"_s);
    properties.emplace_back(u"Data"_s, dataContent.getData());
    properties.emplace_back(u"Address"_s, toLargeHexString(dataContent.getAddress()));
    properties.emplace_back(u"Size"_s, QVariant::fromValue(dataContent.getSize()));
    updateRow(oldRows, properties.size());
}

void DetailsTableModel::load(FillContent fillContent) {
    int oldRows = properties.size();
    properties.clear();
    properties.emplace_back(u"Type"_s, u"FillContent"_s);
    properties.emplace_back(u"Fill"_s, fillContent.getFill());
    properties.emplace_back(u"Address"_s, toLargeHexString(fillContent.getAddress()));
    properties.emplace_back(u"Size"_s, QVariant::fromValue(fillContent.getSize()));
    updateRow(oldRows, properties.size());
}

void DetailsTableModel::load(IdentifierContent identifierContent) {
    int oldRows = properties.size();
    properties.clear();
    properties.emplace_back(u"Type"_s, u"IdentifierContent"_s);
    properties.emplace_back(u"Identifier"_s, identifierContent.getIdentifier());
    properties.emplace_back(u"File"_s, identifierContent.getFile());
    properties.emplace_back(u"Address"_s, toLargeHexString(identifierContent.getAddress()));
    properties.emplace_back(u"Size"_s, QVariant::fromValue(identifierContent.getSize()));
    updateRow(oldRows, properties.size());
}

void DetailsTableModel::load(MemoryConfiguration memoryConfiguration) {
    int oldRows = properties.size();
    properties.clear();
    properties.emplace_back(u"Type"_s, u"MemoryConfiguration"_s);
    properties.emplace_back(u"Number of spaces"_s, memoryConfiguration.getNumberOfSpaces());
    properties.emplace_back(u"Total size"_s, QVariant::fromValue(memoryConfiguration.getTotalSize()));
    updateRow(oldRows, properties.size());
}

void DetailsTableModel::load(MemoryContents memoryContents) {
    int oldRows = properties.size();
    properties.clear();
    properties.emplace_back(u"Type"_s, u"MemoryContents"_s);
    properties.emplace_back(u"Number of identifiers"_s, memoryContents.getNumberOfIdentifierContentSize());
    properties.emplace_back(u"Number of datas"_s, memoryContents.getNumberOfDataContentSize());
    properties.emplace_back(u"Number of fills"_s, memoryContents.getNumberOfFillContentSize());
    updateRow(oldRows, properties.size());
}

void DetailsTableModel::load(MemoryMap memoryMap) {
    int oldRows = properties.size();
    properties.clear();
    properties.emplace_back(u"Type"_s, u"MemoryMap"_s);
    properties.emplace_back(u"Number of regions"_s, memoryMap.getNumberOfRegionSize());
    updateRow(oldRows, properties.size());
}

void DetailsTableModel::load(QString string) {
    int oldRows = properties.size();
    properties.clear();
    updateRow(oldRows, properties.size());
}

void DetailsTableModel::load(Region region) {
    int oldRows = properties.size();
    properties.clear();
    properties.emplace_back(u"Type"_s, u"Region"_s);
    properties.emplace_back(u"Number of subregions"_s, region.getNumberOfSubRegionSize());
    properties.emplace_back(u"Name"_s, region.getName());
    properties.emplace_back(u"Fill"_s, region.getFill());
    properties.emplace_back(u"Address"_s, toLargeHexString(region.getData().getAddress()));
    properties.emplace_back(u"Size"_s, QVariant::fromValue(region.getData().getSize()));
    properties.emplace_back(u"Info"_s, region.getData().getInfo());
    updateRow(oldRows, properties.size());
}

void DetailsTableModel::load(RegionData regionData) {
    int oldRows = properties.size();
    properties.clear();
    properties.emplace_back(u"Type"_s, u"RegionData"_s);
    properties.emplace_back(u"Address"_s, toLargeHexString(regionData.getAddress()));
    properties.emplace_back(u"Size"_s, QVariant::fromValue(regionData.getSize()));
    properties.emplace_back(u"Info"_s, regionData.getInfo());
    updateRow(oldRows, properties.size());
}

void DetailsTableModel::load(Space space) {
    int oldRows = properties.size();
    properties.clear();
    properties.emplace_back(u"Type"_s, u"Space"_s);
    properties.emplace_back(u"Name"_s, space.getName());
    properties.emplace_back(u"Origin"_s, toLargeHexString(space.getOrigin()));
    properties.emplace_back(u"Length"_s, toLargeHexString(space.getLength()));
    properties.emplace_back(u"Erasable"_s, space.getAttributes().isErasable());
    properties.emplace_back(u"Readable"_s, space.getAttributes().isReadable());
    properties.emplace_back(u"Writable"_s, space.getAttributes().isWritable());
    updateRow(oldRows, properties.size());
}

void DetailsTableModel::load(SpaceAttributes spaceAttributes) {
    int oldRows = properties.size();
    properties.clear();
    properties.emplace_back(u"Type"_s, u"SpaceAttributes"_s);
    properties.emplace_back(u"Erasable"_s, spaceAttributes.isErasable());
    properties.emplace_back(u"Readable"_s, spaceAttributes.isReadable());
    properties.emplace_back(u"Writable"_s, spaceAttributes.isWritable());
    updateRow(oldRows, properties.size());
}

void DetailsTableModel::load(SubRegion subRegion) {
    int oldRows = properties.size();
    properties.clear();
    properties.emplace_back(u"Type"_s, u"SubRegion"_s);
    properties.emplace_back(u"Number of regiondata"_s, subRegion.getNumberOfRegionDataSize());
    properties.emplace_back(u"Name"_s, subRegion.getName());
    updateRow(oldRows, properties.size());
}

/// \brief MemoryDetailsComponent::MemoryDetailsComponent
/// \param parent
MemoryDetailsComponent::MemoryDetailsComponent(QWidget* parent)
    : QWidget{parent} {
    // Create contents
    memoryDetailsPanel = new QGroupBox{u"Memory Details"_s, this};
    detailsTable = new QTableView{memoryDetailsPanel};
    detailsTableModel = new DetailsTableModel{detailsTable};
    detailsTable->setModel(detailsTableModel);
    detailsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    detailsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    detailsTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    detailsTable->setWordWrap(false);
    auto layout = new QVBoxLayout{this};
    layout->addWidget(memoryDetailsPanel);
    layout = new QVBoxLayout{memoryDetailsPanel};
    layout->addWidget(detailsTable);

    // Set preferred sizes
    // setMinimumSize(300, 700);
}

void MemoryDetailsComponent::showDetails(QVariant object) {
    detailsTableModel->setDetails(object);
}

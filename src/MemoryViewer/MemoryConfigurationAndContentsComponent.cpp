#include "MemoryConfigurationAndContentsComponent.h"

MemoryConfigurationTableModel::MemoryConfigurationTableModel(QObject* parent)
    : QAbstractTableModel{parent} {
}

auto MemoryConfigurationTableModel::getMemoryConfiguration() const { return memoryConfigurationEnables; }

void MemoryConfigurationTableModel::setMemoryConfiguration(MemoryConfiguration* newMemoryConfiguration) {
    if(newMemoryConfiguration != nullptr) {
        memoryConfiguration = *newMemoryConfiguration;

        auto update = [this] {
            memoryConfigurationEnables.resize(memoryConfiguration.getNumberOfSpaces());
            std::ranges::fill(memoryConfigurationEnables, true);
        };

        // Notify listeners
        if(memoryConfiguration.getNumberOfSpaces() > memoryConfigurationEnables.size()) {
            beginInsertRows({}, memoryConfigurationEnables.size(), memoryConfiguration.getNumberOfSpaces() - 1);
            update();
            endInsertRows();
        } else if(memoryConfiguration.getNumberOfSpaces() < memoryConfigurationEnables.size()) {
            beginRemoveRows({}, memoryConfiguration.getNumberOfSpaces(), memoryConfigurationEnables.size() - 1);
            update();
            endRemoveRows();
        } else {
            update();
            dataChanged(index(0, 0), index(memoryConfigurationEnables.size() - 1, 0), {Qt::DisplayRole, Qt::CheckStateRole});
        }
    }
}

int MemoryConfigurationTableModel::rowCount(const QModelIndex&) const { return memoryConfiguration.getNumberOfSpaces(); }

int MemoryConfigurationTableModel::columnCount(const QModelIndex&) const { return 1; }

QVariant MemoryConfigurationTableModel::data(const QModelIndex& index, int role) const {
    if(role == Qt::CheckStateRole) return memoryConfigurationEnables[index.row()] ? Qt::Checked : Qt::Unchecked;
    if(role == Qt::DisplayRole) return memoryConfiguration.getSpace(index.row()).getName();
    if(role == Qt::UserRole) return QVariant::fromValue(memoryConfiguration.getSpace(index.row()));
    return {};
}

QVariant MemoryConfigurationTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) return "Name";
    return QAbstractTableModel::headerData(section, orientation, role);
}

Qt::ItemFlags MemoryConfigurationTableModel::flags(const QModelIndex& index) const { return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable; }

bool MemoryConfigurationTableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if(role == Qt::CheckStateRole) {
        memoryConfigurationEnables[index.row()] = value.value<Qt::CheckState>() == Qt::Checked;
        return true;
    }
    return {};
}

DefaultListModel::DefaultListModel(QObject* parent)
    : QAbstractListModel{parent} { }

void DefaultListModel::clear() {
    beginRemoveRows({}, 0, data_.size() - 1);
    data_.clear();
    endRemoveRows();
}

int DefaultListModel::rowCount(const QModelIndex&) const { return data_.size(); }

QVariant DefaultListModel::data(const QModelIndex& index, int role) const {
    auto& object = data_[index.row()];
    if(role == Qt::DisplayRole) {
        if(static const auto id = QMetaType::fromType<IdentifierContent>().id(); object.userType() == id) {
            auto identifierContent = object.value<IdentifierContent>();
            return identifierContent.getIdentifier();
        } else if(static const auto id = QMetaType::fromType<DataContent>().id(); object.userType() == id) {
            auto dataContent = object.value<DataContent>();
            return dataContent.getData();
        } else if(static const auto id = QMetaType::fromType<FillContent>().id(); object.userType() == id) {
            auto fillContent = object.value<FillContent>();
            return QString{QString::number(fillContent.getSize()) % " byte(s) of " % QString::number(fillContent.getFill())};
        }
    }
    if(role == Qt::EditRole) return object.typeName();
    if(role == Qt::UserRole) return object;
    return {};
}

MemoryContentsList::MemoryContentsList(QWidget* parent)
    : QListView{parent} {
    // Set specific model and renderer
    proxyModel = new QSortFilterProxyModel{this};
    memoryContentsListModel = new DefaultListModel{proxyModel};
    proxyModel->setSourceModel(memoryContentsListModel);
    proxyModel->setFilterRole(Qt::EditRole);
    QListView::setModel(memoryContentsListModel /*proxyModel*/);
    QListView::setSelectionMode(QListView::SingleSelection);
}

void MemoryContentsList::setMemoryContents(MemoryContents&& memoryContents) {
    // Clear list
    memoryContentsListModel->clear();
    // Add all content, if any
    for(auto&& content: memoryContents.getNumberOfIdentifierContents())
        memoryContentsListModel->addElement(content);
    for(auto&& content: memoryContents.getNumberOfDataContents())
        memoryContentsListModel->addElement(content);
    for(auto&& content: memoryContents.getNumberOfFillContents())
        memoryContentsListModel->addElement(content);
    reset();
}

MemoryConfigurationAndContentsComponent::MemoryConfigurationAndContentsComponent(QWidget* parent)
    : QWidget{parent} {

    // memoryConfigurationEnables = new bool[0];

    // Create contents
    memoryConfigurationPanel = new QGroupBox{u"Memory Configuration"_s, this};
    memoryConfigurationTable = new QTableView{memoryConfigurationPanel};
    memoryConfigurationTableModel = new MemoryConfigurationTableModel{memoryConfigurationTable};
    memoryConfigurationTable->setModel(memoryConfigurationTableModel);
    memoryConfigurationTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    memoryConfigurationTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    memoryConfigurationTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    auto layout = new QVBoxLayout{memoryConfigurationPanel};
    layout->addWidget(memoryConfigurationTable);

    memoryContentsPanel = new QGroupBox{"Memory Contents", this};
    memoryContentsSelectionBox = new QComboBox{memoryContentsPanel};
    memoryContentsSelectionBox->addItem(SELECTION_IDENTIFIER.toString());
    memoryContentsSelectionBox->addItem(SELECTION_DATA.toString());
    memoryContentsSelectionBox->addItem(SELECTION_FILL.toString());
    memoryContentsList = new MemoryContentsList{memoryContentsPanel};
    layout = new QVBoxLayout{memoryContentsPanel};
    layout->addWidget(memoryContentsSelectionBox);
    layout->addWidget(memoryContentsList);
    // layout->setSpacing(0);
    layout->setStretch(1, 1);

    layout = new QVBoxLayout{this};
    layout->addWidget(memoryConfigurationPanel);
    layout->addWidget(memoryContentsPanel);
    layout->setStretch(0, 1);
    layout->setStretch(1, 5);
    // Add listeners
    // memoryConfigurationTableModel.addMemoryConfigurationSelectionListener(this);

    // Set preferred sizes
    // setPreferredSize(300, 100);

    connect(memoryConfigurationTable, &QAbstractItemView::clicked,
        this, &MemoryConfigurationAndContentsComponent::notifyMemoryDetailsListeners);
    connect(memoryContentsList, &QAbstractItemView::clicked,
        this, &MemoryConfigurationAndContentsComponent::notifyMemoryDetailsListeners);
    connect(memoryContentsSelectionBox, &QComboBox::currentIndexChanged,
        this, &MemoryConfigurationAndContentsComponent::itemStateChanged);
}

void MemoryConfigurationAndContentsComponent::addMemoryDetailsListener(MemoryDetailsListener* memoryDetailsListener) {
    memoryDetailsListeners.emplace_back(memoryDetailsListener);
}

void MemoryConfigurationAndContentsComponent::notifyMemoryDetailsListeners(QModelIndex object) {
    for(auto listener: memoryDetailsListeners)
        listener->showDetails(object.data(Qt::UserRole));
    updateContents();
}

void MemoryConfigurationAndContentsComponent::itemStateChanged(int) {
    updateContents();
}

void MemoryConfigurationAndContentsComponent::update(MemoryInfoFactory* memoryInfoFactory) {
    memoryConfiguration = memoryInfoFactory->getMemoryConfiguration();
    memoryContents = memoryInfoFactory->getMemoryContents();

    updateConfiguration();
    updateContents();
}

void MemoryConfigurationAndContentsComponent::updateConfiguration() {
    memoryConfigurationTableModel->setMemoryConfiguration(&memoryConfiguration);
    memoryConfigurationTable->setMinimumHeight(memoryConfigurationTable->rowHeight(0) * 3);
}

void MemoryConfigurationAndContentsComponent::updateContents() {
    memoryContentsList->setMemoryContents(filterMemoryContents());
}

MemoryContents MemoryConfigurationAndContentsComponent::filterMemoryContents() {
    // if(memoryContents == nullptr) return nullptr;
    MemoryContents filteredMemoryContents;
    auto currentText = memoryContentsSelectionBox->currentText();
    if(currentText == SELECTION_IDENTIFIER) {
        // Add all identifiers in selected range
        for(auto content: memoryContents.getNumberOfIdentifierContents())
            if(isInSelectedRange(content.getAddress()))
                filteredMemoryContents.addIdentifierContent(std::move(content));
    } else if(currentText == SELECTION_DATA) {
        // Add all datas in selected range
        for(auto content: memoryContents.getNumberOfDataContents())
            if(isInSelectedRange(content.getAddress()))
                filteredMemoryContents.addDataContent(std::move(content));
    } else if(currentText == SELECTION_FILL) {
        // Add all fills in selected range
        for(auto content: memoryContents.getNumberOfFillContents())
            if(isInSelectedRange(content.getAddress()))
                filteredMemoryContents.addFillContent(std::move(content));
    }

    return filteredMemoryContents;
}

bool MemoryConfigurationAndContentsComponent::isInSelectedRange(long address) {
    for(auto [i, memoryConfigurationEnables]: std::ranges::enumerate_view(memoryConfigurationTableModel->getMemoryConfiguration())) {
        if(memoryConfigurationEnables) {
            Space space = memoryConfiguration.getSpace(i);
            if((space.getOrigin() <= address) && (space.getOrigin() + space.getLength()) > address)
                return true;
        }
    }
    return false;
}

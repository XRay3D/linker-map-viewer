#pragma once
#include "MemoryConfiguration/MemoryConfiguration.h"
#include "MemoryConfiguration/Space.h"
#include "MemoryContents/DataContent.h"
#include "MemoryContents/FillContent.h"
#include "MemoryContents/IdentifierContent.h"
#include "MemoryContents/MemoryContents.h"
#include "MemoryDetailsListener.h"
#include "MemoryInfoFactory.h"
#include <QAbstractTableModel>
#include <QDebug>
#include <QtWidgets>
#include <any>
#include <vector>

class MemoryConfigurationTableModel : public QAbstractTableModel {
    Q_OBJECT
    // Attributes
    // std::vector<MemoryConfigurationSelectionListener> memoryConfigurationSelectionListeners;
    MemoryConfiguration memoryConfiguration;
    std::vector<bool> memoryConfigurationEnables;
    // Constructor
public:
    MemoryConfigurationTableModel(QObject* parent = nullptr)
        : QAbstractTableModel{parent} {
    }
    virtual ~MemoryConfigurationTableModel() override = default;

    bool at(int i) const { return memoryConfigurationEnables.at(i); }

    // Make it possible to set a new configuration
    void setMemoryConfiguration(MemoryConfiguration* newMemoryConfiguration) {
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

    // QAbstractItemModel interface
    int rowCount(const QModelIndex& = {}) const override { return memoryConfiguration.getNumberOfSpaces(); }
    int columnCount(const QModelIndex& = {}) const override { return 1; }
    QVariant data(const QModelIndex& index, int role) const override {
        if(role == Qt::CheckStateRole) return memoryConfigurationEnables[index.row()] ? Qt::Checked : Qt::Unchecked;
        if(role == Qt::DisplayRole) return memoryConfiguration.getSpace(index.row()).getName();
        if(role == Qt::UserRole) return QVariant::fromValue(memoryConfiguration.getSpace(index.row()));
        return {};
    }
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override {
        if(orientation == Qt::Horizontal && role == Qt::DisplayRole) return "Name";
        return QAbstractTableModel::headerData(section, orientation, role);
    }
    Qt::ItemFlags flags(const QModelIndex& index) const override { return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable; }
    bool setData(const QModelIndex& index, const QVariant& value, int role) override {
        if(role == Qt::CheckStateRole) return memoryConfigurationEnables[index.row()] = value.value<Qt::CheckState>() == Qt::Checked, true;
        return {};
    }
signals:
    void selectionChanged(std::vector<bool>);
};

template <typename... Func>
struct Overload : Func... {
    using Func::operator()...;
};
template <typename... Func>
Overload(Func...) -> Overload<Func...>;

template <typename T>
class DefaultListModel : public QAbstractListModel {
    // Q_OBJECT
    std::vector<T> data_;

public:
    DefaultListModel(QObject* parent = nullptr)
        : QAbstractListModel{parent} { }
    virtual ~DefaultListModel() override = default;

    // QAbstractItemModel interface
    int rowCount(const QModelIndex& = {}) const override { return data_.size(); }

    QVariant data(const QModelIndex& index, int role) const override {
        auto& object = data_[index.row()];
        if(role == Qt::DisplayRole) {
            try {
                IdentifierContent identifierContent = std::any_cast<IdentifierContent>(object);
                return identifierContent.getIdentifier();
            } catch(...) { }
            try {
                DataContent dataContent = std::any_cast<DataContent>(object);
                return dataContent.getData();
            } catch(...) { }
            try {
                FillContent fillContent = std::any_cast<FillContent>(object);
                return QString{QString::number(fillContent.getSize()) % " byte(s) of " % QString::number(fillContent.getFill())};
            } catch(...) { }
        }
        if(role == Qt::EditRole) {
            return object.type().name();
        }
        return {};
    }
    void clear() {
        beginRemoveRows({}, 0, data_.size() - 1);
        data_.clear();
        endRemoveRows();
    }
    template <typename Ty>
    void addElement(Ty&& val) {
        beginInsertRows({}, data_.size(), data_.size());
        data_.emplace_back(std::move(val));
        endInsertRows();
    }
};

class MemoryContentsList : public QListView {

    // Attributes
    DefaultListModel<std::any /*MemoryContent*/>* memoryContentsListModel;
    QSortFilterProxyModel* proxyModel;

public:
    // Constructor
    MemoryContentsList(QWidget* parent = nullptr)
        : QListView{parent} {
        // Set specific model and renderer
        proxyModel = new QSortFilterProxyModel{this};
        memoryContentsListModel = new DefaultListModel<std::any /*MemoryContent*/>{proxyModel};
        proxyModel->setSourceModel(memoryContentsListModel);
        proxyModel->setFilterRole(Qt::EditRole);
        QListView::setModel(proxyModel);
        QListView::setSelectionMode(QListView::SingleSelection);
    }

    // Update
    void setMemoryContents(MemoryContents&& memoryContents) {
        // Clear list
        memoryContentsListModel->clear();
        // Add all content, if any
        for(int i = 0; i < memoryContents.getNumberOfIdentifierContents(); i++)
            memoryContentsListModel->addElement(memoryContents.getIdentifierContent(i));
        for(int i = 0; i < memoryContents.getNumberOfDataContents(); i++)
            memoryContentsListModel->addElement(memoryContents.getDataContent(i));
        for(int i = 0; i < memoryContents.getNumberOfFillContents(); i++)
            memoryContentsListModel->addElement(memoryContents.getFillContent(i));
    }

private:
    // Appearance
    // QString getMemoryContentsListText(Object object) {
    //     if(object instanceof IdentifierContent) {
    //         IdentifierContent identifierContent = (IdentifierContent)object;
    //         return identifierContent.getIdentifier();
    //     } else if(object instanceof DataContent) {
    //         DataContent dataContent = (DataContent)object;
    //         return dataContent.getData();
    //     } else if(object instanceof FillContent) {
    //         FillContent fillContent = (FillContent)object;
    //         return fillContent.getSize() + " byte(s) of " + fillContent.getFill();
    //     } else {
    //         return object.toString();
    //     }
    // }

    // class MemoryContentsCellRenderer : public DefaultListCellRenderer {
    //
    // public: Component getListCellRendererComponent (JList<?> list, Object value, int index, bool isSelected, bool hasFocus) {
    //         JLabel label = (JLabel)super.getListCellRendererComponent(list, value, index, isSelected, hasFocus);
    //         label.setText(getMemoryContentsListText(value));
    //         return label;
    //     }
    // };
};

class MemoryConfigurationAndContentsComponent : public QWidget {

    // Constants
    static constexpr QStringView SELECTION_IDENTIFIER = u"Identifier";
    static constexpr QStringView SELECTION_DATA = u"Data";
    static constexpr QStringView SELECTION_FILL = u"Fill";
    // Listeners
    std::vector<MemoryDetailsListener*> memoryDetailsListeners;
    // Memory info
    MemoryConfiguration memoryConfiguration;
    std::vector<bool> memoryConfigurationEnables;
    MemoryContents memoryContents;
    // Configuration view
    MemoryConfigurationTableModel* memoryConfigurationTableModel;
    QTableView* memoryConfigurationTable;
    QGroupBox* memoryConfigurationPanel;
    // Contents view
    QComboBox* memoryContentsSelectionBox;
    MemoryContentsList* memoryContentsList;
    QGroupBox* memoryContentsPanel;

public:
    // Constructor
    MemoryConfigurationAndContentsComponent(QWidget* parent = nullptr)
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
        layout->setSpacing(0);
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

        connect(memoryConfigurationTableModel, &MemoryConfigurationTableModel::selectionChanged,
            this, &MemoryConfigurationAndContentsComponent::selectionChanged);
        connect(memoryConfigurationTable, &QAbstractItemView::clicked,
            this, &MemoryConfigurationAndContentsComponent::notifyMemoryDetailsListeners);
        connect(memoryContentsList, &QAbstractItemView::clicked,
            this, &MemoryConfigurationAndContentsComponent::notifyMemoryDetailsListeners);
        connect(memoryContentsSelectionBox, &QComboBox::currentIndexChanged,
            this, &MemoryConfigurationAndContentsComponent::itemStateChanged);
    }

    void addMemoryDetailsListener(MemoryDetailsListener* memoryDetailsListener) {
        memoryDetailsListeners.emplace_back(memoryDetailsListener);
    }

private:
    void notifyMemoryDetailsListeners(QModelIndex object) {
        for(int i = 0; i < memoryDetailsListeners.size(); i++)
            memoryDetailsListeners.at(i)->showDetails(object.data(Qt::UserRole));
    }

    // MemoryConfigurationSelectionListener
public:
    void selectionChanged(std::vector<bool> newConfigurationEnables) {
        memoryConfigurationEnables = newConfigurationEnables;
        updateContents();
    }

    // ItemListener
    void itemStateChanged(int) {
        updateContents();
    }

    // MemoryInfoUpdateListener
    void update(MemoryInfoFactory* memoryInfoFactory) {
        memoryConfiguration = memoryInfoFactory->getMemoryConfiguration();
        memoryContents = memoryInfoFactory->getMemoryContents();

        updateConfiguration();
        updateContents();
    }

    // Updaters
private:
    void updateConfiguration() {
        memoryConfigurationTableModel->setMemoryConfiguration(&memoryConfiguration);
        memoryConfigurationTable->setMinimumHeight(memoryConfigurationTable->rowHeight(0) * 3);
    }

    void updateContents() {
        memoryContentsList->setMemoryContents(filterMemoryContents());
    }

    // Filter
    MemoryContents filterMemoryContents() {
        // if(memoryContents == nullptr) return nullptr;
        MemoryContents filteredMemoryContents;
        auto currentText = memoryContentsSelectionBox->currentText();
        if(currentText == SELECTION_IDENTIFIER) {
            // Add all identifiers in selected range
            for(int i = 0; i < memoryContents.getNumberOfIdentifierContents(); i++) {
                if(isInSelectedRange(memoryContents.getIdentifierContent(i).getAddress())) {
                    filteredMemoryContents.addIdentifierContent(memoryContents.getIdentifierContent(i));
                }
            }
        } else if(currentText == SELECTION_DATA) {
            // Add all datas in selected range
            for(int i = 0; i < memoryContents.getNumberOfDataContents(); i++) {
                if(isInSelectedRange(memoryContents.getDataContent(i).getAddress())) {
                    filteredMemoryContents.addDataContent(memoryContents.getDataContent(i));
                }
            }
        } else if(currentText == SELECTION_FILL) {
            // Add all fills in selected range
            for(int i = 0; i < memoryContents.getNumberOfFillContents(); i++) {
                if(isInSelectedRange(memoryContents.getFillContent(i).getAddress())) {
                    filteredMemoryContents.addFillContent(memoryContents.getFillContent(i));
                }
            }
        }

        return filteredMemoryContents;
    }

    bool isInSelectedRange(long address) {
        for(int i = 0; i < memoryConfigurationEnables.size(); i++) {
            if(memoryConfigurationEnables.at(i)) {
                Space space = memoryConfiguration.getSpace(i);
                if((space.getOrigin() <= address) && (space.getOrigin() + space.getLength()) > address)
                    return true;
            }
        }
        return false;
    }
};

Q_DECLARE_METATYPE(const std::vector<bool>&)

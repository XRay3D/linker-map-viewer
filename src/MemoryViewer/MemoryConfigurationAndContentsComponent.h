#pragma once
#include "MemoryConfiguration/MemoryConfiguration.h"
#include "MemoryConfiguration/Space.h"
#include "MemoryContents/DataContent.h"
#include "MemoryContents/FillContent.h"
#include "MemoryContents/IdentifierContent.h"
#include "MemoryContents/MemoryContents.h"
#include "MemoryDetailsListener.h"
#include "MemoryInfoFactory.h"
#include "MemoryInfoUpdateListener.h"
#include <QAbstractTableModel>
#include <QDebug>
#include <QtWidgets>
#include <any>
#include <ranges>
#include <vector>

class MemoryConfigurationTableModel final : public QAbstractTableModel {
    // Attributes
    // std::vector<MemoryConfigurationSelectionListener> memoryConfigurationSelectionListeners;
    MemoryConfiguration memoryConfiguration;
    std::vector<bool> memoryConfigurationEnables;
    // Constructor
public:
    MemoryConfigurationTableModel(QObject* parent = nullptr);
    virtual ~MemoryConfigurationTableModel() override = default;

    auto getMemoryConfiguration() const;

    // Make it possible to set a new configuration
    void setMemoryConfiguration(MemoryConfiguration* newMemoryConfiguration);

    // QAbstractItemModel interface
    int rowCount(const QModelIndex& = {}) const override;
    int columnCount(const QModelIndex& = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
};

template <typename... Func>
struct Overload : Func... {
    using Func::operator()...;
};
template <typename... Func>
Overload(Func...) -> Overload<Func...>;

class DefaultListModel final : public QAbstractListModel {
    // Q_OBJECT
    std::vector<QVariant> data_;

public:
    DefaultListModel(QObject* parent = nullptr);
    virtual ~DefaultListModel() override = default;

    void clear();
    template <typename Ty>
    void addElement(Ty&& val) {
        beginInsertRows({}, data_.size(), data_.size());
        data_.emplace_back(QVariant::fromValue(std::move(val)));
        endInsertRows();
    }

    // QAbstractItemModel interface
    int rowCount(const QModelIndex& = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
};

class MemoryContentsList final : public QListView {
    // Attributes
    DefaultListModel* memoryContentsListModel;
    QSortFilterProxyModel* proxyModel;

public:
    // Constructor
    MemoryContentsList(QWidget* parent = nullptr);

    // Update
    void setMemoryContents(MemoryContents&& memoryContents);
};

class MemoryConfigurationAndContentsComponent final : public QWidget, public MemoryInfoUpdateListener {

    // Constants
    static constexpr QStringView SELECTION_IDENTIFIER = u"Identifier";
    static constexpr QStringView SELECTION_DATA = u"Data";
    static constexpr QStringView SELECTION_FILL = u"Fill";
    // Listeners
    std::vector<MemoryDetailsListener*> memoryDetailsListeners;
    // Memory info
    MemoryConfiguration memoryConfiguration;
    // std::vector<bool> memoryConfigurationEnables;
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
    MemoryConfigurationAndContentsComponent(QWidget* parent = nullptr);

    void addMemoryDetailsListener(MemoryDetailsListener* memoryDetailsListener);

private:
    void notifyMemoryDetailsListeners(QModelIndex object);

public:
    // MemoryConfigurationSelectionListener
    // void selectionChanged(std::vector<bool> newConfigurationEnables) {
    //     memoryConfigurationEnables = newConfigurationEnables;
    //     updateContents();
    // }

    // ItemListener
    void itemStateChanged(int);

    // MemoryInfoUpdateListener
    void update(MemoryInfoFactory* memoryInfoFactory) override;

private:
    // Updaters
    void updateConfiguration();

    void updateContents();

    // Filter
    MemoryContents filterMemoryContents();

    bool isInSelectedRange(long address);
};

Q_DECLARE_METATYPE(const std::vector<bool>&)
Q_DECLARE_METATYPE(const IdentifierContent&)
Q_DECLARE_METATYPE(const DataContent&)
Q_DECLARE_METATYPE(const FillContent&)


#pragma once
#include "MemoryDetailsListener.h"
#include "MemoryInfoUpdateListener.h"
#include "MemoryMap/MemoryMap.h"
#include "MemoryMap/Region.h"
#include "MemoryMap/RegionData.h"
#include "MemoryMap/SubRegion.h"
#include <QDebug>
#include <QtWidgets>
// #include <qcustomplot.h>
#include <vector>

struct TreeNode {
    QVariant data_;
    TreeNode* parent;
    std::vector<std::shared_ptr<TreeNode>> children;
    TreeNode(QVariant data);

    template <typename T>
    TreeNode(T&& data)
        : data_{QVariant::fromValue(std::move(data))} { }

    ~TreeNode();
    TreeNode* add(TreeNode* child);

    TreeNode* child(int row);

    int columnCount() const;

    int childCount() const;

    QVariant data(int column, int role) const;
    TreeNode* parentNode();

    int row() const;
};

class MemoryMapTreeModel final : public QAbstractItemModel {
    Q_OBJECT
    std::shared_ptr<TreeNode> root{new TreeNode{u"No map loaded..."_s}};

public:
    MemoryMapTreeModel(QObject* parent = nullptr);
    ~MemoryMapTreeModel() override = default;

    // QAbstractItemModel interface
public:
    void setRoot(TreeNode* newRoot);
    QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // QVariant headerData(int section, Qt::Orientation orientation,
    //     int role) const override {
    //     if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
    //         return root->data(section);
    //     return QVariant();
    // }
};

class MemoryMapTree final : public QTreeView {
#if 0
public:
    using QTreeView::QTreeView;
#else
    // Attributes
    MemoryMapTreeModel* memoryMapTreeModel;

public:
    // Constructor
    MemoryMapTree(QWidget* parent = nullptr);

    void setMemoryMap(const MemoryMap& memoryMap);
#endif
};

Q_DECLARE_METATYPE(const Region&)
Q_DECLARE_METATYPE(const SubRegion&)
Q_DECLARE_METATYPE(const RegionData&)

class MemoryMapPicture final : public QWidget {
    // Constants
    static constexpr double zoomFactor = 2;
    static constexpr double maxZoomIn = 8;
    static constexpr double minZoomIn = 0.001;
    static constexpr int offsetX = 20;
    static constexpr int offsetY = 20;
    static constexpr int width = 200;
    static constexpr int selectedRegionWidth = 50;

    // Attributes
    Region selectedRegion;
    SubRegion selectedSubRegion;
    RegionData selectedRegionData;
    double scale;
    bool focus;

    // Constructor
public:
    MemoryMapPicture(QWidget* parent = nullptr);
    void setSelectedRegion(Region region);
    void setSelectedSubRegion(SubRegion subRegion);
    void setSelectedRegionData(RegionData regionData);
    void zoomIn();
    void zoomOut();
    void paintEvent(QPaintEvent* event) override;
};

class MemoryMapComponent final : public QSplitter, public MemoryInfoUpdateListener /*, public TreeSelectionListener, public ItemListener, public ActionListener */ {

    // Constants
    static constexpr QStringView SELECTION_RAW = u"Raw";
    static constexpr QStringView SELECTION_FILTERED = u"Filtered";
    static constexpr QStringView SELECTION_COMPRESSED = u"Compressed";

    // Listeners
    std::vector<MemoryDetailsListener*> memoryDetailsListeners;
    // QSplitter* splitPane;
    // Memory info
    MemoryMap rawMemoryMap;
    MemoryMap filteredMemoryMap;
    MemoryMap compressedMemoryMap;

    // Tree view
    QComboBox* memoryMapSelectionBox;
    MemoryMapTree* memoryMapTree;
    QGroupBox* memoryTreePanel;

    // Picture view
    MemoryMapPicture* memoryMapPicture;
    // JPanel zoomPanel;
    QPushButton* buttonZoomIn;
    QPushButton* buttonZoomOut;
    QGroupBox* memoryPicturePanel;

    QWidget* memoryTreeBorder;
    QWidget* memoryPictureBorder;

    // Constructor
public:
    MemoryMapComponent(QWidget* parent = nullptr);

    void addMemoryDetailsListener(MemoryDetailsListener* memoryDetailsListener);
    // MemoryInfoUpdateListener
    void update(MemoryInfoFactory* memoryInfoFactory) override;

private:
    void notifyMemoryDetailsListeners(QModelIndex object);

    // ItemListener
    void itemStateChanged(int index);

    // TreeSelectionListener
    void valueChanged(QModelIndex e);

    // Updaters
    void updateTree();

    void updatePicture();
};

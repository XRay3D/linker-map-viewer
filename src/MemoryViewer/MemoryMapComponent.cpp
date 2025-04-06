#include "MemoryMapComponent.h"

TreeNode::TreeNode(QVariant data)
    : data_{data} { }

TreeNode::~TreeNode() { qCritical(""); }

TreeNode* TreeNode::add(TreeNode* child) { return children.emplace_back(child)->parent = this, child; }

TreeNode* TreeNode::child(int row) {
    if(row < 0 || row >= children.size()) return nullptr;
    return children.at(row).get();
}

int TreeNode::columnCount() const { return 1; }

int TreeNode::childCount() const { return children.size(); }

QVariant TreeNode::data(int column, int role) const {
    if(role == Qt::DisplayRole) {
        if(static const auto id = QMetaType::fromType<MemoryMap>().id(); data_.userType() == id) {
            return u"Memory Map"_s;
        } else if(static const auto id = QMetaType::fromType<Region>().id(); data_.userType() == id) {
            return u"Region: "_s + data_.value<Region>().getName();
        } else if(static const auto id = QMetaType::fromType<SubRegion>().id(); data_.userType() == id) {
            return u"SubRegion: "_s + data_.value<SubRegion>().getName();
        } else if(static const auto id = QMetaType::fromType<RegionData>().id(); data_.userType() == id) {
            auto regionData = data_.value<RegionData>();
            return u"Regiondata @ 0x"_s + QString::number(0x100000000L | (regionData.getAddress() & 0xFFFFFFFF)).mid(1).toUpper();
        }
        return data_.toString();
    }
    if(role == Qt::UserRole) return data_;
    return {};
}

TreeNode* TreeNode::parentNode() { return parent; }

int TreeNode::row() const {
    if(parent) {
        auto iter = std::ranges::find(parent->children, this, &std::shared_ptr<TreeNode>::get);
        if(iter == parent->children.end()) return 0;
        return std::ranges::distance(parent->children.begin(), iter);
    }
    return 0;
}

MemoryMapTreeModel::MemoryMapTreeModel(QObject* parent)
    : QAbstractItemModel{parent} { }

void MemoryMapTreeModel::setRoot(TreeNode* newRoot) { root.reset(newRoot); }

QModelIndex MemoryMapTreeModel::index(int row, int column, const QModelIndex& parent) const {
    if(!hasIndex(row, column, parent)) return {};
    TreeNode* parentItem = (!parent.isValid()) ? root.get()
                                               : static_cast<TreeNode*>(parent.internalPointer());
    TreeNode* childItem = parentItem->child(row);
    if(childItem) return createIndex(row, column, childItem);
    return {};
}

QModelIndex MemoryMapTreeModel::parent(const QModelIndex& child) const {
    if(!child.isValid()) return {};
    TreeNode* childItem = static_cast<TreeNode*>(child.internalPointer());
    TreeNode* parentItem = childItem->parentNode();
    if(parentItem == root.get()) return {};
    return createIndex(parentItem->row(), 0, parentItem);
}

int MemoryMapTreeModel::rowCount(const QModelIndex& parent) const {
    if(parent.column() > 0) return 0;
    TreeNode* parentItem = (!parent.isValid()) ? root.get()
                                               : static_cast<TreeNode*>(parent.internalPointer());
    return parentItem->childCount();
}

int MemoryMapTreeModel::columnCount(const QModelIndex& parent) const {
    if(parent.isValid()) return static_cast<TreeNode*>(parent.internalPointer())->columnCount();
    return root->columnCount();
}

QVariant MemoryMapTreeModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid()) return QVariant();
    TreeNode* item = static_cast<TreeNode*>(index.internalPointer());
    return item->data(index.column(), role);
}

Qt::ItemFlags MemoryMapTreeModel::flags(const QModelIndex& index) const {
    if(!index.isValid()) return Qt::NoItemFlags;
    return QAbstractItemModel::flags(index);
}

MemoryMapTree::MemoryMapTree(QWidget* parent)
    : QTreeView{parent} {
    memoryMapTreeModel = new MemoryMapTreeModel{this}; // , new TreeNode("No map loaded..."));
    setSelectionMode(SingleSelection);
    setModel(memoryMapTreeModel);
    setRootIsDecorated(true);
    // setRootVisible(true);
}

void MemoryMapTree::setMemoryMap(const MemoryMap& memoryMap) {
    // Update tree
    TreeNode* mapTreeRoot = new TreeNode{memoryMap};
    for(auto&& region: memoryMap.getNumberOfRegions()) {
        TreeNode* nodeRegion = mapTreeRoot->add(new TreeNode{region});
        for(auto&& subRegion: region.getNumberOfSubRegions()) {
            TreeNode* nodeSubRegion = nodeRegion->add(new TreeNode{subRegion});
            for(auto&& regionData: subRegion.getNumberOfRegionDatas()) {
                nodeSubRegion->add(new TreeNode{regionData});
            }
        }
    }

    memoryMapTreeModel->setRoot(mapTreeRoot);
    reset();
    // expandToDepth(0);
    // setRootVisible(true);
}

MemoryMapPicture::MemoryMapPicture(QWidget* parent)
    : QWidget{parent} {

    scale = 0.25;
    focus = false;
}

void MemoryMapPicture::setSelectedRegion(Region region) {
    selectedRegion = region;
    setSelectedSubRegion(region.getSubRegion(0));
    repaint();
}

void MemoryMapPicture::setSelectedSubRegion(SubRegion subRegion) {
    selectedSubRegion = subRegion;
    setSelectedRegionData(subRegion.getRegionData(0));
    repaint();
}

void MemoryMapPicture::setSelectedRegionData(RegionData regionData) {
    selectedRegionData = regionData;
    repaint();
}

void MemoryMapPicture::zoomIn() {
    scale *= zoomFactor;
    if(scale > maxZoomIn) scale = maxZoomIn;
    focus = true;
    repaint();
}

void MemoryMapPicture::zoomOut() {
    scale /= zoomFactor;
    if(scale < minZoomIn) scale = minZoomIn;
    focus = true;
    repaint();
}

void MemoryMapPicture::paintEvent(QPaintEvent* event) {
    // QCustomPlot::paintEvent(event);
    // return;
    QPainter g{this};
    // g.fillRect(rect(), Qt::lightGray);

    // Check whether there is something to draw
    // if(selectedRegion == nullptr) return;

    // Draw all subregions
    int focusY = 0;
    int maxY = 0;
    long baseAddress = selectedRegion.getData().getAddress();
    for(int i = 0; i < selectedRegion.getNumberOfSubRegionSize(); i++) {
        SubRegion subRegion = selectedRegion.getSubRegion(i);
        for(int j = 0; j < subRegion.getNumberOfRegionDataSize(); j++) {
            RegionData regionData = subRegion.getRegionData(j);

            // Draw regiondata
            int y = offsetY + (int)((regionData.getAddress() - baseAddress) * scale);
            int height = (int)(regionData.getSize() * scale);

            // Draw selected different
            if(subRegion == selectedSubRegion) {
                g.fillRect(offsetX, y, width, height, Qt::red);
                g.setPen(Qt::black);
            }

            g.drawRect(offsetX, y, width, height);

            if(regionData == selectedRegionData) {
                focusY = y;
                g.setPen(Qt::blue);
                g.drawLine(offsetX, y, offsetX + width + selectedRegionWidth, y);
                g.setPen(Qt::black);
            }

            // Determine maxY
            if((height + y) > maxY)
                maxY = height + y;
        }
    }
#if 0
    // Resize panel
        int preferredWidth = 2 * offsetX + width + selectedRegionWidth;
        int preferredHeight = offsetY + maxY;
        super.setPreferredSize(new Dimension(preferredWidth, preferredHeight));
        if(focus) {
            focus = false;
            super.scrollRectToVisible(new Rectangle(0, focusY, 1, (int)(8 * scale)));
        }
#endif
}

MemoryMapComponent::MemoryMapComponent(QWidget* parent)
    : QSplitter{Qt::Horizontal, parent} {

    // Create contents
    // splitPane = new QSplitter{Qt::Horizontal, this};
    memoryTreeBorder = new QWidget{this};
    memoryTreePanel = new QGroupBox{u"Memory Tree"_s, memoryTreeBorder};
    QBoxLayout* layout = new QVBoxLayout{memoryTreeBorder};
    layout->addWidget(memoryTreePanel);
    memoryMapSelectionBox = new QComboBox{memoryTreePanel};
    memoryMapSelectionBox->addItem(SELECTION_RAW.toString());
    memoryMapSelectionBox->addItem(SELECTION_FILTERED.toString());
    memoryMapSelectionBox->addItem(SELECTION_COMPRESSED.toString());
    memoryMapSelectionBox->setCurrentIndex(2);
    memoryMapTree = new MemoryMapTree{memoryTreePanel};
    layout = new QVBoxLayout{memoryTreePanel};
    layout->addWidget(memoryMapSelectionBox);
    layout->addWidget(memoryMapTree);
    // layout->setSpacing(0);
    layout->setStretch(1, 1);

    memoryPictureBorder = new QWidget{this};
    memoryPicturePanel = new QGroupBox{u"Memory Picture"_s, memoryPictureBorder};
    layout = new QVBoxLayout{memoryPictureBorder};
    layout->addWidget(memoryPicturePanel);
    memoryMapPicture = new MemoryMapPicture{memoryPicturePanel};
    buttonZoomIn = new QPushButton{u"+"_s, memoryPicturePanel};
    buttonZoomOut = new QPushButton{u"-"_s, memoryPicturePanel};
    auto gridLayout = new QGridLayout{memoryPicturePanel};
    gridLayout->addWidget(buttonZoomIn, 0, 0);
    gridLayout->addWidget(buttonZoomOut, 0, 1);
    gridLayout->addWidget(memoryMapPicture, 1, 0, 1, 2);
    gridLayout->setRowStretch(1, 1);

    addWidget(memoryTreeBorder);
    addWidget(memoryPictureBorder);

    // layout = new QHBoxLayout{this};

    // layout->addWidget(splitPane);

    // Add listeners
    // memoryMapTree.addTreeSelectionListener(this);
    // memoryMapSelectionBox.addItemListener(this);
    // buttonZoomIn.addActionListener(this);
    // buttonZoomOut.addActionListener(this);

    // Bind contents

    // Set preferred sizes
    // memoryTreePanel.setPreferredSize(300, 700);
    // memoryPicturePanel.setPreferredSize(300, 700);

    connect(memoryMapTree, &QAbstractItemView::clicked,
        this, &MemoryMapComponent::notifyMemoryDetailsListeners);
    connect(memoryMapSelectionBox, &QComboBox::currentIndexChanged,
        this, &MemoryMapComponent::itemStateChanged);

    connect(buttonZoomIn, &QPushButton::clicked, memoryMapPicture, &MemoryMapPicture::zoomIn);
    connect(buttonZoomOut, &QPushButton::clicked, memoryMapPicture, &MemoryMapPicture::zoomOut);
}

void MemoryMapComponent::addMemoryDetailsListener(MemoryDetailsListener* memoryDetailsListener) {
    memoryDetailsListeners.emplace_back(memoryDetailsListener);
}

void MemoryMapComponent::update(MemoryInfoFactory* memoryInfoFactory) {
    rawMemoryMap = memoryInfoFactory->getRawMemoryMap();
    filteredMemoryMap = memoryInfoFactory->getFilteredMemoryMap();
    compressedMemoryMap = memoryInfoFactory->getCompressedMemoryMap();

    updateTree();
    updatePicture();
}

void MemoryMapComponent::notifyMemoryDetailsListeners(QModelIndex object) {
    for(auto listener: memoryDetailsListeners)
        listener->showDetails(object.data(Qt::UserRole));
    valueChanged(object);
}

void MemoryMapComponent::itemStateChanged(int index) {
    updateTree();
    updatePicture();
}

void MemoryMapComponent::valueChanged(QModelIndex e) {

    // Get info of selected node
    // Object component = memoryMapTree.getLastSelectedPathComponent();
    // if(component instanceof TreeNode) {

    // Object nodes[] = ((TreeNode)component).getPath();
    TreeNode* node = static_cast<TreeNode*>(e.internalPointer());
    // for(int i = 0; node && i < node->childCount(); i++) {
    auto data = node /*->child(i)*/->data_;
    if(static const auto id = QMetaType::fromType<Region>().id(); data.userType() == id) {
        memoryMapPicture->setSelectedRegion(data.value<Region>());
    } else if(static const auto id = QMetaType::fromType<SubRegion>().id(); data.userType() == id) {
        memoryMapPicture->setSelectedSubRegion(data.value<SubRegion>());
    } else if(static const auto id = QMetaType::fromType<RegionData>().id(); data.userType() == id) {
        memoryMapPicture->setSelectedRegionData(data.value<RegionData>());
    }

    // if(i == nodes.length - 1)
    // notifyMemoryDetailsListeners(nodeInfo);
    // }
    // }
}

void MemoryMapComponent::updateTree() {
    if(memoryMapSelectionBox->currentText() == SELECTION_RAW) {
        memoryMapTree->setMemoryMap(rawMemoryMap);
    } else if(memoryMapSelectionBox->currentText() == SELECTION_FILTERED) {
        memoryMapTree->setMemoryMap(filteredMemoryMap);
    } else if(memoryMapSelectionBox->currentText() == SELECTION_COMPRESSED) {
        memoryMapTree->setMemoryMap(compressedMemoryMap);
    }
}

void MemoryMapComponent::updatePicture() {
    // Happens automatically by selecting a region
}

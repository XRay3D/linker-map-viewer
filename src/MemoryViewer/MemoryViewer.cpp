#include "MemoryViewer.h"
#include "MemoryConfigurationAndContentsComponent.h"
#include "MemoryDetailsComponent.h"
#include "MemoryInfoFactory.h"
#include "MemoryMapComponent.h"
#include <QDebug>
#include <QtWidgets>

MemoryViewer::MemoryViewer(const QString& fileNameAndPath) {

    // Bind contents
    // setJMenuBar(memoryViewerMenu);
    // JSplitPane memoryInfoPane = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT);
    // JSplitPane memoryDetailsPane = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT);
    // memoryInfoPane.setLeftComponent(memoryConfigurationAndContentsComponent);
    // memoryInfoPane.setRightComponent(memoryMapComponent);
    // memoryDetailsPane.setLeftComponent(memoryInfoPane);
    // memoryDetailsPane.setRightComponent(memoryDetailsComponent);
    // getContentPane().add(memoryDetailsPane);

    auto fileMenu = menuBar()->addMenu(u"File"_s);
    fileMenu->addAction(
        QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), u"Load"_s, QKeySequence::Open,
        this, [this] {
            QSettings settings;
            settings.beginGroup(u"MemoryViewer"_s);
            auto fileName = QFileDialog::getOpenFileName(this, u"Open Map File"_s, settings.value(u"fileNameAndPath"_s).toString(), u"map files (*.map)"_s);
            if(fileName.size()) loadMap(fileName);
        });

    // Setup frame
    setWindowTitle(u"Memory Viewer"_s);
    setCentralWidget(new QWidget{this});

    // Create contents
    splitter = new QSplitter{centralWidget()};
    splitter->setObjectName(u"splitter"_s);
    //////////////////////////////////////////
    splitter->addWidget(memoryConfigurationAndContentsComponent = new MemoryConfigurationAndContentsComponent{this});
    //////////////////////////////////////////
    splitter->addWidget(memoryMapComponent = new MemoryMapComponent{this});
    //////////////////////////////////////////
    splitter->addWidget(memoryDetailsComponent = new MemoryDetailsComponent{this});

    // Add listeners
    // memoryViewerMenu.addMemoryInfoUpdateListener(memoryMapComponent);
    // memoryViewerMenu.addMemoryInfoUpdateListener(memoryConfigurationAndContentsComponent);
    memoryConfigurationAndContentsComponent->addMemoryDetailsListener(memoryDetailsComponent);
    memoryMapComponent->addMemoryDetailsListener(memoryDetailsComponent);

    auto layout = new QVBoxLayout{centralWidget()};
    layout->addWidget(splitter);

    // Load file if specified
    loadMap(fileNameAndPath);

    QSettings settings;
    settings.beginGroup(u"MemoryViewer"_s);
    restoreState(settings.value(u"State"_s).toByteArray());
    restoreGeometry(settings.value(u"Geometry"_s).toByteArray());
    splitter->restoreState(settings.value(u"SplitterState"_s).toByteArray());
};

MemoryViewer::~MemoryViewer() {
    QSettings settings;
    settings.beginGroup(u"MemoryViewer"_s);
    settings.setValue(u"State"_s, saveState());
    settings.setValue(u"Geometry"_s, saveGeometry());
    settings.setValue(u"SplitterState"_s, splitter->saveState());
}

void MemoryViewer::loadMap(const QString& fileNameAndPath) {
    if(!fileNameAndPath.isEmpty()) {
        QFile file{fileNameAndPath};
        if(file.open(QFile::ReadOnly | QFile::Text)) {
            memoryInfoFactory.load(&file);
            QSettings settings;
            settings.beginGroup(u"MemoryViewer"_s);
            settings.setValue(u"fileNameAndPath"_s, fileNameAndPath);
            qInfo() << file.fileName() << u"loaded."_s;

            memoryConfigurationAndContentsComponent->update(&memoryInfoFactory);
            memoryMapComponent->update(&memoryInfoFactory);
        } else {
            qWarning() << file.errorString();
        }
    }
};

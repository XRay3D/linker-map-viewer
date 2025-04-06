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

    auto fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction(
        QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), "Load", QKeySequence::Open,
        this, [this] {
            QSettings settings;
            settings.beginGroup("MemoryViewer");
            auto fileName = QFileDialog::getOpenFileName(this, "Open Map File", settings.value("fileNameAndPath").toString(), "map files (*.map)");
            if(fileName.size()) loadMap(fileName);
        });

    // Setup frame
    setWindowTitle("Memory Viewer");
    setCentralWidget(new QWidget{this});

    // Create contents
    splitter = new QSplitter{centralWidget()};
    splitter->setObjectName("splitter");
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
    settings.beginGroup("MemoryViewer");
    restoreState(settings.value("State").toByteArray());
    restoreGeometry(settings.value("Geometry").toByteArray());
    splitter->restoreState(settings.value("SplitterState").toByteArray());
};

MemoryViewer::~MemoryViewer() {
    QSettings settings;
    settings.beginGroup("MemoryViewer");
    settings.setValue("State", saveState());
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("SplitterState", splitter->saveState());
}

void MemoryViewer::loadMap(const QString& fileNameAndPath) {
    if(!fileNameAndPath.isEmpty()) {
        QFile file{fileNameAndPath};
        if(file.open(QFile::ReadOnly | QFile::Text)) {
            memoryInfoFactory.load(&file);
            QSettings settings;
            settings.beginGroup("MemoryViewer");
            settings.setValue("fileNameAndPath", fileNameAndPath);
            qInfo() << file.fileName() << "loaded.";

            memoryConfigurationAndContentsComponent->update(&memoryInfoFactory);
            memoryMapComponent->update(&memoryInfoFactory);
        } else {
            qWarning() << file.errorString();
        }
    }
};

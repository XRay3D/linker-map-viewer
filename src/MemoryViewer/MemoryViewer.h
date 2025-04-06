#include "MemoryInfoFactory.h"
#include <QMainWindow>

class MemoryViewer final : public QMainWindow {

    // Attributes
    class MemoryConfigurationAndContentsComponent* memoryConfigurationAndContentsComponent;
    class MemoryMapComponent* memoryMapComponent;
    class MemoryDetailsComponent* memoryDetailsComponent;
    MemoryInfoFactory memoryInfoFactory;

    class QSplitter* splitter;
    // Constructor
public:
    MemoryViewer(const QString& fileNameAndPath = {});
    ~MemoryViewer() override;
    void loadMap(const QString& fileNameAndPath);
};

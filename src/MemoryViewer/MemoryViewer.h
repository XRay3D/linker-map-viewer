#include <QMainWindow>

class MemoryViewer : public QMainWindow {


    // Attributes
    class MemoryConfigurationAndContentsComponent* memoryConfigurationAndContentsComponent;
    class MemoryMapComponent* memoryMapComponent;
    class MemoryDetailsComponent* memoryDetailsComponent;
    class QSplitter* splitter;
    // Constructor
public:
    MemoryViewer(const QString& fileNameAndPath = {});
    ~MemoryViewer() override;
    void loadMap(const QString& fileNameAndPath);
};

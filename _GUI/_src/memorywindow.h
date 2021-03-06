#ifndef MEMORYWINDOWWIDGET_H
#define MEMORYWINDOWWIDGET_H

#include "../../_Simulator/_src/compSim.h"
#include <QWidget>

class QString;
class QLabel;
class QSpinBox;
class QGroupBox;
class QPushButton;
class QTableWidget;
class QIntValidator;
class QSpacerItem;
class QRadioButton;
class QAction;
class QSplitter;
class QLineEdit;
class QStringList;
class QTabWidget;
class MemoryMap;
class DropdownWidget;

class MemoryWindow : public QWidget {
    Q_OBJECT
public:
    explicit MemoryWindow(QWidget *parent, ComputerSimulation* simulator);

private:
    ComputerSimulation* simulator;
    memType memtyp = INSTR;
    int lastInstructionCount;
    int fromAddr = 0;
    int toAddr = 0;
    int columnCount = 1;

    /* Main Frame */
    QSplitter* mainFrame;

    // LEFT SIDE
    QTabWidget* leftTabs;
        // Memory Table
    QTableWidget* memoryTable;
    QStringList verticalTableHeader;
    QStringList horizontalTableHeader;
        // Memory Map
    QWidget* memoryMapContainer;
    MemoryMap* memoryMap;
    DropdownWidget* dropUpMenu;
    QGroupBox* configBox;
    QPushButton* toggleBtn;
    QLabel* setPixelSizeLbl;
    QLineEdit* pixelSizeIn;
    QIntValidator* pixelInVal;
    QLabel* setWidthLbl;
    QLineEdit* widthIn;
    QIntValidator* widthInVal;
    QLabel* setHeightLbl;
    QLineEdit* heightIn;
    QIntValidator* heightInVal;
    QPushButton* memoryMapUpdateBtn;
    void createLeftSide();

    // RIGHT SIDE
    QWidget* rightContainer;
        // -TOP
    QGroupBox* topRightBox;
    QLabel* topDescriptionLbl;
    QSpacerItem* rightSpacer;
    QLabel* fromAddrLbl;
    QSpinBox* fromAddrSpnbox;
    QLabel* toAddrLbl;
    QSpinBox* toAddrSpnbox;
        // -MIDDLE
    QGroupBox* midRightBox;
    QLabel* midDescriptionLbl;
    QRadioButton* imBtn;
    QRadioButton* dmBtn;
        // -BOTTOM
    QGroupBox* btmRightBox;
    QLineEdit* nameInput;
    QLabel* setWindowNameLbl;
    QPushButton* updateBtn;
    QPushButton* clearBtn;
    QWidget* windowOptionsContainer;
    QLabel* deleteWindowLbl;
    QLabel* newWindowLbl;
    void createRightSide();

    void createMainFrame();

signals:
    void windowNameChanged(QString /* Name */);
    void deleteRequested(MemoryWindow* /* This Window */);
    void newWindowRequested();

public slots:
    void updateDisplays();
    void clearDisplays();

private slots:
    void clearTableHeaders();
    void updateConfig();
    void updateMemoryMap(); 
};

#endif // MEMORYWINDOWWIDGET_H

#include "runwidget.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPixmap>
#include <QFileDialog>


runWidget::runWidget(QWidget *parent) : QWidget(parent)
{
    // Create instance of ComputerSimulation
    simulator = new ComputerSimulation(simName);

    // Program label and choose file
    program_lbl = new QLabel("Choose file to read instructions from: ");
    program_lbl->setStyleSheet("font-weight: bold;");
    open_btn = new QPushButton("Open file");
    programBox = new QGroupBox;
    QHBoxLayout* programLayout = new QHBoxLayout;
    programLayout->addWidget(program_lbl);
    programLayout->addWidget(open_btn);
    programBox->setLayout(programLayout);

    // Create dropdown menu and group with label and add to
    dropdownMenuBox = new QGroupBox;
    select_mode_lbl = new QLabel("Select simulation mode");
    dropdownMenu = new QComboBox;
    select_mode_lbl->setStyleSheet("font-weight: bold;");
    QVBoxLayout* dropdownMenuLayout = new QVBoxLayout;
    dropdownMenu->addItem("Run");
    dropdownMenu->addItem("Singel step");
    dropdownMenuLayout->addWidget(select_mode_lbl, 0, Qt::AlignCenter);
    dropdownMenuLayout->addWidget(dropdownMenu);
    dropdownMenuBox->setLayout(dropdownMenuLayout);

    // Create and set text of description
    description_lbl = new QLabel;
    QString description_txt = "Simulator: AdHoc16_V03\n";
    description_txt.append("(Pressing \"End\" will stop the simulation and reset PC)");
    description_lbl->setText(description_txt);
    description_lbl->setStyleSheet("font-weight: bold;");

    // Create list with statistics
    stats_lst = new QListWidget;

    // Create icon as QPixmap in a QLabel
    icon_img = new QLabel;
    QPixmap icon(":/images/../_img/SimComp_icon.png");
    icon_img->setPixmap(icon);

    // Create table
    table = new QTableWidget;
    tableHeader << "Instruction count" << "OpCode" << "PC";
    for (int i = 0; i < simulator->cpu.noOfRegisters; i++)
        tableHeader << QString("R") + QString::number(i);
    tableHeader << "next PC";
    table->setColumnCount(12);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setHorizontalHeaderLabels(tableHeader);
    table->verticalHeader()->setVisible(false);

    // Create and group buttons
    buttonBox = new QGroupBox;
    start_btn = new QPushButton("Run");
    end_btn = new QPushButton("End");
    QVBoxLayout* buttonsLayout = new QVBoxLayout;
    buttonsLayout->addWidget(start_btn);
    buttonsLayout->addWidget(end_btn);
    buttonBox->setLayout(buttonsLayout);


    // Create sidepanel
    QVBoxLayout* sidePanelLayout = new QVBoxLayout;
    sidePanelLayout->addWidget(icon_img, 0, Qt::AlignCenter); // TOP
    sidePanelLayout->addWidget(stats_lst, 0, Qt::AlignCenter); // MIDDLE
    sidePanelLayout->addWidget(description_lbl, 0, Qt::AlignCenter); // MIDDLE
    sidePanelLayout->addWidget(buttonBox, 0, Qt::AlignBottom); // BOTTOM

    // Make main layout
    QGridLayout* mainLayout = new QGridLayout;
    mainLayout->addWidget(programBox, 1, 1);
    mainLayout->addWidget(dropdownMenuBox, 1, 2);
    mainLayout->addWidget(table, 2, 1);
    mainLayout->addLayout(sidePanelLayout, 2, 2);

    // Set main layout of runWidget
    setLayout(mainLayout);

    // Make connections
    connect(start_btn, SIGNAL(clicked()), this, SLOT(startSim())); // start_btn -> startSim()
    connect(end_btn, SIGNAL(clicked()), this, SLOT(endSim())); // end_btn -> endSim()
    connect(dropdownMenu, SIGNAL(currentIndexChanged(int)), this, SLOT(setButtonText(int))); // Change in dropdown menu -> change start_btm
    connect(open_btn, SIGNAL(clicked()), this, SLOT(openFile())); // open_btn -> openFile()

    // Set minimum size of this window
    this->setMinimumSize(QSize(MIN_WIDTH, MIN_HEIGHT));
}

void runWidget::startSim() {
    /* startSim() starts the simulation with the selected mode from the dropdown menu */

    // Load .sasm file if not null
    if (filename.isNull()) return;
    simulator->load(filename.toStdString());

    clock_t start;
    // Decide which mode to start simulator in
    switch (dropdownMenu->currentIndex()) {
    case 0: // Run program
        start = readTime();
        simulator->setRunning(true);
        simulator->runProgram(); // Run program
        appendStats(start); // Append stats
        break;
    case 1: // Singel step program
        stats_lst->clear();
        start = readTime();
        simulator->setSingleStepModeGUI(true);
        simulator->setRunning(true);
        nextStep();
        break;
    }
}

double runWidget::getMIPS(clock_t ticks) {
    double seconds = static_cast<double>(ticks) / CLOCKS_PER_SEC; // Calculate seconds
    return (static_cast<double>(simulator->instructionsSimulated) / 1000000.0) / seconds; // Calculate MIPS
}

void runWidget::setButtonText(int currentIndex) {
    switch (currentIndex) {
    case 0:
        start_btn->setText("Run");
        break;
    case 1:
        start_btn->setText("Step");
        break;
    }
}

void runWidget::nextStep() {
    /* Moves to next step */
    word instr = simulator->IM.read(simulator->cpu.PC); // Get instruction
    word thisPC = simulator->cpu.PC; // Save current PC-value
    word opcode = simulator->cpu.getOpCode(instr);

    if (opcode == HLT) { // End if next instruction is HLT
        simulator->setRunning(false);
    }

    if (simulator->isRunning()) {
        simulator->cpu.doInstruction(simulator->cpu.getOpCode(instr), instr, simulator->DM, simulator->IM);
        simulator->instructionsSimulated++;
        std::stringstream stepInfo;

        // Add row for current step (TODO: make better!)
        table->insertRow(table->rowCount());

        stepInfo << simulator->instructionsSimulated;
        table->setItem(table->rowCount()-1, 0, new QTableWidgetItem(QString::fromStdString(stepInfo.str())));
        stepInfo.str(std::string());

        stepInfo << simulator->cpu.getInstr(instr);
        table->setItem(table->rowCount()-1, 1, new QTableWidgetItem(QString::fromStdString(stepInfo.str())));
        stepInfo.str(std::string());

        stepInfo << thisPC;
        table->setItem(table->rowCount()-1, 2, new QTableWidgetItem(QString::fromStdString(stepInfo.str())));
        stepInfo.str(std::string());
        for (int i = 0; i < simulator->cpu.noOfRegisters; i++) {
            stepInfo << simulator->cpu.getRegisterNo(i);
            table->setItem(table->rowCount()-1, i + 3, new QTableWidgetItem(QString::fromStdString(stepInfo.str())));
            stepInfo.str(std::string());
        }
        stepInfo << simulator->cpu.PC;
        table->setItem(table->rowCount()-1, 11, new QTableWidgetItem(QString::fromStdString(stepInfo.str())));

        // Set color of registers that changed
        showChange(table,table->rowCount()-1, 3, simulator->cpu.noOfRegisters);
    }

}

void runWidget::endSim() {
    simulator->setRunning(false);
    simulator->resetStatistics(simulator->cpu);
    simulator->cpu.PC = 0;
}

void runWidget::openFile() {
    // Starts a file dialog which lets the user choose a assembler program to run
    filename = QFileDialog::getOpenFileName(this, tr("Open file"), "/",tr("Assembler program (*.sasm)"));
    program_lbl->setText("Program: " + filename);
}

void runWidget::appendStats(clock_t start) {
    /* Appends statistics from current simulation */

    stats_lst->clear(); // Clear all current stats

    // Create vector with stats
    std::vector<std::string> stats_vec;
    std::stringstream  ss;
    ss << getMIPS(readTime()-start);
    stats_vec.push_back("MIPS: " + ss.str());
    ss.str(std::string());
    simulator->IM.getStats(simulator->cpu, stats_vec);
    simulator->DM.getStats(simulator->cpu, stats_vec);
    simulator->resetStatistics(simulator->cpu);

    // Add stats to stats_lst
    for (auto &item : stats_vec) {
        stats_lst->addItem(QString::fromStdString(item));
    }
}

void showChange(QTableWidget* table, int rowIndex, int from, int to) {
    /* Changes color of cell that changed since previous row */
    if (table->rowCount() < 2) return;
    QTableWidgetItem* cell;
    for (int i = from; i < to; i++) {
        cell = table->item(rowIndex, i);
        if (!(table->item(rowIndex-1, i)->text() == cell->text()) && !(cell->foreground().color() == QColor("red")))
            cell->setForeground(QBrush(QColor("red")));
        else
            cell->setForeground(QBrush(QColor("black")));
    }
}
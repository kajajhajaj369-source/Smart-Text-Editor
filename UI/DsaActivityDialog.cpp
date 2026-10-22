#include "DsaActivityDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>

DsaActivityDialog::DsaActivityDialog(QWidget *parent)
    : QDialog(parent) {
    setWindowTitle("DSA Activity Log — Real-Time Operation Audit");
    resize(720, 420);
    setupUI();
    refreshLog();
}

DsaActivityDialog::~DsaActivityDialog() {}

void DsaActivityDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    countLabel = new QLabel("Total Recorded Operations: 0", this);
    countLabel->setStyleSheet("font-weight: bold; color: #495057;");
    mainLayout->addWidget(countLabel);

    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(6);
    tableWidget->setHorizontalHeaderLabels({"Time", "Action", "Data Structure", "Operation", "Complexity", "Principle / Reason"});
    tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    tableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    tableWidget->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setAlternatingRowColors(true);

    mainLayout->addWidget(tableWidget);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    clearBtn = new QPushButton("&Clear Activity Log", this);
    closeBtn = new QPushButton("&Close", this);
    closeBtn->setDefault(true);

    btnLayout->addWidget(clearBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(closeBtn);

    mainLayout->addLayout(btnLayout);

    connect(clearBtn, &QPushButton::clicked, this, &DsaActivityDialog::onClearClicked);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
}

void DsaActivityDialog::refreshLog() {
    const auto& history = DsaLogger::instance().getHistory();
    tableWidget->setRowCount(0);

    for (const auto& evt : history) {
        appendEvent(evt);
    }
    countLabel->setText(QString("Total Recorded Operations: %1").arg(history.size()));
}

void DsaActivityDialog::appendEvent(const DsaEvent& evt) {
    int row = tableWidget->rowCount();
    tableWidget->insertRow(row);

    QTableWidgetItem* timeItem = new QTableWidgetItem(QString::fromStdString(evt.timestamp));
    QTableWidgetItem* actionItem = new QTableWidgetItem(QString::fromStdString(evt.actionName));
    QTableWidgetItem* dsItem = new QTableWidgetItem(QString::fromStdString(evt.dataStructure));
    QTableWidgetItem* opItem = new QTableWidgetItem(QString::fromStdString(evt.operation));
    QTableWidgetItem* compItem = new QTableWidgetItem(QString::fromStdString(evt.complexity));
    QTableWidgetItem* prinItem = new QTableWidgetItem(QString::fromStdString(evt.principle));

    timeItem->setTextAlignment(Qt::AlignCenter);
    opItem->setTextAlignment(Qt::AlignCenter);
    compItem->setTextAlignment(Qt::AlignCenter);

    tableWidget->setItem(row, 0, timeItem);
    tableWidget->setItem(row, 1, actionItem);
    tableWidget->setItem(row, 2, dsItem);
    tableWidget->setItem(row, 3, opItem);
    tableWidget->setItem(row, 4, compItem);
    tableWidget->setItem(row, 5, prinItem);

    tableWidget->scrollToBottom();
    countLabel->setText(QString("Total Recorded Operations: %1").arg(tableWidget->rowCount()));
}

void DsaActivityDialog::onClearClicked() {
    DsaLogger::instance().clear();
    tableWidget->setRowCount(0);
    countLabel->setText("Total Recorded Operations: 0");
}

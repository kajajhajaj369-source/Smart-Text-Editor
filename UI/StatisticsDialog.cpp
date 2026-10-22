#include "StatisticsDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QGroupBox>

StatisticsDialog::StatisticsDialog(const StatisticsManager::DocumentStats& stats, QWidget *parent)
    : QDialog(parent) {
    setWindowTitle("Document Statistics");
    resize(480, 500);
    setupUI(stats);
}

StatisticsDialog::~StatisticsDialog() {}

void StatisticsDialog::setupUI(const StatisticsManager::DocumentStats& stats) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);

    QGroupBox* metricsGroup = new QGroupBox("General Metrics (Computed via Recursion & Linked List)", this);
    QGridLayout* metricsGrid = new QGridLayout(metricsGroup);

    metricsGrid->addWidget(new QLabel("Words:", this), 0, 0);
    QLabel* wordLabel = new QLabel(QString::number(stats.wordCount), this);
    wordLabel->setStyleSheet("font-weight: bold;");
    metricsGrid->addWidget(wordLabel, 0, 1);

    metricsGrid->addWidget(new QLabel("Characters (with spaces):", this), 1, 0);
    QLabel* charWithSpaces = new QLabel(QString::number(stats.charCountWithSpaces), this);
    charWithSpaces->setStyleSheet("font-weight: bold;");
    metricsGrid->addWidget(charWithSpaces, 1, 1);

    metricsGrid->addWidget(new QLabel("Characters (no spaces):", this), 2, 0);
    QLabel* charNoSpaces = new QLabel(QString::number(stats.charCountWithoutSpaces), this);
    charNoSpaces->setStyleSheet("font-weight: bold;");
    metricsGrid->addWidget(charNoSpaces, 2, 1);

    metricsGrid->addWidget(new QLabel("Lines:", this), 3, 0);
    QLabel* lineLabel = new QLabel(QString::number(stats.lineCount), this);
    lineLabel->setStyleSheet("font-weight: bold;");
    metricsGrid->addWidget(lineLabel, 3, 1);

    metricsGrid->addWidget(new QLabel("Paragraphs:", this), 4, 0);
    QLabel* paraLabel = new QLabel(QString::number(stats.paragraphCount), this);
    paraLabel->setStyleSheet("font-weight: bold;");
    metricsGrid->addWidget(paraLabel, 4, 1);

    metricsGrid->addWidget(new QLabel("Average Word Length:", this), 5, 0);
    QLabel* avgWordLabel = new QLabel(QString::number(stats.averageWordLength, 'f', 1) + " chars", this);
    avgWordLabel->setStyleSheet("font-weight: bold;");
    metricsGrid->addWidget(avgWordLabel, 5, 1);

    mainLayout->addWidget(metricsGroup);

    QGroupBox* freqGroup = new QGroupBox("Most Frequent Words (Ranked via Custom Hash Table & Merge Sort)", this);
    QVBoxLayout* freqLayout = new QVBoxLayout(freqGroup);

    QTableWidget* tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(3);
    tableWidget->setHorizontalHeaderLabels({"Rank", "Word", "Frequency"});
    tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    tableWidget->setRowCount(static_cast<int>(stats.topWords.size()));
    for (size_t i = 0; i < stats.topWords.size(); ++i) {
        QTableWidgetItem* rankItem = new QTableWidgetItem(QString("#%1").arg(i + 1));
        QTableWidgetItem* wordItem = new QTableWidgetItem(QString::fromStdString(stats.topWords[i].first));
        QTableWidgetItem* countItem = new QTableWidgetItem(QString::number(stats.topWords[i].second));

        rankItem->setTextAlignment(Qt::AlignCenter);
        countItem->setTextAlignment(Qt::AlignCenter);

        tableWidget->setItem(static_cast<int>(i), 0, rankItem);
        tableWidget->setItem(static_cast<int>(i), 1, wordItem);
        tableWidget->setItem(static_cast<int>(i), 2, countItem);
    }

    freqLayout->addWidget(tableWidget);
    mainLayout->addWidget(freqGroup);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    QPushButton* closeBtn = new QPushButton("&Close", this);
    closeBtn->setDefault(true);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    btnLayout->addWidget(closeBtn);

    mainLayout->addLayout(btnLayout);
}

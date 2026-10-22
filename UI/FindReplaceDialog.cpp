#include "FindReplaceDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>

FindReplaceDialog::FindReplaceDialog(QWidget *parent)
    : QDialog(parent) {
    setWindowTitle("Find and Replace");
    setModal(false);
    resize(420, 220);
    setupUI();
}

FindReplaceDialog::~FindReplaceDialog() {}

void FindReplaceDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    QGridLayout* inputGrid = new QGridLayout();
    inputGrid->setSpacing(8);

    QLabel* findLabel = new QLabel("Find:", this);
    findLineEdit = new QLineEdit(this);
    findLineEdit->setPlaceholderText("Enter text to find...");

    QLabel* replaceLabel = new QLabel("Replace:", this);
    replaceLineEdit = new QLineEdit(this);
    replaceLineEdit->setPlaceholderText("Enter replacement text...");

    inputGrid->addWidget(findLabel, 0, 0);
    inputGrid->addWidget(findLineEdit, 0, 1);
    inputGrid->addWidget(replaceLabel, 1, 0);
    inputGrid->addWidget(replaceLineEdit, 1, 1);

    mainLayout->addLayout(inputGrid);

    matchCaseCheckBox = new QCheckBox("Match &Case", this);
    mainLayout->addWidget(matchCaseCheckBox);

    statusLabel = new QLabel("Ready", this);
    statusLabel->setStyleSheet("color: #666; font-style: italic;");
    mainLayout->addWidget(statusLabel);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(6);

    findNextBtn = new QPushButton("&Find Next", this);
    findPrevBtn = new QPushButton("Find &Prev", this);
    replaceBtn = new QPushButton("&Replace", this);
    replaceAllBtn = new QPushButton("Replace &All", this);
    closeBtn = new QPushButton("&Close", this);

    findNextBtn->setDefault(true);

    btnLayout->addWidget(findNextBtn);
    btnLayout->addWidget(findPrevBtn);
    btnLayout->addWidget(replaceBtn);
    btnLayout->addWidget(replaceAllBtn);
    btnLayout->addWidget(closeBtn);

    mainLayout->addLayout(btnLayout);

    connect(findNextBtn, &QPushButton::clicked, this, &FindReplaceDialog::onFindNextClicked);
    connect(findPrevBtn, &QPushButton::clicked, this, &FindReplaceDialog::onFindPrevClicked);
    connect(replaceBtn, &QPushButton::clicked, this, &FindReplaceDialog::onReplaceClicked);
    connect(replaceAllBtn, &QPushButton::clicked, this, &FindReplaceDialog::onReplaceAllClicked);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::hide);
    connect(findLineEdit, &QLineEdit::returnPressed, this, &FindReplaceDialog::onFindNextClicked);
}

void FindReplaceDialog::setFindText(const QString& text) {
    findLineEdit->setText(text);
    findLineEdit->selectAll();
    findLineEdit->setFocus();
}

void FindReplaceDialog::updateStatus(int currentMatchIndex, int totalMatches) {
    if (totalMatches <= 0) {
        statusLabel->setText("No matches found.");
        statusLabel->setStyleSheet("color: #d9534f; font-weight: bold;");
    } else {
        statusLabel->setText(QString("Match %1 of %2").arg(currentMatchIndex).arg(totalMatches));
        statusLabel->setStyleSheet("color: #0275d8; font-weight: bold;");
    }
}

void FindReplaceDialog::onFindNextClicked() {
    QString query = findLineEdit->text();
    if (!query.isEmpty()) {
        emit findNextRequested(query, matchCaseCheckBox->isChecked());
    }
}

void FindReplaceDialog::onFindPrevClicked() {
    QString query = findLineEdit->text();
    if (!query.isEmpty()) {
        emit findPreviousRequested(query, matchCaseCheckBox->isChecked());
    }
}

void FindReplaceDialog::onReplaceClicked() {
    QString query = findLineEdit->text();
    QString replacement = replaceLineEdit->text();
    if (!query.isEmpty()) {
        emit replaceRequested(query, replacement, matchCaseCheckBox->isChecked());
    }
}

void FindReplaceDialog::onReplaceAllClicked() {
    QString query = findLineEdit->text();
    QString replacement = replaceLineEdit->text();
    if (!query.isEmpty()) {
        emit replaceAllRequested(query, replacement, matchCaseCheckBox->isChecked());
    }
}

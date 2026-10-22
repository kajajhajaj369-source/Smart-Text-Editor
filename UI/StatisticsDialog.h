#ifndef STATISTICS_DIALOG_H
#define STATISTICS_DIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QLabel>
#include "../Services/StatisticsManager.h"

/**
 * @brief Dialog presenting document metrics, word frequency table, and DSA insights.
 */
class StatisticsDialog : public QDialog {
    Q_OBJECT

public:
    explicit StatisticsDialog(const StatisticsManager::DocumentStats& stats, QWidget *parent = nullptr);
    ~StatisticsDialog();

private:
    void setupUI(const StatisticsManager::DocumentStats& stats);
};

#endif // STATISTICS_DIALOG_H

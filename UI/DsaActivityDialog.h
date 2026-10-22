#ifndef DSA_ACTIVITY_DIALOG_H
#define DSA_ACTIVITY_DIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include "../Services/DsaLogger.h"

/**
 * @brief Dialog displaying a real-time chronological audit trail of all DSA operations.
 */
class DsaActivityDialog : public QDialog {
    Q_OBJECT

public:
    explicit DsaActivityDialog(QWidget *parent = nullptr);
    ~DsaActivityDialog();

public slots:
    void refreshLog();
    void appendEvent(const DsaEvent& evt);

private slots:
    void onClearClicked();

private:
    QTableWidget* tableWidget;
    QPushButton* clearBtn;
    QPushButton* closeBtn;
    QLabel* countLabel;

    void setupUI();
};

#endif // DSA_ACTIVITY_DIALOG_H

#ifndef FIND_REPLACE_DIALOG_H
#define FIND_REPLACE_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QString>

class MainWindow;

/**
 * @brief Search & Replace dialog for Smart Text Editor.
 */
class FindReplaceDialog : public QDialog {
    Q_OBJECT

public:
    explicit FindReplaceDialog(QWidget *parent = nullptr);
    ~FindReplaceDialog();

    void setFindText(const QString& text);
    void updateStatus(int currentMatchIndex, int totalMatches);

signals:
    void findNextRequested(const QString& query, bool matchCase);
    void findPreviousRequested(const QString& query, bool matchCase);
    void replaceRequested(const QString& findQuery, const QString& replaceWith, bool matchCase);
    void replaceAllRequested(const QString& findQuery, const QString& replaceWith, bool matchCase);

private slots:
    void onFindNextClicked();
    void onFindPrevClicked();
    void onReplaceClicked();
    void onReplaceAllClicked();

private:
    QLineEdit* findLineEdit;
    QLineEdit* replaceLineEdit;
    QCheckBox* matchCaseCheckBox;
    QLabel* statusLabel;

    QPushButton* findNextBtn;
    QPushButton* findPrevBtn;
    QPushButton* replaceBtn;
    QPushButton* replaceAllBtn;
    QPushButton* closeBtn;

    void setupUI();
};

#endif // FIND_REPLACE_DIALOG_H

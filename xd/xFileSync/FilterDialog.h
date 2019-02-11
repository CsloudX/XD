#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>
#include <QPlainTextEdit>

namespace Ui {
class FilterDialog;
}

class FilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilterDialog(QWidget *parent = 0);
    ~FilterDialog();

    void setIncludeFilter(const QStringList& filter);
    void setExcludeFilter(const QStringList& filter);

    QStringList includeFilter()const;
    QStringList excludeFitler()const;

private:
    void setFilter(QPlainTextEdit* edit,const QStringList& filter);
    QStringList filter(QPlainTextEdit* edit)const;

private:
    Ui::FilterDialog *ui;
};

#endif // FILTERDIALOG_H

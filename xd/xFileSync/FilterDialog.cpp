#include "FilterDialog.h"
#include "ui_FilterDialog.h"

FilterDialog::FilterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilterDialog)
{
    ui->setupUi(this);
}

FilterDialog::~FilterDialog()
{
    delete ui;
}

void FilterDialog::setIncludeFilter(const QStringList &filter)
{
    setFilter(ui->includeEdit,filter);
}

void FilterDialog::setExcludeFilter(const QStringList &filter)
{
    setFilter(ui->excludeFilter,filter);
}

QStringList FilterDialog::includeFilter() const
{
    return filter(ui->includeEdit);
}

QStringList FilterDialog::excludeFitler() const
{
    return filter(ui->excludeFilter);
}

void FilterDialog::setFilter(QPlainTextEdit *edit, const QStringList &filter)
{
    edit->clear();
    for(auto& var:filter)
        edit->appendPlainText(var);
}

QStringList FilterDialog::filter(QPlainTextEdit *edit) const
{
    QStringList ret;
    QStringList strs = edit->toPlainText().split("\n");
    for(auto& var:strs)
        ret.append(var.trimmed());
    return ret;
}

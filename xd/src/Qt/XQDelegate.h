#ifndef _X_Q_DELEGATE_H
#define _X_Q_DELEGATE_H

#include <QDebug>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDateTimeEdit>
#include <QColorDialog>
#include <QApplication>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include "XQt.h"

class XQDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    XQDelegate(QObject* parent):QStyledItemDelegate(parent)
    ,m_editable(true){}
    virtual ~XQDelegate(){}

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);
        paintEvent(painter,&opt,index);

        const QWidget *widget = opt.widget;
        QStyle *style = widget ? widget->style() : QApplication::style();
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
        endPaintEvent(painter,&opt,index);
    }
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        if(m_editable)
            return createEditorEvent(parent,option,index);
        else
            return nullptr;
    }

    void setEditable(bool editable){m_editable=editable;}
    bool isEditable()const{return m_editable;}

protected:
    virtual void paintEvent(QPainter*, QStyleOptionViewItem*, const QModelIndex&)const{}
    virtual void endPaintEvent(QPainter*, QStyleOptionViewItem*, const QModelIndex&)const{}
    virtual QWidget *createEditorEvent(QWidget *, const QStyleOptionViewItem&, const QModelIndex&) const{return nullptr;}

private:
    bool m_editable;
};

class XQLineEditDelegate : public XQDelegate
{
    Q_OBJECT
public:
    XQLineEditDelegate(QObject* parent=nullptr):XQDelegate(parent)
    ,m_alignment(Qt::AlignLeft|Qt::AlignVCenter){}

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
    {
        model->setData(index,qobject_cast<QLineEdit*>(editor)->text());
    }

protected:
    virtual void paintEvent(QPainter* , QStyleOptionViewItem* opt, const QModelIndex& )const
    {
        opt->displayAlignment=m_alignment;
    }

    virtual QWidget *createEditorEvent(QWidget *parent, const QStyleOptionViewItem& , const QModelIndex& index) const
    {
        QLineEdit* w = new QLineEdit(parent);
        w->setAlignment(m_alignment);
        w->setText(index.data().toString());
        return w;
    }
private:
    Qt::Alignment m_alignment;
};

class XQComboBoxDelegate : public XQDelegate
{
    Q_OBJECT
public:
    XQComboBoxDelegate(QObject* parent=nullptr):XQDelegate(parent),m_editorEditable(false){}

    void addItem(const QString &text, const QVariant &userData = QVariant())
    {addItem(QIcon(),text,userData);}
    void addItem(const QIcon &icon, const QString &text, const QVariant &userData = QVariant())
    {
        Item item;
        item.icon=icon;
        item.text=text;
        item.data=userData;
        m_items.append(item);
    }
    void addItems(const QStringList &texts)
    {
        for(auto& text :texts)
            addItem(text);
    }
    int count()const{return m_items.count();}

    void setEditorEditable(bool editable){m_editorEditable=editable;}
    bool isEditorEditable()const{return m_editorEditable;}

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
    {
        QComboBox* combo = qobject_cast<QComboBox*>(editor);
        auto data = combo->currentData();
        if(!data.isValid())
            data = combo->currentText();
        model->setData(index,data);
    }

protected:
    virtual void paintEvent(QPainter* , QStyleOptionViewItem* opt, const QModelIndex& index)const
    {
        opt->text = dataString(index.data());
    }

    virtual QWidget *createEditorEvent(QWidget *parent, const QStyleOptionViewItem&, const QModelIndex& index) const
    {
        QComboBox* w = new QComboBox(parent);
        w->setEditable(m_editorEditable);
        for(auto& item: m_items)
        {
            w->addItem(item.icon,item.text,item.data);
        }
        w->setCurrentText(dataString(index.data()));
        return w;
    }
    QString dataString(const QVariant& data)const
    {
        for(auto& item:m_items)
        {
            if(QVariant(item.data)==data)
                return item.text;
        }
        return data.toString();
    }
private:
    typedef struct{
        QIcon icon;
        QString text;
        QVariant data;
    }Item;
    QList<Item> m_items;
    bool m_editorEditable;
};

class XQCheckBoxDelegate : public XQDelegate
{
    Q_OBJECT
public:
    XQCheckBoxDelegate(const QString& text, QObject* parent=nullptr)
        :XQDelegate(parent),m_text(text){}
    void setText(const QString& text){m_text=text;}

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
    {
        QCheckBox* w = qobject_cast<QCheckBox*>(editor);
        model->setData(index,w->isChecked());
    }

protected:
    virtual void paintEvent(QPainter* , QStyleOptionViewItem* opt, const QModelIndex& index)const
    {
        opt->text = m_text;
        opt->checkState=index.data().toBool()?Qt::Checked:Qt::Unchecked;
    }

    virtual QWidget *createEditorEvent(QWidget *parent, const QStyleOptionViewItem&, const QModelIndex& index) const
    {
        QCheckBox* w = new QCheckBox(m_text,parent);
        w->setChecked(index.data().toBool());
        return w;
    }
private:
    QString m_text;
};

class XQSpinBoxDelegate : public XQDelegate
{
    Q_OBJECT
public:
    XQSpinBoxDelegate(int minVal, int maxVal, QObject* parent=nullptr)
        :XQDelegate(parent),m_min(minVal),m_max(maxVal){}

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
    {
        model->setData(index,qobject_cast<QSpinBox*>(editor)->value());
    }

protected:
    virtual QWidget *createEditorEvent(QWidget *parent, const QStyleOptionViewItem&, const QModelIndex& index) const
    {
        QSpinBox* w = new QSpinBox(parent);
        w->setRange(m_min,m_max);
        w->setValue(index.data().toInt());
        return w;
    }
private:
    int m_min;
    int m_max;
};
class XQDoubleSpinBoxDelegate : public XQDelegate
{
    Q_OBJECT
public:
    XQDoubleSpinBoxDelegate(int minVal, int maxVal, QObject* parent=nullptr)
        :XQDelegate(parent),m_min(minVal),m_max(maxVal){}

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
    {
        model->setData(index,qobject_cast<QSpinBox*>(editor)->value());
    }

protected:
    virtual QWidget *createEditorEvent(QWidget *parent, const QStyleOptionViewItem&, const QModelIndex& index) const
    {
        QDoubleSpinBox* w = new QDoubleSpinBox(parent);
        w->setRange(m_min,m_max);
        w->setValue(index.data().toDouble());
        return w;
    }
private:
    double m_min;
    double m_max;
};

class XQTextEditDelegate : public XQDelegate
{
    Q_OBJECT
public:
    XQTextEditDelegate(QObject* parent=nullptr):XQDelegate(parent){}
    virtual QSize  sizeHint(const QStyleOptionViewItem &, const QModelIndex &index) const
    {
        QTextDocument doc;
        doc.setHtml(index.data().toString());
        return doc.size().toSize();
    }


protected:
    virtual void paintEvent(QPainter* , QStyleOptionViewItem* opt, const QModelIndex& )const
    { opt->text.clear(); }
    virtual void endPaintEvent(QPainter* painter, QStyleOptionViewItem* opt, const QModelIndex& index)const
    {
        painter->save();
        painter->translate(opt->rect.topLeft());
        QTextDocument doc;
        doc.setHtml(index.data().toString());
        doc.documentLayout()->draw(painter,QAbstractTextDocumentLayout::PaintContext());
        painter->restore();
    }
    virtual QWidget *createEditorEvent(QWidget *, const QStyleOptionViewItem&, const QModelIndex&) const{return nullptr;}

};


class XQDateTimeDelegate : public XQDelegate
{
    Q_OBJECT
public:
    XQDateTimeDelegate(QObject* parent=nullptr)
        :XQDelegate(parent){}

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
    {
        model->setData(index,qobject_cast<QDateTimeEdit*>(editor)->dateTime());
    }

protected:
    virtual void paintEvent(QPainter* , QStyleOptionViewItem* opt, const QModelIndex& index)const
    {
        opt->text  = index.data().toDateTime().toString();
    }
    virtual QWidget *createEditorEvent(QWidget *parent, const QStyleOptionViewItem&, const QModelIndex& index) const
    {
        QDateTimeEdit* w = new QDateTimeEdit(parent);
        w->setDateTime(index.data().toDateTime());
        return w;
    }
private:
};

class XQColorDelegate : public XQDelegate
{
    Q_OBJECT
public:
    XQColorDelegate(QObject* parent=nullptr)
        :XQDelegate(parent){}

protected:
    virtual void paintEvent(QPainter* , QStyleOptionViewItem* opt, const QModelIndex& index)const
    {
        opt->text.clear();
        opt->backgroundBrush=QBrush (QtHelper::toColor(index.data().toString()));
    }

protected:
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &, const QModelIndex &index)
    {
        if (isEditable() && event->type() == QEvent::MouseButtonPress)
        {
            QColor color = QColorDialog::getColor(QtHelper::toColor(index.data().toString()));
            if (color.isValid())
            {
                model->setData(index, QtHelper::toString(color));
            }
            return true;
        }
        return false;
    }

private:
};

#endif //_X_Q_DELEGATE_H

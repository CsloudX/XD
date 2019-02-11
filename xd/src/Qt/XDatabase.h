#ifndef XDATABASE_H
#define XDATABASE_H

/**************************** Example ************************************

#define STUDENT_FLDS_LIST(_) \
	_(ID			,QVariant::Int) \
	_(Name			,QVariant::String) \
	_(Age			,QVariant::Int) \
	_(Score			,QVariant::Double)
X_DEF_DATABASE_TABLE(Student, STUDENT_FLDS_LIST)

class Student :public XDatabaseItem
{
	public:
	int id;
	QString name;
	int age;
	double score;

protected:
	virtual void bindValuesEvent()const
	{
		auto tbl = staticTable();
		bindValue(tbl->fldID(), id);
		bindValue(tbl->fldName(), name);
		bindValue(tbl->fldAge(), age);
		bindValue(tbl->fldScore(), score);
	}
	virtual void unbindValuesEvent()
	{
		auto tbl = staticTable();
		id = unbindValue(tbl->fldID()).toInt();
		name = unbindValue(tbl->fldName()).toString();
		age = unbindValue(tbl->fldAge()).toInt();
		score = unbindValue(tbl->fldScore()).toDouble();
	}

	public:
		virtual const XDatabaseTable* table()const { return staticTable(); }
		static TblStudent* staticTable(){ static TblStudent tbl; return &tbl; }
};
X_DEF_DATABASE_ITEM_CONTAINER(Student)

class StudentQueryCondition :public XSqlWhereCondition
{
	public:
	StudentQueryCondition()
	{
		TblStudent tbl;
		addChild(&id.adjustKey(tbl.fldID()));
		addChild(&name.adjustKey(tbl.fldName()));
	}

    XSqlWhereConditionItem<int>	id;
    XSqlWhereConditionItem<QString> name;
};

******************************** Example End ************************************/

#include <QtCore/qglobal.h>
#include <QVariant>
#include <QDate>
#include <QDateTime>
#include <QList>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlError>
#include <QTableView>
#include <QCoreApplication>
#include <QMutex>
#include <QThread>
#include <QDebug>
#include <XStaticHashMap.hpp>

#define _X_DEF_DATABASE_FLD_ENUM(key,val) key,
#define _X_DEF_DATABASE_FLD1(name,type)	appendField(XDatabaseTableField(#name,type));
#define _X_DEF_DATABASE_FLD2(name,type)	XDatabaseTableField& fld##name(){return field(name);}
#define X_DEF_DATABASE_TABLE(TblName,FldList)	\
	class Tbl##TblName : public XDatabaseTable{ \
	public:enum{FldList(_X_DEF_DATABASE_FLD_ENUM) Count};\
		Tbl##TblName():XDatabaseTable(#TblName){FldList(_X_DEF_DATABASE_FLD1)} FldList(_X_DEF_DATABASE_FLD2) };


class XSqlWhereCondition : public QObject
{
	Q_OBJECT
public:
	enum Type{ AndType, OrType };
	XSqlWhereCondition(QObject* parent=nullptr)
        :QObject(parent),m_type(AndType) {}
    virtual ~XSqlWhereCondition(){}

    void setType(Type type){m_type=type;}
    Type type()const{return m_type;}

    void addChild(XSqlWhereCondition* child){Q_ASSERT(child);m_children.append(child);}
	void addStringCondition(const QString& str);

    QString toString(bool addType = false)const
    {
        QString ret;
        if (m_children.isEmpty())
        {
            ret = toStringEvent();
        }
        else{
            bool isFirst = true;
            int count = m_children.count();
            for (int i = 0; i < count; ++i)
            {
                auto c = m_children.at(i);

                if (c->isValid())
                {
                    ret.append(QString(" %1").arg(c->toString(!isFirst)));
                    isFirst = false;
                }

            }
        }

        if (addType)
        {
            switch (m_type)
            {
            case AndType:ret.prepend("and "); break;
            case OrType:ret.prepend("or "); break;
            }
        }

        return ret;
    }
    virtual bool isValid()const
    {
        for (auto& c : m_children)
        {
            if (c->isValid())
                return true;
        }
        return false;
    }


protected:
    virtual QString toStringEvent()const{return QString();}

private:
    Type m_type;
    QList<XSqlWhereCondition*> m_children;
};

class XStringQueryCondition : public XSqlWhereCondition
{
public:
    XStringQueryCondition(const QString& str,QObject* parent = nullptr)
        :XSqlWhereCondition(parent),m_str(str){}
    ~XStringQueryCondition(){}

	virtual bool isValid()const { return true; }

protected:
    virtual QString toStringEvent()const{return m_str;}

private:
    QString m_str;
};

inline void XSqlWhereCondition::addStringCondition(const QString &str)
{
    addChild(new XStringQueryCondition(str,this));
}


class XDatabaseTableField
{
public:
    XDatabaseTableField(const QString& name, QVariant::Type type)
    {
        m_name = name;
        m_type = type;
        m_isEnabled = true;
        m_alias = name;
        m_isPrimaryKey = false;
        m_isAutoIncrement = false;
        m_isUnique = false;
        m_isNotNull = false;
        m_isDefault = false;
        m_foreignKey = nullptr;
    }
    ~XDatabaseTableField(){}

    const QString& name()const{return m_name;}
    QVariant::Type type()const{return m_type;}

    void setEnabled(bool enable){m_isEnabled=enable;}
    bool isEnabled()const{return m_isEnabled;}

    void setAlias(const QString& alias){m_alias=alias;}
    const QString& alias()const{return m_alias;}

    void setPrimaryKey(){m_isPrimaryKey=true;}
    void setAutoIncrement(){m_isAutoIncrement=true;}
    void setUnique(){m_isUnique=true;}
    void setNotNull(){m_isNotNull=true;}
    void setDefault(const QVariant& defaultValue)
    {
        Q_ASSERT(defaultValue.type() == m_type);
        m_isDefault = true;
        m_defaultValue = defaultValue;
    }
    void setForeignKey( XDatabaseTableField* key) { m_foreignKey=key; }

    bool isPrimaryKey()const{return m_isPrimaryKey;}
    bool isAutoIncrement()const{return m_isAutoIncrement;}
    bool isUnique()const{return m_isUnique;}
    bool isNotNull()const{return m_isNotNull;}
    bool isDefault()const{return m_isDefault;}
    const QVariant& defaultValue()const{return m_defaultValue;}
    XDatabaseTableField* foreignKey()const{return m_foreignKey;}

    void setTableName(const QString& tableName){m_tableName=tableName;}
    const QString& tableName()const{return m_tableName;}

private:
    QString m_name;
    QVariant::Type m_type;

    bool m_isEnabled;

    QString m_alias;

    bool m_isPrimaryKey;
    bool m_isAutoIncrement;
    bool m_isUnique;
    bool m_isNotNull;
    bool m_isDefault;
    QVariant m_defaultValue;
    XDatabaseTableField* m_foreignKey;

    QString m_tableName;
};

class XDatabaseTable
{
public:
	typedef struct{
		QString name;
		bool	desc;
	}OrderByItem;
    XDatabaseTable(const QString& name=QString())
        :m_name(name){}
    ~XDatabaseTable(){}

    const QString& name()const{return m_name;}
    void setName(const QString& name){m_name=name;}
    const std::list<XDatabaseTableField>& fields() const{return m_fields;}
    void appendField(const XDatabaseTableField& field)
    {
        auto fld = field;
        if (fld.tableName().isEmpty())
            fld.setTableName(name());
        m_fields.push_back(fld);
    }
    void appendFields(const std::list<XDatabaseTableField>& fields)
    {
        for (auto& fld : fields)
            appendField(fld);
    }
    XDatabaseTableField& field(int idx)
    {
        auto itr = m_fields.begin();
        for (int i = 0; i < idx; ++i)
            ++itr;

        return *itr;
    }
    XDatabaseTableField* field(XDatabaseTableField& fld)
    {
        for (auto& f : m_fields)
        {
            if (f.tableName() == fld.tableName()
                    && f.name() == fld.name())
                return &f;
        }
        return nullptr;
    }
    int indexOf(const QString& fieldName)const
    {
        int index = 0;
        for (auto& fld : m_fields)
        {
			QString name = fld.name();
			if (name == "''") { // 占位符字段
				name = fld.alias();
			}
			if (name == fieldName)
				return index;

            ++index;
        }
        return -1;
    }
    int indexOf(const XDatabaseTableField& fld)const{  return indexOf(fld.name());}

    void setAllFieldsEnabled(bool enable)
    {
        for (auto& fld : m_fields)
            fld.setEnabled(enable);
    }

    void appendGroupBy(const XDatabaseTableField& field){ m_groupBy.append(field.name()); }
    const QStringList& groupBy()const{return m_groupBy;}
    void clearGroupBy(){m_groupBy.clear();}

    void appendOrderBy(const XDatabaseTableField& field, bool desc = false)
    {
        m_orderBy.append(OrderByItem{ field.name(), desc });
    }
    const QList<OrderByItem>& orderBy()const{ return m_orderBy; }
    void clearOrderBy(){ m_orderBy.clear(); }

    QStringList fieldNames()const
    {
        QStringList ret;
        for (auto& f : m_fields)
            ret.append(f.name());
        return ret;
    }
    QStringList fieldAliasNames()const
    {
        QStringList ret;
        for (auto& f : m_fields)
            ret.append(f.alias());
        return ret;
    }

private:
    QString m_name;
    std::list<XDatabaseTableField> m_fields;
    QStringList m_groupBy;
    QList<XDatabaseTable::OrderByItem> m_orderBy;
};

class XDatabaseItem
{
public:
    XDatabaseItem():m_query(nullptr){}
    virtual ~XDatabaseItem(){}

    void bindValues(QSqlQuery* query) const
    {
        m_query = query;
        bindValuesEvent();
    }
    void unbindValues(QSqlQuery* query)
    {
        m_query = query;
        unbindValuesEvent();
    }

	virtual const XDatabaseTable* table()const=0;
    virtual void setLastInsertId(int id){Q_UNUSED(id);}

protected:
	virtual void bindValuesEvent()const = 0;
	virtual void unbindValuesEvent() = 0;

    void bindValue(const XDatabaseTableField& fld, const QVariant &val) const
    {
        m_query->bindValue(":" + fld.name(), val);
    }
    QVariant unbindValue(const XDatabaseTableField& fld)
    {
        return m_query->value(fld.name());
    }

private:
    mutable QSqlQuery* m_query;
};

class XDatabaseItemContainer
{
public:
    XDatabaseItemContainer()
        :m_item(nullptr){}
    virtual ~XDatabaseItemContainer(){clear();}

    void unbindValues(QSqlQuery* query)
    {
        auto item = createItem();
        item->unbindValues(query);
        m_items.push_back(item);
    }

    int count()const{ return m_items.size(); }
	bool isEmpty()const{ return (count() == 0); }
    void clear()
    {
        for (auto item : m_items)
            delete item;
        m_items.clear();

        if (m_item)
            delete m_item;
        m_item = nullptr;
    }
    const XDatabaseTable* table()
    {
        if (m_item == nullptr)
            m_item = createItem();
        return m_item->table();
    }

    XDatabaseItem* at(int index)const
    {
        if (index<0 || index>count() - 1)
        {
            Q_ASSERT(false);
            return nullptr;
        }

        auto itr = m_items.begin();
        while (index-- > 0)
            ++itr;
        return *itr;
    }

    std::list<XDatabaseItem*>::const_iterator begin()const
    { return m_items.begin(); }
    std::list<XDatabaseItem*>::const_iterator end()const
    { return m_items.end(); }

    XDatabaseItem* appendItem()
    {
        auto item = createItem();
        m_items.push_back(item);
        return item;
    }
    void removeItem(XDatabaseItem* item)
    {m_items.remove(item);}

protected:
	virtual XDatabaseItem* createItem() const = 0;

private:
    std::list<XDatabaseItem*> m_items;
    XDatabaseItem* m_item;
};
#define X_DEF_DATABASE_ITEM_CONTAINER(Cls) \
	class Cls##Container:public XDatabaseItemContainer{ \
	public:	\
	Cls* at(int index) const{return static_cast<Cls*>(XDatabaseItemContainer::at(index));} \
	std::list<Cls*>::const_iterator begin()const \
        {   auto d = XDatabaseItemContainer::begin(); \
            auto p = &d; \
            return *(std::list<Cls*>::const_iterator*)(p);} \
	std::list<Cls*>::const_iterator end()const \
        {   auto d = XDatabaseItemContainer::end(); \
            auto p = &d; \
            return *(std::list<Cls*>::const_iterator*)(p);} \
	Cls* appendItem(){return (Cls*)XDatabaseItemContainer::appendItem();} \
	protected: \
	XDatabaseItem* createItem() const {return new Cls();}};




template <typename _Ty>
class XSqlWhereConditionItem : public XSqlWhereCondition
{
public:
	enum ConditionType{
		Invalid, Equal, NotEqual, Greater, Less,
		GreaterOrEqual, LessOrEqual, Between, Like,In
	};

	XSqlWhereConditionItem(const QString& key = QString(), XSqlWhereCondition* parent = nullptr)
        :XSqlWhereCondition(parent), m_key(key), m_dataType(QVariant(_Ty()).type()), m_conditionType(Invalid){}
    ~XSqlWhereConditionItem(){}

	void setKey(const QString& key){ m_key = key; }
	XSqlWhereConditionItem<_Ty>& adjustKey(const XDatabaseTableField& fld)
	{ 
		setKey(fld.name());
		return *this;
	}

	void setEqual(const _Ty& value){ m_conditionType = Equal; m_value1 = value; }
	void setNotEqual(const _Ty& value){ m_conditionType = NotEqual; m_value1 = value; }
	void setGreater(const _Ty& value){ m_conditionType = Greater; m_value1 = value; }
	void setLess(const _Ty& value){ m_conditionType = Less; m_value1 = value; }
	void setGreaterOrEqual(const _Ty& value){ m_conditionType = GreaterOrEqual; m_value1 = value; }
	void setLessOrEqual(const _Ty& value){ m_conditionType = LessOrEqual; m_value1 = value; }
	void setBetween(const _Ty& value1, const _Ty& value2)
	{
		m_conditionType = Between; m_value1 = value1; m_value2 = value2;
	}
	void setLike(const _Ty& value){ m_conditionType = Like; m_value1 = value; }
	void setIn(const QList<_Ty>& values){ m_conditionType = In; m_values = values; }

	virtual bool isValid()const { return m_conditionType != Invalid; }
	ConditionType conditionType()const{ return m_conditionType; }
	const QVariant& value1()const{ return m_value1; }
	const QVariant& value2()const{ return m_value2; }
	void reset(){ m_conditionType = Invalid; }

protected:
	virtual QString toStringEvent()const
	{
		switch (m_conditionType)
		{
		case Equal:			return QString("%1=%2").arg(strKey(m_key)).arg(strValue(m_value1));
		case NotEqual:		return QString("%1<>%2").arg(strKey(m_key)).arg(strValue(m_value1));
		case Greater:		return QString("%1>%2").arg(strKey(m_key)).arg(strValue(m_value1));
		case Less:			return QString("%1<%2").arg(strKey(m_key)).arg(strValue(m_value1));
		case GreaterOrEqual:return QString("%1>=%2").arg(strKey(m_key)).arg(strValue(m_value1));
		case LessOrEqual:	return QString("%1<=%2").arg(strKey(m_key)).arg(strValue(m_value1));
		case Between:		return QString("%1 between %2 and %3").arg(strKey(m_key)).arg(strValue(m_value1)).arg(strValue(m_value2));
		case Like:			return QString("%1 like %2").arg(strKey(m_key)).arg(strValue(m_value1));
		case In:			return QString("%1 in (%2)").arg(strKey(m_key)).arg(strsValue(m_values).join(","));
        default:break;
		}
		Q_ASSERT(false);
		return QString();
	}

private:
	QString strKey(const QString& key)const
	{
		switch (m_dataType)
		{
		case QVariant::Date:	return QString("date(%1)").arg(key);
		case QVariant::Time:	return QString("time(%1)").arg(key);
		case QVariant::DateTime:return QString("datetime(%1)").arg(key);
        default: break;
		}
		return key;
	}
	QString strValue(const QVariant& val) const
	{
		switch (m_dataType)
		{
		case QVariant::String:		return QString("'%1'").arg(val.toString());
		case QVariant::Int:			return QString("%1").arg(val.toInt());
		case QVariant::Double:		return QString("%1").arg(val.toDouble());
		case QVariant::LongLong:	return QString("%1").arg(val.toLongLong());
		case QVariant::Date:		return QString("date('%1')").arg(val.toDate().toString("yyyy-MM-dd"));
		case QVariant::Time:		return QString("time('%1')").arg(val.toTime().toString("hh:mm:ss"));
		case QVariant::DateTime:	return QString("datetime('%1')").arg(val.toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
		case QVariant::Bool:		return QString("%1").arg(val.toBool() ? 1 : 0);
        default:break;
		}
		Q_ASSERT(false);
		return QString();
	}
	QStringList strsValue(const QList<_Ty>& values) const
	{
		QStringList ret;
		for (auto& val : values)
			ret.append(strValue(val));
		return ret;
	}

private:
	QString			m_key;
	QVariant::Type	m_dataType;
	ConditionType	m_conditionType;
	QVariant		m_value1;
	QVariant		m_value2;
	QList<_Ty>		m_values;
};


class XDatabase;
class XDatabaseExecuter : public QObject, public XHashItem<std::string, XDatabaseExecuter>
{
	Q_OBJECT

protected:
	virtual void setup(void * param);


public:
	static XDatabaseExecuter* getInstance(XDatabase* xdb)
	{
		auto id = XDatabaseExecuter::genId(xdb);
		return dynamic_cast<XDatabaseExecuter*>( XHashItem::getInstance(id,xdb));
	}

	bool appendTable(XDatabaseTable* table)
	{

		Q_ASSERT(!m_tables.contains(table->name()));

		QMutexLocker locker(mutex());
		if (!checkTable(table))
		{
			if (m_db.tables().contains(table->name()))
			{
#ifdef _DEBUG
				if (!execSql(QString("DROP TABLE %1").arg(table->name())))
					return false;
#else
				setLastError(tr("Table(%1) exist! For Data-Safety, please edit table manual. ").arg(table->name()));
				return false;
#endif
			}

			return createTable(table);
		}
		return true;
	}

	bool select(const QString& sql, XDatabaseItemContainer* container)
	{
		QMutexLocker locker(mutex());

		QSqlQuery query(m_db);
		if (!query.exec(sql))
		{
			setLastError(query);
			return false;
		}
		while (query.next())
		{
			container->unbindValues(&query);
		}
		return true;
	}
	bool select(const QString& sql, XDatabaseItem* item)
	{
		QMutexLocker locker(mutex());

		QSqlQuery query(m_db);
		if (!query.exec(sql))
		{
			setLastError(query);
			return false;
		}
		if (query.next())
		{
			item->unbindValues(&query);
			return true;
		}
		return false;
	}

	bool selectEditTable(const XDatabaseTable* table, QTableView* view, const XSqlWhereCondition* whereCond = nullptr)
	{
		QMutexLocker locker(mutex());

		QString tableName = table->name();
		QSqlTableModel* model = static_cast<QSqlTableModel*>(view->model());
		if (model == nullptr)
		{
			model = new QSqlTableModel(view, m_db);
			model->setEditStrategy(QSqlTableModel::OnFieldChange);
			connect(model, &QSqlTableModel::dataChanged, [=]() {emitTableChangedSignal(tableName); });
			view->setModel(model);
		}
		model->setTable(tableName);
		if (whereCond) {
			model->setFilter(whereCond->toString());
		}
		bool rs = model->select();

		auto flds = table->fields();
		int i = 0;
		for (auto& fld : flds)
		{
			model->setHeaderData(i, Qt::Horizontal, fld.alias());
			view->setColumnHidden(i, !fld.isEnabled());
			++i;
		}
		return rs;
	}

	bool haveItem(const XDatabaseTable* table, const XSqlWhereCondition* whereCond = nullptr)
	{
		QMutexLocker locker(mutex());

		auto sql = selectSql(table, whereCond, false);
		QSqlQuery query(m_db);
		if (!query.exec(sql))
		{
			setLastError(query);
			return false;
		}
		return query.next();
	}

	bool insert(XDatabaseItem* item, bool signal = true)
	{
		auto table = item->table();

		QStringList fs;
		QStringList vs;
		for (auto& field : table->fields())
		{
			if (field.isAutoIncrement())
				continue;

			fs.append(field.name());
			vs.append(":" + field.name());
		}

		QString sql = QString("insert into %1(%2) values (%3)").arg(table->name()).arg(fs.join(",")).arg(vs.join(","));

		QMutexLocker locker(mutex());

		QSqlQuery query(m_db);
		if (!query.prepare(sql))
		{
			setLastError(query);
			return false;
		}
		item->bindValues(&query);
		if (!query.exec())
		{
			setLastError(query);
			return false;
		}
		item->setLastInsertId(query.lastInsertId().toInt());

		if (signal)
			emitTableChangedSignal(table->name());

		return true;
	}

	bool update(const XDatabaseItem* item, const XSqlWhereCondition* cond, bool signal = true, const XDatabaseTable* table = nullptr)
	{
		auto tbl = table ? table : item->table();

		QStringList fs;
		for (auto& fld : tbl->fields())
		{
			if (fld.isAutoIncrement())
				continue;

			if (fld.isEnabled()) {
				fs.append(QString("%1=:%1").arg(fld.name()));
			}
		}
		QString sql = QString("update %1 set %2").arg(tbl->name()).arg(fs.join(","));
		setWhereConditon(sql, cond);

		QMutexLocker locker(mutex());

		QSqlQuery query(m_db);
		if (!query.prepare(sql))
		{
			setLastError(query);
			return false;
		}

		item->bindValues(&query);
		if (!query.exec())
		{
			setLastError(query);
			return false;
		}
		if (signal)
		{
			emitTableChangedSignal(tbl->name());
		}

		return true;
	}
	bool remove(const XDatabaseTable* table, const XSqlWhereCondition* cond = nullptr, bool signal = true)
	{
		QMutexLocker locker(mutex());
		QSqlQuery query(m_db);
		QString sql = QString("delete from %1").arg(table->name());
		setWhereConditon(sql, cond);

		if (!query.exec(sql))
		{
			setLastError(query);
			return false;
		}

		if (signal)
			emitTableChangedSignal(table->name());
		return true;
	}

	bool execSql(const QString& sql, QSqlQueryModel* model = nullptr)
	{
		QMutexLocker locker(mutex());

		QSqlQuery query(m_db);
		if (!query.exec(sql))
		{
			setLastError(query);
			return false;
		}

		if (model)
		{
			model->setQuery(query);
		}

		return true;
	}

	bool transaction() 
	{ 
		return m_db.transaction();
	}
	bool commit() 
	{ 
		return m_db.commit(); 
	}


	static QMutex* mutex()
	{
		static QMutex s_mutex(QMutex::Recursive);
		return &s_mutex;
	}
	QString selectSql(const XDatabaseTable* table, const XSqlWhereCondition* cond, bool alias)
	{
		QStringList flds;

		if (alias)
		{
			for (auto& fld : table->fields())
			{
				Q_ASSERT(!fld.tableName().isEmpty());
				if (fld.isEnabled())
				{
					flds.append(QString("%1 AS '%2'").arg(fld.name()).arg(fld.alias()));
				}
			}
		}
		else
		{
			for (auto& fld : table->fields())
			{
				Q_ASSERT(!fld.tableName().isEmpty());
				if (fld.isEnabled())
				{
					flds.append(QString("%1").arg(fld.name()));
				}
			}
		}

		QString sql = QString("select %1 from %2").arg(flds.join(",")).arg(table->name());
		setWhereConditon(sql, cond);
		auto groupBy = table->groupBy();
		if (!groupBy.isEmpty())
		{
			sql.append(QString(" group by %1").arg(groupBy.join(",")));
		}
		auto orderBy = table->orderBy();
		QStringList strsOrderBy;
		for (auto& var : orderBy)
		{
			strsOrderBy.append(var.name + (var.desc ? " desc" : " asc"));
		}
		if (!strsOrderBy.isEmpty())
		{
			sql.append(QString(" order by %1").arg(strsOrderBy.join(",")));
		}
		return sql;
	}


private:
	friend class XHashItem<std::string, XDatabaseExecuter>;
	XDatabaseExecuter(const std::string& id)
		:XHashItem<std::string, XDatabaseExecuter>(id)
	{}

	static std::string genId(XDatabase* xdb);


	bool checkTable(XDatabaseTable* table)
	{
		const auto sql1 = createTableSql(table);
		const auto sql2 = preCreateTableSql(table);
		return (sql1 == sql2);
	}
	bool createTable(XDatabaseTable* table)
	{
		auto sql = createTableSql(table);
		return execSql(sql);
	}
	QString preCreateTableSql(XDatabaseTable* table)
	{
		QString sql = QString("select sql from sqlite_master where type='table' and name='%1'").arg(table->name());
		QSqlQuery query(m_db);
		if (!query.exec(sql))
		{
			setLastError(query);
			return QString();
		}
		if (query.next())
		{
			return query.value(0).toString();
		}
		return QString();
	}
	
	void setWhereConditon(QString& sql, const XSqlWhereCondition* cond)
	{
		QString wh = cond ? cond->toString() : "";
		if (!wh.isEmpty())
			sql.append(QString(" where %1").arg(wh));
	}
	static QString createTableSql(XDatabaseTable* table)
	{
		QStringList flds;
		QList<const XDatabaseTableField*> foreignKeyFlds;
		for (const auto& fld : table->fields())
		{
			flds.append(toString(fld));
			if (fld.foreignKey())
				foreignKeyFlds.append(&fld);
		}

		int i = 0;
		for (auto fld : foreignKeyFlds)
		{
			auto k = fld->foreignKey();
			flds.append(QString("CONSTRAINT f_key%1 FOREIGN KEY (%2) REFERENCES %3(%4)")
				.arg(++i).arg(fld->name()).arg(k->tableName()).arg(k->name()));
		}
		return QString("CREATE TABLE %1 (%2)").arg(table->name()).arg(flds.join(","));
	}
	static QString toString(const XDatabaseTableField& field)
	{
		QString str = QString("%1 %2").arg(field.name()).arg(toSqlString(field.type()));
		if (field.isPrimaryKey())
			str.append(" PRIMARY KEY");
		if (field.isAutoIncrement())
			str.append(" AUTOINCREMENT");
		if (field.isUnique())
			str.append(" UNIQUE");
		if (field.isNotNull())
			str.append(" NOT NULL");
		if (field.isDefault())
		{
			QString defaultValue;
			switch (field.defaultValue().type())
			{
			case QVariant::Int:		defaultValue = QString::number(field.defaultValue().toInt()); break;
			case QVariant::Double:	defaultValue = QString::number(field.defaultValue().toDouble()); break;
			case QVariant::String:	defaultValue = QString("'%1'").arg(field.defaultValue().toString()); break;
			case QVariant::Bool:	defaultValue = QString("'%1'").arg(field.defaultValue().toBool() ? "true" : "false"); break;
			case QVariant::Date:	defaultValue = QString("'%1'").arg(field.defaultValue().toDate().toString()); break;
			case QVariant::DateTime:defaultValue = QString("'%1'").arg(field.defaultValue().toDateTime().toString()); break;
			default: Q_ASSERT(false);
			}
			str.append(" DEFAULT %1").arg(defaultValue);
		}

		return str;
	}
	static QString toSqlString(QVariant::Type type)
	{
		switch (type)
		{
		case QVariant::Int:		return "INTEGER";
		case QVariant::Double:	return "DOUBLE";
		case QVariant::String:	return "TEXT";
		case QVariant::Bool:	return "BOOLEAN";
		case QVariant::Date:	return "DATE";
		case QVariant::DateTime:return "DATETIME";
		default:break;
		}
		Q_ASSERT(false);
		return QString();
	}

	void setLastError(const QString& error);
	void setLastError(const QSqlQuery& query)
	{
		setLastError(QString("%1: %2").arg(query.lastError().text()).arg(query.lastQuery()));
	}
	void emitTableChangedSignal(const QString& tableName);

private:
	XDatabase*	m_xdb;
	QSqlDatabase m_db;
	QStringList		m_tables;
	QHash<QString, XDatabaseExecuter*>* m_instances;
	
};
#define XDB(obj)	(XDatabaseExecuter::getInstance(obj))

class XDatabase:public QObject
{
	Q_OBJECT

signals:
	void tableChanged(const QString& tableName);
	void error(const QString& err);

public:
	enum Driver{ SQLITE,ODBC};
    XDatabase(QObject* parent = nullptr)
        :QObject(parent),m_driver(SQLITE)
    {
        m_databaseName = QString("%1.db").arg(QCoreApplication::applicationName());
    }
    ~XDatabase(){}

    void setDriver(Driver driver){m_driver=driver;}
    void setDatabaseName(const QString& databaseName){m_databaseName=databaseName;}
    void setHostName(const QString& hostName){m_hostName=hostName;}
    void setUserName(const QString& userName){m_userName=userName;}
    void setPassword(const QString& password){m_password=password;}
    void setPort(int port){m_port=port;}
	const QString& databaseName() const{ return m_databaseName; }
	const QString& hostName() const { return m_hostName; }
	const QString& userName() const { return m_userName; }
	const QString& password() const { return m_password; }
	int port() const { return m_port; }

    const QString& lastError()const{return m_lastError;}
    bool appendTable(XDatabaseTable* table)
    {
		return XDB(this)->appendTable(table);
    }

    bool select(const QString& sql, XDatabaseItemContainer* container)
    {
		return XDB(this)->select(sql, container);
    }
    bool select(const QString& sql, XDatabaseItem* item)
    {
		return XDB(this)->select(sql, item);
    }
    bool select(const QString& sql, QTableView* view)
    {return select(sql, tableViewModel(view));}
    bool select(const QString& sql, QSqlQueryModel* model)
    {
        Q_ASSERT(model);
        return XDB(this)->execSql(sql,model);
    }
    bool select(XDatabaseItemContainer* container, const XSqlWhereCondition* whereCond = nullptr, const XDatabaseTable* table =nullptr)
    {
        auto sql =XDB(this)->selectSql(table?table:container->table(), whereCond, false);
        return select(sql, container);
    }
    bool select(XDatabaseItem* item, const XSqlWhereCondition* whereCond = nullptr, const XDatabaseTable* table =nullptr)
    {
        auto sql = XDB(this)->selectSql(table?table:item->table(), whereCond,false);
        return select(sql, item);
    }
    bool select(const XDatabaseTable* table, QTableView* view, const XSqlWhereCondition* whereCond = nullptr)
    {
        auto sql = XDB(this)->selectSql(table, whereCond,true);
        return select(sql, view);
    }
    bool select(const XDatabaseTable* table, QSqlQueryModel* model, const XSqlWhereCondition* whereCond = nullptr)
    {
        auto sql = XDB(this)->selectSql(table, whereCond, true);
        return select(sql, model);
    }
	bool selectEditTable(const XDatabaseTable* table, QTableView* view, const XSqlWhereCondition* whereCond = nullptr)
	{
		return XDB(this)->selectEditTable(table, view, whereCond);
	}
    bool haveItem(const XDatabaseTable* table, const XSqlWhereCondition* whereCond = nullptr)
    {
		return XDB(this)->haveItem(table, whereCond);
    }

    bool insert(XDatabaseItem* item, bool signal = true)
    {
		return XDB(this)->insert(item, signal);
    }
    bool update(const XDatabaseItem* item, const XSqlWhereCondition* cond, bool signal = true,const XDatabaseTable* table =nullptr)
    {
		return XDB(this)->update(item, cond, signal, table);
    }
    bool remove(const XDatabaseTable* table, const XSqlWhereCondition* cond=nullptr, bool signal = true)
    {
		return XDB(this)->remove(table, cond, signal);
    }


    bool transaction(){return  XDB(this)->transaction(); }
    bool commit(){ return XDB(this)->commit(); }

	void clear() { return XDB(this)->clear(); }

    static QString driverName(Driver driver)
    {
        switch (driver)
        {
        case XDatabase::SQLITE:
            return "QSQLITE";
            break;
        case XDatabase::ODBC:
            break;
        }
        Q_ASSERT(false);
        return QString();
    }
	QString driverName()const { return driverName(m_driver); }

    static QSqlQueryModel* tableViewModel(QTableView* view)
    {
        QSqlQueryModel* model = static_cast<QSqlQueryModel*>(view->model());
        if (model == nullptr)
        {
            model = new QSqlQueryModel(view);
            view->setModel(model);
        }
        return model;
    }

public slots:
    void emitTableChangedSignal(const QString& tableName)
    {emit tableChanged(tableName);}

private:
    void setLastError(const QString& err)
    {
        m_lastError = err;
        emit error(m_lastError);
    }

private:
	friend class XDatabaseExecuter;
    XDatabase::Driver m_driver;
    QString m_databaseName;
    QString m_hostName;
    int		m_port;
    QString m_userName;
    QString m_password;
    QString m_lastError;
};

inline void XDatabaseExecuter::setup(void * param)
{
	m_xdb = (XDatabase*)(param);
	setParent(m_xdb);
	QString driverName = m_xdb->driverName();

	m_db = QSqlDatabase::addDatabase(driverName, QString::fromStdString(id()));
	m_db.setDatabaseName(m_xdb->databaseName());
	m_db.setHostName(m_xdb->hostName());
	m_db.setPort(m_xdb->port());
	if (!m_db.open(m_xdb->userName(), m_xdb->password()))
	{
		QSqlDatabase::removeDatabase(m_db.connectionName());
		m_xdb->setLastError(m_db.lastError().text());
	}

	connect(QThread::currentThread(), SIGNAL(finished()), this, SLOT(deleteLater()));
}

inline std::string XDatabaseExecuter::genId(XDatabase* xdb)
{
	return QString("%1%2").arg(xdb->databaseName()).arg((quint64)QThread::currentThreadId())
		.toStdString();
}

inline void XDatabaseExecuter::setLastError(const QString& error)
{
	m_xdb->setLastError(error);
}

inline void XDatabaseExecuter::emitTableChangedSignal(const QString& tableName)
{
	m_xdb->emitTableChangedSignal(tableName);
}

class XDatabaseCommiter
{
public:
	XDatabaseCommiter(XDatabase* db) :m_db(db)
	{ 
		XDB(m_db)->mutex()->lock();
		m_db->transaction(); 
	}
	~XDatabaseCommiter()
	{ 
		m_db->commit();
		XDB(m_db)->mutex()->unlock();
	}

private:
	XDatabase* m_db;
};

class XDatabaseCleaner
{
public:
	XDatabaseCleaner(XDatabase* db):m_db(db){}
	~XDatabaseCleaner() { m_db->clear(); }

private:
	XDatabase* m_db;
};

#endif // XDATABASE_H


#ifndef _X_UNIT_CALCULATOR_HPP
#define _X_UNIT_CALCULATOR_HPP

#include <QList>
#include <QString>
#include <QDebug>

class XUnitCalculator
{
public:
	typedef struct{
		qint64 size;
		QString unit;
	}UnitItem;

	typedef struct{
		double size;
                QString unit;
	}Result;

	void appendUnit(const UnitItem& unitItem)
	{
		Q_ASSERT(validAppendUnitItem(unitItem));
		m_unitItems.append(unitItem);
	}

	void appendUnit(qint64 size, const QString& unit)
	{
		m_unitItems.append(UnitItem{size,unit});
	}

	QStringList units()const
	{
		QStringList ret;
		for (auto& unitItem : m_unitItems)
			ret.append(unitItem.unit);
		return ret;
	}

	Result calculate(qint64 size)
	{
		Q_ASSERT(!m_unitItems.isEmpty());

		int count = m_unitItems.count();
		for (int i = count - 1; i >= 0; --i)
		{
			auto& unitItem = m_unitItems.at(i);
			if (size >= unitItem.size)
				return calculate(size, unitItem);
		}

		return calculate(size, m_unitItems.first());
	}

	static Result calculate(qint64 size, const UnitItem& unitItem)
	{
		Result ret;
		ret.size = size*1.0 / unitItem.size;
		ret.unit = unitItem.unit;
		return ret;
	}

private:
	bool validAppendUnitItem(const UnitItem& unitItem)
	{
		if (m_unitItems.isEmpty())
			return true;

		auto lastItem = m_unitItems.last();
		return lastItem.size < unitItem.size;
	}

private:
	QList<UnitItem> m_unitItems;
};

#endif // _X_UNIT_CALCULATOR_HPP

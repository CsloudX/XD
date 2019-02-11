#ifndef _X_TIME_USE_NODE_H
#define _X_TIME_USE_NODE_H

#include <QString>
#include <QList>
#include <QStringList>

class XTimeUseNode
{
public:
	XTimeUseNode(XTimeUseNode* parent=nullptr);
	~XTimeUseNode();

	const QString& text()const { return m_text; }
	int timeUse()const { return m_timeUse; }
	const QList<XTimeUseNode*>& children()const { return m_children; }
	XTimeUseNode* parent() { return m_parent; }

	void setParent(XTimeUseNode* parent) { connect(parent, this); }
	bool parse(const QStringList& lines, int* lineNum);

private:
	bool parseBEGIN(const QStringList& lines, int* lineNum);
	bool parseToEND(const QStringList& lines, int* lineNum);
	static bool readLine(const QStringList& lines, int* lineNum, int* tabCount, QString* text, int* timeUse);
	static void connect(XTimeUseNode* parent, XTimeUseNode* child);
	static void disconnect(XTimeUseNode* parent, XTimeUseNode* child);

private:
	int m_tabCount;
	QString m_text;
	int m_timeUse;
	XTimeUseNode* m_parent;
	QList<XTimeUseNode*> m_children;
};


#endif // !_X_TIME_USE_NODE_H




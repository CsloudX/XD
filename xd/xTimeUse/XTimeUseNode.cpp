#include "XTimeUseNode.h"

XTimeUseNode::XTimeUseNode(XTimeUseNode* parent)
	:m_parent(nullptr)
{
	if (parent) {
		connect(parent, this);
	}
}


XTimeUseNode::~XTimeUseNode()
{
	if (m_parent) {
		disconnect(m_parent, this);
	}
	qDeleteAll(m_children);
	m_children.clear();
}

bool XTimeUseNode::parse(const QStringList& lines, int* lineNum)
{
	if (!parseBEGIN(lines, lineNum))	return false;
	if (!parseToEND(lines, lineNum))	return false;
	return true;
}

bool XTimeUseNode::parseBEGIN(const QStringList& lines, int* lineNum)
{
	static QString BEGIN(": BEGIN");

	int tabCount;
	QString text;
	int timeUse;

	while (readLine(lines, lineNum, &tabCount, &text, &timeUse))  {
		if (text.endsWith(BEGIN)) {
			m_tabCount = tabCount;
			m_text = text.left(text.count()-BEGIN.count());
			return true;
		}
	}
	return false;
}

bool XTimeUseNode::parseToEND(const QStringList& lines, int* lineNum)
{
	static QString END(": END");

	int tabCount;
	QString text;
	int timeUse;

	while (readLine(lines, lineNum, &tabCount, &text, &timeUse)) {
		if (m_tabCount== tabCount && text.startsWith(m_text)) {
			if (text.endsWith(END)) {
				m_timeUse = timeUse;
				return true;
			}
			else {
				XTimeUseNode* node = new XTimeUseNode(this);
				node->m_text = text.mid(m_text.count()+2);
				node->m_timeUse = timeUse;
			}
		}
		if (tabCount == m_tabCount + 1) {
			--(*lineNum);
			XTimeUseNode* node = new XTimeUseNode(this);
			node->parse(lines, lineNum);
		}

	}
	return false;
}

bool XTimeUseNode::readLine(const QStringList& lines, int* lineNum, int* tabCount, QString* text, int* timeUse)
{
	static QString X_TIME_USE("X_TIME_USE: ");
	while (true) {
		if (*lineNum >= lines.count())
			return false;

		QString line = lines.at(*lineNum).trimmed();
		++(*lineNum);

		if (line.startsWith(X_TIME_USE)) {
			line = line.mid(X_TIME_USE.count());

			int beginPos = 0;
			int endPos = line.count();

			while (line.at(beginPos) == '\t') {
				++beginPos;
			}

			if (line.endsWith("ms")) {
				endPos = line.lastIndexOf('\t');
				QString strMS = line.mid(endPos);
				strMS = strMS.left(strMS.count()-2);
				*timeUse = strMS.toInt();
			}

			*tabCount = beginPos;
			*text = line.mid(beginPos, endPos - beginPos);
			return true;
		}
	}
}

void XTimeUseNode::connect(XTimeUseNode* parent, XTimeUseNode* child)
{
	Q_ASSERT(parent&& child);

	if (child->m_parent) {
		disconnect(child->m_parent, child);
	}

	parent->m_children.append(child);
	child->m_parent = parent;
}

void XTimeUseNode::disconnect(XTimeUseNode* parent, XTimeUseNode* child)
{
	Q_ASSERT(parent&& child &&parent->m_children.contains(child));

	parent->m_children.removeOne(child);
	child->m_parent = nullptr;
}

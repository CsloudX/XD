#ifndef _XOPENGLSHADER_H_
#define _XOPENGLSHADER_H_

#include <QObject>
#include <QFile>
#include <QOpenGLFunctions>

class XOpenGLShader : public QObject
{
	Q_OBJECT

public:
	XOpenGLShader(GLenum type, QOpenGLFunctions* func, QObject* parent = nullptr)
		:QObject(parent),m_type(type), m_func(func)
	{
		Q_ASSERT((m_type == GL_VERTEX_SHADER) || (m_type == GL_FRAGMENT_SHADER));
		Q_ASSERT(m_func);

		m_id = m_func->glCreateShader(m_type);
		Q_ASSERT(m_id);
	}
	~XOpenGLShader()
	{
		m_func->glDeleteShader(m_id);
	}

	GLuint id()const { return m_id; }
	GLenum type()const { return m_type; }

	bool compileSourceCode(const QByteArray& code)
	{
		const char* srcCode = code.data();
		m_func->glShaderSource(m_id, 1, &srcCode, 0);
		m_func->glCompileShader(m_id);

		GLint rs=0;
		m_func->glGetShaderiv(m_id, GL_COMPILE_STATUS, &rs);
		if (!rs) {
			GLint len = 0;
			m_func->glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &len);
			if (len > 0) {
				char* log = new char[len];
				m_func->glGetShaderInfoLog(m_id, len, NULL, log);
				m_log = log;
				delete[] log;
			}
		}

		return rs;
	}

	bool compileSourceFile(const QString& fileName)
	{
		QFile file(fileName);
		if (!file.open(QIODevice::ReadOnly)) {
			m_log = file.errorString();
			return false;
		}

		return compileSourceCode(file.readAll());
	}

	const QString& log()const { return m_log; }
	
private:
	GLenum	m_type;
	QOpenGLFunctions* m_func;
	GLuint m_id;
	QString m_log;
};

#endif // _XOPENGLSHADER_H_

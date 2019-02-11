#ifndef _XOPENGLSHADERPROGRAM_H_
#define _XOPENGLSHADERPROGRAM_H_

#include <QObject>
#include <QOpenGLFunctions>
#include "XOpenGLShader.h"

class XOpenGLShaderProgram : public QObject
{
	Q_OBJECT
public:
	XOpenGLShaderProgram(QOpenGLFunctions* func, QObject* parent=nullptr)
		:QObject(parent),m_func(func)
	{
		Q_ASSERT(m_func);

		m_id = m_func->glCreateProgram();
		Q_ASSERT(m_id);
	}
	~XOpenGLShaderProgram()
	{
		m_func->glDeleteProgram(m_id);
	}

	void attachShader(XOpenGLShader* shader)
	{
		m_func->glAttachShader(m_id, shader->id());
	}
	void bindAttributeLocation(const QString& name, int location)
	{
		const char* s = name.toLocal8Bit().data();
		m_func->glBindAttribLocation(m_id, location, s);
	}

	bool link()
	{
		m_func->glLinkProgram(m_id);
		GLint rs = 0;
		m_func->glGetProgramiv(m_id, GL_LINK_STATUS, &rs);
		if (!rs) {
			GLint len = 0;
			m_func->glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &len);
			if (len > 0) {
				char* log = new char[len];
				m_func->glGetProgramInfoLog(m_id, len, nullptr, log);
				m_log = log;
				delete[] log;
			}
		}

		return rs;
	}
	void use() { m_func->glUseProgram(m_id); }

	const QString& log()const { return m_log; }

private:
	GLuint m_id;
	QOpenGLFunctions* m_func;
	QString m_log;
};

#endif // _XOPENGLSHADERPROGRAM_H_


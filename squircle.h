/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef SQUIRCLE_H
#define SQUIRCLE_H

#include <QtQuick/QQuickItem>
#include <QtGui/QOpenGLShaderProgram>
#include <QGLFunctions>
#include <QOpenGLFunctions>

//! [1]
class SquircleRenderer : public QObject ,protected QOpenGLFunctions {
    Q_OBJECT
public:
    SquircleRenderer() :  m_program(0) { }
    ~SquircleRenderer();

    void setViewportSize(const QSize &size) { m_viewportSize = size; }

public slots:
    void Init_opengl();
    void paint();
    void cleanup2();

private:
    QSize m_viewportSize;

    QOpenGLShaderProgram *m_program;
};
//! [1]

//! [2]
class Squircle : public QQuickItem
{
    Q_OBJECT

public:
    Squircle();

public slots:
    void sync();
    void cleanup();
    void repaint(void);


private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    SquircleRenderer *m_renderer;
};
//! [2]

#endif // SQUIRCLE_H

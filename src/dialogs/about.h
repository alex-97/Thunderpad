//
//  This file is part of Thunderpad
//
//  Copyright (c) 2013-2014 Alex Spataru <alex_spataru@outlook.com>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301
//  USA
//

#ifndef ABOUT_H
#define ABOUT_H

#include <QIcon>
#include <QLabel>
#include <QDebug>
#include <QDialog>
#include <QLayout>
#include <QDateTime>
#include <QPushButton>
#include <QApplication>

#include "assembly_info.h"
#include "platform.h"

/*!
 * \class About
 *
 * The \c About class implements a QDialog that shows information about the
 * current version of Thunderpad.
 */

class About : public QDialog {
    Q_OBJECT

public:
    explicit About(QWidget *parent = 0);

private:
    QLabel *m_icon;
    QLabel *m_name;
    QLabel *m_information;

    QVBoxLayout *m_layout;
};

#endif

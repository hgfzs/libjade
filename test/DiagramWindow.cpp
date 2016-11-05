/* DiagramWindow.cpp
 *
 * Copyright (C) 2013-2016 Jason Allen
 *
 * This file is part of the jade library.
 *
 * jade is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * jade is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with jade.  If not, see <http://www.gnu.org/licenses/>
 */

#include "DiagramWindow.h"
#include "DiagramWidget.h"

DiagramWindow::DiagramWindow() : QMainWindow()
{
	mDiagramWidget = new DiagramWidget();
	setCentralWidget(mDiagramWidget);
	
	setWindowTitle("libJade test");
	resize(1024, 768);
}

DiagramWindow::~DiagramWindow()
{
	
}

//==================================================================================================

void DiagramWindow::showEvent(QShowEvent* event)
{
	QMainWindow::showEvent(event);
	if (!event->spontaneous()) mDiagramWidget->zoomFit();
}

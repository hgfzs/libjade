/* Drawing.h
 *
 * Copyright (C) 2013-2017 Jason Allen
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

#ifndef _DRAWING_H_
#define _DRAWING_H_

#include <DrawingView.h>
#include <DrawingScene.h>
#include <DrawingItem.h>
#include <DrawingItemPoint.h>

#include <DrawingArrow.h>
#include <DrawingCurveItem.h>
#include <DrawingEllipseItem.h>
#include <DrawingLineItem.h>
#include <DrawingPathItem.h>
#include <DrawingPolygonItem.h>
#include <DrawingPolylineItem.h>
#include <DrawingRectItem.h>
#include <DrawingTextEllipseItem.h>
#include <DrawingTextItem.h>
#include <DrawingTextRectItem.h>
#include <DrawingItemGroup.h>

/*! \mainpage
 *
 * The jade library provides DrawingScene, a surface for managing a large number of 
 * two-dimensional graphical items within a scene, as well as DrawingView, a widget for viewing the
 * contents of a DrawingScene.  DrawingView supports the following features:
 *
 * \li Add and remove large numbers of items to the scene
 * \li Use the mouse to select items and manipulate them within the scene (move, resize, rotate,
 * flip, etc)
 * \li Undo/redo support
 * \li Cut/copy/paste support
 * \li Select all/select none support
 * \li Snap items to grid
 * \li Reorder items (bring to front, send to back, etc)
 * \li Group and ungroup items together
 * \li Connect items together and resize one when the other is moved
 * \li Zoom in/out/fit support
 *
 * DrawingItem is the base class for all graphical items in a DrawingScene.  It provides a
 * lightweight foundation for writing custom items. This includes defining the item's geometry,
 * painting implementation, and item interaction through event handlers.
 *
 * DrawingView, DrawingScene, and DrawingItem are highly extensible; many functions are virtual and 
 * may be overridden in a derived class implementation to alter the default behavior.
 *
 * Please see the full <a href="annotated.html">class list</a> to get started with the jade library.
 *
 * libjade is available under the <a href="https://www.gnu.org/licenses/gpl-3.0-standalone.html">
 * GNU General Public License version 3.0</a>.
 */

#endif

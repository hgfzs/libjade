/* Drawing.h
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

#ifndef __DRAWING__H__
#define __DRAWING__H__

#include <DrawingTypes.h>
#include <DrawingWidget.h>
#include <DrawingItem.h>
#include <DrawingItemPoint.h>
#include <DrawingItemStyle.h>

#include <DrawingLineItem.h>
#include <DrawingItemGroup.h>

/*! \mainpage
 *
 * The jade library provides a Qt widget for managing a large number of two-dimensional graphical
 * items within a scene: DrawingWidget.  DrawingWidget supports the following features:
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
 * DrawingItem is the base class for all graphical items in a DrawingWidget.  It provides a
 * lightweight foundation for writing custom items. This includes defining the item's geometry,
 * painting implementation, and item interaction through event handlers.
 *
 * DrawingWidget and DrawingItem are highly extensible; many functions are virtual and may
 * be overridden in a derived class implementation to alter the default behavior.
 *
 * Please see the full <a href="annotated.html">class list</a> to get started with the jade library.
 *
 * libjade is available under the <a href="https://www.gnu.org/licenses/gpl-3.0-standalone.html">
 * GNU General Public License version 3.0</a>.
 */
 
/*
 * Todo:
 * - add support for item styles
 * - add support for item behavior hints, like MapFirstItemPointToOrigin
 * Todo (future):
 * - layers and/or pages
 * - hierarchial items (parent/children)
 * - units (inches, mm, etc)
 * - can place multiple new items at once (including paste)
 */

#endif

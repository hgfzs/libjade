/* DrawingTypes.h
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

#ifndef DRAWINGTYPES_H
#define DRAWINGTYPES_H

#include <QtCore>

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include <QtGui>
#else
#include <QtWidgets>
#endif

class DrawingWidget;
class DrawingItem;
class DrawingItemPoint;

//==================================================================================================

/*! \brief Provides mouse events in a DrawingWidget scene.
 *
 * When a DrawingWidget receives a QMouseEvent, it translates it to a DrawingMouseEvent.  
 * DrawingMouseEvent stores the screen position and scene position for important event information.  
 * The event is then used in the various DrawingWidget and DrawingItem event handlers.
 */
class DrawingMouseEvent : public QEvent
{
	friend class DrawingWidget;
	
private:
	QPoint mPos;
	QPointF mScenePos;

	QPoint mButtonDownPos;
	QPointF mButtonDownScenePos;

	Qt::MouseButton mButton;
	Qt::MouseButtons mButtons;
	Qt::KeyboardModifiers mModifiers;

	bool mDragged;

public:
	/*! \brief Create a new DrawingMouseEvent with default settings.
	 *
	 * The new mouse event needs to be initialized by DrawingWidget before it can be used.
	 */
	DrawingMouseEvent();
	
	//! \brief Delete an existing DrawingMouseEvent object.
	~DrawingMouseEvent();

	
	/*! \brief Returns the mouse cursor position in screen coordinates.
	 *
	 * \sa scenePos()
	 */
	QPoint pos() const;
	
	/*! \brief Returns the mouse cursor position in scene coordinates.
	 *
	 * \sa pos()
	 */
	QPointF scenePos() const;
	
	/*! \brief Returns the mouse button down position in screen coordinates.
	 *
	 * \sa buttonDownScenePos()
	 */
	QPoint buttonDownPos() const;
	
	/*! \brief Returns the mouse button down position in scene coordinates.
	 *
	 * \sa buttonDownPos()
	 */
	QPointF buttonDownScenePos() const;
	
	/*! \brief Returns the mouse button (if any) that caused the event.
	 *
	 * \sa buttons(), modifiers()
	 */
	Qt::MouseButton button() const;
	
	/*! \brief Returns the combination of mouse buttons that were pressed at the time the event was sent.
	 *
	 * \sa button(), modifiers()
	 */
	Qt::MouseButtons buttons() const;
	
	/*! \brief Returns the keyboard modifiers in use at the time the event was sent.
	 *
	 * \sa button(), buttons()
	 */
	Qt::KeyboardModifiers modifiers() const;
	
	
	/*! \brief Returns whether the mouse has moved
	 *
	 * If the Manhattan length of the difference between the event position and button down
	 * position is less than the application's drag distance QApplication::startDragDistance(),
	 * this function returns true; otherwise it returns false.
	 */
	bool isDragged() const;
	
private:
	void setFromEvent(QMouseEvent* event, DrawingWidget* widget);
};

#endif

/* DrawingLineItem.h
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

#include <DrawingItem.h>

class DrawingLineItem : public DrawingItem
{
public:
	DrawingLineItem();
	DrawingLineItem(const DrawingLineItem& item);
	virtual ~DrawingLineItem();

	virtual DrawingItem* copy() const;

	// Selectors
	void setLine(const QLineF& line);
	void setLine(qreal x1, qreal y1, qreal x2, qreal y2);
	QLineF line() const;
	
	// Description
	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual bool isValid() const;

	virtual void paint(QPainter* painter);

	// Slots
	virtual void resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos);
	
protected:
	virtual bool newItemCopyEvent();
	virtual void newMousePressEvent(DrawingMouseEvent* event);
	virtual void newMouseMoveEvent(DrawingMouseEvent* event);
	virtual bool newMouseReleaseEvent(DrawingMouseEvent* event);
};
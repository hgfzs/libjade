/* DrawingArrow.h
 *
 * Copyright (C) 2019- Jason Allen
 *
 * This file is part of the jade application.
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

#ifndef DRAWINGARROW_H
#define DRAWINGARROW_H

#include <QPainterPath>

class DrawingArrow
{
public:
	enum Style { None, Normal, Triangle, TriangleFilled, Concave, ConcaveFilled, Circle, CircleFilled };

private:
	QPointF mPosition;
	qreal mPenWidth;
	qreal mLength;
	qreal mAngle;
	Style mStyle;
	qreal mSize;

	QPainterPath mShape;
	QPolygonF mPolygon;

public:
	DrawingArrow(Style style = None, qreal size = 0);
	DrawingArrow(const DrawingArrow& arrow);
	~DrawingArrow();

	DrawingArrow& operator=(const DrawingArrow& arrow);

	void setVector(const QPointF& pos, qreal penWidth, qreal length, qreal angle);
	void setStyle(Style style);
	void setSize(qreal size);
	QPointF vectorPosition() const;
	qreal vectorPenWidth() const;
	qreal vectorLength() const;
	qreal vectorAngle() const;
	Style style() const;
	qreal size() const;

	QPainterPath shape() const;

	void render(QPainter* painter, const QBrush& backgroundBrush);

private:
	void updateGeometry();
	QPainterPath strokePath(const QPainterPath& path) const;
};

#endif

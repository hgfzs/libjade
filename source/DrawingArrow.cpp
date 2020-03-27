/* DrawingArrow.cpp
 *
 * Copyright (C) 2019-2020 Jason Allen
 *
 * This file is part of the libjade library.
 *
 * libjade is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libjade is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libjade.  If not, see <http://www.gnu.org/licenses/>
 */

#include "DrawingArrow.h"
#include <QPainter>
#include <QtMath>

DrawingArrow::DrawingArrow(Style style, qreal size)
{
	mPosition = QPointF();
	mPenWidth = 0;
	mLength = 0;
	mAngle = 0;
	mStyle = style;
	mSize = size;

	updateGeometry();
}

DrawingArrow::DrawingArrow(const DrawingArrow& arrow)
{
	mPosition = arrow.mPosition;
	mPenWidth = arrow.mPenWidth;
	mLength = arrow.mLength;
	mAngle = arrow.mAngle;
	mStyle = arrow.mStyle;
	mSize = arrow.mSize;
	mShape = arrow.mShape;
	mPolygon = arrow.mPolygon;
}

DrawingArrow::~DrawingArrow() { }

//==================================================================================================

DrawingArrow& DrawingArrow::operator=(const DrawingArrow& arrow)
{
	mPosition = arrow.mPosition;
	mPenWidth = arrow.mPenWidth;
	mLength = arrow.mLength;
	mAngle = arrow.mAngle;
	mStyle = arrow.mStyle;
	mSize = arrow.mSize;
	mShape = arrow.mShape;
	mPolygon = arrow.mPolygon;
	return *this;
}

//==================================================================================================

void DrawingArrow::setVector(const QPointF& pos, qreal penWidth, qreal length, qreal angle)
{
	mPosition = pos;
	mPenWidth = penWidth;
	mLength = length;
	mAngle = angle;
	updateGeometry();
}

void DrawingArrow::setStyle(Style style)
{
	mStyle = style;
	updateGeometry();
}

void DrawingArrow::setSize(qreal size)
{
	mSize = size;
	updateGeometry();
}

DrawingArrow::Style DrawingArrow::style() const
{
	return mStyle;
}

qreal DrawingArrow::size() const
{
	return mSize;
}

QPointF DrawingArrow::vectorPosition() const
{
	return mPosition;
}

qreal DrawingArrow::vectorPenWidth() const
{
	return mPenWidth;
}

qreal DrawingArrow::vectorLength() const
{
	return mLength;
}

qreal DrawingArrow::vectorAngle() const
{
	return mAngle;
}

//==================================================================================================

QPainterPath DrawingArrow::shape() const
{
	return mShape;
}

//==================================================================================================

void DrawingArrow::render(QPainter* painter, const QBrush& backgroundBrush)
{
	if (mStyle != None && mSize <= mLength)
	{
		QBrush originalBrush = painter->brush();
		QPen originalPen = painter->pen();

		// Set pen
		QPen arrowPen = originalPen;
		arrowPen.setStyle(Qt::SolidLine);
		painter->setPen(arrowPen);

		// Set brush
		switch (mStyle)
		{
		case TriangleFilled:
		case ConcaveFilled:
		case CircleFilled:
			painter->setBrush(arrowPen.brush());
			break;
		case Triangle:
		case Concave:
		case Circle:
			painter->setBrush(backgroundBrush);
			break;
		default:
			painter->setBrush(Qt::transparent);
			break;
		}

		// Draw arrow
		switch (mStyle)
		{
		case Normal:
			painter->drawLine(mPolygon[0], mPolygon[1]);
			painter->drawLine(mPolygon[0], mPolygon[2]);
			break;
		case Circle:
		case CircleFilled:
			painter->drawEllipse(mPosition, mSize / 2, mSize / 2);
			break;
		default:
			painter->drawPolygon(mPolygon);
			break;
		}

		painter->setPen(originalPen);
		painter->setBrush(originalBrush);
	}
}

//==================================================================================================

void DrawingArrow::updateGeometry()
{
	mShape = QPainterPath();
	mPolygon.clear();

	if (mStyle != None && mSize <= mLength)
	{
		// Calculate polygon points
		const qreal sqrt2 = qSqrt(2);
		qreal direction = mAngle * 3.141592654 / 180;
		qreal angle = 0;
		QPainterPath drawPath;

		switch (mStyle)
		{
		case Normal:
		case Triangle:
		case TriangleFilled:
			angle = 3.141592654 / 6;

			mPolygon.append(mPosition);
			mPolygon.append(QPointF(mPosition.x() + mSize / sqrt2 * qCos(direction - angle),
									mPosition.y() + mSize / sqrt2 * qSin(direction - angle)));
			mPolygon.append(QPointF(mPosition.x() + mSize / sqrt2 * qCos(direction + angle),
									mPosition.y() + mSize / sqrt2 * qSin(direction + angle)));
			break;
		case Concave:
		case ConcaveFilled:
			angle = 3.141592654 / 6;

			mPolygon.append(mPosition);
			mPolygon.append(QPointF(mPosition.x() + mSize / sqrt2 * qCos(direction - angle),
									mPosition.y() + mSize / sqrt2 * qSin(direction - angle)));
			mPolygon.append(QPointF(mPosition.x() + mSize / sqrt2 / 2 * qCos(direction),
									mPosition.y() + mSize / sqrt2 / 2 * qSin(direction)));
			mPolygon.append(QPointF(mPosition.x() + mSize / sqrt2 * qCos(direction + angle),
									mPosition.y() + mSize / sqrt2 * qSin(direction + angle)));
			break;
		default:
			break;
		}

		// Create shape
		switch (mStyle)
		{
		case Circle:
		case CircleFilled:
			mShape.addEllipse(mPosition, (mSize + mPenWidth) / 2, (mSize + mPenWidth) / 2);
			break;
		default:
			drawPath.addPolygon(mPolygon);
			drawPath.closeSubpath();

			mShape = strokePath(drawPath);
			mShape.addPolygon(mPolygon);
			break;
		}
	}
}
QPainterPath DrawingArrow::strokePath(const QPainterPath& path) const
{
	if (path == QPainterPath()) return path;

	QPainterPathStroker ps;
	const qreal penWidthZero = qreal(0.00000001);

	if (mPenWidth <= 0.0)
		ps.setWidth(penWidthZero);
	else
		ps.setWidth(mPenWidth);

	ps.setCapStyle(Qt::SquareCap);
	ps.setJoinStyle(Qt::BevelJoin);

	return ps.createStroke(path);
}

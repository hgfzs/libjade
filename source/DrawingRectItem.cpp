/* DrawingRectItem.cpp
 *
 * Copyright (C) 2013-2020 Jason Allen
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

#include "DrawingRectItem.h"
#include "DrawingItemPoint.h"

DrawingRectItem::DrawingRectItem() : DrawingItem()
{
	mRect = QRectF(0, 0, 0, 0);
	mCornerRadius = 0;
	mPen = QPen(Qt::black, 16, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	mBrush = Qt::white;

	setFlags(CanMove | CanResize | CanRotate | CanFlip | CanSelect | CanDelete);

	DrawingItemPoint::Flags flags = (DrawingItemPoint::Control | DrawingItemPoint::Connection);
	for(int i = 0; i < 8; i++) addPoint(new DrawingItemPoint(QPointF(0, 0), flags));

	updateGeometry();
}

DrawingRectItem::DrawingRectItem(const DrawingRectItem& item) : DrawingItem(item)
{
	mRect = item.mRect;
	mCornerRadius = item.mCornerRadius;
	mPen = item.mPen;
	mBrush = item.mBrush;

	mBoundingRect = item.mBoundingRect;
	mShape = item.mShape;
}

DrawingRectItem::~DrawingRectItem() { }

//==================================================================================================

DrawingItem* DrawingRectItem::copy() const
{
	return new DrawingRectItem(*this);
}

//==================================================================================================

void DrawingRectItem::setRect(const QRectF& rect)
{
	mRect = rect;

	// Update points
	QList<DrawingItemPoint*> points = DrawingRectItem::points();
	if (points.size() >= 8)
	{
		points[TopLeft]->setPosition(rect.left(), rect.top());
		points[TopMiddle]->setPosition(rect.center().x(), rect.top());
		points[TopRight]->setPosition(rect.right(), rect.top());
		points[MiddleRight]->setPosition(rect.right(), rect.center().y());
		points[BottomRight]->setPosition(rect.right(), rect.bottom());
		points[BottomMiddle]->setPosition(rect.center().x(), rect.bottom());
		points[BottomLeft]->setPosition(rect.left(), rect.bottom());
		points[MiddleLeft]->setPosition(rect.left(), rect.center().y());
	}

	updateGeometry();
}

void DrawingRectItem::setRect(qreal left, qreal top, qreal width, qreal height)
{
	setRect(QRectF(left, top, width, height));
}

QRectF DrawingRectItem::rect() const
{
	return mRect;
}

//==================================================================================================

void DrawingRectItem::setCornerRadius(qreal radius)
{
	mCornerRadius = radius;
	updateGeometry();
}

qreal DrawingRectItem::cornerRadius() const
{
	return mCornerRadius;
}

//==================================================================================================

void DrawingRectItem::setPen(const QPen& pen)
{
	mPen = pen;
	updateGeometry();
}

QPen DrawingRectItem::pen() const
{
	return mPen;
}

//==================================================================================================

void DrawingRectItem::setBrush(const QBrush& brush)
{
	mBrush = brush;
	updateGeometry();
}

QBrush DrawingRectItem::brush() const
{
	return mBrush;
}

//==================================================================================================

QRectF DrawingRectItem::boundingRect() const
{
	return mBoundingRect;
}

QPainterPath DrawingRectItem::shape() const
{
	return mShape;
}

bool DrawingRectItem::isValid() const
{
	return (mRect.width() != 0 && mRect.height() != 0);
}

//==================================================================================================

void DrawingRectItem::render(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		// Draw rect
		painter->setBrush(mBrush);
		painter->setPen(mPen);
		painter->drawRoundedRect(mRect, mCornerRadius, mCornerRadius);

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
	}
}

//==================================================================================================

void DrawingRectItem::resize(DrawingItemPoint* point, const QPointF& pos)
{
	DrawingItem::resize(point, pos);

	QList<DrawingItemPoint*> points = DrawingRectItem::points();

	if (points.size() >= 8)
	{
		int pointIndex = points.indexOf(point);

		if (0 <= pointIndex && pointIndex < 8)
		{
			QRectF rect;

			rect.setTopLeft(points[0]->position());
			rect.setBottomRight(points[1]->position());

			switch (pointIndex)
			{
			case TopLeft: rect.setTopLeft(point->position()); break;
			case TopMiddle:	rect.setTop(point->y()); break;
			case TopRight: rect.setTopRight(point->position()); break;
			case MiddleRight: rect.setRight(point->x()); break;
			case BottomRight: rect.setBottomRight(point->position()); break;
			case BottomMiddle: rect.setBottom(point->y()); break;
			case BottomLeft: rect.setBottomLeft(point->position()); break;
			case MiddleLeft: rect.setLeft(point->x()); break;
			default: break;
			}

			setRect(rect);
		}
	}
}

//==================================================================================================

void DrawingRectItem::updateGeometry()
{
	mBoundingRect = QRectF();
	mShape = QPainterPath();

	if (isValid())
	{
		qreal halfPenWidth = mPen.widthF() / 2;
		QRectF normalizedRect = mRect.normalized();
		QPainterPath drawPath;

		// Bounding rect
		mBoundingRect = normalizedRect;
		mBoundingRect.adjust(-halfPenWidth, -halfPenWidth, halfPenWidth, halfPenWidth);

		// Shape
		drawPath.addRoundedRect(normalizedRect, mCornerRadius, mCornerRadius);

		mShape = strokePath(drawPath, mPen);
		if (mBrush.color().alpha() > 0) mShape.addPath(drawPath);
	}
}

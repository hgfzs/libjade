/* DrawingRectItem.cpp
 *
 * Copyright (C) 2013-2017 Jason Allen
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

#include "DrawingRectItem.h"
#include "DrawingItemPoint.h"
#include "DrawingItemStyle.h"

DrawingRectItem::DrawingRectItem() : DrawingItem()
{
	mCornerRadiusX = 0;
	mCornerRadiusY = 0;

	setFlags(CanMove | CanResize | CanRotate | CanFlip | CanSelect | CanDelete | PlaceByMousePressAndRelease | AdjustPositionOnResize);

	DrawingItemPoint::Flags flags = (DrawingItemPoint::Control | DrawingItemPoint::Connection);
	for(int i = 0; i < 8; i++) addPoint(new DrawingItemPoint(QPointF(0, 0), flags));

	DrawingItemStyle* style = DrawingItem::style();
	style->setValue(DrawingItemStyle::PenStyle,
		style->valueLookup(DrawingItemStyle::PenStyle, QVariant((uint)Qt::SolidLine)));
	style->setValue(DrawingItemStyle::PenColor,
		style->valueLookup(DrawingItemStyle::PenColor, QVariant(QColor(0, 0, 0))));
	style->setValue(DrawingItemStyle::PenOpacity,
		style->valueLookup(DrawingItemStyle::PenOpacity, QVariant(1.0)));
	style->setValue(DrawingItemStyle::PenWidth,
		style->valueLookup(DrawingItemStyle::PenWidth, QVariant(12.0)));
	style->setValue(DrawingItemStyle::PenCapStyle,
		style->valueLookup(DrawingItemStyle::PenCapStyle, QVariant((uint)Qt::RoundCap)));
	style->setValue(DrawingItemStyle::PenJoinStyle,
		style->valueLookup(DrawingItemStyle::PenJoinStyle, QVariant((uint)Qt::RoundJoin)));

	style->setValue(DrawingItemStyle::BrushStyle,
		style->valueLookup(DrawingItemStyle::BrushStyle, QVariant((uint)Qt::SolidPattern)));
	style->setValue(DrawingItemStyle::BrushColor,
		style->valueLookup(DrawingItemStyle::BrushColor, QVariant(QColor(255, 255, 255))));
	style->setValue(DrawingItemStyle::BrushOpacity,
		style->valueLookup(DrawingItemStyle::BrushOpacity, QVariant(1.0)));
}

DrawingRectItem::DrawingRectItem(const DrawingRectItem& item) : DrawingItem(item)
{
	mCornerRadiusX = item.mCornerRadiusX;
	mCornerRadiusY = item.mCornerRadiusY;
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
	QList<DrawingItemPoint*> points = DrawingRectItem::points();
	points[TopLeft]->setPosition(rect.left(), rect.top());
	points[TopMiddle]->setPosition(rect.center().x(), rect.top());
	points[TopRight]->setPosition(rect.right(), rect.top());
	points[MiddleRight]->setPosition(rect.right(), rect.center().y());
	points[BottomRight]->setPosition(rect.right(), rect.bottom());
	points[BottomMiddle]->setPosition(rect.center().x(), rect.bottom());
	points[BottomLeft]->setPosition(rect.left(), rect.bottom());
	points[MiddleLeft]->setPosition(rect.left(), rect.center().y());
}

void DrawingRectItem::setRect(qreal left, qreal top, qreal width, qreal height)
{
	setRect(QRectF(left, top, width, height));
}

QRectF DrawingRectItem::rect() const
{
	QList<DrawingItemPoint*> points = DrawingRectItem::points();
	return (points.size() >= 8) ? QRectF(points[TopLeft]->position(), points[BottomRight]->position()) : QRectF();
}

//==================================================================================================

void DrawingRectItem::setCornerRadii(qreal radiusX, qreal radiusY)
{
	mCornerRadiusX = radiusX;
	mCornerRadiusY = radiusY;
}

qreal DrawingRectItem::cornerRadiusX() const
{
	return mCornerRadiusX;
}

qreal DrawingRectItem::cornerRadiusY() const
{
	return mCornerRadiusY;
}

//==================================================================================================

QRectF DrawingRectItem::boundingRect() const
{
	QRectF rect;

	if (isValid())
	{
		qreal penWidth = style()->valueLookup(DrawingItemStyle::PenWidth).toReal();

		rect = DrawingRectItem::rect().normalized();
		rect.adjust(-penWidth/2, -penWidth/2, penWidth/2, penWidth/2);
	}

	return rect;
}

QPainterPath DrawingRectItem::shape() const
{
	QPainterPath shape;

	if (isValid())
	{
		QPainterPath drawPath;

		DrawingItemStyle* style = DrawingItem::style();
		QPen pen = style->pen();
		QBrush brush = style->brush();

		// Add rect
		drawPath.addRoundedRect(rect().normalized(), mCornerRadiusX, mCornerRadiusY);

		// Determine outline path
		shape = strokePath(drawPath, pen);

		if (brush.color().alpha() > 0) shape.addPath(drawPath);
	}

	return shape;
}

bool DrawingRectItem::isValid() const
{
	QList<DrawingItemPoint*> points = DrawingRectItem::points();
	return (points.size() >= 8 && points[TopLeft]->position() != points[BottomRight]->position());
}

//==================================================================================================

void DrawingRectItem::render(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		DrawingItemStyle* style = DrawingItem::style();
		QPen pen = style->pen();
		QBrush brush = style->brush();

		// Draw rect
		painter->setBrush(brush);
		painter->setPen(pen);
		painter->drawRoundedRect(rect(), mCornerRadiusX, mCornerRadiusY);

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
	}
}

//==================================================================================================

void DrawingRectItem::resizeEvent(DrawingItemPoint* itemPoint, const QPointF& parentPos)
{
	DrawingItem::resizeEvent(itemPoint, parentPos);

	QList<DrawingItemPoint*> points = DrawingRectItem::points();
	if (points.size() >= 8)
	{
		QRectF rect = DrawingRectItem::rect();
		int pointIndex = points.indexOf(itemPoint);

		if (0 <= pointIndex && pointIndex < 8)
		{
			switch (pointIndex)
			{
			case TopLeft: rect.setTopLeft(itemPoint->position()); break;
			case TopMiddle:	rect.setTop(itemPoint->y()); break;
			case TopRight: rect.setTopRight(itemPoint->position()); break;
			case MiddleRight: rect.setRight(itemPoint->x()); break;
			case BottomRight: rect.setBottomRight(itemPoint->position()); break;
			case BottomMiddle: rect.setBottom(itemPoint->y()); break;
			case BottomLeft: rect.setBottomLeft(itemPoint->position()); break;
			case MiddleLeft: rect.setLeft(itemPoint->x()); break;
			default: break;
			}

			points[TopLeft]->setPosition(rect.left(), rect.top());
			points[TopMiddle]->setPosition(rect.center().x(), rect.top());
			points[TopRight]->setPosition(rect.right(), rect.top());
			points[MiddleRight]->setPosition(rect.right(), rect.center().y());
			points[BottomRight]->setPosition(rect.right(), rect.bottom());
			points[BottomMiddle]->setPosition(rect.center().x(), rect.bottom());
			points[BottomLeft]->setPosition(rect.left(), rect.bottom());
			points[MiddleLeft]->setPosition(rect.left(), rect.center().y());
		}
	}
}

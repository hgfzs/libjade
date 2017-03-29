/* DrawingEllipseItem.cpp
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

#include "DrawingEllipseItem.h"
#include "DrawingItemPoint.h"
#include "DrawingItemStyle.h"

DrawingEllipseItem::DrawingEllipseItem() : DrawingItem()
{
	setFlags(CanMove | CanResize | CanRotate | CanFlip | CanSelect | PlaceByMousePressAndRelease | AdjustPositionOnResize);

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

DrawingEllipseItem::DrawingEllipseItem(const DrawingEllipseItem& item) : DrawingItem(item) { }

DrawingEllipseItem::~DrawingEllipseItem() { }

//==================================================================================================

DrawingItem* DrawingEllipseItem::copy() const
{
	return new DrawingEllipseItem(*this);
}

//==================================================================================================

void DrawingEllipseItem::setEllipse(const QRectF& rect)
{
	QList<DrawingItemPoint*> points = DrawingEllipseItem::points();
	points[TopLeft]->setPosition(rect.left(), rect.top());
	points[TopMiddle]->setPosition(rect.center().x(), rect.top());
	points[TopRight]->setPosition(rect.right(), rect.top());
	points[MiddleRight]->setPosition(rect.right(), rect.center().y());
	points[BottomRight]->setPosition(rect.right(), rect.bottom());
	points[BottomMiddle]->setPosition(rect.center().x(), rect.bottom());
	points[BottomLeft]->setPosition(rect.left(), rect.bottom());
	points[MiddleLeft]->setPosition(rect.left(), rect.center().y());
}

void DrawingEllipseItem::setEllipse(qreal left, qreal top, qreal width, qreal height)
{
	setEllipse(QRectF(left, top, width, height));
}

QRectF DrawingEllipseItem::ellipse() const
{
	QList<DrawingItemPoint*> points = DrawingEllipseItem::points();
	return (points.size() >= 8) ? QRectF(points[TopLeft]->position(), points[BottomRight]->position()) : QRectF();
}

//==================================================================================================

QRectF DrawingEllipseItem::boundingRect() const
{
	QRectF rect;

	if (isValid())
	{
		qreal penWidth = style()->valueLookup(DrawingItemStyle::PenWidth).toReal();

		rect = DrawingEllipseItem::ellipse().normalized();
		rect.adjust(-penWidth/2, -penWidth/2, penWidth/2, penWidth/2);
	}

	return rect;
}

QPainterPath DrawingEllipseItem::shape() const
{
	QPainterPath shape;

	if (isValid())
	{
		QPainterPath drawPath;

		DrawingItemStyle* style = DrawingItem::style();
		QPen pen = style->pen();
		QBrush brush = style->brush();

		// Add ellipse
		drawPath.addEllipse(ellipse().normalized());

		// Determine outline path
		shape = strokePath(drawPath, pen);

		if (brush.color().alpha() > 0) shape.addPath(drawPath);
	}

	return shape;
}

bool DrawingEllipseItem::isValid() const
{
	QList<DrawingItemPoint*> points = DrawingEllipseItem::points();
	return (points.size() >= 8 && points[TopLeft]->position() != points[BottomRight]->position());
}

//==================================================================================================

void DrawingEllipseItem::render(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		DrawingItemStyle* style = DrawingItem::style();
		QPen pen = style->pen();
		QBrush brush = style->brush();

		// Draw ellipse
		painter->setBrush(brush);
		painter->setPen(pen);
		painter->drawEllipse(ellipse());

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
	}
}

//==================================================================================================

void DrawingEllipseItem::resizeEvent(DrawingItemPoint* itemPoint, const QPointF& scenePos)
{
	DrawingItem::resizeEvent(itemPoint, scenePos);

	QList<DrawingItemPoint*> points = DrawingEllipseItem::points();
	if (points.size() >= 8)
	{
		QRectF rect = DrawingEllipseItem::ellipse();
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

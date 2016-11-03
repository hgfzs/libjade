/* DrawingItemStyle.h
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

#ifndef DRAWINGITEMSTYLE_H
#define DRAWINGITEMSTYLE_H

#include <DrawingTypes.h>

enum DrawingArrowStyle { ArrowNone, ArrowNormal, ArrowReverse, ArrowTriangle, ArrowTriangleFilled,
	ArrowConcave, ArrowConcaveFilled, ArrowCircle, ArrowCircleFilled,
	ArrowDiamond, ArrowDiamondFilled, ArrowHarpoon, ArrowHarpoonMirrored, ArrowX };

class DrawingItemStyle
{
private:
	QHash<QString,QVariant> mProperties;

public:
	DrawingItemStyle();
	DrawingItemStyle(const DrawingItemStyle& style);
	~DrawingItemStyle();

	void addProperty(const QString& name, const QVariant& value);
	void removeProperty(const QString& name);
	void clearProperties();
	bool hasProperty(const QString& name) const;
	QVariant propertyValue(const QString& name) const;

	QVariant propertyLookup(const QString& name) const;

	// Convenience lookup functions
	QPen pen() const;
	QBrush brush() const;
	QFont font() const;
	QBrush textBrush() const;
	Qt::Alignment textAlignment() const;

	DrawingArrowStyle startArrowStyle() const;
	qreal startArrowSize() const;
	DrawingArrowStyle endArrowStyle() const;
	qreal endArrowSize() const;


private:
	static QHash<QString,QVariant> mProperties;
};

#endif

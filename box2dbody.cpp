/*
 * Box2D QML plugin
 * Copyright (C) 2010 Nokia Corporation
 *
 * This file is part of the QmlArcade.
 *
 * QmlArcade is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * QmlArcade is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with QmlArcade.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "box2dbody.h"

#include "box2dworld.h"

#include <Box2D.h>
#include <cmath>

static const float scaleRatio = 32.0f; // 32 pixels in one meter

Box2DBody::Box2DBody(QDeclarativeItem *parent) :
    QDeclarativeItem(parent),
    mBody(0),
    mFixed(false)
{
}

Box2DBody::~Box2DBody()
{
    if (Box2DWorld *world = qobject_cast<Box2DWorld*>(parent()))
        world->unregisterBody(this);
}

void Box2DBody::componentComplete()
{
    if (Box2DWorld *world = qobject_cast<Box2DWorld*>(parent()))
        world->registerBody(this);
    else
        qWarning() << "Warning: Body should have World as parent";
}

void Box2DBody::initialize(b2World *world)
{
    b2BodyDef bodyDef;
    bodyDef.position.Set((x() + width() / 2) / scaleRatio,
                         -(y() + height() / 2) / scaleRatio);
    bodyDef.angle = -(rotation() * (2 * M_PI)) / 360.0;

    mBody = world->CreateBody(&bodyDef);

    // TODO: Create a shape for each child shape
    b2PolygonDef shapeDef;
    shapeDef.SetAsBox(width() / 2.0f / scaleRatio,
                      height() / 2.0f / scaleRatio);
    shapeDef.density = 1.0f;    // TODO: Make a property for it in Box2DShape
    shapeDef.friction = 0.3f;   // TODO: Make a property for it in Box2DShape
    shapeDef.restitution = 0.5f;
    mBody->CreateShape(&shapeDef);

    if (!mFixed)
        mBody->SetMassFromShapes(); // TODO: Make a property for it in Box2DBody
}

/**
 * Synchronizes the state of this body with the internal Box2D state.
 */
void Box2DBody::synchronize()
{
    Q_ASSERT(mBody);
    const b2Vec2 position = mBody->GetPosition();
    const float32 angle = mBody->GetAngle();
    setPos(position.x * scaleRatio - width() / 2,
           -position.y * scaleRatio - height() / 2);
    setRotation(-(angle * 360.0) / (2 * M_PI));
}

void Box2DBody::cleanup(b2World *world)
{
    world->DestroyBody(mBody);
}

#ifndef HITBOX_H
#define HITBOX_H

/*
  Copyright (c) 1997-2002 MathEngine PLC

  $Name: t-stevet-RWSpre-030110 $

  Date: $Date: 2002/04/04 15:29:12 $ - Revision: $Revision: 1.4.2.1 $

  This software and its accompanying manuals have been developed
  by MathEngine PLC ("MathEngine") and the copyright and all other
  intellectual property rights in them belong to MathEngine. All
  rights conferred by law (including rights under international
  copyright conventions) are reserved to MathEngine. This software
  may also incorporate information which is confidential to
  MathEngine.

  Save to the extent permitted by law, or as otherwise expressly
  permitted by MathEngine, this software and the manuals must not
  be copied (in whole or in part), re-arranged, altered or adapted
  in any way without the prior written consent of the Company. In
  addition, the information contained in the software may not be
  disseminated without the prior written consent of MathEngine.
*/

enum axises     { axisX, axisY, axisZ, axisesCount};

enum tmRows     { tmRotation = 0, tmTranslation = 3 };

enum sides
{
    sideFront, sideLeft, sideBack, sideRight,
    sideBase, sideTop,
    sidesCount
};

enum shapeKinds
{
    shapeDynamic, shapeFixed, shapeInvisible, shapeVisual,
    shapeKindsCount
};

enum shapeTypes
{
    shapeBox, shapeBall, shapePipe,
    shapeConvexBox, shapeConvexCone, shapeConvexSlice,
    shapeSide, shapeSideOL, shapeGround, shapeAxis,
    shapeTypesCount
};

struct shapeType
{
    enum shapeKinds     k;
    enum shapeTypes     t;

    unsigned            count;

    struct shapeAny     *v;
};

/*
    Oh to have flavours...
*/

struct shapeDynamic
{
    McdGeometryID       g;
    McdModelID          m;
    MdtBodyID           b;

    RGraphic            **r;
    unsigned            rCount;
};

struct shapeFixed
{
    McdGeometryID       g;
    McdModelID          m;

    RGraphic            **r;
    unsigned            rCount;
};

struct shapeInvisible
{
    McdGeometryID       g;
    McdModelID          m;
};

struct shapeVisual
{
    RGraphic            **r;
    unsigned            rCount;
};

struct shapeAny
{
    enum shapeKinds     k;
    enum shapeTypes     t;

    union
    {
        struct shapeDynamic     d;
        struct shapeFixed       f;
        struct shapeInvisible   i;
        struct shapeVisual      v;
    }
                        u;
};
#endif

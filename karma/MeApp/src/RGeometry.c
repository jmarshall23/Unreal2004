#include <RGeometry.h>
#include <RConvex.h>
#include <McdSphyl.h>

/**
 * Create an RGraphic from an McdGeometry.
 */
RGraphic *MEAPI 
RGraphicCreateFromGeometry(RRender *rc, McdGeometryID geom, float color[4])
{
    RGraphic *g = 0;
    int type = McdGeometryGetTypeId(geom);

    switch(type)
    {
    case kMcdGeometryTypeSphere:
        g = RGraphicSphereCreate(rc,McdSphereGetRadius(geom), color,0);
        break;
        
    case kMcdGeometryTypeBox:
        {
            MeReal width,height,depth;
            McdBoxGetDimensions(geom,&width,&height,&depth);
            g = RGraphicBoxCreate(rc,width,height,depth,color,0);
        }
        break;
        
    case kMcdGeometryTypeCylinder:
        g = RGraphicCylinderCreate(rc,McdCylinderGetRadius(geom),
            McdCylinderGetHeight(geom),color,0);
        break;
        
    case kMcdGeometryTypePlane:
        g = RGraphicSquareCreate(rc,24,color,0);
        break;

    case kMcdGeometryTypeConvexMesh:
        g = RGraphicConvexCreate(rc,(McdConvexMeshID)geom,color,0);
        break;

    case kMcdGeometryTypeAggregate:
        g = RGraphicAggregateCreate(rc,(McdAggregateID)geom,color,0);
        break;

    case kMcdGeometryTypeSphyl:
        g = RGraphicSphylCreate(rc, McdSphylGetRadius(geom), McdSphylGetHeight(geom), color,0);
        break;
    }
    return g;
}

/**
 * Create an RGraphic from an McdGeometry of type aggregate.
 */
RGraphic *MEAPI 
RGraphicAggregateCreate(RRender *rc, McdAggregateID ag, float color[4], MeMatrix4Ptr tm)
{
    McdGeometryID elemGeom;
    MeMatrix4Ptr elemTM;
    RGraphic *elemGraphic = 0;
    RGraphic *graphic = 0;
    int i;

    for (i = 0; i < McdAggregateGetElementCount(ag); i++)
    {
        elemGeom = McdAggregateGetElementGeometry(ag,i);

        if (elemGeom)
        {
            elemTM = McdAggregateGetElementTransformPtr(ag,i);
            elemGraphic = RGraphicCreateFromGeometry(rc,elemGeom,color);

            /*  If we already have a graphic, add these vertices to it 
                (with a relative transform) */
            if (graphic)
            {
           
                RGraphic *tempG = RGraphicCombine(rc,graphic,elemGraphic,elemTM);
                RGraphicDelete(rc,graphic,0);
                graphic = tempG;
                RGraphicDelete(rc,elemGraphic,0);
            }
            /*  Otherwise, use this as the graphic 
                (but still take account of elemTM) */
            else
            {
                int  j;
                MeVector4 pretrans, posttrans;

                RObjectVertex* vtx = elemGraphic->m_pVertices;

                for(j=0; j<elemGraphic->m_pObject->m_nNumVertices; j++)
                {
                    /* x,y,z */
                    pretrans[0] = vtx->m_X;
                    pretrans[1] = vtx->m_Y;
                    pretrans[2] = vtx->m_Z;
                    pretrans[3] = 1;
                    MeMatrix4MultiplyVector(posttrans, elemTM, pretrans);
                    vtx->m_X = posttrans[0];
                    vtx->m_Y = posttrans[1];
                    vtx->m_Z = posttrans[2];
                
                    /* normals */
                    pretrans[0] = vtx->m_NX;
                    pretrans[1] = vtx->m_NY;
                    pretrans[2] = vtx->m_NZ;
                    pretrans[3] = 0;
                    MeMatrix4MultiplyVector(posttrans, elemTM, pretrans);
                    vtx->m_NX = posttrans[0];
                    vtx->m_NY = posttrans[1];
                    vtx->m_NZ = posttrans[2];
                
                    vtx++;
                }
            
                graphic = elemGraphic;
            }
        }
    }
    RGraphicSetTransformPtr(graphic,tm);
    return graphic;
}

/**
 * Given an McdModel, create an RGraphic from its geometry.
 */
RGraphic *MEAPI 
RGraphicCreateFromModel(RRender *rc, McdModelID m, float color[4])
{
    RGraphic *g = 0;
    McdGeometryID geom = McdModelGetGeometry(m);

    g = RGraphicCreateFromGeometry(rc,geom,color);

    /* null geometry will have no graphic */
    if (g)
        RGraphicSetTransformPtr(g,McdModelGetTransformPtr(m));

    return g;
}

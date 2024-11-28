/****************************************************************************
  This is two functions, one to read a XML test case file and the other
  is to create a model/body/graphic.

*/

#include <RConvex.h>

#include "ReadTestXML.h"

/****************************************************************************
  global data
*/

int number_of_tests = 0;
int currentModel = 0;
int normalIndex = 0;
int posIndex = 0;

Props properties[32] = {0};
Result result[32] = {0};

/****************************************************************************
  XML handlers
*/
static MeXMLError MEAPI Handle_ContactPosition(MeXMLElement *elem, void *result, void *userdata)
{
    MeXMLError error = MeXMLErrorNone;
    
    Result *res = (Result*)result;

    /* initialise each flag for each position */
    res->flag[posIndex] = 0;
    MeVector3Copy(res->position[posIndex++], res->tempPos);

    return error;
}

static MeXMLError MEAPI Handle_ContactNormal(MeXMLElement *elem, void *result, void *userdata)
{
    MeXMLError error = MeXMLErrorNone;

    Result *res = (Result*)result;
    MeVector3Copy(res->normal[normalIndex++], res->tempNormal);

    return error;
}

static MeXMLError MEAPI Handle_ExpectedResults(MeXMLElement *elem, PElement *parent)
{
    MeXMLError error;
    Result res = {0};
    MeXMLHandler handlers[] = 
    {
        ME_XML_INT_HANDLER("TOUCH", Result, touch, 0),
        ME_XML_INT_HANDLER("CONTACTS", Result, contacts, 0),
		ME_XML_FLOAT_HANDLER("PENETRATION", Result, penetration, 0),
		ME_XML_FLOAT_ARRAY_HANDLER("C_POSITION", Result, tempPos, 3, Handle_ContactPosition),
		ME_XML_FLOAT_ARRAY_HANDLER("C_NORMAL", Result, tempNormal, 3, Handle_ContactNormal),
        ME_XML_HANDLER_END
    };

    error = MeXMLElementProcess(elem, handlers, &result, 0);

    result[number_of_tests]= res;
    
    return error;
}

static MeXMLError MEAPI Handle_Body(MeXMLElement *elem, PElement *parent)
{
    MeXMLError error;
    Props props = {0};
    MeXMLHandler handlers[] = 
    {
        ME_XML_INT_HANDLER("TYPE", Props, type, 0),
        ME_XML_MEREAL_HANDLER("SCALE", Props, scale, 0),
        ME_XML_MEREAL_HANDLER("FATNESS", Props, fatness, 0),
		ME_XML_FLOAT_ARRAY_HANDLER("POSITION", Props, position, 3, 0),
		ME_XML_FLOAT_ARRAY_HANDLER("COLOUR", Props, color, 4, 0),
		ME_XML_FLOAT_ARRAY_HANDLER("QUATERNION", Props, quaternion, 4, 0),
        ME_XML_HANDLER_END
    };

    error = MeXMLElementProcess(elem, handlers, &props, 0);
    
    properties[currentModel++] = props;
    return error;
}

static MeXMLError MEAPI Handle_TestInputData(MeXMLElement *elem, PElement *parent)
{
    MeXMLHandler handlers[] = 
    {
        ME_XML_ELEMENT_HANDLER("BODY", Handle_Body),
        ME_XML_ELEMENT_HANDLER("EXPECTED_RESULTS", Handle_ExpectedResults),
        ME_XML_HANDLER_END
    };
    number_of_tests += 1;
    return MeXMLElementProcess(elem, handlers, 0, 0);
}

static MeXMLError MEAPI Handle_KarmaTest(MeXMLElement *elem, PElement *parent)
{
    MeXMLHandler handlers[] = 
    {
        ME_XML_ELEMENT_HANDLER("TEST_INPUT_DATA", Handle_TestInputData),
        ME_XML_HANDLER_END
    };
    return MeXMLElementProcess(elem, handlers, 0, 0);
}

MeBool MEAPI XMLInputRead(MeStream stream)
{
    MeXMLInput *input;
    MeXMLError error;

    MeXMLHandler handlers[] = 
    {
        ME_XML_ELEMENT_HANDLER("KARMA_TEST", Handle_KarmaTest),
        ME_XML_HANDLER_END
    };

    if (!stream) 
    {
        ME_REPORT(MeWarning(3, "Invalid stream."));
        return 0;
    }

    input = MeXMLInputCreate(stream);

    error = MeXMLInputProcess(input, handlers, 0);

    if (error != MeXMLErrorNone)
    {
        MeWarning(3,"Parse Error in file %s.\n%s", stream->filename, MeXMLInputGetErrorString(input));
        MeXMLInputDestroy(input);
        return 0;
    }

    MeXMLInputDestroy(input);

    return 1;
}

/****************************************************************************
  This is all the stuff used to create convex shapes
*/
/* define dimensions */
MeReal rcyl  = (MeReal)2;
MeReal hcyl  = (MeReal)4;
MeReal dimbox[3] = {5,5,5};
MeReal sphereRadius = (MeReal)1;

/* parameters used in constructing convex objects */
#define ROOT2           (1.414213562f)

#define _R1             (0.5f)
#define _R2             (0.35f)
#define _R2a            (_R2*1.35f)

#define _L2             (_R2*(ROOT2+1.0f))
#define _L2a            (_L2*1.35f )

#define _O              (_R2*0.5f)

#define convexBoxVERTICES       12
#define convexConeVERTICES      17
#define convexSliceVERTICES     16

static MeReal convexBox[convexBoxVERTICES][3] =
{
    {   _R1,            _R1*2,          _R1     },
    {   -_R1,           _R1*2,          _R1     },
    {   -_R1,           _R1*2,          -_R1    },
    {   _R1,            _R1*2,          -_R1    },
    {   _R1*2,          0,              _R1*2   },
    {   -_R1*2,         0,              _R1*2   },
    {   -_R1*2,         0,              -_R1*2  },
    {   _R1*2,          0,              -_R1*2  },
    {   _R1,            -_R1*2,         _R1     },
    {   -_R1,           -_R1*2,         _R1     },
    {   -_R1,           -_R1*2,         -_R1    },
    {   _R1,            -_R1*2,         -_R1    }
};

static MeReal convexCone[convexConeVERTICES][3] =
{
    {   _L2,            _O+_R2,         _R2     },
    {   _R2,            _O+_R2,         _L2     },
    {   -_R2,           _O+_R2,         _L2     },
    {   -_L2,           _O+_R2,         _R2     },
    {   -_L2,           _O+_R2,         -_R2    },
    {   -_R2,           _O+_R2,         -_L2    },
    {   _R2,            _O+_R2,         -_L2    },
    {   _L2,            _O+_R2,         -_R2    },
    {   _L2a,           _O+0,           _R2a    },
    {   _R2a,           _O+0,           _L2a    },
    {   -_R2a,          _O+0,           _L2a    },
    {   -_L2a,          _O+0,           _R2a    },
    {   -_L2a,          _O+0,           -_R2a   },
    {   -_R2a,          _O+0,           -_L2a   },
    {   _R2a,           _O+0,           -_L2a   },
    {   _L2a,           _O+0,           -_R2a   },
    {   0,              _O+-_R2*4,      0       }
};

static MeReal convexSlice[convexSliceVERTICES][3] =
{
    {   _L2,            _R2,            _R2     },
    {   _R2,            _R2,            _L2     },
    {   -_R2,           _R2,            _L2     },
    {   -_L2,           _R2,            _R2     },
    {   -_L2,           _R2,            -_R2    },
    {   -_R2,           _R2,            -_L2    },
    {   _R2,            _R2,            -_L2    },
    {   _L2,            _R2,            -_R2    },
    {   _L2a,           -_R2,           _R2a    },
    {   _R2a,           -_R2,           _L2a    },
    {   -_R2a,          -_R2,           _L2a    },
    {   -_L2a,          -_R2,           _R2a    },
    {   -_L2a,          -_R2,           -_R2a   },
    {   -_R2a,          -_R2,           -_L2a   },
    {   _R2a,           -_R2,           -_L2a   },
    {   _L2a,           -_R2,           -_R2a   }
};

MeReal convexSliceScaled[convexSliceVERTICES][3];
MeReal convexConeScaled[convexConeVERTICES][3];
MeReal convexBoxScaled[convexBoxVERTICES][3];


/****************************************************************************
  This is a function Phil F wrote to create a model, body, and graphic 
  from a "props".
*/
McdModelID CreateProps(MstUniverseID universe,       
                       McdFrameworkID framework,
                       RRender *rc,                     
                       Props *props)                    
{
    int i, j;
    McdGeometryID g;
    McdModelID m;
    RGraphic *graphic;
    MdtBodyID b;    /* dynamics body */

    switch(props->type)
    {
    case 1:
        /* primitive sphere */
        g = (McdGeometryID) McdSphereCreate(framework, props->scale);
        m = MstModelAndBodyCreate(universe, g, 1.0);
        b = McdModelGetBody(m);
        graphic = RGraphicSphereCreate(rc, props->scale, props->color, 
                  MdtBodyGetTransformPtr(b));
        break;

    case 2:
        /* primitive box */
        g = (McdGeometryID) McdBoxCreate(framework, 
             props->scale, props->scale, props->scale);
        m = MstModelAndBodyCreate(universe, g, 1.0);
        b = McdModelGetBody(m);
        graphic = RGraphicBoxCreate(rc, props->scale, props->scale, props->scale, props->color, 
                  MdtBodyGetTransformPtr(b));
        break;

    case 3:
        /* primitive cylinder */
        g = (McdGeometryID) McdCylinderCreate(framework, props->scale, props->scale);
        m = MstModelAndBodyCreate(universe, g, 1.0);
        b = McdModelGetBody(m);
        graphic = RGraphicCylinderCreate(rc, props->scale, props->scale, props->color, 
                  MdtBodyGetTransformPtr(b));
        break;

    case 4:
        /* convex box */
        for (i=0; i<3; i++)
        {
            for (j=0; j<convexBoxVERTICES; j++)
            {
                convexBoxScaled[j][i] = props->scale*convexBox[j][i];
            }
        }
        g = (McdGeometryID) McdConvexMeshCreateHull(
            framework,convexBoxScaled,convexBoxVERTICES,props->fatness);
        m = MstModelAndBodyCreate(universe, g, 1.0);
        b = McdModelGetBody(m);
        graphic = RGraphicConvexCreate(rc,
        g, props->color,MdtBodyGetTransformPtr(b));
        break;   

    case 5:
        /* convex slice */
        for (i=0; i<3; i++)
        {
            for (j=0; j<convexSliceVERTICES; j++)
            {
                convexSliceScaled[j][i] = props->scale*convexSlice[j][i];
            }
        }
        g = (McdGeometryID) McdConvexMeshCreateHull(
            framework,convexSliceScaled,convexSliceVERTICES,props->fatness);
        m = MstModelAndBodyCreate(universe, g, 1.0);
        b = McdModelGetBody(m);
        graphic = RGraphicConvexCreate(rc,
        g, props->color,MdtBodyGetTransformPtr(b));
        break;  

    case 6:
        /* convex cone */
        for (i=0; i<3; i++)
        {
            for (j=0; j<convexConeVERTICES; j++)
            {
                convexConeScaled[j][i] = props->scale*convexCone[j][i];
            }
        }
        g = (McdGeometryID) McdConvexMeshCreateHull(
            framework,convexConeScaled,convexConeVERTICES,props->fatness);
        m = MstModelAndBodyCreate(universe, g, 1.0);
        b = McdModelGetBody(m);
        graphic =RGraphicConvexCreate(rc,
        g, props->color,MdtBodyGetTransformPtr(b));
        break;
    default:
        return 0;
    }

    /* set the graphic to be wireframe */
    RGraphicSetWireframe(graphic,1);
    /* set the position and the quarternion of the body */
    MdtBodySetPosition(b,props->position[0],props->position[1],props->position[2]);
    MdtBodySetQuaternion(b,props->quaternion[0],props->quaternion[1],props->quaternion[2],props->quaternion[3]);
    // TODO : add fatness for convex !!

    return m;
}


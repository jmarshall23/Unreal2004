
#include <MePrecision.h>
#include <MeMath.h>

#include <MstUniverse.h>
#include <MeViewer.h>

#include <MeXMLParser.h>
#include <MeXMLTree.h>

#define MAX_CONTACTS (50)

/* Object Properties */
typedef struct Props
{
    int type;
    MeReal scale;
    MeReal fatness;
    MeVector3 position;
    MeVector4 color;
    MeVector4 quaternion;
} Props;

typedef struct Result
{
    int touch;
    int contacts;
    float penetration;
    MeVector3 tempPos;
    MeVector3 position[MAX_CONTACTS];
    MeVector3 tempNormal;
    MeVector3 normal[MAX_CONTACTS];
    int flag[MAX_CONTACTS];

} Result;

extern int number_of_tests;
extern Props properties[32];
extern Result result[32];

/****************************************************************************
  These are the functions to load an XML test file
*/

MeBool MEAPI XMLInputRead(MeStream stream);

McdModelID CreateProps(MstUniverseID universe,       
                       McdFrameworkID framework,
                       RRender *rc,                     
                       Props *props);


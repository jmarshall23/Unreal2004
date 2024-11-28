
#include <QuickDemo.h>

//   MAIN FUNCTION

int main(int argc, char **argv)
{
    CreateUniverse(argc, argv, -30,"skydome","checkerboard");

    //  create legs and head of robot

    SetColor(white,"wood");

    BeginAggregate(3);

    CreateSphere(2);

    SetPosition(2,0,0);
    CreateSphere(1);

    SetColor(blue,0);
    SetPosition(0,0,2);
    CreateSphere(1);

    SetPosition(0,4,0);
    EndAggregate();

    MdtBodyID b = lastBody();
    MeVector3 pos = { 0, 0, 2.1f };
    MeVector3 po2 = { 0, 4, 2.1f };

//    MdtBodySetCenterOfMassRelativePosition(b, pos);

    MdtBodySetCenterOfMassPosition(b, po2);

    MdtBodyGetCenterOfMassPosition(b, pos);

    MdtBodyGetCenterOfMassRelativePosition(b, pos);

    RunApp();

    return 0;
}

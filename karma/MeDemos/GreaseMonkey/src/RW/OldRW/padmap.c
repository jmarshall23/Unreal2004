#include <rwcore.h>

#if defined(__KATANA__)
#if defined(RW_USE_SPF)
#include <mathf.h>
#else  /* RW_USE_SPF */
#include <math.h>
#endif /* RW_USE_SPF */
#endif /* __KATANA__ */

#if !(defined(__KATANA__))
#include <math.h>
#endif

#include "padmap.h"

/* map the dpad to keyboard cursor events */

void
PadCursor(RsEvent event, void *param)
{
    switch (event)
    {
        case rsPADBUTTONDOWN:
            {
                RsPadButtonStatus  *pb = (RsPadButtonStatus *) param;
                RsKeyStatus         ks;

                if (pb->padButtons & rsPADDPADLEFT)
                {
                    ks.keyCharCode = rsLEFT;
                    RsKeyboardEventHandler(rsKEYDOWN, &ks);
                }
                if (pb->padButtons & rsPADDPADRIGHT)
                {
                    ks.keyCharCode = rsRIGHT;
                    RsKeyboardEventHandler(rsKEYDOWN, &ks);
                }
                if (pb->padButtons & rsPADDPADUP)
                {
                    ks.keyCharCode = rsUP;
                    RsKeyboardEventHandler(rsKEYDOWN, &ks);
                }
                if (pb->padButtons & rsPADDPADDOWN)
                {
                    ks.keyCharCode = rsDOWN;
                    RsKeyboardEventHandler(rsKEYDOWN, &ks);
                }
            }
            break;

        case rsPADBUTTONUP:
            {
                RsPadButtonStatus  *pb = (RsPadButtonStatus *) param;
                RsKeyStatus         ks;

                if (pb->padButtons & rsPADDPADLEFT)
                {
                    ks.keyCharCode = rsLEFT;
                    RsKeyboardEventHandler(rsKEYUP, &ks);
                }
                if (pb->padButtons & rsPADDPADRIGHT)
                {
                    ks.keyCharCode = rsRIGHT;
                    RsKeyboardEventHandler(rsKEYUP, &ks);
                }
                if (pb->padButtons & rsPADDPADUP)
                {
                    ks.keyCharCode = rsUP;
                    RsKeyboardEventHandler(rsKEYUP, &ks);
                }
                if (pb->padButtons & rsPADDPADDOWN)
                {
                    ks.keyCharCode = rsDOWN;
                    RsKeyboardEventHandler(rsKEYUP, &ks);
                }
            }
            break;

        default:
            break;
    }

    return;
}

/* simulate a mouse on pad based platforms */

void
PadMouse(RsEvent event, void *param)
{
    static RsMouseStatus ms;    /* we simulate a mouse on pad based platforms */

    switch (event)
    {
        case rsINITIALIZE:
            ms.shift = ms.control = 0;
            ms.pos.x = ms.pos.y = ms.delta.x = ms.delta.y = 0;
            break;

        case rsPADBUTTONDOWN:
            {
                RsPadButtonStatus  *pb = (RsPadButtonStatus *) param;

                if (pb->padButtons & rsPADBUTTON5)
                {
                    ms.shift = TRUE;
                }
                if (pb->padButtons & rsPADBUTTON6)
                {
                    ms.control = TRUE;
                }
                if (pb->padButtons & rsPADBUTTON7)
                {
                    RsMouseEventHandler(rsLEFTBUTTONDOWN, &ms);
                }
                if (pb->padButtons & rsPADBUTTON8)
                {
                    RsMouseEventHandler(rsRIGHTBUTTONDOWN, &ms);
                }
            }
            break;

        case rsPADBUTTONUP:
            {
                RsPadButtonStatus  *pb = (RsPadButtonStatus *) param;

                if (pb->padButtons & rsPADBUTTON5)
                {
                    ms.shift = FALSE;
                }
                if (pb->padButtons & rsPADBUTTON6)
                {
                    ms.control = FALSE;
                }
                if (pb->padButtons & rsPADBUTTON7)
                {
                    RsMouseEventHandler(rsLEFTBUTTONUP, NULL);
                }
                if (pb->padButtons & rsPADBUTTON8)
                {
                    RsMouseEventHandler(rsRIGHTBUTTONUP, NULL);
                }
            }
            break;

        case rsPADANALOGUELEFT:
            {
                /* cheat by making it look like a mouse event */
                ms.delta = *(RwV2d *) param;

                ms.delta.x =
                    (ms.delta.x * (RwReal)RwRealAbs(ms.delta.x)) * 64.0f;
                ms.delta.y =
                    (ms.delta.y * (RwReal)RwRealAbs(ms.delta.y)) * 64.0f;

                RsMouseEventHandler(rsMOUSEMOVE, &ms);
            }
            break;

        default:
            break;
    }

    return;
}

void
MousePad(RsEvent event, void *param)
{
    static RwBool       left, right;
    RsMouseStatus      *ms = (RsMouseStatus *) param;

    switch (event)
    {
        case rsINITIALIZE:
            {
                left = right = FALSE;
            }
            break;

        case rsLEFTBUTTONDOWN:
            {
                left = TRUE;
            }
            break;

        case rsLEFTBUTTONUP:
            {
                left = FALSE;
            }
            break;

        case rsRIGHTBUTTONDOWN:
            {
                right = TRUE;
            }
            break;

        case rsRIGHTBUTTONUP:
            {
                right = FALSE;
            }
            break;

        case rsMOUSEMOVE:
            {
                RwV2d               delta;

                delta = ms->delta;

                delta.x /= 8.0f;
                delta.y /= 8.0f;

                if (left)
                {
                    RsPadEventHandler(rsPADANALOGUELEFT, &delta);
                }

                if (right)
                {
                    RsPadEventHandler(rsPADANALOGUERIGHT, &delta);
                }
            }
            break;

        default:
            break;
    }

    return;
}

void
KeysPad(RsEvent event, void *param)
{
    static RsPadButtonStatus ps;
    RsKeyStatus        *ks = (RsKeyStatus *) param;

    switch (event)
    {
        case rsINITIALIZE:
            {
                ps.padID = ps.padButtons = 0;

                break;
            }

        case rsKEYDOWN:
            {
                switch (ks->keyCharCode)
                {
                    case rsLEFT:
                        {
                            ps.padButtons |= rsPADBUTTON3;
                            RsPadEventHandler(rsPADBUTTONDOWN, &ps);

                            break;
                        }

                    case rsRIGHT:
                        {
                            ps.padButtons |= rsPADBUTTON4;
                            RsPadEventHandler(rsPADBUTTONDOWN, &ps);

                            break;
                        }

                    case rsUP:
                        {
                            ps.padButtons |= rsPADBUTTON1;
                            RsPadEventHandler(rsPADBUTTONDOWN, &ps);

                            break;
                        }

                    case rsDOWN:
                        {
                            ps.padButtons |= rsPADBUTTON2;
                            RsPadEventHandler(rsPADBUTTONDOWN, &ps);

                            break;
                        }

                    case rsLSHIFT:
                        {
                            ps.padButtons |= rsPADBUTTON5;
                            RsPadEventHandler(rsPADBUTTONDOWN, &ps);

                            break;
                        }

                    case rsLCTRL:
                        {
                            ps.padButtons |= rsPADBUTTON6;
                            RsPadEventHandler(rsPADBUTTONDOWN, &ps);

                            break;
                        }

                    case rsRSHIFT:
                        {
                            ps.padButtons |= rsPADBUTTON7;
                            RsPadEventHandler(rsPADBUTTONDOWN, &ps);

                            break;
                        }

                    case rsRCTRL:
                        {
                            ps.padButtons |= rsPADBUTTON8;
                            RsPadEventHandler(rsPADBUTTONDOWN, &ps);

                            break;
                        }

                    case 'i':
                        {
                            ps.padButtons |= rsPADDPADUP;
                            RsPadEventHandler(rsPADBUTTONDOWN, &ps);

                            break;
                        }

                    case 'm':
                        {
                            ps.padButtons |= rsPADDPADDOWN;
                            RsPadEventHandler(rsPADBUTTONDOWN, &ps);

                            break;
                        }

                    case 'j':
                        {
                            ps.padButtons |= rsPADDPADLEFT;
                            RsPadEventHandler(rsPADBUTTONDOWN, &ps);

                            break;
                        }

                    case 'k':
                        {
                            ps.padButtons |= rsPADDPADRIGHT;
                            RsPadEventHandler(rsPADBUTTONDOWN, &ps);

                            break;
                        }

                    default:
                        break;
                }

                break;
            }

        case rsKEYUP:
            {
                switch (ks->keyCharCode)
                {
                    case rsLEFT:
                        {
                            RsPadEventHandler(rsPADBUTTONUP, &ps);
                            ps.padButtons &= ~rsPADBUTTON3;

                            break;
                        }

                    case rsRIGHT:
                        {
                            RsPadEventHandler(rsPADBUTTONUP, &ps);
                            ps.padButtons &= ~rsPADBUTTON4;

                            break;
                        }

                    case rsUP:
                        {
                            RsPadEventHandler(rsPADBUTTONUP, &ps);
                            ps.padButtons &= ~rsPADBUTTON1;

                            break;
                        }

                    case rsDOWN:
                        {
                            RsPadEventHandler(rsPADBUTTONUP, &ps);
                            ps.padButtons &= ~rsPADBUTTON2;

                            break;
                        }

                    case rsLSHIFT:
                        {
                            RsPadEventHandler(rsPADBUTTONDOWN, &ps);
                            ps.padButtons &= ~rsPADBUTTON5;

                            break;
                        }

                    case rsLCTRL:
                        {
                            RsPadEventHandler(rsPADBUTTONDOWN, &ps);
                            ps.padButtons &= ~rsPADBUTTON6;

                            break;
                        }

                    case rsRSHIFT:
                        {
                            RsPadEventHandler(rsPADBUTTONDOWN, &ps);
                            ps.padButtons &= ~rsPADBUTTON7;

                            break;
                        }

                    case rsRCTRL:
                        {
                            RsPadEventHandler(rsPADBUTTONDOWN, &ps);
                            ps.padButtons &= ~rsPADBUTTON8;

                            break;
                        }

                    case 'i':
                        {
                            RsPadEventHandler(rsPADBUTTONDOWN, &ps);
                            ps.padButtons &= ~rsPADDPADUP;

                            break;
                        }

                    case 'm':
                        {
                            RsPadEventHandler(rsPADBUTTONDOWN, &ps);
                            ps.padButtons &= ~rsPADDPADDOWN;

                            break;
                        }

                    case 'j':
                        {
                            RsPadEventHandler(rsPADBUTTONDOWN, &ps);
                            ps.padButtons &= ~rsPADDPADLEFT;

                            break;
                        }

                    case 'k':
                        {
                            RsPadEventHandler(rsPADBUTTONDOWN, &ps);
                            ps.padButtons &= ~rsPADDPADRIGHT;

                            break;
                        }

                    default:
                        break;
                }

                break;
            }

        default:
            break;
    }

    return;
}

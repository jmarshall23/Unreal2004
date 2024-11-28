// ====================================================================
//  Written by Joe Wilcox
//  (c) 2002, Epic Games, Inc.  All Rights Reserved
//
//  Normal push buttons (OK, Cancel, Apply)
// ====================================================================

class STY2RoundButton extends GUI2Styles;

defaultproperties
{
    KeyName="RoundButton"
/*
	FontColors(0)=(R=14,G=37,B=95,A=180)
	FontColors(1)=(R=44,G=108,B=203,A=255)
	FontColors(2)=(R=14,G=41,B=106,A=255)
	FontColors(3)=(R=14,G=41,B=106,A=255)
	FontColors(4)=(R=32,G=32,B=80,A=180)
*/
    FontNames(5)="UT2SmallHeaderFont"
    FontNames(6)="UT2SmallHeaderFont"
    FontNames(7)="UT2SmallHeaderFont"
    FontNames(8)="UT2SmallHeaderFont"
    FontNames(9)="UT2SmallHeaderFont"
    FontNames(10)="UT2HeaderFont"
    FontNames(11)="UT2HeaderFont"
    FontNames(12)="UT2HeaderFont"
    FontNames(13)="UT2HeaderFont"
    FontNames(14)="UT2HeaderFont"

    Images(0)=Material'2K4Menus.Controls.buttonthick_b'
    Images(1)=Material'2K4Menus.Controls.buttonthick_w'
    Images(2)=Material'2K4Menus.Controls.buttonthick_f'
    Images(3)=Material'2K4Menus.Controls.buttonthick_p'
    Images(4)=Material'2K4Menus.Controls.buttonthick_d'

    ImgStyle(0)=ISTY_PartialScaled
    ImgStyle(1)=ISTY_PartialScaled
    ImgStyle(2)=ISTY_PartialScaled
    ImgStyle(3)=ISTY_PartialScaled
    ImgStyle(4)=ISTY_PartialScaled
}
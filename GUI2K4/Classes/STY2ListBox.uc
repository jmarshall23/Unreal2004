// ====================================================================
//  Class:  GUI2K4.STY_ListBox
//  Parent: GUI2K4.STY_SquareButton
//
//  Background style for the actual "list" area of a listbox
// ====================================================================

class STY2ListBox extends STY2SquareButton;

defaultproperties
{
    KeyName="ListBox"
    Images(0)=Material'2K4Menus.NewControls.SectionHeaderBase'
    Images(1)=Material'2K4Menus.NewControls.SectionHeaderBase'
    Images(2)=Material'2K4Menus.NewControls.SectionHeaderBase'
    Images(3)=Material'2K4Menus.NewControls.SectionHeaderBase'
    Images(4)=Material'2K4Menus.NewControls.SectionHeaderBase'

    ImgStyle(0)=ISTY_PartialScaled
    ImgStyle(1)=ISTY_PartialScaled
    ImgStyle(2)=ISTY_PartialScaled
    ImgStyle(3)=ISTY_PartialScaled
    ImgStyle(4)=ISTY_PartialScaled

    FontNames(10)="UT2HeaderFont"
    FontNames(11)="UT2HeaderFont"
    FontNames(12)="UT2HeaderFont"
    FontNames(13)="UT2HeaderFont"
    FontNames(14)="UT2HeaderFont"

    BorderOffsets(0)=3
    BorderOffsets(1)=3
    BorderOffsets(2)=3
    BorderOffsets(3)=3

	FontColors(0)=(R=255,B=0,G=195,A=255)
	FontColors(1)=(R=255,B=0,G=210,A=255)
	FontColors(2)=(R=255,B=255,G=255,A=255)
	FontColors(3)=(R=255,B=255,G=255,A=255)
	FontColors(4)=(R=192,B=192,G=192,A=255)
}

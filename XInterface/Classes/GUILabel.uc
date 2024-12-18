// ======================================================================================================
//  GUILabel - A text label that get's displayed.  By default, it
//  uses the default font, however you can override it if you wish.
//
//  By default, labels will never be focused, since bAcceptsInput is false
//  for GUILabel.  This means that GUILabel will never be put into the MSAT_Focused
//  state (thus changing the color of the text to FocusedTextColor) unless the GUILabel
//  is associated with a component that manually sets the label's state, such as a GUIMenuOption.
//
//  The easiest way to override this default behavior, if desired, is by setting StyleName to "TextLabel".
//
//  Written by Joe Wilcox
//  Updated by Ron Prestenback
//  (c) 2002 - 2003, Epic Games, Inc.  All Rights Reserved
// ======================================================================================================

class GUILabel extends GUIComponent
    Native;

cpptext
{
        void Draw(UCanvas* Canvas);
}

var()   localized   string              Caption;            // The text to display
var()               eTextAlign          TextAlign;          // How is the text aligned in it's bounding box
var()               color               TextColor;          // The Color to display this in.
var()               color               FocusedTextColor;   // The Color to display this in.
var()               EMenuRenderStyle    TextStyle;          // What canvas style to use
var()               string              TextFont;           // The Font to display it in
var()               bool                bTransparent;       // Draw a Background for this label
var()               bool                bMultiLine;         // Will cut content to display on multiple lines when too long
var()				eTextAlign          VertAlign;			// Vertical alignment (only if bMultiLine) - Left = Top
var()               color               BackColor;          // Background color for this label
var()               color               ShadowColor;        // Shadow Color
var()               float               ShadowOffsetX;      // if > 0 draw shadow
var()               float               ShadowOffsetY;      // if > 0 draw shadow
var()               color               HilightColor;       // Shadow Color
var()				float               HilightOffsetX;     // if > 0 draw shadow
var()               float               HilightOffsetY;     // if > 0 draw shadow

defaultproperties
{
    TextAlign=TXTA_Left
    TextColor=(R=0,G=0,B=64,A=255)
    FocusedTextColor=(R=32,G=32,B=80,A=255)
    TextStyle=MSTY_Normal
    TextFont="UT2MenuFont"
    bTransparent=true
    BackColor=(R=0,G=0,B=0,A=255)
    RenderWeight=0.4
    WinHeight=0.06
    VertAlign=TXTA_Left
    ShadowColor=(R=0,G=0,B=0,A=196)
    ShadowOffsetX=0
    ShadowOffsetY=0
    HilightColor=(R=255,G=255,B=255,A=196)
    HilightOffsetX=0
    HilightOffsetY=0
}

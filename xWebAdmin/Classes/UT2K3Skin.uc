//-----------------------------------------------------------
//
//-----------------------------------------------------------
class UT2K3Skin extends WebSkin;

function Init(UTServerAdmin WebAdmin)
{
	WebAdmin.SkinPath = "";
	WebAdmin.SiteBG = DefaultBGColor;
	WebAdmin.SiteCSSFile = SkinCSS;
}

DefaultProperties
{
	DisplayName="Standard UT2K3"
}

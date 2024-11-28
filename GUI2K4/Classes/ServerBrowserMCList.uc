//==============================================================================
//	Base class for server browser multi-column listboxes
//
//	Created by Ron Prestenback
//	© 2003, Epic Games, Inc.  All Rights Reserved
//==============================================================================
class ServerBrowserMCList extends GUIMultiColumnList;

var UT2K4Browser_ServerListPageBase tp_MyPage;

function SetAnchor(UT2K4Browser_ServerListPageBase Anchor)
{
	tp_MyPage = Anchor;
}

function MyOnDrawItem(Canvas Canvas, int i, float X, float Y, float W, float H, bool bSelected, bool bPending);

DefaultProperties
{
	OnDrawItem=MyOnDrawItem
	Index=-1
	bVisible=True
	bVisibleWhenEmpty=True
	RenderWeight=1
	StyleName="ServerBrowserGrid"
	SelectedStyleName="BrowserListSelection"
}

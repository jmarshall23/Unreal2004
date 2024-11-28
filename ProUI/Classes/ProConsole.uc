// ====================================================================
// (C) 2002, Epic Games
// ====================================================================

class ProConsole extends ExtendedConsole
	config(ProUI);

var bool bUnLockConsole;
var config string UnlockPassword;

exec function ConsoleOpen()
{
	if (!bUnlockConsole)
    {
	    ViewportOwner.Actor.ClientMessage("Console Access Denied!");
		return;
    }

	Super.ConsoleOpen();
}

exec function UnlockConsole(string Pass)
{
	if ( (UnlockPassword!="") && (UnlockPassword~=Pass) )
    {
	    ViewportOwner.Actor.ClientMessage("Console Unlocked!");
    	bUnlockConsole = true;
    }
}

exec function LockConsole()
{
	bUnlockConsole = false;
    ViewportOwner.Actor.ClientMessage("Console Locked!");
}

state Typing
{

	function bool KeyEvent( EInputKey Key, EInputAction Action, FLOAT Delta )
	{
		local string Temp;

		if (Action== IST_PRess)
		{
			bIgnoreKeys=false;
		}

		if( Key==IK_Escape )
		{
			if( TypedStr!="" )
			{
				TypedStr="";
				HistoryCur = HistoryTop;
                return( true );
			}
			else
			{
                TypingClose();
                return( true );
			}
		}
		else if( Action != IST_Press )
		{
            return( false );
		}
		else if( Key==IK_Enter )
		{
			if( TypedStr!="" )
			{

				if ( !AllowCommand(TypedStr) )
				{
				    ViewportOwner.Actor.ClientMessage("Console Access Denied!");
                    TypedStr="";
                    TypingClose();
                    return true;
                }

				History[HistoryTop] = TypedStr;
                HistoryTop = (HistoryTop+1) % ArrayCount(History);

				if ( ( HistoryBot == -1) || ( HistoryBot == HistoryTop ) )
                    HistoryBot = (HistoryBot+1) % ArrayCount(History);

				HistoryCur = HistoryTop;

				// Make a local copy of the string.
				Temp=TypedStr;
				TypedStr="";

				if( !ConsoleCommand( Temp ) )
					Message( Localize("Errors","Exec","Core"), 6.0 );

				Message( "", 6.0 );
			}

            TypingClose();

            return( true );
		}
		else if( Key==IK_Up )
		{
			if ( HistoryBot >= 0 )
			{
				if (HistoryCur == HistoryBot)
					HistoryCur = HistoryTop;
				else
				{
					HistoryCur--;
					if (HistoryCur<0)
                        HistoryCur = ArrayCount(History)-1;
				}

				TypedStr = History[HistoryCur];
			}
            return( true );
		}
		else if( Key==IK_Down )
		{
			if ( HistoryBot >= 0 )
			{
				if (HistoryCur == HistoryTop)
					HistoryCur = HistoryBot;
				else
                    HistoryCur = (HistoryCur+1) % ArrayCount(History);

				TypedStr = History[HistoryCur];
			}

		}
		else if( Key==IK_Backspace || Key==IK_Left )
		{
			if( Len(TypedStr)>0 )
				TypedStr = Left(TypedStr,Len(TypedStr)-1);
            return( true );
		}
        return( true );
	}
}

function bool AllowCommand(string cmd)
{
	if (bUnLockConsole)
    	return true;

    if (left(Cmd,13) ~= "unlockconsole")
    	return true;

    if (left(cmd,3) ~= "say")
    	return true;

    if (left(cmd,7) ~= "teamsay")
    	return true;

    if (left(cmd,4) ~= "quit")
    	return true;

    return false;
}

defaultproperties
{
}
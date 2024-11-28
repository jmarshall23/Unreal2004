BOOL 
XE3GetUCPlayer (PSTR szName, PUINT puiTeam, PUINT puiVoiceMask);

BOOL 
XE3WriteUCPlayer (PSTR szName, UINT uiTeam, UINT uiTeamWin, UINT uiFrags, UINT uiFlagCaptures, UINT uiReturns, UINT uiSpecials, UINT uiAccuracy, UINT uiFavoriteWeapon);

BOOL UtilCreateDummyUCFile(PSTR szName, UINT uiTeam, UINT uiVoiceMask);


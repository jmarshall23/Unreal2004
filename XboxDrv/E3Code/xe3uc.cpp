#include <xtl.h>
#include "xe3uc.h"

// TODO: The file spec is not clear as to whether the player name will take up a
// full 12 bytes or if it should be read in up to the terminating NULL and then 
// the team number expected after that.  It is also not clear if the 12 includes
// the terminating NULL.

#ifdef _DEBUG
#define ODS(s) OutputDebugStringA(s)
#else // !_DEBUG
#define ODS(s) (VOID)(s)
#endif // _DEBUG

#define MAX_PLAYER_NAME 13 // Maximum length for the name of a player including NULL

static const char c_pszInputFile[] = "T:\\E3USERIN.INI";
static const char c_pszOutputFile[] = "T:\\E3USEROUT.INI";

/*************************************************************************
*
* Function:     XE3GetUCPlayer
*
* Purpose:      Reads the user and team settings from T:\E3USERIN.INI 
*               (format below) and returns the data. The function caches
*               the timestamp of the e3userin.ini. 
*
* E3USERIN.INI Format:
*               Name= <TextString, max 12 Characters>
*               Team=1|2 (team 1 or Team 2)
*		VoiceMask= <0 to x>
*
* Parameters:   szName - name of player
*               puiTeam - 1 for team 1, 2 for team 2   
*               puiVoiceMask - number of voice mask for this player
*
* Return:       BOOL indicating success or failure.  The function should 
*               return FALSE if the file does not exist, cannot be read, 
*               etc, or if the timestamp on the file matches the last 
*               cached timestamp. Otherwise, it should return TRUE.
*
*************************************************************************/
BOOL 
XE3GetUCPlayer (PSTR szName, PUINT puiTeam, PUINT puiVoiceMask)
{
    char szPlayer[MAX_PLAYER_NAME];
    UINT uiTeam;
    UINT uiVoiceMask;
    HANDLE hFile;
    DWORD dwNumBytesRead;
    FILETIME ftLastModified;
    static FILETIME s_ftCachedFileTime = {0,0};

    // Open the file
    hFile = CreateFile(c_pszInputFile, GENERIC_READ, FILE_SHARE_READ, NULL,
                        OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        ODS("CreateFile: File not found!\n");
        return FALSE;
    }

    // Get the time the file was last modified
    if (!GetFileTime(hFile, NULL, NULL, &ftLastModified))
    {
        ODS("GetFileTime: Failed retrieving file time!\n");
        CloseHandle(hFile);
        return FALSE;
    }

    // If the file hasn't been modified, we return FALSE
    if (!CompareFileTime(&s_ftCachedFileTime, &ftLastModified))
    {
        ODS("CompareFileTime: File hasn't been modified!\n");
        CloseHandle(hFile);
        return FALSE;
    }

    // Cache the new file time
    s_ftCachedFileTime = ftLastModified;

    // Read the player name
    if (!ReadFile(hFile, szPlayer, MAX_PLAYER_NAME, &dwNumBytesRead, NULL))
    {
        ODS("ReadFile(): Failed reading player name!\n" );
        CloseHandle(hFile);
        return FALSE;
    }

    // Read the player's team number
    if (!ReadFile(hFile, &uiTeam, sizeof(uiTeam), &dwNumBytesRead, NULL))
    {
        ODS("ReadFile(): Failed reading team number!\n" );
        CloseHandle(hFile);
        return FALSE;
    }

    // Read the player's voice mask
    if (!ReadFile(hFile, &uiVoiceMask, sizeof(uiVoiceMask), &dwNumBytesRead, NULL))
    {
        ODS("ReadFile(): Failed reading voice mask!\n" );
        CloseHandle(hFile);
        return FALSE;
    }

    // Done with the file
    CloseHandle(hFile);

    // All succeeded, populate the return values
    lstrcpyA(szName, szPlayer);
    *puiTeam = uiTeam;
    *puiVoiceMask = uiVoiceMask;

    return TRUE;
}

/*************************************************************************
*
* Function:     XE3WriteUCPlayer
*
* Purpose:      Writes to the file T:\E3USEROUT.INI in the following 
*               format.
*
* E3USEROUT.INI Format:
*               Name = <TextString, max 12 characters>
*               Team=1|2 (team 1 or Team 2)
*               TeamWin= 1|2 (team 1 or Team 2)
*               Frags=<Integer number of frags for player>
*               FlagCaptures=<Integer number of flag captures for player>
*
* Parameters:   szName - name of player
*               uiTeam - 1 for team 1, 2 for team 2   
*               uiTeamWin - 1 for team 1, 2 for team 2
*               uiFrags - player frags during match
*               uiFlagCaptures - player flag captures during match
*               uiReturns
*               uiSpecials
*               uiAccuracy
*               uiFavoriteWeapon
*
* Return:       BOOL indicating success or failure
*
*************************************************************************/
BOOL 
XE3WriteUCPlayer (PSTR szName, UINT uiTeam, UINT uiTeamWin, UINT uiFrags, UINT uiFlagCaptures, UINT uiReturns, UINT uiSpecials, UINT uiAccuracy, UINT uiFavoriteWeapon)
{
    HANDLE hFile;
    DWORD dwNumBytesWritten;

    // Open the file
    hFile = CreateFile(c_pszOutputFile, GENERIC_WRITE, 0, NULL,
                       CREATE_ALWAYS, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        ODS("CreateFile: Output file could not be created!\n");
        return FALSE;
    }

    // Write out the player name
    if (!WriteFile(hFile, szName, MAX_PLAYER_NAME, &dwNumBytesWritten, NULL))
    {
        ODS("WriteFile: Failed writing player name!\n");
        CloseHandle(hFile);
        return FALSE;
    }

    // Write out the player's team number
    if (!WriteFile(hFile, &uiTeam, sizeof(uiTeam), &dwNumBytesWritten, NULL))
    {
        ODS("WriteFile: Failed writing player team number!\n");
        CloseHandle(hFile);
        return FALSE;
    }

    // Write out which team won
    if (!WriteFile(hFile, &uiTeamWin, sizeof(uiTeamWin), &dwNumBytesWritten, NULL))
    {
        ODS("WriteFile: Failed writing winning team number!\n");
        CloseHandle(hFile);
        return FALSE;
    }

    // Write out the number of frags
    if (!WriteFile(hFile, &uiFrags, sizeof(uiFrags), &dwNumBytesWritten, NULL))
    {   
        ODS("WriteFile: Failed writing player frag count!\n");
        CloseHandle(hFile);
        return FALSE;
    }

    // Write out the number of flag captures
    if (!WriteFile(hFile, &uiFlagCaptures, sizeof(uiFlagCaptures), &dwNumBytesWritten, NULL))
    {
        ODS("WriteFile: Failed writing player flag capture count!\n");
        CloseHandle(hFile);
        return FALSE;
    }

    // Write out the number of returns
    if (!WriteFile(hFile, &uiReturns, sizeof(uiReturns), &dwNumBytesWritten, NULL))
    {
        ODS("WriteFile: Failed writing player return count!\n");
        CloseHandle(hFile);
        return FALSE;
    }

    // Write out the number of specials
    if (!WriteFile(hFile, &uiSpecials, sizeof(uiSpecials), &dwNumBytesWritten, NULL))
    {
        ODS("WriteFile: Failed writing player specials!\n");
        CloseHandle(hFile);
        return FALSE;
    }

    // Write out the accuracy
    if (!WriteFile(hFile, &uiAccuracy, sizeof(uiAccuracy), &dwNumBytesWritten, NULL))
    {
        ODS("WriteFile: Failed writing player accuracy!\n");
        CloseHandle(hFile);
        return FALSE;
    }

    // Write out the favorite weapon
    if (!WriteFile(hFile, &uiFavoriteWeapon, sizeof(uiFavoriteWeapon), &dwNumBytesWritten, NULL))
    {
        ODS("WriteFile: Failed writing player favorite weapon!\n");
        CloseHandle(hFile);
        return FALSE;
    }

    // Done with the file
    CloseHandle(hFile);

    return TRUE;
}


// Create a test input file for the XE3GetUCPlayer function
BOOL UtilCreateDummyUCFile(PSTR szName, UINT uiTeam)
{
    HANDLE hFile;
    DWORD dwNumBytesWritten;

    DeleteFile(c_pszInputFile);

    // Open the file
    hFile = CreateFile(c_pszInputFile, GENERIC_WRITE, 0, NULL,
                       CREATE_ALWAYS, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        ODS("CreateFile: Dummy UC file could not be created!\n");
        return FALSE;
    }

    // Write out the player name
    if (!WriteFile(hFile, szName, MAX_PLAYER_NAME, &dwNumBytesWritten, NULL))
    {
        ODS("WriteFile: Failed writing UC player name!\n");
        CloseHandle(hFile);
        return FALSE;
    }

    // Write out the player team
    if (!WriteFile(hFile, &uiTeam, sizeof(uiTeam), &dwNumBytesWritten, NULL))
    {
        ODS("WriteFile: Failed writing UC player team!\n");
        CloseHandle(hFile);
        return FALSE;
    }

    CloseHandle(hFile);

    return TRUE;
}


// Create a test input file for the XE3GetUCPlayer function
BOOL UtilCreateDummyUCFile(PSTR szName, UINT uiTeam, UINT uiVoiceMask)
{
    HANDLE hFile;
    DWORD dwNumBytesWritten;

    DeleteFile(c_pszInputFile);

    // Open the file
    hFile = CreateFile(c_pszInputFile, GENERIC_WRITE, 0, NULL,
                       CREATE_ALWAYS, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        ODS("CreateFile: Dummy file could not be created!\n");
        return FALSE;
    }

    // Write out the player name
    if (!WriteFile(hFile, szName, MAX_PLAYER_NAME, &dwNumBytesWritten, NULL))
    {
        ODS("WriteFile: Failed writing player name!\n");
        CloseHandle(hFile);
        return FALSE;
    }

    // Write out the player team
    if (!WriteFile(hFile, &uiTeam, sizeof(uiTeam), &dwNumBytesWritten, NULL))
    {
        ODS("WriteFile: Failed writing player team!\n");
        CloseHandle(hFile);
        return FALSE;
    }

    // Write out the player voice mask
    if (!WriteFile(hFile, &uiVoiceMask, sizeof(uiVoiceMask), &dwNumBytesWritten, NULL))
    {
        ODS("WriteFile: Failed writing player voice mask!\n");
        CloseHandle(hFile);
        return FALSE;
    }

    CloseHandle(hFile);

    return TRUE;
}


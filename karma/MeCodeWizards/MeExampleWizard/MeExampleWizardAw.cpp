// MeExampleWizardaw.cpp : implementation file
//

#include "stdafx.h"
#include "MeExampleWizard.h"
#include "MeExampleWizardaw.h"
#include "chooser.h"
#include <atlbase.h>
#include "AFXPRIV2.H"

#ifdef _PSEUDO_DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// This is called immediately after the custom AppWizard is loaded.  Initialize
//  the state of the custom AppWizard here.
void CMeExampleWizardAppWiz::InitCustomAppWiz()
{
	// Create a new dialog chooser; CDialogChooser's constructor initializes
	//  its internal array with pointers to the steps.
	m_pChooser = new CDialogChooser;

	// Set the maximum number of steps.
	SetNumberOfSteps(LAST_DLG);

	// TODO: Add any other custom AppWizard-wide initialization here.
}

// This is called just before the custom AppWizard is unloaded.
void CMeExampleWizardAppWiz::ExitCustomAppWiz()
{
	// Deallocate memory used for the dialog chooser
	ASSERT(m_pChooser != NULL);
	delete m_pChooser;
	m_pChooser = NULL;

	// TODO: Add code here to deallocate resources used by the custom AppWizard
}

// This is called when the user clicks "Create..." on the New Project dialog
//  or "Next" on one of the custom AppWizard's steps.
CAppWizStepDlg* CMeExampleWizardAppWiz::Next(CAppWizStepDlg* pDlg)
{
	// Delegate to the dialog chooser
	return m_pChooser->Next(pDlg);
}

// This is called when the user clicks "Back" on one of the custom
//  AppWizard's steps.
CAppWizStepDlg* CMeExampleWizardAppWiz::Back(CAppWizStepDlg* pDlg)
{
	// Delegate to the dialog chooser
	return m_pChooser->Back(pDlg);
}

void CMeExampleWizardAppWiz::CustomizeProject(IBuildProject* pProject)
{
    IConfigurations *pAllConfigurations;
    IConfiguration *pConf1, *pConf2;
    IConfiguration *pConfRelease, *pConfDebug;
    pProject->get_Configurations( &pAllConfigurations );
    
    CComVariant iIndex = 1, iReserved;
    pAllConfigurations->Item( iIndex, &pConf1 );
    CComVariant iIndex2 = 2;
    pAllConfigurations->Item( iIndex2, &pConf2 );
    
    // determine which is debug and which is release by looking at name
    // of first configuration
    BSTR conf1Name;
    pConf1->get_Name(&conf1Name);
    CString name;
    AfxBSTR2CString(&name, conf1Name);
    if (name.Find("Debug") != -1)
    {
        pConfDebug = pConf1;
        pConfRelease = pConf2;
    }
    else
    {
       pConfDebug = pConf2;
       pConfRelease = pConf1;
    }

    CString strRoot;
    m_Dictionary.Lookup(_T("root"), strRoot);
    
    CString szTool, szSetting;
    BSTR bstrTool, bstrSetting;

    //Set compiler options
    szTool = "cl.exe", 
        szSetting = "/I \"../../MeGlobals/include\" /I \"../../MeApp/include\" /I \"../../MeViewer2/include\""
        " /I \"../../MeAssetDB/include\" /I \"../../MeAssetDBXMLIO/include\" /I \"../../MeXML/include\" /I \"../../Mcd/include\""
        " /I \"../../Mdt/include\" /I \"../../MdtBcl/include\" /I \"../../MdtKea/include\""
        " /I \"../../Mst/include\" /I \"../../MeAssetFactory/include\"";
    bstrTool = szTool.AllocSysString();
    bstrSetting = szSetting.AllocSysString();
    pConfRelease->AddToolSettings( bstrTool, bstrSetting, iReserved); 
    pConfDebug->AddToolSettings( bstrTool, bstrSetting, iReserved); 

    szSetting = "/D \"_AFXDLL\"";
    bstrSetting = szSetting.AllocSysString();
    pConfRelease->RemoveToolSettings( bstrTool, bstrSetting, iReserved); 
    pConfDebug->RemoveToolSettings( bstrTool, bstrSetting, iReserved); 

    //Set linker options Release
    szTool = "link.exe"; 
    szSetting.Format("McdConvex.lib McdConvexCreateHull.lib MeAssetFactory.lib MeAssetDB.lib MeXML.lib MeAssetDBXMLIO.lib "
		"Mst.lib MeApp.lib McdFrame.lib McdCommon.lib McdPrimitives.lib MdtBcl.lib Mdt.lib MeViewer2.lib MdtKea.lib "
		"MeGlobals.lib kernel32.lib user32.lib gdi32.lib advapi32.lib "
		"/out:\"../bin.rel/win32/%s.exe\" /libpath:\"../../lib.rel/win32\" /libpath:\"../../tools/glut\" /subsystem:console /incremental:no", strRoot );
    bstrTool = szTool.AllocSysString();
    bstrSetting = szSetting.AllocSysString();
    pConfRelease->AddToolSettings( bstrTool, bstrSetting, iReserved); 
    //Set linker options Debug
    szSetting.Format("McdConvex.lib McdConvexCreateHull.lib MeAssetFactory.lib MeAssetDB.lib MeXML.lib MeAssetDBXMLIO.lib "
		"Mst.lib MeApp.lib McdFrame.lib McdCommon.lib McdPrimitives.lib MdtBcl.lib Mdt.lib MeViewer2.lib MdtKea.lib "
		"MeGlobals.lib kernel32.lib user32.lib gdi32.lib advapi32.lib "
		"/out:\"../bin.dbg/win32/%s.exe\" /libpath:\"../../lib.dbg/win32\" /libpath:\"../../tools/glut\" /subsystem:console /incremental:no", strRoot );
    bstrSetting = szSetting.AllocSysString();
    pConfDebug->AddToolSettings( bstrTool, bstrSetting, iReserved);


	// This is called immediately after the default Debug and Release
	//  configurations have been created for each platform.  You may customize
	//  existing configurations on this project by using the methods
	//  of IBuildProject and IConfiguration such as AddToolSettings,
	//  RemoveToolSettings, and AddCustomBuildStep. These are documented in
	//  the Developer Studio object model documentation.

	// WARNING!!  IBuildProject and all interfaces you can get from it are OLE
	//  COM interfaces.  You must be careful to release all new interfaces
	//  you acquire.  In accordance with the standard rules of COM, you must
	//  NOT release pProject, unless you explicitly AddRef it, since pProject
	//  is passed as an "in" parameter to this function.  See the documentation
	//  on CCustomAppWiz::CustomizeProject for more information.
    pAllConfigurations->Release();
    pConf1->Release();
    pConf2->Release();
}


// Here we define one instance of the CMeExampleWizardAppWiz class.  You can access
//  m_Dictionary and any other public members of this class through the
//  global MeExampleWizardaw.
CMeExampleWizardAppWiz MeExampleWizardaw;


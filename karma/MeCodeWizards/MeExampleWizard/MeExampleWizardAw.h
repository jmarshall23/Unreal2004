#if !defined(AFX_MEEXAMPLEWIZARDAW_H__93C9B621_ACEB_4AA9_AAE3_47B0C352F3A0__INCLUDED_)
#define AFX_MEEXAMPLEWIZARDAW_H__93C9B621_ACEB_4AA9_AAE3_47B0C352F3A0__INCLUDED_

// MeExampleWizardaw.h : header file
//

class CDialogChooser;

// All function calls made by mfcapwz.dll to this custom AppWizard (except for
//  GetCustomAppWizClass-- see MeExampleWizard.cpp) are through this class.  You may
//  choose to override more of the CCustomAppWiz virtual functions here to
//  further specialize the behavior of this custom AppWizard.
class CMeExampleWizardAppWiz : public CCustomAppWiz
{
public:
	virtual CAppWizStepDlg* Next(CAppWizStepDlg* pDlg);
	virtual CAppWizStepDlg* Back(CAppWizStepDlg* pDlg);
		
	virtual void InitCustomAppWiz();
	virtual void ExitCustomAppWiz();
	virtual void CustomizeProject(IBuildProject* pProject);

protected:
	CDialogChooser* m_pChooser;
};

// This declares the one instance of the CMeExampleWizardAppWiz class.  You can access
//  m_Dictionary and any other public members of this class through the
//  global MeExampleWizardaw.  (Its definition is in MeExampleWizardaw.cpp.)
extern CMeExampleWizardAppWiz MeExampleWizardaw;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MEEXAMPLEWIZARDAW_H__93C9B621_ACEB_4AA9_AAE3_47B0C352F3A0__INCLUDED_)

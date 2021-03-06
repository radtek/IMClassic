// Sentry Spelling Checker Engine
// CCheckStringDlg: Example showing how to spell-check CStrings
//	interactively

// Copyright (c) 2000 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed
// by a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: CheckStringDlg.cpp,v 5.14 2000/06/28 18:01:15 wsi Exp wsi $

#include <afxwin.h>
#include <afxcmn.h>
#include <sscemfc.hpp>
#include "CheckStringDlg.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(CCheckStringDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECKSPELLING, OnCheckSpelling)
END_MESSAGE_MAP()

CCheckStringDlg::CCheckStringDlg(CWnd *parentWnd) :
  CDialog(IDD_CHECKSTRING, parentWnd) {
	// Do nothing.
}

BOOL CCheckStringDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	CString sampleText(_T("Alice was begining to get very tired of sittign by her sister on the "
	  "bank, and of having nothing to do: once or twice she had peeped into the "
	  "book her sister was reading, but it had no pitures or conversatoins in "
	  "it, \"and what is the use of a book,\" thought Alice \"without pictures or "
	  "conversation?\"\r\n\r\n"
	  "So she was considerring in her own mind (as well as she could, for the "
	  "hot day made her feel very sleepy and stuppid), whether the plesure of "
	  "making a daisy-chain would be worth the trouble of getting up and "
	  "picking the daisys, when sudenly a White Rabbit with pink eyes ran "
	  "close by her."));

	CEdit *editCtrl = (CEdit *)GetDlgItem(IDC_EDIT1);
	editCtrl->SetWindowText(sampleText);
	editCtrl->SetSel(0, 0);

	return (TRUE);
}

void CCheckStringDlg::OnCheckSpelling() {
	// Load the contents of the edit control into a CString.
	CEdit *editCtrl = (CEdit *)GetDlgItem(IDC_EDIT1);
	CString text;
	editCtrl->GetWindowText(text);

	// Interactively check the spelling of the CString.
	// The CString's contents will be displayed so the user can see
	// errors in context. Any corrections made by the user will update
	// the CString.
	CSentrySpellDlg sentry(this);
	SSCE_S32 rv = sentry.Check(text, TRUE);
	if (rv >= 0) {
		// Put the corrected string back into the edit control.
		editCtrl->SetWindowText(text);
	}
	else {
		CString msg;
		msg.Format(_T("Check returned %d"), rv);
	}
}

/*----------------------------------------------------------------------
	Copyright � 2001-2010 JRS Technology, Inc.
-----------------------------------------------------------------------*/

#if !defined(AFX_SETUSER_H__EA4D9861_066C_4DEE_B9FC_FC20B95DBDF1__INCLUDED_)
#define AFX_SETUSER_H__EA4D9861_066C_4DEE_B9FC_FC20B95DBDF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetUser.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetUser recordset

class CSetUser : public CRecordset
{
public:
	CSetUser(CDatabase* pDatabase);
	DECLARE_DYNAMIC(CSetUser)

// Field/Param Data
	//{{AFX_FIELD(CSetUser, CRecordset)
	CString	m_name;
	int		m_uid;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetUser)
	public:
	virtual CString GetDefaultConnect();    // Default connection string
	virtual CString GetDefaultSQL();    // Default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);  // RFX support
	//}}AFX_VIRTUAL

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUSER_H__EA4D9861_066C_4DEE_B9FC_FC20B95DBDF1__INCLUDED_)

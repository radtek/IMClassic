// SetPOMarketDivision.h : Declaration of the CSetPOMarketDivision

#pragma once

// code generated on Thursday, January 16, 2003, 7:45 PM

class CSetPOMarketDivision : public CRecordset
{
public:
	CSetPOMarketDivision(CDatabase* pDatabase);
	DECLARE_DYNAMIC(CSetPOMarketDivision)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	CStringA	m_PurchaseOrderNumber;
	CStringA	m_Division;
	CStringA	m_Market;

// Overrides
	// Wizard generated virtual function overrides
	public:
	virtual CString GetDefaultSQL(); 	// default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);	// RFX support

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

};



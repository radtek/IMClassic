#include "stdafx.h"
#include "Flooring.h"
#include ".\gridchecklist.h"
#include ".\SetChecks.h"
#include "DlgCheckNew.h"
#include "DialogChecking.h"
#include "SetChecks.h"

CGridCheckList::CGridCheckList(void)
{
	HighlightCurrentRow();
}

CGridCheckList::~CGridCheckList(void)
{
}

void CGridCheckList::OnSetup()
{
	CCFGrid::OnSetup() ;

	AddColumn("     ID     ", ID);
	AddColumn(" Check Number ", CHECK_NUMBER);
	AddColumn("     Date     ", CHECK_DATE);
	AddColumn("     Amount     ", CHECK_AMOUNT);
//	AddColumn("     Vendor ID     ", VENDOR_ID);
	AddColumn("     QBTxnID     ", QBTXN_ID);

	InitColumnHeadings();

	EnableMenu(TRUE) ;
	EnableExcelBorders(TRUE) ;

	CUGCell cell;

	GetColDefault(ID, &cell) ;
	cell.SetNumberDecimals(0) ;
	SetColDefault(ID, &cell) ;

	GetColDefault(CHECK_AMOUNT, &cell) ;
	cell.SetNumberDecimals(2) ;
	cell.SetDataType(UGCELLDATA_NUMBER) ;
	cell.SetFormatClass(&m_formatCurrency) ;
	SetColDefault(CHECK_AMOUNT, &cell) ;
}

void CGridCheckList::Update()
{
	CWaitCursor cur ;
	EnableUpdate(FALSE);
	
	// get rid of stale data
	while(GetNumberRows())
	{
		DeleteRow(0) ;
	} ;

	CSetChecks setCheck(&g_dbFlooring);
	setCheck.m_strSort = "CheckDate DESC";
	setCheck.Open() ;
	CString strTemp;
	while (!setCheck.IsEOF())
	{
		AppendRow() ;
		long lRow = this->GetNumberRows() - 1 ;

		CString strID ;
		strID.Format("%d", setCheck.m_CheckID) ;
		QuickSetText(ID, lRow, strID) ;

		QuickSetText(CHECK_DATE, lRow, setCheck.m_CheckDate.Format("%Y-%m-%d")) ;

		QuickSetText(CHECK_AMOUNT, lRow, setCheck.m_Amount) ; 
		QuickSetText(CHECK_NUMBER, lRow, setCheck.m_CheckNumber) ; 

//		strTemp.Format("%s - %s", setCheck.m_Description, setCheck.m_VendorNumber);
//		QuickSetText(VENDOR_ID, lRow, strTemp);
		QuickSetText(QBTXN_ID, lRow, setCheck.m_QBTxnId);

		setCheck.MoveNext() ;
	}
	setCheck.Close() ;
	EnableUpdate(TRUE);
	RedrawAll() ;
}

int CGridCheckList::OnMenuStart(int /* col */, long row, int section)
{
	if (section == UG_GRID)
	{
		//* Empty the Menu!!
		EmptyMenu();

		if ((row >= 0) && (row < this->GetNumberRows()))
		{
			AddMenuItem(EDIT_CHECK, "Edit") ;
			AddMenuItem(EDIT_CHECK_DETAILS, "Edit Check Details") ;
			AddMenuItem(PRINT_CHECK, "Print Check Report") ;
			AddMenuItem(VIEW_CHECK, "View Check Report") ;
			AddMenuItem(NEW_CHECK, "New") ;
			CString test = QuickGetText(QBTXN_ID, row);
			AddMenuItem(ADD_QUICKBOOKS, "Add to Quickbooks", strlen(test) != 0);
		}
	}
	return TRUE ;
}

void CGridCheckList::OnMenuCommand(int /* col */, long row, int section, int item)
{
	if (section == UG_GRID)
	{
		switch (item)
		{
		case EDIT_CHECK:
			EditCheck(row) ;
			break ;

		case EDIT_CHECK_DETAILS:
			EditCheckDetail(row) ;
			break ;

		case NEW_CHECK:
			NewCheck() ;
			break ;

		case PRINT_CHECK:
			PrintCheck(row);
			break;

		case VIEW_CHECK:
			ViewCheck(row);
			break;

		case ADD_QUICKBOOKS:
			AddToQuickbooks(row);
			break;

		default:
			break ;
		}
	}
}

void CGridCheckList::OnSH_DClicked(int /* col */,long row, RECT * /* rect */, POINT * /* point */, BOOL /* processed */)
{
	EditCheckDetail(row) ;
}

void CGridCheckList::OnDClicked(int /* col */,long row, RECT * /* rect */, POINT * /* point */, BOOL /* processed */)
{
	EditCheckDetail(row) ;
}

void CGridCheckList::PrintCheck(long row)
{
	CString strId = QuickGetText(ID, row) ;
	int iId = atoi(strId) ;
	CGlobals::PrintCheck(iId) ;
}

void CGridCheckList::ViewCheck(long row)
{
	CString strId = QuickGetText(ID, row) ;
	int iId = atoi(strId) ;
	CGlobals::ViewCheck(iId) ;
}

void CGridCheckList::EditCheck(long row)
{
	CString strId = QuickGetText(ID, row) ;
	int iId = atoi(strId) ;

	CDlgCheckNew dlg ;
	dlg.SetCheckID(iId) ;
	if (dlg.DoModal() == IDOK)
	{

		CSetChecks setCheck(&g_dbFlooring) ;
		setCheck.m_strFilter.Format("[CheckID] = '%d'", iId) ;
		setCheck.Open() ;
		if (!setCheck.IsEOF())
		{
			QuickSetText(CHECK_DATE, row, setCheck.m_CheckDate.Format("%Y-%m-%d")) ;
			QuickSetText(CHECK_AMOUNT, row, setCheck.m_Amount) ; 
			QuickSetText(CHECK_NUMBER, row, setCheck.m_CheckNumber) ;
		}
		setCheck.Close() ;
		this->RedrawRow(row) ;
	}
}
void CGridCheckList::AddToQuickbooks(long row)
{
	CWaitCursor curWait;

	CString strId = QuickGetText(ID, row);
	int iId = atoi(strId);
	if (CGlobals::AddCheckToQuickbooks(iId))
	{
		QuickSetText(QBTXN_ID, row, "DONE");
	}
	else
	{
		MessageBox("Unable to add to Quickbooks.", "Error");
	}
}

void CGridCheckList::EditCheckDetail(long row )
{
	CString strId = QuickGetText(ID, row) ;
	int iId = atoi(strId) ;

	CDialogChecking dlg(iId) ;
	dlg.DoModal() ;
}

void CGridCheckList::NewCheck()
{
	CDlgCheckNew dlg ;
	if (dlg.DoModal() == IDOK)
	{
		Update() ;
		
		CDialogChecking dlg2(dlg.GetCheckID()) ;
		dlg2.DoModal() ;
	}
}

bool CGridCheckList::IsRowDirty(long lRow)
{
	UNREFERENCED_PARAMETER(lRow);
	return false;
}

bool CGridCheckList::IsColumnDirty(int iCol, long lRow)
{
	UNREFERENCED_PARAMETER(lRow);
	UNREFERENCED_PARAMETER(iCol);
	return false;
}
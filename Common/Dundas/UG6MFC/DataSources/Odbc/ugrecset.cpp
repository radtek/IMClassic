// ugrecset.cpp : implementation file
//
// (c) Dundas Software Ltd. 1994-1999

#include "stdafx.h"

#include "ugrecset.h"


//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif


/***************************************************
****************************************************/
CUGRecordset::CUGRecordset(CDatabase* data) : CRecordset(data)
{
	m_pSQLColumnInfoList = NULL;
	m_defaultConnect = "";
}
/***************************************************
****************************************************/
CUGRecordset::~CUGRecordset(){
	CleanUp();

}
/***************************************************
****************************************************/
CString CUGRecordset::GetDefaultSQL(){
	ASSERT_VALID((CObject*)this);

	// Override and add table name or entire SQL SELECT statement
	return _T("");
}
/***************************************************
****************************************************/
void CUGRecordset::CleanUp()
{
	// need to delete all the var objects allocated
	// during BindFields...
	int sz;
	SQLColumnInfo * cfi;

	if(m_pSQLColumnInfoList != NULL) 
	{
		sz = m_pSQLColumnInfoList->GetSize();
		for (int i = 1; i < sz; i++)
		{
			cfi = (SQLColumnInfo *) m_pSQLColumnInfoList->GetAt(i);
			if (cfi->pValue != NULL)
			{
				// ok - figure out what it was we allocated...
				switch (cfi->nSQLType)
				{
				case SQL_CHAR:
				case SQL_VARCHAR:
				case SQL_LONGVARCHAR:
					delete (CString*) cfi->pValue;
					break;
				// boolean
				case SQL_BIT:
					delete (BOOL*) cfi->pValue;
					break;
				// integer
				case SQL_TINYINT:
					delete (unsigned char*) cfi->pValue;
					break;
				case SQL_SMALLINT:
					delete (int *) cfi->pValue;
					break;
				case SQL_INTEGER:
					delete (long *) cfi->pValue;
					break;
				case SQL_BIGINT:
					delete (double *) cfi->pValue;
					break;
				// floating point 
				case SQL_NUMERIC:
				case SQL_DECIMAL:
				case SQL_REAL:
				case SQL_FLOAT:
					delete (float *) cfi->pValue;
					break;
				case SQL_DOUBLE:
					delete (double *) cfi->pValue;
					break;
				// binary 
				case SQL_BINARY:
				case SQL_VARBINARY:
				case SQL_LONGVARBINARY:		
					delete (CByteArray *) cfi->pValue;
					break;
				// date and time
				case SQL_DATE:
				case SQL_TIME:
				case SQL_TIMESTAMP:
					delete (TIMESTAMP_STRUCT *) cfi->pValue;
					break;
				}

				delete cfi;
			}
		}

		delete m_pSQLColumnInfoList;
		m_pSQLColumnInfoList = NULL;
	}
}
/***************************************************
****************************************************/
void CUGRecordset::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CUGRecordset)
	//}}AFX_FIELD_MAP 
	pFX->SetFieldType( CFieldExchange::outputColumn );
	
	for(UINT i=0;i < m_nFields; i++)
	{
		SQLColumnInfo*	cfi;
		cfi = (SQLColumnInfo *) m_pSQLColumnInfoList->GetAt(i);

		switch(cfi->nSQLType)
		{
		case SQL_NUMERIC:
		case SQL_DECIMAL:
		case SQL_REAL:
		case SQL_FLOAT:
			RFX_Single(pFX, (const char *) cfi->strName,
							*(float *) cfi->pValue); 
			break;
		case SQL_DOUBLE:
			RFX_Double(pFX, (const char *) cfi->strName,
							*(double *) cfi->pValue); 
			break;
		// strings...
		case SQL_CHAR:
		case SQL_VARCHAR:
			RFX_Text(pFX, (const char *) cfi->strName,
							*(CString *) cfi->pValue);
			break;
		case SQL_LONGVARCHAR:
			RFX_Text(pFX, (const char *) cfi->strName,
							*(CString *) cfi->pValue, UG_ODBC_MAXTEXT-1, cfi->nSQLType);// cfi->nScale);
			break;
		// integers
		case SQL_BIT:
			RFX_Bool(pFX, (const char *) cfi->strName,
							*(BOOL *) cfi->pValue); 
			break;					
		case SQL_TINYINT:
			RFX_Byte(pFX, (const char *) cfi->strName,
							*(unsigned char *) cfi->pValue); 
			break;					
		case SQL_SMALLINT:
		case SQL_IS_SMALLINT:
			RFX_Int(pFX, (const char *) cfi->strName,
							*(int *) cfi->pValue); 
			break;					
		case SQL_INTEGER:
			RFX_Long(pFX, (const char *) cfi->strName,
							*(long *) cfi->pValue); 
			break;					
		case SQL_BIGINT:
			RFX_Double(pFX, (const char *) cfi->strName,
							*(double *) cfi->pValue); 
			break;					
		// binary 
		case SQL_BINARY:
		case SQL_VARBINARY:
		case SQL_LONGVARBINARY:
			RFX_Binary(pFX, (const char *) cfi->strName,
							*(CByteArray *) cfi->pValue,UG_ODBC_MAXBINARY ); 
			break;					
		// date time
		case SQL_DATE:
		case SQL_TIME:
		case SQL_TIMESTAMP:
			RFX_Date(pFX, (const char *) cfi->strName,
							*(TIMESTAMP_STRUCT *) cfi->pValue); 
			break;					
		// uh oh...
		default:
			ASSERT(0);
			break;
		}
	}
}
/***************************************************
****************************************************/
BOOL CUGRecordset::Open(UINT nOpenType, LPCSTR lpszSQL, DWORD dwOptions)
{
	Close();
	char    func[80];
	RETCODE retcode = 0;

	TRACE("%s\n", lpszSQL);

	CleanUp();	// deallocate bindings

	// first we execute in order to get info to bind the fields.
	// then, we call open in CRecordset, with our binding in place
	try {
		if(!AllocHstmt())
			ASSERT(0);
		retcode = SQLExecDirect(m_hstmt,(UCHAR*)lpszSQL,strlen((const char*)lpszSQL));
	}
	catch(CException* e) {
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO){
		strcpy (func, "SQLExecDirect");
		ReportSQLError(retcode, func, m_hstmt);
	
		SQLFreeStmt(m_hstmt, SQL_CLOSE);

		return FALSE;
	}


	if(!BindFields(m_hstmt))		// allocates storage etc
		return FALSE;

	SQLFreeStmt(m_hstmt, SQL_CLOSE);
	Close();

	//	m_Database->SetSynchronousMode(TRUE);	//obsolete in MFC 4.2


	try {
		CRecordset::Open(nOpenType,lpszSQL,dwOptions);
	}
	catch(CMemoryException* e){
		#ifdef UG_ODBC_REPORTERROR
			e->ReportError();
		#endif
		e->Delete();
		AfxAbort();
	}
	catch(CDBException* e){
		int er = e->m_nRetCode;
 		TRACE(_T("CDBException %d, caught at line %d in %s:\n'%s'\n"), 
				e->m_nRetCode,__LINE__,"ugrecset.cpp", e->m_strError);
		#ifdef UG_ODBC_REPORTERROR
			char buf[20];
			wsprintf(buf,"Error %d",er);
			AfxMessageBox(buf);
			e->ReportError();
		#endif
		e->Delete();
		return FALSE;
	}
	catch(CFileException* e) {
		int er = e->m_cause;
		#ifdef UG_ODBC_REPORTERROR
			char buf[20];
			wsprintf(buf,"Error %d",er);
			AfxMessageBox(buf);
			e->ReportError();
		#endif
		e->Delete();
		return FALSE;
	}
	
	return TRUE;
}
/***************************************************
****************************************************/
CString CUGRecordset::GetDefaultConnect()
{
	if(m_pDatabase == NULL)
		return "ODBC:";

	if (!m_defaultConnect.IsEmpty()) {
		return m_defaultConnect;
	} 
	else {
	
		CString dataName = m_pDatabase->GetDatabaseName();
		int backslash = dataName.ReverseFind('\\');
		m_dataFileName ="";
	
		for(int x=backslash+1;x<dataName.GetLength();x++)
			m_dataFileName += dataName[x];

		char buf[255];
		wsprintf(buf,"ODBC;DSN=%s",m_dataFileName);
		return buf;
	}
}
/***************************************************
****************************************************/
/////////////////////////////////////////////////////////////////////////////
// CUGRecordset diagnostics
#ifdef _DEBUG
void CUGRecordset::AssertValid() const
{
	CRecordset::AssertValid();
}
/***************************************************
****************************************************/
void CUGRecordset::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG


/***************************************************
****************************************************/
BOOL CUGRecordset::BindFields(HSTMT hstmt) {
	// given a valid statement handle, gather information
	// and allocate space for the RFX function

	UCHAR 	szColName[100];		// returned from SQLDescribeCol...
	SWORD 	cbColName;
	SWORD 	swSqlType;
	UDWORD	cbColDef;
	SWORD	swScale;
	SWORD	swNullable;

	SWORD	swFieldCount;		// ...from SQLNumResultCols
		
	RETCODE retcode;
	
	ASSERT(hstmt);

	// get number of fields
	retcode = ::SQLNumResultCols(hstmt, &swFieldCount);

	if (retcode != SQL_SUCCESS) {
		ReportSQLError(retcode,"BindFields");
		return FALSE;
	}

	// allocate enough room for field info structures
	// in list of pointers to field info structures...

	m_pSQLColumnInfoList = new CPtrArray();
	m_pSQLColumnInfoList->SetSize(swFieldCount);
	
	m_nFields = swFieldCount;

	// determine if the SQLColAttributes call is supported before 
	// entering loop - sqlcaSupported will be TRUE or FALSE.
	UWORD sqlcaSupported;
	::SQLGetFunctions(m_pDatabase->m_hdbc, SQL_API_SQLCOLATTRIBUTES, &sqlcaSupported);

	for(int i = 1; i <= swFieldCount; i++) {

		// get the ODBC 1.0 stuff...
		retcode = ::SQLDescribeCol(hstmt, i, szColName, 100, &cbColName, &swSqlType,
									&cbColDef, &swScale, &swNullable);

		if(retcode != SQL_SUCCESS) {
			ReportSQLError(retcode, "BindFields");
			return FALSE;
		}

		// create a field info struct to hold info
		SQLColumnInfo*	cfi = new SQLColumnInfo;
		

		cfi->strName = szColName;
		cfi->nSQLType = swSqlType;
		cfi->nPrecision = cbColDef;
		cfi->nScale = swScale;
		cfi->nNullable = swNullable;

		// ok - this moves into ODBC 2.0 and 3.0 territory
		// extra info if available!
		if(sqlcaSupported) {
			// some support vars
			UCHAR		szCharAttr[256];
			SQLSMALLINT nStringLength;
			SQLINTEGER	nNumericAttr;

			SQLRETURN	retCode;

			// is there a label beyond column name?
			// We already have col name or alias, if
			// returned by SQLDescribeCol...
			retCode = ::SQLColAttribute(m_hstmt, i, 
						SQL_DESC_LABEL, 
						szCharAttr,			// label returned here
						255,				// length of buffer
						&nStringLength,		// length of result
						NULL				// (nNumericAttr)
						);					
			
			if(nStringLength)
				cfi->strLabel = szCharAttr;

			// next lets find out if the field is auto increment
			retcode = ::SQLColAttribute(m_hstmt, i,
						SQL_DESC_AUTO_UNIQUE_VALUE,
						NULL,				// sz buf not needed
						0,
						0,
						&nNumericAttr		// return val here
						);

			if(SQL_SUCCESS == retCode) {
				cfi->nAutoValue = nNumericAttr;
			}
			else {
				cfi->nAutoValue = FALSE;	// don't know...
				TRACE("Warning: AutoIncrement status unknown for field %d\n", i);
			}

			// lastly, for now, is the field updatable?
			retcode = ::SQLColAttribute(m_hstmt, i,
						SQL_DESC_UPDATABLE,
						NULL,				// sz buf not needed
						0,
						0,
						&nNumericAttr		// return val here
						);

			if(SQL_SUCCESS == retCode) {
				cfi->nUpdatable = nNumericAttr;
			}
			else {
				cfi->nUpdatable = TRUE;	// don't know...
				TRACE("Warning: Updatable status unknown for field %d\n", i);
			}

		}		


		////////////////////////////////////////////////////////
		// initialize storage for fields
		switch(swSqlType){
		
		// character types
		case SQL_CHAR:
		case SQL_VARCHAR:
		case SQL_LONGVARCHAR:
			cfi->pValue = (void *) new CString;
			*((CString *) cfi->pValue)->GetBuffer(UG_ODBC_MAXTEXT);
			*((CString *) cfi->pValue) = "";
		break;

		// boolean
		case SQL_BIT:
			cfi->pValue = (void *) new BOOL;
			*((BOOL *) cfi->pValue) = FALSE;
		break;

		// integer
		case SQL_TINYINT:
			cfi->pValue = (void *) new unsigned char;
			*((unsigned char *) cfi->pValue) = '\0';
		break;
		case SQL_SMALLINT:
			cfi->pValue = (void *) new int;
			*((int *) cfi->pValue) = 0;
		break;
		case SQL_INTEGER:
			cfi->pValue = (void *) new long;
			*((long *) cfi->pValue) = 0;
		break;
		case SQL_BIGINT:
			cfi->pValue = (void *) new double;
			*((double *) cfi->pValue) = 0;
		break;

		// floating point 
		case SQL_NUMERIC:
		case SQL_DECIMAL:
		case SQL_REAL:
		case SQL_FLOAT:
			cfi->pValue = (void *) new float;
			*((float *) cfi->pValue) = (float) 0;
		break;
		case SQL_DOUBLE:
			cfi->pValue = (void *) new double;
			*((double *) cfi->pValue) = 0.0;
		break;
		
		// binary 
		case SQL_BINARY:
		case SQL_VARBINARY:
		case SQL_LONGVARBINARY:
			cfi->pValue = (void *) new CByteArray;
			((CByteArray *) cfi->pValue)->SetSize(UG_ODBC_MAXBINARY);
		break;
		// date and time.  CString limited, as is OLEDate variant.
		// Though there are DATE_ and TIME_STRUCTs, RFX_DATE uses
		// TIMESTAMP_STRUCT so all these mapped to same...
		case SQL_DATE: 
		case SQL_TIME:
		case SQL_TIMESTAMP:
			cfi->pValue = (void *) new TIMESTAMP_STRUCT;
			memset(cfi->pValue, 0, sizeof(TIMESTAMP_STRUCT));
			break;
		break;

		// oops...
		default:
			//ASSERT(0);
			return FALSE;
		break;
		}
		
		m_pSQLColumnInfoList->SetAt(i-1,cfi);
	}

	return TRUE;
}

/***************************************************
****************************************************/
void CUGRecordset::ReportSQLError(RETCODE retcode, LPCSTR func, HSTMT hstmt) {

	char  buf[512];

	UCHAR FAR  sqlerror[255];
	UCHAR FAR  sqlstate[255];
	SDWORD nerr;
	SWORD num;
	RETCODE rc;

	switch (retcode) {
	case SQL_SUCCESS: {

		//AfxMessageBox("SQLExecDirect returned success ");
		//AfxMessageBox((LPCSTR)buf);
		break;
					  }
	case SQL_SUCCESS_WITH_INFO:{
		//AfxMessageBox("SQLExecDirect returned success with info");
		// this indicates that there is a driver specific warning
		// to be investigated...
		rc = SQLError(SQL_NULL_HENV,m_pDatabase->m_hdbc,SQL_NULL_HSTMT,sqlstate,
							&nerr,sqlerror,254,&num);

		if (rc == SQL_NO_DATA_FOUND || rc == SQL_ERROR)
			wsprintf(buf,"An unknown error occurred in %s", func);
		else 
			wsprintf(buf,"SQL_ERROR retcode in function:%s  SqlState: %s\nError: %s", func, sqlstate, sqlerror);
		

		AfxMessageBox((LPSTR)buf);
		break;
							   }
	case SQL_NEED_DATA: {
		wsprintf(buf,"%s returned SQL_NEED_DATA", func);
		AfxMessageBox((LPSTR)buf);
		break;
						}
	case SQL_STILL_EXECUTING: {
		wsprintf(buf,"%s  returned SQL_STILL_EXECUTING", func);
		AfxMessageBox((LPSTR)buf);
		break;
							  }
	case SQL_ERROR:{
		rc = SQLError(SQL_NULL_HENV,m_pDatabase->m_hdbc,hstmt,sqlstate,
							&nerr,sqlerror,254,&num);
		if (rc == SQL_NO_DATA_FOUND || rc == SQL_ERROR)
			wsprintf(buf,"An unknown error occurred in %s", func);
		else 
			wsprintf(buf,"SQL_ERROR retcode in function:%s  SqlState: %s\nError: %s", func, sqlstate, sqlerror);
		
		AfxMessageBox((LPSTR)buf);

		break;
				   }
	case SQL_INVALID_HANDLE:{
		wsprintf(buf,"%s returned SQL_INVALID_HANDLE", func);
		AfxMessageBox((LPSTR)buf);
		break;
							}
	default:
		break;
	}

}


SQLColumnInfo* CUGRecordset::GetFieldInfo(int col){
	// return a pointer to the field info structure for this column
	if (col <= (int)m_nFields)
		return (SQLColumnInfo *) m_pSQLColumnInfoList->GetAt(col);
	else
		return NULL;
}



//
//
//                                IBM INC.
//
//                           PROPRIETARY DATA
//
// Licensed Material - Property Of IBM
//
// "Restricted Materails of IBM"
//
// IBM Informix Client SDK
//
// (c)  Copyright IBM Corporation 1997, 2004. All rights reserved.
//  Author             : Satyan
//  Last Modified      : March/05/2002
///////////////////////////////////////////////////////////////////////////




// CLSID For IBM-Informix Client Side OLE DB Provider
const GUID CLSID_IFXOLEDBC = {0xa6d00422, 0xfd6c, 0x11d0,
 { 0x80, 0x43, 0x0, 0xa0, 0xc9, 0xf, 0x1c, 0x59 } };




class DbConnect
{
public:
	DbConnect( void );
	
    HRESULT MyOpenDataSource( const CHAR *DsnName, const CHAR *ConnStr);
    HRESULT DbConnect::MyOpenDataSource( const CHAR *DsnName, const CHAR *UserName, const CHAR *PassWord );

    HRESULT MySetAllProperties( 
   					const CHAR *DsnName, 
                    const CHAR *UserName, 
                    const CHAR *PassWord, 
                    const CHAR *ConnStr);

	HRESULT MyCloseDataSource( void );
	HRESULT MyCreateSession( void );
	HRESULT MyDeleteSession( void );
	HRESULT MyCreateCmd( void );
	HRESULT MyDeleteCmd( void );
	HRESULT MyExecuteImmediateCommandText( WCHAR *wSQLcmd );


private:

    HRESULT MyOpenDataSource( REFCLSID ClassID, 
					const CHAR *DsnName, 
                    const CHAR *UserName, 
                    const CHAR *PassWord,
                    const CHAR *ConnStr);

	IDBInitialize		*pIDBInitialize;
	IUnknown			*pSession;
	ITransactionJoin	*pITransactionJoin;
	ICommandText		*pCommandText;
    BOOL UseConnStrFlag;
};



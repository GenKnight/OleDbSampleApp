#include "OledbConnect.h"
#include "DbConnect.h"

extern BOOL  UseConnStrFlag;


DbConnect::DbConnect( void )
{
    pIDBInitialize		= NULL;
    pITransactionJoin	= NULL;
    pSession			= NULL;
    pCommandText	= NULL;
    UseConnStrFlag  = FALSE;
}

HRESULT DbConnect::MyOpenDataSource(
                   const CHAR *DsnName,
                   const CHAR *ConnStr)
{
    HRESULT hr = S_OK;

    UseConnStrFlag = FALSE;
    hr = MyOpenDataSource( (REFCLSID) CLSID_IFXOLEDBC, DsnName, NULL, NULL, ConnStr);

    return(hr);
}


HRESULT DbConnect::MyOpenDataSource(
					const CHAR *DsnName,
                    const CHAR *UserName,
                    const CHAR *PassWord )
{
    HRESULT hr = S_OK;
    const CHAR *ConnStr = "Provider=Ifxoledbc;Data Source=db1@ids5;User ID=informix;Password=xyz;DB_LOCALE=en_us.8859-1;CLIENT_LOCALE=es_ES.1252;";

    UseConnStrFlag = FALSE;
    if ( UseConnStrFlag == FALSE )
        ConnStr = NULL;

    hr = MyOpenDataSource( (REFCLSID) CLSID_IFXOLEDBC, DsnName, UserName, PassWord, ConnStr);

    return(hr);
}


HRESULT DbConnect::MyOpenDataSource(REFCLSID ClassID,
					const CHAR *DsnName,
                    const CHAR *UserName,
                    const CHAR *PassWord,
                    const CHAR *ConnStr)
{
    HRESULT hr = S_OK;

    // Instantiate a data source object
    if(FAILED(hr = CoCreateInstance(ClassID, NULL,
			    CLSCTX_INPROC_SERVER, IID_IDBInitialize,
			    (void **) &pIDBInitialize)))
    {
	    pIDBInitialize = NULL;
	    printf( "\n Failed: CoCreateInstance( IID_IDBInitialize )");
	    return( hr );
    }


    // Set all Properties
    if( FAILED(hr = MySetAllProperties( DsnName,
                            UserName, PassWord, ConnStr ) ) )
    {
	    printf( "\nFailed : MySetAllProperties()");
	    return( hr );
    }

    // Connect to the Database Server
    if (FAILED(hr = pIDBInitialize->Initialize()))
    {
	    printf( "\nFailed : pIDBInitialize->Initialize()");
	    return( hr );
    }

    return(hr);
}


HRESULT DbConnect::MyCloseDataSource( void )
{
    HRESULT	hr = S_OK;

    if ( pIDBInitialize != NULL )
    {
        hr = pIDBInitialize -> Uninitialize();
        hr = pIDBInitialize -> Release();
        pIDBInitialize = NULL;
    }
    return( hr );
}


HRESULT DbConnect::MySetAllProperties(
					const CHAR *DsnName,
                    const CHAR *UserName,
                    const CHAR *PassWord,
                    const CHAR *ConnStr)
{
    const ULONG		nProps = 4;
    IDBProperties*	pIDBProperties;
    DBPROP			InitProperties[nProps];
    DBPROPSET		rgInitPropSet;
    HRESULT			hr = S_OK;

    // Construct BSTR String
    _bstr_t			bstrDsnName = DsnName;
    _bstr_t			bstrUserName = UserName;
    _bstr_t			bstrPassWord = PassWord;
    _bstr_t			bstrConnStr = ConnStr;


    if ( pIDBInitialize == NULL )
    {
        return( S_FALSE );
    }

    // Initialize common property options.
    for (ULONG i = 0; i < nProps; i++ )
    {
        VariantInit(&InitProperties[i].vValue);
        InitProperties[i].dwOptions = DBPROPOPTIONS_REQUIRED;
        InitProperties[i].colid = DB_NULLID;
    }

    // Level of prompting that will be done to complete the
    // connection process
    InitProperties[0].dwPropertyID = DBPROP_INIT_PROMPT;
    InitProperties[0].vValue.vt = VT_I2;
    InitProperties[0].vValue.iVal = DBPROMPT_NOPROMPT;

    if( UseConnStrFlag ) // TODO: yet to test
    {
        int i = 0;

        // Data source name
        ++i;
        InitProperties[i].dwPropertyID = DBPROP_INIT_DATASOURCE;
        InitProperties[i].vValue.vt = VT_BSTR;
        InitProperties[i].vValue.bstrVal = bstrDsnName;

        // ConnStr
        ++i;
        InitProperties[i].dwPropertyID = DBPROP_INIT_PROVIDERSTRING;
        InitProperties[i].vValue.vt = VT_BSTR;
        InitProperties[i].vValue.bstrVal = bstrConnStr;

        rgInitPropSet.guidPropertySet = DBPROPSET_DBINIT;
        rgInitPropSet.cProperties = i + 1; //nProps-1;
        rgInitPropSet.rgProperties = InitProperties;
    }
    else
    {
        int i = 0;

        // Data source name
        ++i;
        InitProperties[i].dwPropertyID = DBPROP_INIT_DATASOURCE;
        InitProperties[i].vValue.vt = VT_BSTR;
        InitProperties[i].vValue.bstrVal = bstrDsnName;

        // User ID
        ++i;
        InitProperties[i].dwPropertyID = DBPROP_AUTH_USERID;
        InitProperties[i].vValue.vt = VT_BSTR;
        InitProperties[i].vValue.bstrVal = bstrUserName;

        // Password
        ++i;
        InitProperties[i].dwPropertyID = DBPROP_AUTH_PASSWORD;
        InitProperties[i].vValue.vt = VT_BSTR;
        InitProperties[i].vValue.bstrVal = bstrPassWord;

        rgInitPropSet.guidPropertySet = DBPROPSET_DBINIT;
        rgInitPropSet.cProperties = i+1; // nProps;
        rgInitPropSet.rgProperties = InitProperties;
    }


    // Get initialization properties.Interface
    hr = pIDBInitialize->QueryInterface(IID_IDBProperties, (void**)
        &pIDBProperties);

    if (FAILED(hr))
    {
        printf( "\nQI for IDBProperties failed!!");
        return( hr );
    }

    hr = pIDBProperties->SetProperties( 1, &rgInitPropSet);

    if (FAILED(hr))
    {
        printf( "\nQI for IDBProperties failed!!");
        return( hr );
    }

    SysFreeString(InitProperties[1].vValue.bstrVal);
    SysFreeString(InitProperties[2].vValue.bstrVal);


    if( !UseConnStrFlag )
    {
        SysFreeString(InitProperties[3].vValue.bstrVal);
    }

    pIDBProperties->Release();

    return (hr);
}


HRESULT DbConnect::MyCreateSession( void )
{
    HRESULT hr = S_OK;
    IDBCreateSession *pCreateSession = NULL; // Session Interface

    if ( pIDBInitialize == NULL )
    {
        return( E_FAIL );
    }

    // Get an Interface for creaiting Session Object
    if (FAILED(hr = pIDBInitialize->QueryInterface(IID_IDBCreateSession,
    								(void **) &pCreateSession)))
    {
        printf("\nQI for IDBCreateSession failed!!");
        return( hr );
    }

    // Create a Session Object
    hr = pCreateSession->CreateSession(NULL, IID_IUnknown,
    							(IUnknown **) &pSession);

    pCreateSession->Release();
    if ( FAILED( hr ) )
    {
        printf( "\nIDBCreateSession->CreateSession failed!!");
        return( hr );
    }

    return(hr);
}

HRESULT DbConnect::MyDeleteSession( void )
{
    HRESULT hr = S_OK;

    hr = pSession->Release();
    pSession = NULL;

    return(hr);
}





HRESULT DbConnect::MyCreateCmd( void )
{
    HRESULT hr = S_OK;
    IDBCreateCommand *pCreateCommand = NULL;

    // pSession must have some valid address
    if ( pSession == NULL )
    {
        return( E_FAIL );
    }


    // Get an Interface for creating Command Object
    if (FAILED(hr = pSession->QueryInterface( IID_IDBCreateCommand,
    								(void **) &pCreateCommand)))
    {
        printf("\nQI for IDBCreateSession failed!!");
        return( hr );
    }

    // Create Command Object and get ICommandText Interface
    hr = pCreateCommand->CreateCommand(NULL, IID_ICommandText,
                (IUnknown **) &pCommandText );


    pCreateCommand->Release();

    return(hr);
}




HRESULT DbConnect::MyDeleteCmd( void )
{
    HRESULT hr = S_OK;

    if ( pCommandText )
    {
        pCommandText->Release();
        pCommandText = NULL;
    }

    return( hr );
}


HRESULT DbConnect::MyExecuteImmediateCommandText( WCHAR *wSQLcmd )
{
    HRESULT hr = S_OK;
    _bstr_t bstrCommand = (WCHAR *)wSQLcmd;


    if ( pCommandText == NULL )
        return( E_FAIL );


    if (FAILED( hr = pCommandText->SetCommandText(DBGUID_DBSQL, bstrCommand)))
    {
        //printf( "\npCommandText->SetCommandText failed!!");
        return( hr );
    }

    // Executing before prepare
    if (FAILED(hr = pCommandText->Execute( NULL, IID_NULL, NULL, NULL, NULL)))
    {
        //printf( "\npCommandText->Execute failed!!");
        return( hr );
    }

    return( hr );
}



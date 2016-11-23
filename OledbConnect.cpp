
#include "OledbConnect.h"
#include "DbConnect.h"
#include <conio.h>
#include <ctype.h>


#define MAX_DATA	1024



int main( int argc, char *argv[] )
{
    /////////////////////////////////////////////////////
    // User has to modify according to his server setting
    BOOL        ConnectOnly = FALSE; //TRUE;
    const char  *UserName = "informix";
    const char  *DsnName = "db1@ids5";
    const char  *PassWord = "xyz";
    //////////////////////////////////////////////////////

    DbConnect   MyDb1;
    HRESULT     hr = S_OK;
    int         tmp  = 0;

    WCHAR wSQLcmd[MAX_DATA];

    CoInitialize( NULL );

    if ( argc == 2 )
    {
        PassWord = argv[1];
    }

    // Create DataSouce Object and Opent A Database Connection
    if (FAILED(hr = MyDb1.MyOpenDataSource( DsnName, UserName, PassWord)) )
    {
    	printf( "\n Connection Failed");
        printf( "\n Credential used are : [DsnName=%s; UserName=%s  PassWord=%s ] \n", DsnName, UserName, PassWord);

        CoUninitialize();
    	return( hr );
    }
    else
    {
        printf( "\n Connection Successful ");
        printf( "\n Credential used are : [DsnName=%s; UserName=%s  PassWord=%s ] \n", DsnName, UserName, PassWord);
    }

    if ( ConnectOnly )
    {
        goto Exit;
    }

    // Create Session Object
    if (FAILED( hr = MyDb1.MyCreateSession() ) )
    {
    	printf( "\nMyCreateSession Failed" );
    	return( hr );
    }


    // Create Command Object
    if (FAILED( hr = MyDb1.MyCreateCmd() ) )
    {
    	printf( "\nMyCreateCmd Failed" );
    	return( hr );
    }

    ///////// Drop the Table if it exist /////
    swprintf( wSQLcmd, sizeof(wSQLcmd)-4, L"DROP TABLE mytmptab1" );
    MyDb1.MyExecuteImmediateCommandText( wSQLcmd );


    ///////// Create A Table (mytmptab1)  /////////
    swprintf( wSQLcmd, (sizeof(wSQLcmd)-4),
            L"CREATE TABLE mytmptab1              \
            (                                   \
                AcNum         INTEGER NOT NULL, \
                Name          CHAR(20),         \
                PRIMARY KEY   (AcNum)           \
            );" );

    if (FAILED( hr = MyDb1.MyExecuteImmediateCommandText( wSQLcmd ) ) )
    {
    	printf( "\nMyExecuteImmediateCommandText Failed" );
    	return( hr );
    }


    ///////// Insert A Record into the Table /////
    swprintf( wSQLcmd, (sizeof(wSQLcmd) - 4),
        L"INSERT INTO mytmptab1   VALUES ( 100, \'John\' );" );

    if (FAILED( hr = MyDb1.MyExecuteImmediateCommandText( wSQLcmd ) ) )
    {
    	printf( "\nMyExecuteImmediateCommandText Failed" );
    	return( hr );
    }

    swprintf( wSQLcmd, (sizeof(wSQLcmd) - 4),
        L"INSERT INTO mytmptab1  VALUES ( 101, \'Tom\' );" );

    if (FAILED( hr = MyDb1.MyExecuteImmediateCommandText( wSQLcmd ) ) )
    {
    	printf( "\nMyExecuteImmediateCommandText Failed" );
    	return( hr );
    }


    tmp = MyDb1.MyDeleteCmd();
    tmp = MyDb1.MyDeleteSession();


Exit:
    tmp = MyDb1.MyCloseDataSource();


    CoUninitialize();

    return(0);
}



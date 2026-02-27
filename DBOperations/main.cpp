#include <iostream>
#include <string>
#include "DB.h"
using namespace std;
//-------------------------------------------------------------------------------------------/
//REQUIREMENTS:
// - ODBC Driver 17 for SQL Server must be installed on the machine running this code (can change these specifics later)
// - The connection string must be correctly configured with the appropriate server, database, user ID, and password
// - The SQL Database must allow connections from the IP address of the machine running this code (check firewall settings for Azure portal)
//-------------------------------------------------------------------------------------------/
//Useful documentation for ODBC: https://github.com/microsoft/VCSamples/blob/master/VC2015Samples/ODBC%20database%20sample%20(windows)/C%2B%2B/odbcsql.cpp

//Notes: sometimes the connection fails even if all of the above are true.
// 	 Message might look like the following: 
// [Microsoft][ODBC Driver 17 for SQL Server]TCP Provider: Timeout error [258]. (SQL State: 08001, Native Error: 258)
//	 my personal fix has been adding a firewall rule for my client and then removing it again lol
//	 this seems to be an issue with the database falling asleep or something but not entirely sure
 
int main() {
	//IMPORTANT: you will need to add your own UID and PASSWORD since this is a public repo
	string connectionString = //ODBC connection string direct from Azure portal 
		"Driver={ODBC Driver 17 for SQL Server};"
		"Server=tcp:rfid309.database.windows.net,1433;"
		"Database={RFID DB};"
		"Uid=REMOVED_USERNAME;"
		"Pwd=REMOVED_PASSWORD;"
		"Encrypt=yes;"
		"TrustServerCertificate=no;"
		"Connection Timeout=60;";

	
	DbConnection db;
	
	if(!dbConnect(db, connectionString)) {
		cerr << "Failed to connect to database. Exiting." << endl;
		return 1; //Exit with error code if connection fails
	}

	//Documentation: https://learn.microsoft.com/en-us/sql/odbc/reference/develop-app/executing-a-statement?view=sql-server-ver17

	SQLHSTMT stmtHandle; //Statement handle for executing SQL commands
	SQLCHAR* Table;

	if (SQLAllocHandle(SQL_HANDLE_STMT, db.connHandle, &stmtHandle) != SQL_SUCCESS)
	{
		print_error("SQLAllocHandle for statement failed", db.connHandle, SQL_HANDLE_DBC);
		return 1;
	}
	//Allocate a statement handle using the connection handle

	SQLRETURN retTables = SQLExecDirectA(stmtHandle, (SQLCHAR*)"SELECT TABLE_NAME FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_TYPE = 'BASE TABLE'", SQL_NTS); //Execute a query to get table names
	SQLCHAR col1[256]; //Buffer to hold the table name 
	SQLLEN col1Len; //Variable to hold the length of the retrieved data

	SQLBindCol(stmtHandle, 1, SQL_C_CHAR, col1, sizeof(col1), &col1Len); //Bind the first column of the result to col1 buffer

	while ((retTables = SQLFetch(stmtHandle)) == SQL_SUCCESS || retTables == SQL_SUCCESS_WITH_INFO) {
		//Fetch each row of the result
		cout << col1 << endl;
	}

	



	dbDisconnect(db); //Disconnect from the database when done
	return 0; //Exit with success code
}
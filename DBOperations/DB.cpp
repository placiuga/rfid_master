#include "DB.h"
#include <iostream>

//-------------------------------------------------------------------------------------------/
//Function: print_error
//Used for debugging to print out the error message from ODBC calls
//Inputs: message: provides error context, handle: the ODBC handle to get the error from,
// type: the type of ODBC handle (e.g., SQL_HANDLE_ENV, SQL_HANDLE_DBC, SQL_HANDLE_STMT)
//Outputs: None (prints error details to standard error stream)
//Sources: https://learn.microsoft.com/en-us/sql/odbc/reference/syntax/sqlgetdiagrec-function?view=sql-server-ver16
//-------------------------------------------------------------------------------------------/
void print_error(const std::string& message, SQLHANDLE handle, SQLSMALLINT type) {
	SQLCHAR sqlState[6] = { 0 }, errorMessage[256] = { 0 }; //These hold the SQL state code and error message retrieved from ODBC
	SQLINTEGER nativeError; //This holds the native error code in int form
	SQLSMALLINT textLength; //This holds the length of the error message text

	if (SQLGetDiagRecA(type, handle, 1, sqlState, &nativeError, errorMessage, sizeof(errorMessage), &textLength) == SQL_SUCCESS) {
		std::cerr << message << ": " << errorMessage << " (SQL State: " << sqlState << ", Native Error: " << nativeError << ")" << std::endl;
	}
	else {
		std::cerr << message << ": Unable to retrieve error details." << std::endl;
	}
	//The above if statement retrieves error details and prints them to the standard error stream.
	// If it fails to retrieve details, it prints a generic message.
}

bool dbConnect(DbConnection& db, const std::string& connectionString) {
	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &db.envHandle); //Allocate an environment handle
	SQLSetEnvAttr(db.envHandle, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0); //Set the ODBC version to 3.0
	SQLAllocHandle(SQL_HANDLE_DBC, db.envHandle, &db.connHandle); //Allocate a connection handle

	SQLCHAR outConnStr[1024]; //This will hold the output connection string that actually gets used
	SQLSMALLINT outLen; //This will hold the length

	//The below call makes an attempt to connect to the database using the connection string.
	//It also retrieves the actual connection string used and its length;
	//if the connection fails, it will print an error message
	auto connection = SQLDriverConnectA(db.connHandle, NULL,
		(SQLCHAR*)connectionString.c_str(), SQL_NTS,
		outConnStr, sizeof(outConnStr), &outLen,
		SQL_DRIVER_NOPROMPT);

	if (!(connection == SQL_SUCCESS || connection == SQL_SUCCESS_WITH_INFO)) {
		print_error("SQLDriverConnectA", db.connHandle, SQL_HANDLE_DBC);
		return false;
	}

	std::cout << "Connected to database successfully!" << std::endl;

	return true;
}

void dbDisconnect(DbConnection& db) {
	if (db.connHandle) {
		SQLDisconnect(db.connHandle); //Disconnect from the database
		SQLFreeHandle(SQL_HANDLE_DBC, db.connHandle); //Free the connection handle
		db.connHandle = NULL; //Set the connection handle to NULL to indicate it's no longer valid
	}
	if (db.envHandle) {
		SQLFreeHandle(SQL_HANDLE_ENV, db.envHandle); //Free the environment handle
		db.envHandle = NULL; //Set the environment handle to NULL to indicate it's no longer valid
	}
	std::cout << "Disconnected from database and cleaned up resources." << std::endl;
}
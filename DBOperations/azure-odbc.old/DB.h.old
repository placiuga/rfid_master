#pragma once
#include <string>
#include <windows.h>
#include <sqlext.h>


//struct to hold ODBC handles (easier to manage)
struct DbConnection {

	SQLHANDLE envHandle = NULL; // ODBC environment handle 
	//In detail: this handle is created to hold an ODBC configuration for the app
	//We set some attributes on it later to specify ODBC 3.0 behavior - it mainly provides this context



	SQLHANDLE connHandle = NULL; // ODBC connection handle
	//In detail: this is a handle to a structure which holds the connection state
	//Identifies a connection (what is the driver, where is the data) and points to internal info
	//Later when the driver connection happens, it represents the connection created by the driver manager
	//When it succeeds, this is further used to track the state of the connection
	//Through this connection, SQL statements can be executed etc
};

//error printer function declaration for use in DB.cpp
void print_error(const std::string& message, SQLHANDLE handle, SQLSMALLINT type);

//connect and disconnect from db
bool dbConnect(DbConnection& db, const std::string& connectionString);
void dbDisconnect(DbConnection& db);

<?php
//NOTE: THIS DOES REQUIRE TWO SPECIAL ENTRIES IN STUDENTTABLE AND MACHINETABLE.
//IT REQUIRES A MACHINE ID '0' AND A STUDENT ID 'DNE' 
//THIS IS SO LOGGING MACHINES & STUDENT IDS THAT DO NOT EXIST CAN OCCUR

$conn = new mysqli("localhost", "root", "", "projecttest");

if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

//Check for all required inputs
if (!isset($_GET['machineID'], $_GET['studentID'], $_GET['action'])) {
    die("Missing parameters");
}

//Set inputs
$machineID = $_GET['machineID'];
$studentID = $_GET['studentID'];
$action = $_GET['action'];

//Check if user exists
$userQuery = "SELECT * FROM StudentTable 
              WHERE StudentID='$studentID'";
$userResult = $conn->query($userQuery);
$userExists = ($userResult && $userResult->num_rows > 0);

$sql = "";

//Check if machine exists
$machineQuery = "SELECT * FROM MachineTable 
              WHERE MachineID='$machineID'";
$machineResult = $conn->query($machineQuery);
$machineExists = ($machineResult && $machineResult->num_rows > 0);

//log errors
$sql = "";

if(!$userExists && !$machineExists) {
    $sql = "INSERT INTO LoggingTable (startTime, endTime, MachineID, StudentID, Notes)
    VALUES (NOW(), NOW(), 'DNE', 'DNE', 'Failed User ID: $studentID, Failed Machine ID: $machineID')";
    $conn->query($sql);
    die("User and Machine do not exist.");
}
else if(!$userExists) {
    $sql = "INSERT INTO LoggingTable (startTime, endTime, MachineID, StudentID, Notes)
    VALUES (NOW(), NOW(), '$machineID', 'DNE', 'Failed User ID: $studentID')";
    $conn->query($sql);
    die("User does not exist.");
}
else if(!$machineExists) {
    $sql = "INSERT INTO LoggingTable (startTime, endTime, MachineID, StudentID, Notes)
    VALUES (NOW(), NOW(), '0', '$studentID', 'Failed Machine ID: $machineID')";
    $conn->query($sql);
    die("Machine does not exist.");
}

$sql = "";

//Check if user is authenticated
$authQuery = "SELECT * FROM AuthenticationTable
             WHERE MachineID = '$machineID' AND StudentID = '$studentID'";
$authResult = $conn->query($authQuery);
$authorized = ($authResult && $authResult->num_rows > 0);

//check if machine or user is already active
$activeQuery = "SELECT * FROM LoggingTable
                WHERE (MachineID='$machineID'
                OR StudentID='$studentID')
                AND endTime IS NULL";

$activeResult = $conn->query($activeQuery);
$activeSessionExists = ($activeResult && $activeResult->num_rows > 0);

//log
if ($authorized && $action == "start") {
    if ($activeSessionExists) {
        die("User or machine already in use.");
    }
    
    echo("Authorization success. Access allowed. ");
    $sql = "INSERT INTO LoggingTable (startTime, MachineID, StudentID, Notes)
            VALUES (NOW(), '$machineID', '$studentID', 'Successful Authorization')";
} 
else if ($authorized && $action == "end") {
    if (!$activeSessionExists) {
        die("No active session to end.");
    }
    
    $sql = "UPDATE LoggingTable 
            SET endTime = NOW()
            WHERE MachineID='$machineID' 
            AND StudentID='$studentID' 
            AND endTime IS NULL
            ORDER BY startTime DESC
            LIMIT 1";
} 
else if (!$authorized) {
    $sql = "INSERT INTO LoggingTable (startTime, endTime, MachineID, StudentID, Notes)
            VALUES (NOW(), NOW(), '$machineID', '$studentID', 'Authorization Failed')";
    echo("Authorization failed. Access denied. ");
}
else {
    die("Invalid action");
}

if ($conn->query($sql) === TRUE) {
    echo "Logged successfully. ";
} else {
    echo "Logging error: " . $conn->error;
}

$conn->close();
?>
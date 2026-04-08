<?php
//NOTE: THIS DOES REQUIRE TWO SPECIAL ENTRIES IN STUDENTTABLE AND MACHINETABLE.
//IT REQUIRES A MACHINE ID '0' AND A STUDENT ID 'DNE' 
//THIS IS SO LOGGING MACHINES & STUDENT IDS THAT DO NOT EXIST CAN OCCUR.
//IT ALSO REQUIRES THE DB TO BE NAMED PROJECTTEST.
//WORKS AT http://localhost/phpmyadmin/index.php

//REQUIRES FILE "rfid_app.conf", on Pi309 this is at /etc/ as expressed below

$config = parse_ini_file('/etc/rfid_app.conf');


$conn = new mysqli(
	$config['DB_HOST'],
	$config['DB_USER'],
	$config['DB_PASS'],
	$config['DB_NAME']
);

if ($conn->connect_error) {
    die("ERROR\nConnection failed: " . $conn->connect_error);
}

//check for parameters
if (!isset($_GET['machineID'], $_GET['rfid_uid'], $_GET['action'])) {
    die("ERROR\nMissing parameters");
}

//sanitize
$machineID = $conn->real_escape_string($_GET['machineID']);
$RFID_UID = $conn->real_escape_string($_GET['rfid_uid']);
$action = $conn->real_escape_string($_GET['action']);

//user existence check
$userQuery = "SELECT * FROM UserTable WHERE rfid_uid='$RFID_UID'";
$userResult = $conn->query($userQuery);
$userExists = ($userResult && $userResult->num_rows > 0);
$studentID = null;

if ($userExists) {
    $row = $userResult->fetch_assoc();
    $studentID = $row['UserID'];
}

//machine existence check
$machineQuery = "SELECT * FROM MachineTable WHERE MachineID='$machineID'";
$machineResult = $conn->query($machineQuery);
$machineExists = ($machineResult && $machineResult->num_rows > 0);

//logging for nonexistent user/machine
if (!$userExists && !$machineExists) {
    $sql = "INSERT INTO LoggingTable (startTime, endTime, MachineID, UserID, Notes)
            VALUES (NOW(), NOW(), 'DNE', 'DNE', 'Failed RFID: $RFID_UID, Failed Machine: $machineID')";
    $conn->query($sql);
    die("ERROR\nUser and Machine do not exist.");
} 
else if (!$userExists) {
    $sql = "INSERT INTO LoggingTable (startTime, endTime, MachineID, UserID, Notes)
            VALUES (NOW(), NOW(), '$machineID', 'DNE', 'Failed RFID: $RFID_UID')";
    $conn->query($sql);
    die("ERROR\nUser does not exist.");
} 
else if (!$machineExists) {
    $sql = "INSERT INTO LoggingTable (startTime, endTime, MachineID, UserID, Notes)
            VALUES (NOW(), NOW(), '0', '$studentID', 'Failed Machine: $machineID')";
    $conn->query($sql);
    die("ERROR\nMachine does not exist.");
}

//authorization check
$authQuery = "SELECT * FROM MachineAuthorizations WHERE MachineID='$machineID' AND UserID='$studentID'";
$authResult = $conn->query($authQuery);
$authorized = ($authResult && $authResult->num_rows > 0);

//check if active
$activeQuery = "SELECT * FROM LoggingTable
                WHERE (MachineID='$machineID' OR UserID='$studentID') AND endTime IS NULL";
$activeResult = $conn->query($activeQuery);
$activeSessionExists = ($activeResult && $activeResult->num_rows > 0);

//authorization/denial check
if ($authorized && $action === "start") {
    if ($activeSessionExists) {
        die("DENIED\nUser or machine already in use.");
    }
    $sql = "INSERT INTO LoggingTable (startTime, MachineID, UserID, Notes)
            VALUES (NOW(), '$machineID', '$studentID', 'Successful Authorization')";
    if ($conn->query($sql) === TRUE) {
        echo "AUTHORIZED\nAccess allowed";
    } else {
        echo "ERROR\nLogging error: " . $conn->error;
    }
} 
else if ($authorized && $action === "end") {
    if (!$activeSessionExists) {
        die("DENIED\nNo active session to end.");
    }
    $sql = "UPDATE LoggingTable 
            SET endTime = NOW()
            WHERE MachineID='$machineID' 
            AND UserID='$studentID' 
            AND endTime IS NULL
            ORDER BY startTime DESC
            LIMIT 1";
    if ($conn->query($sql) === TRUE) {
        echo "AUTHORIZED\nSession ended";
    } else {
        echo "ERROR\nLogging error: " . $conn->error;
    }
} 
else if (!$authorized) {
    $sql = "INSERT INTO LoggingTable (startTime, endTime, MachineID, UserID, Notes)
            VALUES (NOW(), NOW(), '$machineID', '$studentID', 'Authorization Failed')";
    $conn->query($sql);
    echo "DENIED\nAuthorization failed";

} 
else {
    echo "ERROR\nInvalid action";
}

$conn->close();
?>

<?php

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
if (!isset($_GET['machineID'])) {
    die("ERROR\nMissing parameters");
}

$machineID = $conn->real_escape_string($_GET['machineID']);
$machineQuery = "SELECT * FROM MachineTable WHERE ArduinoMAC ='$machineID'";
$machineResult = $conn->query($machineQuery);
$machineExists = ($machineResult && $machineResult->num_rows > 0);
if ($machineExists) {
    die("VALID\n");
} else {
    $machineInsertion = "INSERT INTO MachineTable (ArduinoMAC) VALUES ('$machineID')";
    $insertionResult = $conn->query($machineInsertion);
    if ($insertionResult) {
        die("CREATED\n");
    } else {
        die("ERROR\nFailed to create machine entry.");
    }
}

$conn->close();
?>


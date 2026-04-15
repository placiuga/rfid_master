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
if (!isset($_GET['machineMAC'])) {
    die("ERROR\nMissing parameters");
}

$machineMAC = $conn->real_escape_string($_GET['machineMAC']);
$machineQuery = "SELECT * FROM MachineTable WHERE ArduinoMAC ='$machineMAC'";
$machineResult = $conn->query($machineQuery);
$row = $machineResult->fetch_assoc();
$machineID = $row['MachineID'];
echo $machineID;


$conn->close();
?>


<?php

$hostname = "localhost";
$username = "admin";
$password = "your_password";
$db = "sensor_table";

$dbconnect= new mysqli($hostname,$username,$password, $db);

if ($dbconnect->connect_error) {
  die("Database connection failed: " . $dbconnect->connect_error);
}

if (isset($_POST['score']) && isset($_POST['temp_c']) && isset($_POST['moisture']) &&isset($_POST['uv']) && isset($_POST['euid']) && isset($_POST["humid"]) && isset($_POST['not']) && isset($_POST['not_cnt'])) {
	$score_nr = $_POST['score'];
	$temp_nr = $_POST['temp_c'];
	$moisture_nr = $_POST['moisture'];
	$uv_nr = $_POST['uv'];
	$euid_nr = $_POST['euid'];
	$humid_nr = $_POST['humid'];
	$not_nr = $_POST['not'];
	$not_ctr_nr = $_POST['not_cnt'];
	if ($dbconnect->connect_error) {
  		die("Database connection failed: " . $dbconnect->connect_error);
	}
	$query = "SELECT * FROM euid_table WHERE EUID='$euid_nr'";
	$result = $dbconnect->query($query);
	if ($result->num_rows > 0) {
		$query = "INSERT INTO sensor_data_table (score, temp_c, moisture, uv, euid, humidity, notification, notification_cnt) VALUES ('$score_nr', '$temp_nr', '$moisture_nr', '$uv_nr', '$euid_nr', '$humid_nr', '$not_nr', '$not_ctr_nr')";
		if (!mysqli_query($dbconnect, $query)) {
				die('An error occurred. Data not inserted.');
		} else {
				 echo "Data Successfully Inserted.";
		}
	}
	else{
		die('invalid ID');
	}
}
?>

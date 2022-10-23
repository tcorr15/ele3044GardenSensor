<?php
// array for JSON response
 
$hostname = "localhost";
$username = "admin";
$password = "your_password";
$db = "sensor_table";

$dbconnect=mysqli_connect($hostname,$username,$password,$db);

if ($dbconnect->connect_error) {
  die("Database connection failed: " . $dbconnect->connect_error);
}

if (isset($_POST['euid'])){
	$euid = $_POST['euid'];
	// get ID of EUID from euid_table
	$query = "SELECT * FROM euid_table WHERE EUID=$euid;";
	$result = mysqli_query($dbconnect, $query); 
	if (mysqli_num_rows($result) > 0) {	 
		while ($row = mysqli_fetch_assoc($result)) { 
			$euid = $row["ID"];
		}
		// get all products from products table
		$query = "SELECT * FROM euid_plant_table WHERE EUID=$euid;";
		$result = mysqli_query($dbconnect, $query); 
		// check for empty result
		if (mysqli_num_rows($result) > 0) {
			// looping through all results
			// products node
		 
			while ($row = mysqli_fetch_assoc($result)) { 
				$Plant->ID = $row["plant_ID"];
			}
		 
			// echoing JSON response
			echo json_encode($Plant);
		} else {
			// no products found
			$response->success = 0;
			$response->message = "No data found";
		 
			// echo no users JSON
			echo json_encode($response);
		}
	} else {
		// no products found
		$response->success = 0;
		$response->message = "EUID Invalid";
		
		// echo no users JSON
		echo json_encode($response);
	}
	
	mysqli_close($dbconnect);
}

?>

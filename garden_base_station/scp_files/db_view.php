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
 
// get all products from products table
$query = "SELECT * FROM sensor_data_table WHERE data_ID IN (SELECT MAX(data_ID) FROM sensor_data_table GROUP BY euid);";
// $query = "SELECT * FROM sensor_data_table;";
$result = mysqli_query($dbconnect, $query); 
// check for empty result
if (mysqli_num_rows($result) > 0) {
    // looping through all results
    // products node
 
    while ($row = mysqli_fetch_assoc($result)) {
        // temp user array
        // $Sensor->ID = $row["data_ID"];        
        // $tstamp = $row["timestamp"];
        $euid = $row["euid"];
        // $score = $row["score"];
	    // $temp = $row["temp_c"];
	    // $uv = $row["uv"];
        // $moisture = $row["moisture"];
        // $humidity = $row["humidity"];
        // push single product into final response array
        $Sensor = array();
        $Sensor["tstamp"] = $row["timestamp"];
        $Sensor["score"] = $row["score"];
        $Sensor["temp"] = $row["temp_c"];
        $Sensor["uv"] = $row["uv"];
        $Sensor["moisture"] = $row["humidity"];
        $Sensor["humidity"] = $row["humidity"];
        // echo json_encode($Sensor);
        
	    $response->$euid = $Sensor;
    }
 
    // echoing JSON response
    echo json_encode($response);
} else {
    // no products found
    $response->success = 0;
    $response->message = "No data found";
 
    // echo no users JSON
    echo json_encode($response);
}
mysqli_close($dbconnect);
?>

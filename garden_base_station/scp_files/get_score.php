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

if (isset($_POST["euid"])){
    // get all products from products table
    $euid = $_POST["euid"];
    $query = "SELECT * FROM sensor_data_table WHERE euid=$euid ORDER BY data_ID DESC LIMIT 1;";
    $result = mysqli_query($dbconnect, $query); 
    // check for empty result
    if (mysqli_num_rows($result) > 0) {
        // looping through all results
        // products node
    
        while ($row = mysqli_fetch_assoc($result)){
            $Sensor->score = $row["score"];
            $Sensor->not = $row["notification"];
            $Sensor->not_ctr = $row["notification_cnt"];
        }
    
        // echoing JSON response
        echo json_encode($Sensor);
    } else {
        // no products found
        $response->success = 0;
        $response->message = "No data found";
    
        // echo no users JSON
        echo json_encode($response);
    }
}
else{
    echo json_encode("Error: EUID not found");
}
mysqli_close($dbconnect);
?>

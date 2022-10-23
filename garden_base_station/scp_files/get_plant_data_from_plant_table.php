<?php
    $hostname = "localhost";
    $username = "admin";
    $password = "your_password";
    $db = "sensor_table";

    if (isset($_POST['ID'])){
        $dbconnect=mysqli_connect($hostname,$username,$password,$db);
        $ID = $_POST['ID'];
        if ($dbconnect->connect_error) {
            die("Database connection failed: " . $dbconnect->connect_error);
        }
        $query = "SELECT lux_max, lux_min, temp_max, temp_min, moisture_max, moisture_min, humidity_max, humidity_min FROM plant_table WHERE ID = $ID;";
        // $query = "SELECT * FROM sensor_data_table;";
        $result = mysqli_query($dbconnect, $query); 
        // check for empty result
        if (mysqli_num_rows($result) > 0) {
            // looping through all results
            // products node
        
            while ($row = mysqli_fetch_assoc($result)) {
                // temp user array
                // $Sensor->ID = $row["data_ID"];        
            $Plant->lux_max = $row["lux_max"];
            $Plant->lux_min = $row["lux_min"];
            $Plant->temp_max = $row["temp_max"];
            $Plant->temp_min = $row["temp_min"];
            $Plant->moisture_max = $row["moisture_max"];
            $Plant->moisture_min = $row["moisture_min"];
            $Plant->humidity_max = $row["humidity_max"];
            $Plant->humidity_min = $row["humidity_min"];
                // push single product into final response array
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
        mysqli_close($dbconnect);
    }
    else{
        echo json_encode("Error: ID not set");
    }
?>
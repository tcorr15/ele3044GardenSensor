<?php
$url = "https://raw.githubusercontent.com/vrachieru/plant-database/master/json/abelia%20dielsii.json";
$opts = [
    'http' => [
        'method' => 'GET',
        'header' => [
                'User-Agent: PHP'
        ]
    ]
];

$json = file_get_contents($url, false, stream_context_create($opts));
var_dump($json);
?>
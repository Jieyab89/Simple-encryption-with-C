<?php

$code = 200;
$msg  = NULL;

if (!isset($_POST["hw_info"]) || !is_string($_POST["hw_info"])) {
    $msg = "Missing hw_info (string)";
    $code = 400;
    goto err;
}

if (!isset($_POST["hw_name"]) || !is_string($_POST["hw_name"])) {
    $msg = "Missing hw_name (string)";
    $code = 400;
    goto err;
}

// Sanitize input to prevent exploit.
if (preg_match("/^[a-zA-Z0-9\-\_]$/", $_POST["hw_name"])) {
    $msg = "Invalid hw_name";
    $code = 400;
    goto err;
}

$fname = date("Y_m_d_H_i_s")."_".$_POST["hw_name"].".txt";
file_put_contents("result/{$fname}", $_POST["hw_info"]);
$code = 200;
$msg  = "Success!";

err:
header("Content-Type: application/json");
http_response_code($code);
echo json_encode([
  "status"   => $code,
  "message"  => $msg
]);

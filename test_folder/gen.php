<?php

if ($_SERVER["argv"][1] === "gen_file") {
    file_put_contents("test_folder/aaa.txt", "Hello World! AAAAAAA");
    file_put_contents("test_folder/bbb.txt", "Hello World! BBBBBBB");
    exit;
}

$a = "1234567890-=!@#$%^&*()_+QWERTYUIOP[]\qwertyuiop{}|asdfghjkl;'ASDFGHJKL;'XCVBNM,./xcvbnm<>Zz";
$a = str_split($a);
$a = array_unique($a);
$b = $a;
shuffle($b);
$b = array_values($b);
$a = array_values($a);
$orig_b = $b;

// Encrypt map
foreach ($a as $k => $v) {
    if (in_array($v, ["'", "\\"])) {
        $v = "\\".$v;
    }
    if (in_array($b[$k], ["'", "\\"])) {
        $b[$k] = "\\'";
    }
    echo "['{$v}'] = '{$b[$k]}',\n";
}
echo "--------------------------------\n";

$b = $orig_b;
// Decrypt map
foreach ($b as $k => $v) {
    if (in_array($v, ["'", "\\"])) {
        $v = "\\".$v;
    }
    if (in_array($a[$k], ["'", "\\"])) {
        $a[$k] = "\\'";
    }
    echo "['{$v}'] = '{$a[$k]}',\n";
}

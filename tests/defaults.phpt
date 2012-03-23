--TEST--
Check shout initialization defaults
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

$shout = new Shout();
var_dump($shout->get_host());
var_dump($shout->get_port());
var_dump($shout->get_user());
var_dump($shout->get_password());

echo "Done\n";

?>
--EXPECT--
string(9) "localhost"
int(8000)
string(6) "source"
NULL
Done

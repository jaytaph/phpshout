--TEST--
Check if getters/setters are working
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

$shout = new Shout();

$shout->set_host("foo");
var_dump($shout->get_host());
$shout->set_host("bar");
var_dump($shout->get_host());

$shout->set_description("a description");
var_dump($shout->get_description());

$shout->set_nonblocking(true);
var_dump($shout->get_nonblocking());
$shout->set_nonblocking(false);
var_dump($shout->get_nonblocking());
$shout->set_nonblocking("not ok");
var_dump($shout->get_nonblocking());
$shout->set_nonblocking("");
var_dump($shout->get_nonblocking());

# Long ints should be shorts (& 0xFFFF)
$shout->set_port(0x51000);
var_dump($shout->get_port());
echo "Done\n";

?>
--EXPECT--
string(3) "foo"
string(3) "bar"
string(13) "a description"
bool(true)
bool(false)
bool(true)
bool(false)
int(4096)
Done

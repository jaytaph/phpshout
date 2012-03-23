<?php
/*
 * Simple example that streams a mp3 file to an icecast server
 */

// Initialize shout object
$shout = new Shout();
$shout->set_protocol(shout::PROTOCOL_HTTP);
$shout->set_host("localhost");
$shout->set_port("8000");
$shout->set_mount("/listen.m3u");
$shout->set_user("source");
$shout->set_password("secret");
$shout->set_format(shout::FORMAT_MP3);

// Open stream
if ($shout->open() != shout::ERR_SUCCESS) {
  die ("Cannot open stream to server");
}

$f = fopen("file.mp3", "r");
if ($f == null) {
  die ("Cannot open MP3 file");
}

// Repeat until the end of file, or the stream closed
while (!feof($f) && $shout->get_connected() == shout::ERR_CONNECTED) {

  // Read data from file
  $data = fread($f, 4096);

  // Push it to the stream
  $shout->send($data);

  // Sync / wait until we are allowed to send more..
  $shout->sync();
}

// Finialize
fclose($f);
$shout->close();


?>

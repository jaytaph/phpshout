<?php
/*
 * Simple example that streams a mp3 file to an icecast server
 */

// Initialize shout object
$shout = new Shout();

// Protocol information where to connect to
$shout->set_protocol(shout::PROTOCOL_HTTP);
$shout->set_host("localhost");
$shout->set_port("8000");
$shout->set_mount("/listen");

// Authentication
$shout->set_user("source");
$shout->set_password("secret");

// Stream type
$shout->set_format(shout::FORMAT_MP3);

// Stream detail information
$shout->set_name("My super awesome music stream");
$shout->set_url("http://www.adayinthelifeof.nl");
$shout->set_description("A description of the stream");

// Additional audio information
$shout->set_audio_info($shout::AI_BITRATE, 320);
$shout->set_audio_info($shout::AI_SAMPLERATE, 44100);
$shout->set_audio_info($shout::AI_CHANNELS, 2);


// Open the stream
if ($shout->open() != shout::ERR_SUCCESS) {
  die ("Cannot open stream to server: ".$shout->get_error()."\n");
}

// Open MP3 file to stream
$f = fopen("file.mp3", "r");
if ($f == null) {
  die ("Cannot open MP3 file");
}

// Set the author and song title like: "<artist> - <title>"
$meta = array("song" => "The Artist - A title - An Album");
$shout->set_metadata($meta);

// Repeat until the end of file, or until the stream has closed
while (!feof($f) && $shout->get_connected() == shout::ERR_CONNECTED) {
  print ".";

  // Read data from file
  $data = fread($f, 4096);

  // Push it to the stream
  $shout->send($data);

  // Sync / wait until we are allowed to send more..
  $shout->sync();
}

// Finalize
fclose($f);
$shout->close();

?>

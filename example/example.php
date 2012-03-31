<?php
/*
 * Simple example that streams a mp3 file to an icecast server
 */

// Initialize shout object
$shout = new Shout();

// Protocol information where to connect to
$shout->setProtocol(shout::PROTOCOL_HTTP);
$shout->setHost("localhost");
$shout->setPort("8000");
$shout->setMount("/listen");

// Authentication
$shout->setUser("source");
$shout->setPassword("blaataap");

// Stream type
$shout->setFormat(shout::FORMAT_MP3);

// Stream detail information
$shout->setName("My super awesome music stream");
$shout->setUrl("http://www.adayinthelifeof.nl");
$shout->setDescription("A description of the stream");

// Additional audio information
$shout->setAudioInfo($shout::AI_BITRATE, 320);
$shout->setAudioInfo($shout::AI_SAMPLERATE, 44100);
$shout->setAudioInfo($shout::AI_CHANNELS, 2);


// Open the stream
if ($shout->open() != shout::ERR_SUCCESS) {
  die ("Cannot open stream to server: ".$shout->getError()."\n");
}

// Open MP3 file to stream
$f = fopen("file.mp3", "r");
if ($f == null) {
  die ("Cannot open MP3 file");
}

// Set the author and song title like: "<artist> - <title>"
$meta = array("song" => "The Artist - A title - An Album");
$shout->setMetadata($meta);

// Repeat until the end of file, or until the stream has closed
while (!feof($f) && $shout->getConnected() == shout::ERR_CONNECTED) {
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

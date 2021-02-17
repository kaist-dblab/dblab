<?php 

$logFile = 'downloadLog_EduCOSMOS.txt';
$counter = 'counter.txt'; // text file to store download count - create manually and put a 0 (zero) in it to begin the count 
$download = 'Download/ODYSSEUS-EduCOSMOS-master.zip'; // the link to your download file 
date_default_timezone_set("Asia/Seoul");

$fp = fopen($logFile, 'a');
flock($fp, LOCK_EX);
$line = date('r')."\t".$_SERVER['REMOTE_ADDR']."\t$download\n";
fwrite($fp, $line);
flock($fp, LOCK_UN);
fclose($fp);

$fp = fopen($counter, 'r');
$nDownloads = intval(fgets($fp));
fclose($fp);

$nDownloads++;

$fp = fopen($counter, 'w');
flock($fp, LOCK_EX);
fwrite($fp, $nDownloads);
flock($fp, LOCK_UN);
fclose($fp);

header("Location: $download"); // get download 
   
?>

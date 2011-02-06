<?php
class KalturaLogger { //implements IKalturaLogger {
	function log($str) {
		if (!file_exists(file_directory_path() .'/kaltura.log')) {
			$klog = fopen(file_directory_path() .'/kaltura.log', 'w');
			if($klog) fclose($klog);
		}
		if (file_exists(file_directory_path() .'/kaltura.log')) {
			$klog = fopen(file_directory_path() .'/kaltura.log', 'a');
			if (!$klog) watchdog('kaltura client', $str);
			else {
			    fwrite($klog, $str . PHP_EOL);
			    fclose($klog);
			}
		}
	}
}
?>
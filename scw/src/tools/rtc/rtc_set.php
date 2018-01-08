<?php header("Access-Control-Allow-Origin: *") ?>
<?php
		$data = json_decode(file_get_contents('php://input'), true);
		$type = $data["type"];
		if ($type == "update_time")
		{
			$result = exec("rtc  /dev/rtc0");
			echo "$result";
		}
		elseif ($type == "set_time")
		{
			$year=$data["year"];
			$month=$data["month"];
			$day=$data["day"];
			$hour=$data["hour"];
			$min=$data["min"];
			$sec=$data["sec"];
			$result = exec("rtc /dev/rtc0  $year $month $day $hour $min $sec ");
			echo "$result";
		}
		
		shell_exec('sync');
?>

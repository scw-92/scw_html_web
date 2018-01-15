<?php header("Access-Control-Allow-Origin: *") ?>
<?php
		$data = json_decode(file_get_contents('php://input'), true);
		$type = $data["type"];
		if ($type == "eeprom_read")
		{
			$iic_con=$data["iic_con"];
			$eeprom_rw_opt=$data["eeprom_rw_opt"];
			$eeprom_slave_addr=$data["eeprom_slave_addr"];
			$eeprom_reg_addr=$data["eeprom_reg_addr"];
			$result = exec("eeprom $iic_con  $eeprom_rw_opt $eeprom_slave_addr $eeprom_reg_addr $eeprom_write_value");
			echo "$result";
			//echo "read";
		}
		elseif ($type == "eeprom_write")
		{
			$iic_con=$data["iic_con"];
			$eeprom_rw_opt=$data["eeprom_rw_opt"];
			$eeprom_slave_addr=$data["eeprom_slave_addr"];
			$eeprom_reg_addr=$data["eeprom_reg_addr"];
			$eeprom_write_value=$data["eeprom_write_value"];
			$result = exec("eeprom $iic_con  $eeprom_rw_opt $eeprom_slave_addr $eeprom_reg_addr $eeprom_write_value ");
			echo "$result";
		//	echo "write";
		}
		
		shell_exec('sync');
?>

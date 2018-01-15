<div class="container">

			<h1>welcome to EEPROM !</h1>
	  
			<div class = "container">
				<div class = "row">
					<div class = "col-xs-8 clomd-offset-2">
						<table class="table">
							<thead>
								<tr>
									<th>EEPROM</th>
								</tr>
							</thead>
							<tbody>
								<tr class="active">
									<td>IIC控制器</td>
									<td>
										<select name="iic_con" id="iic_con">
											<option value="i2c-1">i2c-1</option>
											<option value="i2c-1"selected="selected">i2c-1</option>
											<option value="i2c-2">i2c-2</option>
											<option value="i2c-3">i2c-3</option>
										</select>
									</td>
									
									<td>读写操作</td>
									<td>
										<select name="iic_opt" id="iic_opt">
											<option value=0>read</option>
											<option value=1>write</option>
										
											<option value=2 selected="selected">none</option>
										</select>
									</td>
								</tr>
								<tr class="success">
									<td>芯片寻址</td>
									<td>
										<div class="input-group">
											<span class="input-group-addon">0x</span>
											<input type="text" class="form-control" id="eeprom_slave_addr" value=50></input>	
										</div>
										
									</td>
									<td>片内地址</td>
									<td>
										<div class="input-group">
											<span class="input-group-addon">0x</span>
											<input type="text" class="form-control" id="eeprom_reg_addr" value=00></input>	
										</div>
									</td>
								</tr>
								<tr  class="warning">
									<td>设置值</td>
									<td>
									<div class="input-group">
											<span class="input-group-addon">char</span>
											<input type="text" class="form-control" id="eeprom_write_value" value='A'></input>	
										</div>	
									</td>
									<td>读取值</td>
									<td>
										<input type="text" class="form-control" id="eeprom_read_value" ></input>
									</td>
									
								</tr>
								
							</tbody>
							
						</table>
						
						
					</div>
					<div class = "col-xs-8 clomd-offset-2">
						
							<div class="btn-group btn-group-mx " >
								<button type="button" class="btn btn-success" onclick="javascript:eeprom_write(this)">write</button>
								<button type="button" class="btn btn-success"  onclick="javascript:eeprom_read(this)">read</button>
							</div>
					</div>
				</div>
			</div>

	<script>
	
	function eeprom_write(obj)
	{
		eeprom_set();
		if(80 > eeprom_slave_addr || eeprom_slave_addr > 87 )
		{
			alert("please input  legal eeprom_slave_addr!!! ");
		}else if(0>eeprom_reg_addr ||eeprom_reg_addr >255 )
		{
			alert("please input legal eeprom_reg_addr!!! ");
		}else if(eeprom_rw_opt >1 || eeprom_rw_opt <1)
		{
			alert("eeprom_rw_opt is write ???");
		}else
		{
			alert("write time");
			var ajaxPostData = {"type":"eeprom_write",
								"iic_con":iic_con,
								"eeprom_rw_opt":eeprom_rw_opt,
								"eeprom_slave_addr":eeprom_slave_addr,
								"eeprom_reg_addr":eeprom_reg_addr,
								"eeprom_write_value":eeprom_write_value};
			$.ajax({
			url: "src/tools/eeprom/eeprom_set.php",
			type: 'POST',
			contentType:'application/json; charset=utf-8',
			data: JSON.stringify(ajaxPostData),
			success: function(data){
				if(data == "ok")
					alert("eeprom_write is ok ");
				else
					alert(data);
				},
				error:function(){
					alert("error");
				}
			});
		}
		
		//alert(1);
		
	}
	
	function eeprom_read(obj)
	{
		eeprom_set();
		//alert(2);
		eeprom_set();
		if(80 > eeprom_slave_addr || eeprom_slave_addr > 87 )
		{
			alert("please input  legal eeprom_slave_addr!!! ");
		}else if(0>eeprom_reg_addr ||eeprom_reg_addr >255 )
		{
			alert("please input legal eeprom_reg_addr!!! ");
		}else if(eeprom_rw_opt != 0)
		{
			alert("eeprom_rw_opt is read ???");
		}else
		{
			alert("read time");
			var ajaxPostData = {"type":"eeprom_read",
								"iic_con":iic_con,
								"eeprom_rw_opt":eeprom_rw_opt,
								"eeprom_slave_addr":eeprom_slave_addr,
								"eeprom_reg_addr":eeprom_reg_addr};
			$.ajax({
			url: "src/tools/eeprom/eeprom_set.php",
			type: 'POST',
			contentType:'application/json; charset=utf-8',
			data: JSON.stringify(ajaxPostData),
			success: function(data){
				$("#eeprom_read_value").val(data);
				//alert("read");
				},
				error:function(){
					alert("error");
				}
			});
		}
	}
	
		
	function eeprom_set()
	{
			
			iic_con = $("#iic_con option:selected").text();
			eeprom_rw_opt = $("#iic_opt option:selected").val(); 
		
			eeprom_slave_addr=$("#eeprom_slave_addr").val();
			eeprom_reg_addr=$("#eeprom_reg_addr").val();
			eeprom_write_value=$("#eeprom_write_value").val();
		
			eeprom_slave_addr=parseInt(eeprom_slave_addr,16);
			eeprom_reg_addr=parseInt(eeprom_reg_addr,16);
			eeprom_rw_opt = parseInt(eeprom_rw_opt,16);
		//	console.log(iic_con);
		//	console.log(eeprom_rw_opt);
		//	console.log(eeprom_slave_addr);
		//	console.log(eeprom_reg_addr);
		//	console.log(eeprom_write_value);
		}
eeprom_set();
	</script>

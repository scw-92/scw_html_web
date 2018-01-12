	
		<div class="container">

			<h1>welcome to UART !</h1>
	  
			<div class = "container">
				<div class = "row">
					<div class = "col-xs-8 clomd-offset-2">
						<table class="table">
							<thead>
								<tr>
									<th>uart</th>
								</tr>
							</thead>
							<tbody>
								<tr class="active">
									<td>串口号</td>
									<td>
										<select name="UARTPorts" id="UARTPorts">
											<option value="ttyO0">ttyO0</option>
											<option value="ttyO1"selected="selected">ttyO1</option>
											<option value="ttyO2">ttyO2</option>
											<option value="ttyO3">ttyO3</option>
											<option value="ttyO4">ttyO4</option>
											<option value="ttyO5">ttyO5</option>
										</select>
									</td>
									<td>波特率</td>
									<td>
										<select name="UARTBaudRate" id="UARTBaudRate">
											<option value="9600">9600</option>
											<option value="19200">19200</option>
											<option value="38400">38400</option>
											<option value="57600">57600</option>
											<option value="115200" selected="selected">115200</option>
										</select>
									</td>
									<td>停止位</td>
									<td>
										<select name="UARTStopBit" id="UARTStopBit">
											<option value="1" selected="selected">1</option>
											<option value="2">2</option>
										</select>
									</td>
								</tr>
								<tr class="success">
									<td>数据位</td>
									<td>
										<select name="UARTDataLen" id="UARTDataLen">
											<option value="7">7</option>
											<option value="8" selected="selected">8</option>
										</select>
									</td>
									<td>检验位</td>
									<td>
										<select name="UARTCheckBit" id="UARTCheckBit">
											<option value="0" selected="selected">None</option>
											<option value="1">Odd</option>
											<option value="2">Even</option>
										</select>
									</td>
									<td>流控制</td>
									<td>
										<select name="UARTFlow" id="UARTFlow">
										<option value="None" selected="selected">None</option>
										<option value="Odd">软件流</option>
										<option value="Even">硬件流</option>
										</select>
									</td>
								</tr>
								<tr  class="warning">
									<td>发送方式</td>
									<td>
										<select name="UARTWaySend" id="UARTWaySend">
											<option value="None" selected="selected">手动</option>
											<option value="Odd">手动</option>
											<option value="Even">uart_send_auto</option>
										</select>
									</td>
									<td>定时发送</td>
									<td>
										<div class="input-group">
											<input type="text" class="form-control" id="uart_set_time" value=1000> </input>
											<span class="input-group-addon">ms</span>
										</div>
									</td>
									<td>启动/停止</td>
									<td>
										<select name="UARTOnOff" id="UARTOnOff" onclick="javascript:uart_on_or_off(this)">
											<option value="None" selected="selected">None</option>
											<option  value="ON"  >ON</option>
											<option  value="OFF">OFF</option>
										</select>
									</td>
								</tr>
								
							</tbody>
							
						</table>
					</div>
				</div>
			</div>
			
			
			<div class = "container">
				<h4>数据区 !</h4>
				<div class = "row">
					<div class = "col-xs-8 clomd-offset-2">
						<table class="table">
							<tbody>
								<tr  class="danger">
									<div class="input-group input-group-lg" >
										<span class="input-group-addon">接收区</span>
										<div  id = "uart_recv_date" data-spy="scroll"  data-offset="150px" style="height:150px;overflow:auto;text-indent: 2em;padding-top:15px;padding-left:15px;text-overflow: ellipsis; word-break: break-all;text-shadow: 2px 2px #f2e78d; position: relative;border:1px solid #000 " >
												
										</div>	
									</div>
								<tr  class="danger" >
									<div class="input-group input-group-lg" >
										<span class="input-group-addon">发送区</span>
										<div contenteditable="true" id = "uart_send_date" data-spy="scroll"  data-offset="200px" style="height:200px;overflow:auto;text-indent: 2em;padding-top:15px;padding-left:15px;text-overflow: ellipsis; word-break: break-all;text-shadow: 2px 2px #f2e78d; position: relative;border:1px solid #000 " ></div>	
									</div>
								</tr>
							</tbody>
						</table>
						<div class="btn-group btn-group-lg">
							<button type="button" class="btn btn-sueecss" onclick="javascript:uart_send(this)">send</button>
							<button type="button" class="btn btn-success" onclick="javascript:uart_recv(this)">recv</button>
						</div>
					</div>
				</div>
			</div>
	
	
	<script>
	var ip_addr = document.location.hostname;//客服端要访问的主机IP地址（列如：192.168.4.250）
	window.WebSocket = window.WebSocket || window.MozWebSocket;
	//var websocket = new WebSocket('ws://' + '192.168.4.250' +':9988',
	var websocket = new WebSocket('ws://' + ip_addr +':9988',
                              'dumb-increment-protocol');
	
	
	function uart_send(obj)
	{
		uart_set();
		//alert(ip_addr);
		
		webSocketData = //将要发送给服务器的数据封装成一定的格式
				{
					"type":"uart_send",
					"uart_data_send":uart_data_send,
					"uart_how_to_send":uart_how_to_send,
					"uart_set_time":parseInt(uart_set_time)
				};
		websocket.send(JSON.stringify(webSocketData));
		
	}
	
	function uart_recv(obj)
	{
		webSocketData = //将要发送给服务器的数据封装成一定的格式
				{
					"type":"uart_read"
				};
		websocket.send(JSON.stringify(webSocketData));
		
	}
	
		function uart_on_or_off(obj)
		{
			uart_set();
			var selectedOption=obj.options[obj.selectedIndex];
			if(selectedOption.value=="ON")
			{
				alert("uart_on");
				if(obj.options[0].value=="None")
				{	
					obj.options[0].value="ON";
					obj.options[0].innerHTML="ON";
				}
				else if(obj.options[1].value=="None")
				{
					obj.options[1].value="ON";
					obj.options[1].innerHTML="ON";
				}
				else if(obj.options[2].value=="None")
				{	
					obj.options[2].value="ON";
					obj.options[2].innerHTML="ON";
				}
				selectedOption.value="None";
				selectedOption.innerHTML="None";

				webSocketData = 
					{
						"type":"uart_on", 
						"uart_number":uart_number, 
						"uart_baudrate":parseInt(uart_baudrate), 
						"uart_stop_bit":parseInt(uart_stop_bit), 
						"uart_data_bit":parseInt(uart_data_bit),
						"uart_check_bit":parseInt(uart_check_bit),
						"uart_flow_con":parseInt(uart_flow_con),
						"uart_how_to_send":uart_how_to_send,
						"uart_set_time":parseInt(uart_set_time),
						//"uart_data_recv":uart_data_recv,
						"uart_data_send":uart_data_send
					};
				websocket.send(JSON.stringify(webSocketData));

			}else if(selectedOption.value=="OFF")
			{
				alert("uart_off");
				if(obj.options[0].value=="None")
				{
					obj.options[0].value="OFF";
					obj.options[0].innerHTML="OFF";
				}
				else if(obj.options[1].value=="None")
				{
					obj.options[1].value="OFF";
					obj.options[1].innerHTML="OFF";
				}
				else if(obj.options[2].value=="None")
				{
					obj.options[2].value="OFF";
					obj.options[2].innerHTML="OFF";
				}
				selectedOption.value="None";
				selectedOption.innerHTML="None";
				
				webSocketData = //将要发送给服务器的数据封装成一定的格式
				{
					"type":"uart_off"
				};
				websocket.send(JSON.stringify(webSocketData));
			}
		}
		
		function uart_set()
		{
			
			uart_number = $("#UARTPorts option:selected").text();
			uart_baudrate = $("#UARTBaudRate option:selected").text(); 
			uart_stop_bit = $("#UARTStopBit option:selected").text(); 
			uart_data_bit = $("#UARTDataLen option:selected").text(); 
			uart_check_bit = $("#UARTCheckBit option:selected").val(); 
			uart_flow_con=$("#UARTFlow option:selected").text();
			uart_how_to_send =$("#UARTWaySend option:selected").text();
			uart_set_time=$("#uart_set_time").val();
	
			uart_data_recv = $("#uart_recv_date").text();
			uart_data_send= $("#uart_send_date").text();
			
			//alert(UARTPorts2);
			//console.log(uart_number);
			//console.log(uart_baudrate);
			//console.log(uart_stop_bit);
			//console.log(uart_data_bit);
			//console.log(uart_check_bit);
			//console.log(uart_flow_con);
			//console.log(uart_how_to_send);
			//console.log(uart_set_time);
			//console.log(uart_data_recv);
			//console.log(uart_data_send);
			//$("#uart_set_time").val('1500');
		//	$("#uart_recv_date").append("sjdfoisafijsoa");
			//$("#uart_send_date").append("sfhaslkfjskajf;ljdjsafj");
			var uart_on_or_off=$("#UARTOnOff option:selected").text();
			//alert("uart: "+uart_on_or_off);
		}
		
    websocket.onopen = function () {     //注册3个事件
        console.info("WebSocket connect success.");
    };
    websocket.onerror = function () {
        console.info("WebSocket error.");
        alert("Please Check WebSocket Server Working Well.");
    };
    websocket.onmessage = function (message) {

        console.log(message.data);

       // root = eval("(" + message.data + ")");
	   $("#uart_recv_date").append(message.data); //将接收到的数据添加到接收区中

	} 

// 创建一个Socket实例
//var socket = new WebSocket('ws://localhost:8080'); 

// 打开Socket 
//ocket.onopen = function(event) { 

  // 发送一个初始化消息
  //socket.send('I am the client and I\'m listening!'); 

  // 监听消息
  //socket.onmessage = function(event) { 
  //  console.log('Client received a message',event); 
  //}; 

  // 监听Socket的关闭
 // socket.onclose = function(event) { 
   // console.log('Client notified socket has closed',event); 
 // }; 

  // 关闭Socket.... 
  //socket.close() 
//};




	</script>

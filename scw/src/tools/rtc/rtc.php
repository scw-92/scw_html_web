
<div class="container container-fluid">
	<div class="jumbotron text-center">
			<h1>
			<input type="datetime-local" value=<?php $result = exec("rtc  /dev/rtc0"); echo "$result";?> id="time-input"/>
			</h1>
		<div class="btn-group btn-group-lg ">
			<button type="button" class="btn btn-default" id="time-update"  onclick="update_time()">更新</button>
			<button type="button" class="btn btn-default" id= "time-set"  onclick="set_time()">设置</button>
		</div>
	</div>	
</div>

<script type="text/javascript" >
	var x = document.getElementById("time-input");
	var year="2018";
	var month="10";
	var day="24";
	var hour="13";
	var min="59";
	var sec = "59";
	var time;

	function update_time()
	{
		var ajaxPostData = {"type":"update_time"};
		$.ajax({
			url: "src/tools/rtc/rtc_set.php",
			type: 'POST',
			contentType:'application/json; charset=utf-8',
			data: JSON.stringify(ajaxPostData),
			success: function(time){
				x.value = time;
			},
			error: function () {
				alert('没有进入');
			}
		});
	}

	function  set_time()
	{
		alert("setint time");
		get_data_src();
		my_split(time);
		var ajaxPostData = {"type":"set_time","year":year,"month":month,"day":day,"hour":hour,"min":min,"sec":sec};
		$.ajax({
			url: "src/tools/rtc/rtc_set.php",
			type: 'POST',
			contentType:'application/json; charset=utf-8',
			data: JSON.stringify(ajaxPostData),
			success: function(time){
				if(time == "ok")
					alert("set_time is ok ");
				else
					alert(time);
			},
			error:function(){
				alert("error");
			}
		});
	}

	function my_split(time_src)
	{
		var data= new Array();
		var data_data = new Array();
		var data_time = new Array();
		
		data=time_src.split("T");
		data_data = data[0].split("-");
		data_time = data[1].split(":");
		
		year=data_data[0];
		month=data_data[1];
		day=data_data[2];
		hour=data_time[0];
		min=data_time[1];
		sec=data_time[2];
	}

	function set_data_format()
	{
		time = year+"-"+month+"-"+day+"T"+hour+":"+min+":"+sec;
		x.value=time;
	}
	function get_data_src()
	{
		time = x.value;
	}
	$(function(){
		update_time();
	});
</script>

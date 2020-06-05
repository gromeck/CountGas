<?php
/*
**	server side script to configure an uController
**	which runs the count sketch and provides
**	a tiny http server
**
**	(c) 2013 by Christian.Lorenz@gromeck.de
**	(c) 2020 by Christian.Lorenz@gromeck.de
*/

/*
**	hostname of the uController
*/
define('UCONTROLLER','gasmeter.site');
define('CAPTCHA_LENGTH',3);

/*
**	if set, we will offer options to configure the uController
*/
$config = (@$_GET['config']) ? 1 : 0;

if (@$_POST['set']) {
	/*
	**	write the data into the uController
	*/
	if ($_POST['captcharequest'] == $_POST['captchareply']) {
		/*
		**	build up the url
		*/
		$url = 'http://'.UCONTROLLER.'/?';
		$msg = 'uController is '.UCONTROLLER.'<br>';
		if (@$_POST['setmacaddr'] && $macaddr = @$_POST['macaddr']) {
			$url .= "&macaddr=$macaddr";
			$msg .= "Setting MAC address to $macaddr.<br>";
		}
		if (@$_POST['setntpip'] && $ntpip = @$_POST['ntpip']) {
			$url .= "&ntpip=$ntpip";
			$msg .= "Setting NTP server IP address to $ntpip.<br>";
		}
		if (@$_POST['setcounter'] && $counter = @$_POST['counter']) {
			$url .= "&counter=$counter";
			$msg .= "Setting counter value to $counter.<br>";
		}
		if (@$_POST['setincrement'] && $increment = @$_POST['increment']) {
			$url .= "&increment=$increment";
			$msg .= "Setting counter increment to $increment.<br>";
		}
		$msg .= "<br>uController GET request:<br><pre>$url</pre>";

		/*
		**	do the http request
		*/
		$reply = file_get_contents($url);
		$msg .= "<br>uController reply:<br><pre>$reply</pre>";
	}
	else {
		/*
		**	captcha was wrong
		*/
		$error = "Unexpected Captcha";
	}
}

/*
**	find out some network parameters
*/
$macaddr = exec('arp '.UCONTROLLER.' | tail -1 | tr -s " " | cut -f3 -d" "');

/*
**	get the current values from the selected uController
*/
$ucontrollerreply = explode("\n",file_get_contents('http://'.UCONTROLLER.'/?all=1'));
foreach ($ucontrollerreply as $reply) {
	list ($key,$value) = explode(" ",$reply,2);
	switch ($key) {
		case 'COUNTER':		$counter = $value;		break;
		case 'INCREMENT':	$increment = $value;	break;
		case 'MACADDR':		$macadd = $value;		break;
		case 'NTPIP':		$ntpip = $value;		break;
	}
}
#echo "<code>counter=".$counter."</code><br>";
#echo "<code>increment=".$increment."</code><br>";
#echo "<code>ntpip=".$ntpip."</code><br>";
#echo "<code>macaddr=".$macaddr."</code><br>";

/*
**	compute a simple captcha
*/
srand(time());
$captcha = rand(pow(10,CAPTCHA_LENGTH - 1),pow(10,CAPTCHA_LENGTH) - 1);

?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
<title>&#181;Controller <?php print UCONTROLLER ?></title>
<meta http-equiv="Pragma" content="no-cache">
<script language="JavaScript" type="text/javascript">
function captcha_changed()
{
	var setbutton = document.getElementById('setbutton');
	var captcha = document.getElementById('captcha');

	if (captcha.value != '<?php print $captcha ?>') {
		captcha.style.backgroundColor = '#ff8080';
		setbutton.style.backgroundColor = '#f0f0f0';
		setbutton.disabled = 1;
	}
	else {
		captcha.style.backgroundColor = '#ffffff';
		setbutton.style.backgroundColor = '#c0ffc0';
		setbutton.disabled = 0;
	}
//	alert('input=' + captcha.value);
}
function init()
{
	captcha_changed();
	var counter = document.getElementById('counter');

	if (counter)
		counter.focus();
}
</script>
</head>
<body onload="init()">
<style>
body {
	margin:0;
	border:0;
	border:0;
	font-size:1.5rem;
}
h1 {
	font-size:2rem;
}
.header {
	background-color:#c0c0c0;
	width:100%;
	padding:0.1rem;
	padding-left:1rem;
}
.content {
	width:100%;
	padding:0.5rem;
}
.footer {
	background-color:#c0c0c0;
	width:100%;
	padding:1rem;
}
.message {
	background-color:#a0f0a0;
	padding:1rem;
}

.error {
	background-color:#f0a0a0;
	padding:1rem;
}

input[type="text"], input[type="number"] {
	font-family:Courier,Monospace;
	font-size:1.2em;
	width:10em;
	border:1px solid black;
	padding:2px;
}

input[type="checkbox"] {
	width:1em;
}

input[type="submit"], input[type="button"] {
	font-size:1.5rem;
	background-color:#f0f0f0;
	border:1px solid black;
	margin-right:0.3rem;
	padding-left:1rem;
	padding-right:1rem;
}

</style>
<div class=header>
	<h1>&#181;Controller <?php print UCONTROLLER ?></h1>
</div>
<?php if (@$msg) { ?>
<div style="background-color:#a0f0a0; padding:4px; margin:0px;"><b><font familiy="courier"><?php print $msg ?></font></b></div>
<?php } ?>
<?php if (@$error) { ?>
<div style="background-color:#f0a0a0; padding:4px; margin:0px;"><b><font familiy="courier"><?php print $error ?></font></b></div>
<?php } ?>
<form method="POST">
<div class=content>
<table>
	<?php if ($config) { ?>
	<tr>
		<td><input type="checkbox" name="setmacaddr" value="1"></td>
		<td><b>MAC Address:</b></td>
	</tr>
	<tr>
		<td></td>
		<td><input type="text" name="macaddr" value="<?php print $macaddr ?>"></td>
	</tr>
	<tr>
		<td><input type="checkbox" name="setntpip" value="1"></td>
		<td><b>NTP Server IP Address:</b></td>
	</tr>
	<tr>
		<td></td>
		<td><input type="text" name="ntpip" value="<?php print $ntpip ?>"></td>
	</tr>
	<?php } ?>
	<tr>
		<td><input type="checkbox" name="setcounter" value="1" checked></td>
		<td><b>Counter Value:</b></td>
	</tr>
	<tr>
		<td></td>
		<td><input type="text" name="counter" id="counter" value="<?php print $counter ?>"></td>
	</tr>
	<?php if ($config) { ?>
	<tr>
		<td><input type="checkbox" name="setincrement" value="1"></td>
		<td><b>Counter Increment:</b></td>
	</tr>
	<tr>
		<td></td>
		<td><input type="text" name="increment" value="<?php print $increment ?>"></td>
	</tr>
	<?php } ?>
	<tr>
		<td></td>
		<td><b>Captcha:</b> <?php print $captcha ?></td>
	</tr>
	<tr>
		<td></td>
		<td><input type="number" step="any" name="captchareply" id="captcha" value=""
				onkeyup="captcha_changed()" maxlength=<?php print CAPTCHA_LENGTH ?>>
			<input type="hidden" name="captcharequest" value="<?php print $captcha ?>"></td>
	</tr>
</table>
</div>
<div class=footer>
	<input type="submit" name="set" id="setbutton" value="Set">
	<input type="button" name="reload" id="reload" value="Reload" onClick="window.document.location.href = '?';">
	<?php if (!$config) { ?>
	<input type="button" name="config" id="config" value="Configure" onClick="window.document.location.href = '?config=1';">
	<?php } ?>
</div>
</form>
</body>
</html>

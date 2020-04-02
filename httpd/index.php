<?php
/*
**	server side script to configure an Arduino
**	which runs the powecount sketch and provides
**	a tiny http server
**
**	(c) 2013 by Christian.Lorenz@gromeck.de
*/

/*
**	hostname of the arduino
*/
define('ARDUINO','gmeter.site');
define('CAPTCHA_LENGTH',3);

/*
**	if set, we will offer options to configure the device
*/
$config = (@$_GET['config']) ? 1 : 0;

if (@$_POST['set']) {
	/*
	**	write the data into arduino
	*/
	if ($_POST['captcharequest'] == $_POST['captchareply']) {
		/*
		**	build up the url
		*/
		$url = 'http://'.ARDUINO.'/?';
		$msg = 'Arduino is '.ARDUINO.'<br>';
		if (@$_POST['setmacaddr'] && $macaddr = @$_POST['macaddr']) {
			$url .= "&macaddr=$macaddr";
			$msg .= "Setting MAC address to $macaddr.<br>";
		}
		if (@$_POST['setntpip'] && $ntpip = @$_POST['ntpip']) {
			$url .= "&ntpip=$ntpip";
			$msg .= "Setting NTP server IP address to $ntpip.<br>";
		}
		if (@$_POST['setcounterval'] && $counterval = @$_POST['counterval']) {
			$url .= "&counterval=$counterval";
			$msg .= "Setting counter value to $counterval.<br>";
		}
		if (@$_POST['setcounterinc'] && $counterinc = @$_POST['counterinc']) {
			$url .= "&counterinc=$counterinc";
			$msg .= "Setting counter increment to $counterinc.<br>";
		}
		//$msg .= "<br>Arduino GET request:<br><pre>$url</pre>";

		/*
		**	do the http request
		*/
		$reply = file_get_contents($url);
		$msg .= "<br>Arduino reply:<br><pre>$reply</pre>";
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
$macaddr = exec('arp '.ARDUINO.' | tail -1 | tr -s " " | cut -f3 -d" "');

/*
**	get the current values from the selected arduino
*/
$arduinoreply = file_get_contents('http://'.ARDUINO.'/');
//echo "<code>arduinoreply=".$arduinoreply."</code><br>";
$counterval = preg_filter("/(.*)COUNTER (\d+)(.*)/msi","$2",$arduinoreply);
$counterinc = preg_filter("/(.*)INCREMENT (\d+)(.*)/msi","$2",$arduinoreply);
$ntpip = preg_filter("/(.*)NTPIP (\d+\.\d+\.\d+\.\d+)(.*)/msi","$2",$arduinoreply);
//echo "<code>counterval=".$counterval."</code><br>";
//echo "<code>counterinc=".$counterinc."</code><br>";
//echo "<code>netip=".$netip."</code><br>";

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
<title>Arduino GAS</title>
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" href="rrd.css" type="text/css">
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
	var counterval = document.getElementById('counterval');

	if (counterval) {
		counterval.focus();
		var val = counterval.value;
		counterval.value = '';
		counterval.value = val;
	}
}
</script>
</head>
<body onload="init()">
<style>
body {
	margin:0;
	padding-left:0.5em;
	border:0;
	font-size:1.5em;
}
h1 {
	padding:0;
	margin:0;
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
	font-size:1.2em;
	width:5em;
	border:1px solid black;
	padding:2px;
	background-color:#f0f0f0;
}
#config {
	width:10em;
}

</style>
<h1>Arduino GAS</h1>
<?php if (@$msg) { ?>
<div style="background-color:#a0f0a0; padding:4px; margin:0px;"><b><font familiy="courier"><?php print $msg ?></font></b></div>
<?php } ?>
<?php if (@$error) { ?>
<div style="background-color:#f0a0a0; padding:4px; margin:0px;"><b><font familiy="courier"><?php print $error ?></font></b></div>
<?php } ?>
<table>
	<form method="POST">
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
		<td><input type="checkbox" name="setcounterval" value="1" checked></td>
		<td><b>Counter Value:</b></td>
	</tr>
	<tr>
		<td></td>
		<td><input type="number" step="any" name="counterval" id="counterval" value="<?php print $counterval ?>"></td>
	</tr>
	<?php if ($config) { ?>
	<tr>
		<td><input type="checkbox" name="setcounterinc" value="1"></td>
		<td><b>Counter Increment:</b></td>
	</tr>
	<tr>
		<td></td>
		<td><input type="number" step="any" name="counterinc" value="<?php print $counterinc ?>"></td>
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
	<tr>
		<td></td>
		<td>
			<input type="button" name="reload" value="Reload" onClick="window.document.location.href = '?';">
			<input type="submit" name="set" id="setbutton" value="Set"></td>
		</td>
	</tr>
	<?php if (!$config) { ?>
	<tr>
		<td></td>
		<td>
			<input type="button" id="config" name="config" value="Configure" onClick="window.document.location.href  = '?config=1';">
		</td>
	</tr>
	<?php } ?>
	</form>
</table>
</body>
</html>

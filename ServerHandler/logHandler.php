<?php
require_once("Mailer/PHPMailerAutoload.php");

class EMail {
	
	function sendEmail($subject, $message, $adress, $filePath) 
	{   
        $mail = new PHPMailer();
        $mail->IsSMTP();
        $mail->Host = 'pavelk';
        $mail->SMTPAuth = true;
		$mail->SMTPSecure = 'ssl';
        $mail->Port = 465;
        $mail->Username = 'infoPavelK';
        $mail->Password = 'df2w34rda2345@@$SSSsd1111';
        $mail->SetFrom('info@pavelk.ru', 'PavelK Updatus');
        $mail->IsHTML(true);
        $mail->CharSet='utf-8';
        $mail->Subject = $subject;
        $mail->Body = $message;
        $mail->AddAddress($adress);
        $mail->AddAddress("2me@pavelk.ru");
        if ($filePath!="") $mail->AddAttachment($filePath, "updatus log.txt");
        
                
		return $mail->Send();
	}
	
	function header_encode($str, $data_charset, $send_charset){
		if($data_charset != $send_charset) $str=iconv($data_charset,$send_charset.'//IGNORE',$str); //-- при необходимости изменим кодировку самого текста
		return ('=?'.$send_charset.'?B?'.base64_encode($str).'?=');
	}
	
	
}

$email = new EMail();


//-- Узнаём, что к нам пришло и отправляем уведомлялку
if ( isset($_REQUEST["HandlerType"]) ) {
	if ( $_REQUEST["HandlerType"]==="UpdatusLog" ) {
		$status = $_REQUEST["status"];
		$hasNewInstalled = (!empty($_REQUEST["hasNewInstalled"]))? ($_REQUEST["hasNewInstalled"]==="yes") : false;
		$uuid = $_REQUEST["uuid"];
		
		if ( $status==="OK" && !$hasNewInstalled ) return;

		$subject = "";
		if ( $status==="OK" ) {
			$subject = sprintf("Updatys: Обновления успешно установлены у %s", $uuid);	
		} else {
			$subject = sprintf("Updatys: Возникла проблема при обновлении у %s", $uuid);	
		}

		
		$msg = sprintf("<b>UUID:</b> %s<br/> <b>Дамп лога в аттаче.</b>", $uuid);
		
		$extAdress = "";
		$logDumpPath = "";

		if (isset($_FILES['logDump'])) {
			$file = $_FILES['logDump'];
			if ($file['error'] == UPLOAD_ERR_OK) {				
        		$logDumpPath = $file['tmp_name'];
			}
		}
		$email->sendEmail($subject, $msg, $extAdress, $logDumpPath);
	}
} 



?>



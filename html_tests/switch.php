 <?php

$mode=$_POST['mode'];

if ($mode=='true') //mode is true when button is enabled 
{
    //Retrive the values from database you want and send using json_encode
    //example
    $message='Hey my button is enabled!!';
    $success='true';
	header('Content-Type: application/json');
    echo json_encode(array('mode'=>'true','success'=>$success));
}

else if ($mode=='false')  //mode is false when button is disabled
{
    //Retrive the values from database you want and send using json_encode
    //example
    $message='Hey my button is disabled!!';
    $success='true';
	header('Content-Type: application/json');
    echo json_encode(array('mode'=>'false','success'=>$success));

} 
 ?>
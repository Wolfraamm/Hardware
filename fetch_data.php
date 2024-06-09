<?php
    require 'database.php';
    
    $id = null;
    if (!empty($_GET['id'])) {
        $id = $_REQUEST['id'];
    }

    $pdo = Database::connect();
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    $sql = "SELECT * FROM table_the_iot_projects WHERE id = ?";
    $q = $pdo->prepare($sql);
    $q->execute(array($id));
    $data = $q->fetch(PDO::FETCH_ASSOC);
    Database::disconnect();

    $response = array();
    if (null == $data['name']) {
        $response['message'] = "The ID of your Card / KeyChain is not registered !!!";
        $response['id'] = $id;
        $response['name'] = "--------";
        $response['gender'] = "--------";
        $response['email'] = "--------";
        $response['mobile'] = "--------";
    } else {
        $response = $data;
    }

    header('Content-Type: application/json');
    echo json_encode($response);
?>
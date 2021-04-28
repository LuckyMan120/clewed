<?php require_once 'lib/init.php';

error_reporting(0);

$controller = preg_replace('#[^a-z0-9\-]#', '', $_GET['controller']);
$action = preg_replace('#[^a-z0-9\-]#', '', $_GET['action']);
if (empty($controller) || empty($action))
    die();

$controllerClass = "\\Clewed\\" . str_replace(' ', "\\", ucwords(str_replace('-', ' ', $controller))) . "\\Controller";
if (class_exists($controllerClass)) {
    $controller = new $controllerClass;
    $methodName = lcfirst(str_replace(' ', '', ucwords(str_replace('-', ' ', $action))));
    if (method_exists($controller, $methodName)) {
        $params = $_POST;
        ksort($params);
        $hash = $params['hash'];
        if (!empty($hash)) {
            unset($params['hash']);
            $paramsString = implode(':', array_values($params)) . ':kyarata75';
            $calculatedHash = md5($paramsString);
            if ($hash === $calculatedHash)
                echo $controller->$methodName();
        }
    }
}



<?php

namespace Clewed\User;

class User {

    public $email;
    public $saltedPassword;

    public function __construct() {
        $source = $_REQUEST;
        if (array_key_exists('email', $source)) {
            $this->email = $source['email'];
        }
        if (array_key_exists('password', $source)) {
            $this->saltedPassword = $this->salt($source['password']);
        }
    }

    public function salt($password) {
        return md5($password);
    }

    public function getUserName($id)
    {
        $db = \Clewed\Db::get_instance();
        if (empty($id)) return 'invalid id';
        $ridn = $db->get_row("SELECT rid FROM users_roles WHERE uid = :uid LIMIT 1 ",array('uid' => $id));
        if ($ridn['rid'] == '3') {
            $sql = "SELECT users_roles.*, maenna_company.projname FROM users_roles, maenna_company WHERE users_roles.uid = :uid AND maenna_company.companyid = :uid1 LIMIT 1";
        } else {
            $sql = "SELECT users_roles.*, IF (maenna_people.username_type = 1,maenna_people.firstname,CONCAT(maenna_people.firstname,' ', maenna_people.lastname)) AS firstname FROM users_roles, maenna_people WHERE users_roles.uid = :uid AND maenna_people.pid = :uid1 LIMIT 1";
        }
        $Row = $db->get_row($sql, array('uid' => $id,"uid1" => $id));
        $rid = $ridn['rid'];
        $firstname = ucwords(strtolower($Row['firstname']));
        if ($rid == 6) {
            $output = 'Admin';
        } elseif ($rid == 10) $output = 'Clewed';
        elseif ($rid == "3") {
            if ($Row['projname'] != '') {
                $output = strtoupper($Row['projname']);
            } else $output = 'Project ' . (string)($id + 100);
        } else {
            $output = $firstname;
        }
        return $output;
    }

}

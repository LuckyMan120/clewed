<?php

namespace Clewed\User;

use Clewed\Db;

class Model {

    /** @var Db */
    protected $db;
    /** @var  User */
    protected $user;

    public function __construct(User $user) {
        $this->db = Db::get_instance();
        $this->user = $user;
    }

    /**
     * Validate user credentians
     *
     * @return bool
     */
    public function validate() {
        $sql = 'SELECT `name`, `status` FROM `users` WHERE mail = :email AND pass = :password';
        $data = array(
            ':email'    => $this->user->email,
            ':password' => $this->user->saltedPassword
        );
        $result = $this->db->get_row($sql, $data);
        return $result['name'] && $result['status'];
    }
}

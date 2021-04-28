<?php

class MaennaPermit {

  var $user_type = 'anonymous';

  var $functions = array();

  static function create($user_type = null) {
    $user_class = 'MaennaPermit_' . ucwords($user_type);
    if (class_exists($user_class)) {
      return new $user_class($user_type);
    }
    return new self($user_type);
  }

  function __construct($user_type = null) {
    if ($user_type) {
      $this->user_type = $user_type;
    }
    $this->_add_functions();
  }

  function check($function, $access) {
    if ($this->user_type == 'super') {
      return true;
    }

    // since we haven't defined anything yet
    return true;

    if (!empty($this->functions[$function][$access])) {
      return true;
    }
    return false;
  }

  function _add_functions() {
  }
}

class MaennaPermit_Company extends MaennaPermit {

  var $membership = 'regular';
  var $premium = false;

  function _set_premium() {
    global $user;

    $this->premium = false;
    if ($row = db_fetch_object(db_query('select membership from maenna_company where companyid = %d', $user->uid))) {
      if (!empty($row->membership) and $row->membership == 'premium') {
        $this->premium = true;
      }
    }
  }

  function _add_functions() {
    global $user;

    // right at the moment, nobody but company users
    // run company functions, and read/write is globally
    // determined by membership status (premium or not)

    // so we don't really care what functions you're trying to run.

    // eventually we will, probably.

    if ($this->user_type != 'company') {
      return;
    }

    if ($user->uid) {
      $this->_set_premium();
    }

  }

  function check($function, $access) {
    if ($this->user_type == 'super') {
      return true;
    }
    if ($this->user_type != 'company') {
      return false;
    }
    if ($this->premium) {
      return true;
    }
    if ($access == 'read') {
      return true;
    }
    return false;
  }

}

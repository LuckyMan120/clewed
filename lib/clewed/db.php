<?php
namespace Clewed;

use PDO;

/**
 * Database access layer
 *
 * @method int lastInsertId()
 * @author Dmitry Vovk <dmitry.vovk@gmail.com>
 */
class Db {

    protected static $dsn = '';
    protected static $user = '';
    protected static $pass = '';
    /** @var PDO */
    protected $conn;
    /** @var Db */
    protected static $instance;

    protected function __construct() {
        if (empty(self::$dsn)) {
            throw new \Exception('Call init first');
        }
        $this->connect();
    }

    /**
     * @param string $dsn
     *
     * @throws \Exception
     */
    public static function init($dsn) {
        if (preg_match('|mysql://(.+):(.+)@(.+)/(.+)|', $dsn, $m)) {
            self::$user = $m[1];
            self::$pass = $m[2];
            self::$dsn = sprintf('mysql:host=%s;dbname=%s', $m[3], $m[4]);
        } else {
            throw new \Exception('Could not parse DSN');
        }
    }

    protected function connect() {
        $this->conn = new PDO(self::$dsn, self::$user, self::$pass, array(
            PDO::MYSQL_ATTR_INIT_COMMAND => "SET NAMES 'utf8' COLLATE 'utf8_unicode_ci'"
        ));
    }

    /**
     * @return Db
     */
    public static function get_instance() {
        if (null === self::$instance) {
            self::$instance = new self;
        }
        return self::$instance;
    }

    /**
     * @param string $query
     * @param array $params
     *
     * @return array
     */
    public function get_array($query, array $params = array()) {
        if (empty($params)) {
            $result = $this->conn->query($query);
            return $result->fetchAll(PDO::FETCH_ASSOC);
        } else {
            $statement = $this->conn->prepare($query);
            $statement->execute($params);
            return $statement->fetchAll(PDO::FETCH_ASSOC);
        }
    }

    /**
     * @param string $query
     * @param array $params
     *
     * @return mixed
     */
    public function get_row($query, array $params = array()) {
        if (empty($params)) {
            $result = $this->conn->query($query);
            return $result->fetch(PDO::FETCH_ASSOC);
        } else {
            $statement = $this->conn->prepare($query);
            $statement->execute($params);
            return $statement->fetch(PDO::FETCH_ASSOC);
        }
    }

    /**
     * @param string $query
     * @param array $params
     * @param int $column
     *
     * @return string
     */
    public function get_column($query, array $params = array(), $column = 0) {
        if (empty($params)) {
            $result = $this->conn->query($query);
            return $result->fetchColumn($column);
        } else {
            $statement = $this->conn->prepare($query);
            $statement->execute($params);
            return $statement->fetchColumn($column);
        }
    }

    /**
     * @param string $query
     * @param array $params
     *
     * @return bool|int
     */
    public function run($query, array $params = array()) {
        if (empty($params)) {
            return $this->conn->exec($query);
        } else {
            return $this->conn->prepare($query)->execute($params);
        }
    }

    /**
     * @param string $name
     * @param array $params
     * @return mixed
     */
    public function __call($name, array $params = null) {
        return call_user_func_array(array($this->conn, $name), $params);
    }

    public function get_error() {

        return $this->conn->errorInfo();

    }

    /**
     * Replaces any parameter placeholders in a query with the value of that
     * parameter. Useful for debugging. Assumes anonymous parameters from
     * $params are are in the same order as specified in $query
     *
     * @param string $query The sql query with parameter placeholders
     * @param array $params The array of substitution parameters
     * @return string The interpolated query
     */
    public static function interpolate_query($query, $params) {
        $keys = array();
        $values = $params;

        # build a regular expression for each parameter
        foreach ($params as $key => $value) {
            if (is_string($key)) {
                $keys[] = '/:'.$key.'/';
            } else {
                $keys[] = '/[?]/';
            }

            if (is_array($value))
                $values[$key] = implode(',', $value);

            if (is_null($value))
                $values[$key] = 'NULL';
        }
        // Walk the array to see if we can add single-quotes to strings
        array_walk($values, create_function('&$v, $k', 'if (!is_numeric($v) && $v!="NULL") $v = "\'".$v."\'";'));

        $query = preg_replace($keys, $values, $query, 1, $count);

        return $query;
    }
}

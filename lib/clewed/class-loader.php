<?php
/**
 * Class autoloader
 *
 * @author Dmitry Vovk <dmitry.vovk@gmail.com>
 */
class ClewedClassLoader {

    /** @var string Filesystem prefix for class files */
    protected static $prefix = '';

    /**
     * @param string $prefix
     */
    public static function register($prefix = '') {
        self::$prefix = rtrim($prefix, '/') . '/';
        spl_autoload_register(__CLASS__ . '::loadClass');
    }

    /**
     * @param string $className
     *
     * @return bool
     */
    public static function loadClass($className) {
        $filePath = self::$prefix . self::getFilePath_byClassName($className) . '.php';
        if (is_readable($filePath)) {
            require $filePath;
        }
        return class_exists($className);
    }

    /**
     * @param string $className
     *
     * @return string
     */
    public static function getFilePath_byClassName($className) {
        $parts = explode('\\', $className);
        $pathParts = array();
        foreach ($parts as $part) {
            $part = preg_replace('|([A-Z]{1})|', '-$1', $part);
            $part = trim(strtolower($part), '-');
            $pathParts[] = $part;
        }
        return implode($pathParts, DIRECTORY_SEPARATOR);
    }
}

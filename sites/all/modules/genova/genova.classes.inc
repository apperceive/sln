<?php

/**
 * Genova Exception.
 */
class GenException extends Exception {

}

interface GenMessenger {

  function msg($msg);
  function err($err);
}

class GenDrushMessenger implements GenMessenger {

  function msg($msg) {
    drush_print($msg);
  }

  function err($err) {
    drush_set_error('ERROR', $err);
  }

}

/**
 * Genova class loader
 */
class GenLoader {

  static $moduleDir;

  /**
   * @static
   * Class loader init function.
   *
   * @param string $module_dir
   *   Dir to a module which provides component generator.
   */
  public static function init($module_dir) {
    self::$moduleDir = $module_dir . '/genova/';
  }

  /**
   * @static
   *
   * Loader logic.
   */
  public static function load() {

    if ($handle = opendir(self::$moduleDir)) {
      while (FALSE !== ($entry = readdir($handle))) {
        if (is_file(self::$moduleDir . $entry)) {
          require_once self::$moduleDir . $entry;
        }
      }
    }
  }
}

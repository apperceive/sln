<?php

/**
 * CSS component generator.
 */
class GenCSS extends GenComponent {

  public $cssFile;
  public $infoFile;

  /**
   * Inits needed paths and creates css dir and file.
   *
   * @param string $module_name
   *   Name of the module where the component is going to be generated.
   * @param string $module_path
   *   Path to the module where the component is going to be generated.
   * @param array $params
   *   Init parameters passed into constructor.
   */
  public function drushInit($module_name, $module_path, $params = array()) {
    parent::init($module_name, $module_path, $params);

    if (!file_exists($this->modulePath . '/css')) {
      mkdir($module_path . '/css');
    }

    $this->cssFile = $this->modulePath . '/css/' . $this->moduleName . '.css';

    if (!file_exists($this->cssFile)) {
      touch($this->cssFile);
    }

    $this->infoFile = $this->modulePath . '/' . $this->moduleName . '.info';
  }


  /**
   * Implements logic to validate all inputs needed to build a component.
   *
   * @throws GenException
   *   On input error.
   */
  public function validateInputs() {
    if (!file_exists($this->cssFile)) {
      throw new GenException(strtr('Could not create css file at @file', array('@file' => $this->cssFile)));
    }
  }

  /**
   * Implements logic to build component.
   */
  public function buildComponent() {
    genova_put_code_snippet($this->infoFile, "\nstylesheets[all][] = css/{$this->moduleName}.css");
  }

  /**
   * Gets component name.
   *
   * @return string
   */
  public function componentName() {
    return 'CSS';
  }

}

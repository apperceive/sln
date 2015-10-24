<?php

/**
 * JS component generator.
 */
class GenJS extends GenComponent {

  public $JSFile;
  public $infoFile;

  /**
   * Inits needed paths and creates JS dir and file.
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

    if (!file_exists($this->modulePath . '/js')) {
      mkdir($module_path . '/js');
    }

    $this->JSFile = $this->modulePath . '/js/' . $this->moduleName . '.js';

    if (!file_exists($this->JSFile)) {
      touch($this->JSFile);
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
    if (!file_exists($this->JSFile)) {
      throw new GenException('Could not create JS file at ' . $this->JSFile);
    }
  }

  /**
   * Implements logic to build component.
   */
  public function buildComponent() {
    genova_put_code_snippet($this->infoFile, "\nscripts[] = js/{$this->moduleName}.js");
  }

  /**
   * Gets component name.
   *
   * @return string
   */
  public function componentName() {
    return 'JS';
  }

}

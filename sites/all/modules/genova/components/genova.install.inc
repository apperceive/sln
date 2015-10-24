<?php

class GenInstall extends GenComponent {

  public $installFilePath;

  public function drushInit($module_name, $module_path, $params = array()) {
    parent::init($module_name, $module_path, $params);

    $this->installFilePath = $this->modulePath . '/' . $this->moduleName . '.install';

    if (!file_exists($this->installFilePath)) {
      genova_put_code_snippet($this->installFilePath, $this->getCodeSnippet('php', array(), 'general'));
    }
  }

  /**
   * Implements logic to validate all inputs needed to build a component.
   *
   * @throws GenException
   *   On input error.
   */
  public function validateInputs() {
    // Nothing to validate.
  }

  /**
   * Implements logic to build component.
   */
  public function buildComponent() {
    $snippet_install = $this->getCodeSnippet('install', array('name' => $this->moduleName), 'hooks');
    $snippet_uninstall = $this->getCodeSnippet('uninstall', array('name' => $this->moduleName), 'hooks');

    genova_put_code_snippet($this->installFilePath, $snippet_install);
    genova_put_code_snippet($this->installFilePath, $snippet_uninstall);
  }

  /**
   * Gets component name.
   *
   * @return string
   */
  public function componentName() {
    return 'install';
  }

}

<?php

/**
 * Classes component generator.
 */
class GenClasses extends GenComponent {

  public $classesFile;
  public $infoFile;

  /**
   * Inits needed paths and creates classes dir and file.
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

    $this->classesFile = $this->modulePath . '/' . $this->moduleName . '.classes.inc';

    if (!file_exists($this->classesFile)) {
      touch($this->classesFile);
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
    if (!file_exists($this->classesFile)) {
      throw new GenException('Could not create classes file at ' . $this->classesFile);
    }
  }

  /**
   * Implements logic to build component.
   */
  public function buildComponent() {
    genova_put_code_snippet($this->classesFile, $this->getCodeSnippet('php', array(), 'general'));
    genova_put_code_snippet($this->infoFile, "\nfiles[] = {$this->moduleName}.classes.inc");
  }

  /**
   * Gets component name.
   *
   * @return string
   */
  public function componentName() {
    return 'classes';
  }

}

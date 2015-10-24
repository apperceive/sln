<?php

/**
 * Generic class to generate a component.
 */
abstract class GenComponent {

  /**
   * Name of the module where the component is going to be generated.
   * @var string
   */
  public $moduleName;

  /**
   * Path to the module where the component is going to be generated.
   * @var string
   */
  public $modulePath;

  /**
   * Init parameters passed into constructor.
   * @var array
   */
  public $params;

  /**
   * Generic constructor - should be called by implementation constructor.
   *
   * @param string $module_name
   *   Name of the module where the component is going to be generated.
   * @param string $module_path
   *   Path to the module where the component is going to be generated.
   * @param array $params
   *   Init parameters passed into constructor.
   */
  public function init($module_name, $module_path, $params = array()) {
    $this->moduleName = $module_name;
    $this->modulePath = $module_path;
    $this->params = $params;

    genova_get_messenger()->msg('+++ ' . strtr('Entering @name component.', array('@name' => $this->componentName())) . ' +++');
  }

  /**
   * Override to provide drush init mechanism.
   *
   * @param $module_name
   * @param $module_path
   * @param array $params
   */
  public function drushInit($module_name, $module_path, $params = array()) {

  }

  /**
   * Override to provide gui init mechanism.
   *
   * @param $module_name
   * @param $module_path
   * @param array $params
   */
  public function guiInit($module_name, $module_path, $params = array()) {

  }

  /**
   * @abstract
   * Implements logic to validate all inputs needed to build a component.
   *
   * @throws GenException
   *   On input error.
   */
  abstract public function validateInputs();

  /**
   * @abstract
   * Implements logic to build component.
   */
  abstract public function buildComponent();

  /**
   * @abstract
   * Gets component name.
   *
   * @return string
   */
  abstract public function componentName();

  /**
   * Allows to output success message or implement closing logic on success.
   */
  public function onSuccess() {
    genova_get_messenger()->msg(strtr('Component @name created.', array('@name' => $this->componentName())));
  }

  /**
   * Allows to output a failure message or implement closing logic on failure.
   */
  public function onFail() {
    genova_get_messenger()->err(strtr('Component @name could not be created.', array('@name' => $this->componentName())));
  }

  /**
   * Override this method to return current GenComponent dir path. This method
   * is called to determine path to custom code templates.
   *
   * @return string
   *   The path of genova dir of current GenComponent implementation.
   */
  public function genovaPath() {
    return '';
  }

  /**
   * Util method to get code snippet. In case genovaPath() is overridden it
   * will be used to determine path to custom code templates.
   *
   * @param $name
   *   Name of the code template.
   * @param array $data
   *   Data to be used in the code template.
   * @param string $type
   *   Type or subdirectory within the templates directory.
   *
   * @return string
   *   Built code snippet.
   * @throws GenException
   */
  public function getCodeSnippet($name, $data = array(), $type = NULL) {

    $genova_path = drupal_get_path('module', 'genova') . '/';

    if (!empty($type)) {
      $tpl_path = 'templates/' . $type . '/' . $name . '.txt';
    }
    else {
      $tpl_path = 'templates/' . $name . '.txt';
    }

    if (file_exists($genova_path . $tpl_path)) {
      $contents = file_get_contents($genova_path . $tpl_path);
    }
    elseif (file_exists($this->genovaPath() . $tpl_path)) {
      $contents = file_get_contents($this->genovaPath() . $tpl_path);
    }
    else {
      throw new GenException(strtr('Template file not found @file.', array('@file' => $tpl_path)));
    }

    foreach ($data as $key => $value) {
      $contents = str_replace('{' . $key . '}', $value, $contents);
    }

    return $contents;
  }

  /**
   * Helper function to get prompt from cmd while validating input and
   * prompting again for correct input.
   *
   * @param string $prompt
   *   Prompt to be displayed in the cmd.
   * @param mixed $default
   *   Default value if no value required.
   * @param bool $required
   *   Required flag.
   * @param string $validator
   *   Validator callback - it has to be a member function of component class.
   *
   * @return string
   *   CMD user input.
   */
  protected function drushPrompt($prompt, $default = NULL, $required = TRUE, $validator = NULL) {

    $res = drush_prompt($prompt, $default, $required);

    if (!empty($validator)) {
      try {

        if (method_exists($this, $validator)) {
          $this->$validator($res);
        }
        else {
          $validator($res);
        }
      }
      catch (GenException $e) {
        drush_set_error('Input error', $e->getMessage());
        $res = $this->drushPrompt($prompt, $default, $required, $validator);
      }
    }

    return $res;
  }

}

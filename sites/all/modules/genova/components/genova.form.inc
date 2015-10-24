<?php

/**
 * Form component generator.
 */
class GenForm extends GenComponent {

  public $formFuncName;
  public $formFileName;
  public $formFilePath;

  public $createFitems;

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

    $this->formFileName = isset($params['form_file_name']) ? $params['form_file_name'] :
      $this->drushPrompt('Enter form file name', $this->moduleName . '.form.inc', TRUE, 'validateFormFileName');
    $this->formFuncName = isset($params['form_func_name']) ? $params['form_func_name'] :
      $this->drushPrompt('Enter form function name', NULL, TRUE, 'validateFormFuncName');
    $this->formFilePath = $this->modulePath . '/' . $this->formFileName;

    $this->createFitems = isset($params['create_fitems']) ? $params['create_fitems'] :
      $this->drushPrompt('Do you wish to create form items? [yes/no]', 'yes', TRUE);
  }

  /**
   * TODO - filename should match following pattern:
   * module_name.module or module_name.CUSTOM_STRING.inc
   * @param $input
   */
  public function validateFormFileName($input) {

  }

  /**
   * Validates form function name.
   *
   * @param $input
   * @throws GenException
   */
  public function validateFormFuncName($input) {
    if (!genova_is_valid_func_name($input)) {
      throw new GenException(strtr('Provided form function name @name is not a valid PHP function name.', array('@name' => $input)));
    }
  }

  /**
   * Implements logic to validate all inputs needed to build a component.
   *
   * @throws GenException
   *   On input error.
   */
  public function validateInputs() {
    $this->validateFormFileName($this->formFileName);
    $this->validateFormFuncName($this->formFuncName);
  }

  /**
   * Implements logic to build component.
   */
  public function buildComponent() {

    if (!file_exists($this->formFilePath)) {
      genova_put_code_snippet($this->formFilePath, $this->getCodeSnippet('php', array(), 'general'));
    }

    genova_put_code_snippet($this->formFilePath, $this->getCodeSnippet('build', array(
      'func_name' => $this->formFuncName,
      'additional_params' => !empty($this->params['additional_params']) ? ', ' . $this->params['additional_params'] : '',
      'form_items' => $this->getFormItems(),
    ), 'form'));

    genova_put_code_snippet($this->formFilePath, $this->getCodeSnippet('validate', array(
      'func_name' => $this->formFuncName,
    ), 'form'));

    genova_put_code_snippet($this->formFilePath, $this->getCodeSnippet('submit', array(
      'func_name' => $this->formFuncName,
    ), 'form'));

    // Create form items if requested.
    if ($this->createFitems == 'yes') {
      genova_component_create($this->moduleName, 'fitems', array(
        'form_file_name' => $this->formFileName,
        'scope' => $this->formFuncName,
      ));
    }
  }

  /**
   * Gets component name.
   *
   * @return string
   */
  public function componentName() {
    return 'form';
  }

  /**
   * Override to provide custom code of additional form params.
   *
   * @return string
   */
  public function getAdditionalFormParams() {
    return '';
  }

  /**
   * Override to provide own code of form elements.
   *
   * @return string
   */
  public function getFormItems() {
    return '';
  }

}

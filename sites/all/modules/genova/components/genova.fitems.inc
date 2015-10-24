<?php

class GenFitems extends GenComponent {

  public $items;
  public $scope;
  public $formFileName;
  public $formFilePath;
  public $defaultValuesVariable;
  public $validElementTypes = array('textarea', 'textfield');

  public function drushInit($module_name, $module_path, $params = array()) {
    parent::init($module_name, $module_path, $params);

    $this->defaultValuesVariable = isset($params['default_values_variable']) ? $params['default_values_variable'] : NULL;

    $this->formFileName = isset($params['form_file_name']) ? $params['form_file_name'] :
      $this->drushPrompt('Enter file name where the form resides.', $this->moduleName . '.form.inc', TRUE, 'validateFormFileName');
    $this->formFilePath = $this->modulePath . '/' . $this->formFileName;

    $this->scope = isset($params['scope']) ? $params['scope'] :
      $this->drushPrompt('Enter form function name.', NULL, TRUE, 'validateFormFunctionName');

    $this->items = isset($params['elements']) ? $params['elements'] :
      $this->drushPrompt('Enter elements definition in following pattern form_key@element_type@[element_name], ...', NULL, TRUE, 'validateElementDefinition');

    if (!is_array($this->items)) {
      $this->items = genova_get_options_array($this->items, 'first_option_key');
    }
  }

  public function guiInit($module_name, $module_path, $params = array()) {
    parent::init($module_name, $module_path, $params);
  }

  public function validateFormFunctionName($name) {

    // If the target file does not exists yet we do not need to run validation.
    if (!file_exists($this->formFilePath)) {
      return;
    }

    if (!genova_function_exists($this->formFilePath, $name)) {
      throw new GenException(strtr('Function with provided name @name does not exists.', array('@name' => $name)));
    }
  }

  public function validateFormFileName($file_name) {
    // TODO - validate form file name - it should match following pattern:
    // module_name.custom_key.inc
  }

  public function validateElementDefinition($definition) {
    if (!is_array($definition)) {
      $definition = genova_get_options_array($definition, 'first_option_key');
    }

    foreach ($definition as $key => $val) {
      if (!genova_is_valid_func_name($key)) {
        throw new GenException(strtr('Form key @name must comply with PHP function name string pattern.', array('@name' => $key)));
      }
      if (!in_array($val[0], $this->validElementTypes)) {
        throw new GenException(strtr('Element type @name is not supported.', array('@name' => $val[0])));
      }
    }
  }

  /**
   * Implements logic to validate all inputs needed to build a component.
   *
   * @throws GenException
   *   On input error.
   */
  public function validateInputs() {
    $this->validateFormFunctionName($this->scope);
    $this->validateElementDefinition($this->items);
    $this->validateFormFileName($this->formFileName);
  }

  /**
   * Implements logic to build component.
   */
  public function buildComponent() {
    $elements_snippet = '';

    if (!file_exists($this->formFilePath)) {
      genova_put_code_snippet($this->formFilePath, $this->getCodeSnippet('php', array(), 'general'));
    }

    foreach ($this->items as $element_id => $element_info) {

      // TODO - default_value must consider its type - i.e. for select it will be an array.
      if (!empty($this->defaultValuesVariable)) {
        $default_value = "isset({$this->defaultValuesVariable}['$element_id']) ? {$this->defaultValuesVariable}['$element_id'] : NULL";
      }
      else {
        $default_value = 'NULL';
      }

      $elements_snippet .= $this->getCodeSnippet($element_info[0], array(
        'element_id' => $element_id,
        'element_name' => isset($element_info[1]) ? $element_info[1] : ucfirst($element_id),
        'default_value' => $default_value,
      ), 'form');
    }

    genova_inject_code_snippet($this->formFilePath, $this->scope, $elements_snippet, 'return $form;', "\n  return \$form;");
  }

  /**
   * Gets component name.
   *
   * @return string
   */
  public function componentName() {
    return 'fitems';
  }

}

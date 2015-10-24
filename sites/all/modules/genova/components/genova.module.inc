<?php

/**
 * Generates Drupal module with requested components.
 */
class GenModule extends GenComponent {

  public $humanReadableName;
  public $description;
  public $dependencies;
  public $components = array();

  /**
   * Init.
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

    $this->components = drush_get_option_list('components');

    $this->humanReadableName = !empty($params['human_readable_name']) ? $params['human_readable_name'] :
        $this->drushPrompt('Enter human readable module name');

    $this->description = !empty($params['description']) ? $params['description'] :
        $this->drushPrompt('Enter module description', NULL, FALSE);

    $this->dependencies = !empty($params['dependencies']) ? $params['dependencies'] :
        $this->drushPrompt('Enter comma separated dependencies', NULL, FALSE, 'validateDependencies');
    $this->dependencies = genova_get_options_array($this->dependencies, '1D');
  }

  /**
   * Implements logic to validate all inputs needed to build a component.
   *
   * @throws GenException
   *   On input error.
   */
  public function validateInputs() {

    if (!genova_is_valid_func_name($this->moduleName)) {
      throw new GenException(strtr('Invalid module name provided "@module". Make sure it complies with Drupal coding standards.',
        array('@module' => $this->moduleName)));
    }

    $this->validateDependencies($this->dependencies);
  }

  /**
   * Validates provided dependencies.
   *
   * @param mixed $dependencies
   * @throws GenException
   */
  public function validateDependencies($dependencies) {

    if (empty($dependencies)) {
      return;
    }

    if (!is_array($dependencies)) {
      $dependencies = genova_get_options_array($dependencies, '1D');
    }

    foreach ($dependencies as $dependency) {
      if (!genova_is_valid_func_name($dependency)) {
        throw new GenException(strtr('Provided dependency @name is not a valid module name.', array('@name' => $dependency)));
      }
    }
  }

  /**
   * Implements logic to build component.
   */
  public function buildComponent() {
    $this->buildModule();
    $this->buildComponents();
  }

  /**
   * Prints success message.
   */
  public function onSuccess() {
    genova_get_messenger()->msg(strtr('Module @module created at @dir.',
      array('@module' => $this->moduleName, '@dir' => $this->modulePath)));
  }

  /**
   * Gets component name.
   *
   * @return string
   */
  public function componentName() {
    return 'module';
  }


  /**
   * Builds basic module structure.
   *
   * @throws GenException
   */
  private function buildModule() {
    $data = array(
      'name' => $this->moduleName,
      'human_readable_name' => $this->humanReadableName,
      'description' => $this->description,
    );

    // Remove description line from snippet if none was provided.
    $info_file_snippet = $this->getCodeSnippet('info', $data, 'general');
    if (empty($data['description'])) {
      $info_file_snippet = genova_remove_line('description =', $info_file_snippet);
    }

    // Add dependencies.
    foreach ($this->dependencies as $dependency) {
      $info_file_snippet .= "dependencies[] = $dependency\n";
    }

    // Create info file.
    genova_put_code_snippet($this->modulePath . '/' . $this->moduleName . '.info', $info_file_snippet);
    // Create module file.
    genova_put_code_snippet($this->modulePath . '/' . $this->moduleName . '.module',
      $this->getCodeSnippet('module', $data, 'general'));
    // Create pages file.
    genova_put_code_snippet($this->modulePath . '/' . $this->moduleName . '.pages.inc',
      $this->getCodeSnippet('php', $data, 'general'));
  }

  /**
   * Builds requested components.
   */
  private function buildComponents() {
    foreach ($this->components as $component) {
      genova_component_create($this->moduleName, $component);
    }
  }

}

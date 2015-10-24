<?php

class GenPermission extends GenComponent {

  public $moduleFilePath;
  public $permId;
  public $permTitle;
  public $permDescription;

  public function drushInit($module_name, $module_path, $params = array()) {
    parent::init($module_name, $module_path, $params);

    $this->moduleFilePath = $this->modulePath . '/' . $this->moduleName . '.module';

    // Implement hook_menu() if not exists yet.
    if (!genova_hook_exists($this->moduleName, 'permission')) {
      $hook_menu_snippet = $this->getCodeSnippet('permission', array('name' => $this->moduleName), 'hooks');
      genova_put_code_snippet($this->moduleFilePath, $hook_menu_snippet);
    }

    $this->permId = !empty($params['perm_id']) ? $params['perm_id'] : $this->drushPrompt('Enter permission id');
    $this->permTitle = !empty($params['perm_title']) ? $params['perm_title'] : $this->drushPrompt('Enter permission title');
    $this->permDescription = !empty($params['perm_description']) ? $params['perm_description'] : $this->drushPrompt('Enter permission description');
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

    $data = array(
      'perm_id' => $this->permId,
      'perm_title' => $this->permTitle,
      'perm_description' => $this->permDescription,
      'name' => $this->moduleName,
    );

    $perms_snippet = $this->getCodeSnippet('permission_item', $data, 'drupal_arrays');

    genova_inject_code_snippet($this->moduleFilePath, $this->moduleName . '_permission', $perms_snippet,
      'return $perms;', "\n  return \$perms;");
  }

  /**
   * Gets component name.
   *
   * @return string
   */
  public function componentName() {
    return 'permission';
  }

}

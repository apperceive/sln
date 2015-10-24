<?php

class GenTheme extends GenComponent {

  public $moduleFilePath;
  public $themeFilePath;
  public $themeId;
  public $themeType;
  public $preproces;

  public function drushInit($module_name, $module_path, $params = array()) {
    parent::init($module_name, $module_path, $params);

    $this->moduleFilePath = $this->modulePath . '/' . $this->moduleName . '.module';
    $this->themeFilePath = $this->modulePath . '/' . $this->moduleName . '.theme.inc';

    // Implement hook_menu() if not exists yet.
    if (!genova_hook_exists($this->moduleName, 'theme')) {
      $hook_menu_snippet = $this->getCodeSnippet('theme', array('name' => $this->moduleName), 'hooks');
      genova_put_code_snippet($this->moduleFilePath, $hook_menu_snippet);
    }

    $this->themeId = !empty($params['theme_id']) ? $params['theme_id'] :
        $this->drushPrompt('Enter theme id', NULL, TRUE, 'validateThemeId');
    $this->themeType = !empty($params['theme_type']) ? $params['theme_type'] :
        $this->drushPrompt('Select theme type [function, template]', NULL, TRUE, 'validateThemeType');
    $this->preproces = isset($params['preprocess']) ? $params['preprocess'] :
        $this->drushPrompt('Do you wish to implement preprocess callback? (yes/no)', 'no');

    // If we need theme dir for templates, create it.
    if ($this->themeType == 'template' && !file_exists($this->modulePath . '/theme')) {
      mkdir($this->modulePath . '/theme');
    }

    // If there is need for theme.inc file, create it.
    if (($this->themeType == 'function' || $this->preproces == 'yes') && !file_exists($this->themeFilePath)) {
      genova_put_code_snippet($this->themeFilePath, $this->getCodeSnippet('php', array(), 'general'));
    }
  }

  /**
   * Implements logic to validate all inputs needed to build a component.
   *
   * @throws GenException
   *   On input error.
   */
  public function validateInputs() {
    $this->validateThemeId($this->themeId);
    $this->validateThemeType($this->themeType);

    if ($this->themeType == 'template' && !file_exists($this->modulePath . '/theme')) {
      throw new GenException(strtr('Unable to create theme directory at @path.', array('@path' => $this->moduleFilePath)));
    }
  }

  /**
   * Implements logic to build component.
   */
  public function buildComponent() {

    $data = array(
      'theme_id' => $this->themeId,
      'template' => str_replace('_', '-', $this->themeId),
      'name' => $this->moduleName,
    );

    $hook_theme_array_snippet = $this->getCodeSnippet('theme_item', $data, 'drupal_arrays');

    if ($this->themeType == 'function') {
      $hook_theme_array_snippet = genova_remove_line("'template", $hook_theme_array_snippet);
      $hook_theme_array_snippet = genova_remove_line("'path", $hook_theme_array_snippet);

      $theme_function_data = array(
        'func_name' => $this->moduleName . '_' . $this->themeId,
        'func_args' => '$variables',
        'func_code' => $this->getThemeCode(),
      );

      genova_put_code_snippet($this->themeFilePath, $this->getCodeSnippet('function', $theme_function_data, 'general'));
    }
    else {
      $theme_template_data = array(
        'template_code' => $this->getThemeCode(),
      );

      genova_put_code_snippet($this->modulePath . '/theme/' . str_replace('_', '-', $this->themeId) . '.tpl.php',
        $this->getCodeSnippet('template', $theme_template_data, 'general'));
    }

    if ($this->preproces == 'yes') {
      $preprocess_function_data = array(
        'func_name' => $this->moduleName . '_preprocess_' . $this->themeId,
        'func_args' => '&$variables',
        'func_code' => '',
      );

      genova_put_code_snippet($this->themeFilePath, $this->getCodeSnippet('function', $preprocess_function_data, 'general'));
    }

    if (file_exists($this->themeFilePath)) {
      genova_add_file_include($this->moduleName, $this->moduleName . '.theme.inc');
    }

    genova_inject_code_snippet($this->moduleFilePath, $this->moduleName . '_theme',
      $hook_theme_array_snippet, 'return $theme;', "\n  return \$theme;");
  }

  /**
   * Gets theme output. You can override this method to get custom code in your
   * generated theme callback or template.
   *
   * @return string
   */
  public function getThemeCode() {
    if ($this->themeType == 'function') {
      return "return 'Output of {$this->moduleName}_{$this->themeId}';";
    }

    return "Output of {$this->moduleName}_{$this->themeId}";
  }

  /**
   * Gets component name.
   *
   * @return string
   */
  public function componentName() {
    return 'theme';
  }

  public function validateThemeId($theme_id) {
    if (!genova_is_valid_func_name($theme_id)) {
      throw new GenException('Provided theme id is not in valid format. The string has to be a valid PHP function name.');
    }
  }

  public function validateThemeType($theme_type) {
    if (!in_array($theme_type, array('function', 'template'))) {
      throw new GenException('Invalid theme type provided. Theme type can be either "function" or "template".');
    }
  }

}

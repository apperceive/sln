<?php

class GenMenu extends GenComponent {

  public $moduleFilePath;
  public $menuItem;

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

    $this->moduleFilePath = $this->modulePath . '/' . $this->moduleName . '.module';

    // Implement hook_menu() if not exists yet.
    if (!genova_hook_exists($this->moduleName, 'menu')) {
      $hook_menu_snippet = $this->getCodeSnippet('menu', array('name' => $this->moduleName), 'hooks');
      genova_put_code_snippet($this->moduleFilePath, $hook_menu_snippet);
    }

    // Menu item basics.
    $this->menuItem['path'] = !empty($params['path']) ? $params['path'] : $this->drushPrompt('Menu item path');
    $this->menuItem['title'] = !empty($params['title']) ? $params['title'] : $this->drushPrompt('Menu item title');
    $this->menuItem['description'] = !empty($params['description']) ? $params['description'] : $this->drushPrompt('Menu item description [empty]', NULL, FALSE);

    // Page callback.
    $this->menuItem['page callback'] = !empty($params['page callback']) ? $params['page callback'] :
        $this->drushPrompt('Page callback function name', NULL, TRUE, 'validatePageCallback');

    if ($this->menuItem['page callback'] == 'drupal_get_form') {
      $this->menuItem['page arguments'] = !empty($params['page arguments']) ? $params['page arguments'] :
          $this->drushPrompt('Provide comma delimited page arguments. The first argument must be the form id.', NULL, TRUE, 'validateFormArguments');
    }
    else {
      $this->menuItem['page arguments'] = !empty($params['page arguments']) ? $params['page arguments'] :
          $this->drushPrompt('Page arguments as they will be passed into array without spaces (i.e. "op_1",0,1) [empty]', NULL, FALSE, 'validateArguments');
    }

    // Access callback
    $this->menuItem['access callback'] = !empty($params['access callback']) ? $params['access callback'] :
        $this->drushPrompt('Access callback function name [empty]', NULL, FALSE, 'validateAccessCallback');
    $this->menuItem['access arguments'] = !empty($params['access arguments']) ? $params['access arguments'] :
        $this->drushPrompt('Access arguments as they will be passed into array without spaces (i.e. "op_1",0,1) [empty]', NULL, FALSE, 'validateArguments');


    $this->menuItem['type'] = !empty($params['type']) ? $params['type'] : $this->drushPrompt('Menu item type', 'MENU_LOCAL_TASK');
    $this->menuItem['file'] = !empty($params['file']) ? $params['file'] : $this->drushPrompt('Menu item file', $this->moduleName . '.pages.inc');
    $this->menuItem['weight'] = !empty($params['weight']) ? $params['weight'] : $this->drushPrompt('Menu item weight', '0', FALSE);
  }

  /**
   * Implements logic to validate all inputs needed to build a component.
   *
   * @throws GenException
   *   On input error.
   */
  public function validateInputs() {
    $this->validatePageCallback($this->menuItem['page callback']);

    if ($this->menuItem['page callback'] == 'drupal_get_form') {
      $this->validateFormArguments($this->menuItem['page arguments']);
    }
    else {
      $this->validateArguments($this->menuItem['page arguments']);
    }

    if (empty($this->menuItem['access callback']) && empty($this->menuItem['access arguments'])) {
      throw new GenException('Access definition not provided. You need to define one or both of the following: "access callback", "access arguments".');
    }

    if (!empty($this->menuItem['access callback'])) {
      $this->validateAccessCallback($this->menuItem['access callback']);
    }
    if (!empty($this->menuItem['access arguments'])) {
      $this->validateArguments($this->menuItem['access arguments']);
    }
  }

  /**
   * Implements logic to build component.
   */
  public function buildComponent() {
    $menu_item_data = array(
      'path' => $this->menuItem['path'],
      'title' => $this->menuItem['title'],
      'description' => !empty($this->menuItem['description']) ? "'{$this->menuItem['description']}'" : NULL,
      'page_callback' => $this->menuItem['page callback'],
      'page_arguments' => genova_process_array_params($this->menuItem['page arguments']),
      'access_callback' => !empty($this->menuItem['access callback']) ? "'{$this->menuItem['access callback']}'" : NULL,
      'access_arguments' => genova_process_array_params($this->menuItem['access arguments']),
      'file' => $this->menuItem['file'],
      'type' => $this->menuItem['type'],
      'weight' => $this->menuItem['weight'],
    );

    // Create page callback function.
    $page_callback_data = array(
      'func_name' => $this->menuItem['page callback'],
      'func_args' => implode(', ', genova_get_func_args_from_path($this->menuItem['path'], $this->menuItem['page arguments'])),
      'func_code' => $this->getPageCodeSnippet(),
    );

    if (!file_exists($this->modulePath . "/" . $this->menuItem['file'])) {
      genova_put_code_snippet($this->modulePath . "/" . $this->menuItem['file'], $this->getCodeSnippet('php', array(), 'general'));
    }

    // For drupal_get_form page callback go for form component.
    if ($this->menuItem['page callback'] == 'drupal_get_form') {

      $page_arguments = genova_get_options_array($this->menuItem['page arguments'], '1D');

      $form_params['form_func_name'] = array_shift($page_arguments);
      $form_params['form_file_name'] = $this->menuItem['file'];

      $page_arguments = implode(', ', genova_get_func_args_from_path($this->menuItem['path'], $page_arguments));
      $form_params['additional_params'] = (!empty($page_arguments) ? $page_arguments : '');

      genova_component_create($this->moduleName, 'form', $form_params);
    }
    else {
      genova_put_code_snippet($this->modulePath . "/" . $this->menuItem['file'],
        $this->getCodeSnippet('function', $page_callback_data, 'general'));
    }

    // Create access callback function.
    if (!empty($this->menuItem['access callback'])) {

      $access_callback_data = array(
        'func_name' => $this->menuItem['access callback'],
        'func_args' => implode(', ', genova_get_func_args_from_path($this->menuItem['path'], $this->menuItem['access arguments'])),
        'func_code' => $this->getAccessCodeSnippet(),
      );

      genova_put_code_snippet($this->moduleFilePath,
        $this->getCodeSnippet('function', $access_callback_data, 'general'));
    }

    // Inject menu item code.
    $item_snippet = $this->getCodeSnippet('menu_item', $menu_item_data, 'drupal_arrays');

    // Remove undesired snippet lines.
    if (empty($menu_item_data['description'])) {
      $item_snippet = genova_remove_line("'description'", $item_snippet);
    }
    if (empty($menu_item_data['access_callback'])) {
      $item_snippet = genova_remove_line("'access callback'", $item_snippet);
    }
    if (empty($menu_item_data['page_arguments'])) {
      $item_snippet = genova_remove_line("'page arguments'", $item_snippet);
    }
    if (empty($menu_item_data['access_arguments'])) {
      $item_snippet = genova_remove_line("'access arguments'", $item_snippet);
    }
    if (str_replace('.module', '', $this->menuItem['file']) == $this->moduleName) {
      $item_snippet = genova_remove_line("'file'", $item_snippet);
    }

    genova_inject_code_snippet($this->moduleFilePath, $this->moduleName . '_menu', $item_snippet, 'return $menu;', "\n  return \$menu;");
  }

  /**
   * You can override this method to customize the page code snippet.
   *
   * @return string
   */
  public function getPageCodeSnippet() {
    return "return array('#markup' => t('Hello world'));";
  }

  /**
   * You can override this method to customize the access code snippet.
   *
   * @return string
   */
  public function getAccessCodeSnippet() {
    return "return TRUE";
  }

  /**
   * Prints success msg.
   */
  public function onSuccess() {
    genova_get_messenger()->msg(strtr('Menu path @path added.', array('@path' => $this->menuItem['path'])));
  }

  /**
   * Gets component name.
   *
   * @return string
   */
  public function componentName() {
    return 'menu item';
  }


  /**
   * Page definition input validator.
   *
   * @param string $page_callback
   *   User CMD input.
   *
   * @throws GenException
   */
  public function validatePageCallback($page_callback) {
    if (!genova_is_valid_func_name($page_callback)) {
      throw new GenException('Invalid page callback name provided. It cannot be used as a function name.');
    }
  }

  /**
   * Access definition input validator.
   *
   * @param string $access_callback
   *   User CMD input.
   *
   * @throws GenException
   */
  public function validateAccessCallback($access_callback) {
    if (!empty($access_callback) && !genova_is_valid_func_name($access_callback)) {
      throw new GenException('Invalid access callback name provided. It cannot be used as a function name.');
    }
  }

  /**
   * Validates path arguments.
   *
   * @param $arguments
   *
   * @throws GenException
   */
  public function validateArguments($arguments) {
    // Validation occurs within genova_get_func_args_from_path() function, so
    // just let's reuse it.
    genova_get_func_args_from_path($this->menuItem['path'], $arguments);
  }

  /**
   * Validates path arguments in case of form.
   *
   * @param $arguments
   *   User input.
   *
   * @throws GenException
   */
  public function validateFormArguments($arguments) {
    if (!is_array($arguments)) {
      $arguments = genova_get_options_array($arguments, '1D');
    }

    if (!genova_is_valid_func_name(array_shift($arguments))) {
      throw new GenException('Provided form id is not a valid PHP function name.');
    }

    $this->validateArguments($arguments);
  }
}

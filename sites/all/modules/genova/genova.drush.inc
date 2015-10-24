<?php
/**
 * @file
 *   Drush commands.
 */

/**
 * Implements hook_drush_command().
 */
function genova_drush_command() {
  $items = array();

  $items['gen-module-create'] = array(
    'callback' => 'genova_drush_module_create',
    'description' => "Creates basic Drupal module structure.",
    'aliases' => array('gen-mc'),
    'arguments' => array(
      'name' => 'The name of the module to create',
    ),
    'options' => array(
      'components' => 'Components that will be present in the module.',
    ),
    'examples' => array(
      'drush gen-module-create somemodule --components=css,js,menu,theme' =>
      'Will create a module named "somemodule" with css and js files, hooks menu and theme together with .info file entries.',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_CONFIGURATION,
  );
  $items['gen-component-create'] = array(
    'callback' => 'genova_drush_component_create',
    'description' => "Creates a component.",
    'aliases' => array('gen-cc'),
    'arguments' => array(
      'module' => 'The name of the module where to create the component',
      'component' => 'The name of the component to create',
    ),
    'examples' => array(
      'drush gen-component-create mymodule css' =>
      'Will create css directory and css files css_file_1.css and css_file_2.css. Will add style include into .info file.',
      'drush gen-component-create mymodule schema' =>
      'Will implement hook_schema and prompt you for schema fields and their types.',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_CONFIGURATION,
  );

  return $items;
}

/**
 * Command callback for gen-component-create.
 *
 * @param string $module_name
 *   Module name.
 * @param string $component
 *   Hook to be implemented.
 * @param array $params
 *   Optional params array to be used instead of cmd input.
 *
 * @return bool
 *   Command success status.
 */
function genova_drush_component_create($module_name = NULL, $component = NULL, $params = array()) {
  $arguments = drush_get_arguments();

  if (empty($module_name)) {
    $module_name = $arguments[1];
  }

  if (empty($component)) {
    $component = $arguments[2];
  }

  if (!genova_module_exists($module_name)) {
    return drush_set_error('ERROR', strtr('Module @name does not exist yet. First create the module.',
      array('@name' => $module_name)));
  }

  if (empty($component) || !is_string($component)) {
    return drush_set_error('ERROR', 'No valid component name provided.');
  }

  $messenger = new GenDrushMessenger();
  genova_set_messenger($messenger);

  try {
    genova_component_create($module_name, $component, $params);
    drush_print(strtr('Component output directory: @dir', array('@dir' => genova_get_module_path($module_name))));
  }
  catch (GenException $e) {
    return drush_set_error('ERROR', $e->getMessage());
  }

  return TRUE;
}

/**
 * Command callback for gen-module-create.
 *
 * @param string $module_name
 *   Module name.
 *
 * @return bool
 *   Command result.
 */
function genova_drush_module_create($module_name = NULL) {

  if (empty($module_name)) {
    $arguments = drush_get_arguments();
    $module_name = $arguments[1];
  }

  if (module_exists($module_name)) {
    drush_print(strtr('Module @name already exists. Skipping process.', array('@name' => $module_name)));
    return FALSE;
  }

  $messenger = new GenDrushMessenger();
  genova_set_messenger($messenger);

  try {

    $module_path = genova_get_module_path($module_name);

    $module_component = new GenModule();
    $module_component->drushInit($module_name, $module_path);
    $module_component->validateInputs();
    $module_component->buildComponent();
    $module_component->onSuccess();
  }
  catch (GenException $e) {
    return drush_set_error('ERROR', $e->getMessage());
  }

  return TRUE;
}

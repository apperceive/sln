<?php


/**
 * Determines if function exists in the specified file.
 *
 * @param $path
 *   Path to the file where to check if function exists.
 * @param $function_name
 *   Name of the function to check.
 *
 * @return bool
 *   Check result.
 */
function genova_function_exists($path, $function_name) {

  $content = file_get_contents($path);

  if (preg_match("<function\s?" . $function_name . ">", $content) == 0) {
    return FALSE;
  }

  return TRUE;
}

/**
 * Processes cmd input into two dimensional array, where the second level
 * is exploded into array by "@" character.
 *
 * @param string $cmd_input
 *   Cmd input.
 * @param string $mode
 *   Flag to determine if first option in the second level
 *   should be used as its key.
 *
 * @return array
 *   Processed cmd input into array.
 */
function genova_get_options_array($cmd_input, $mode = '2D') {

  if (empty($cmd_input)) {
    return array();
  }

  $cmd_input = explode(',', $cmd_input);
  $options = array();

  foreach ($cmd_input as $option) {
    $option = trim($option);

    if ($mode == 'first_option_key') {
      $params = explode('@', $option);
      $key = array_shift($params);
      $options[$key] = $params;
    }
    elseif ($mode == '1D') {
      $options[] = $option;
    }
    elseif ($mode == '2D') {
      $options[] = explode('@', $option);
    }
  }

  return $options;
}

/**
 * Sanitizes parameters to be used in array from cmd input.
 *
 * @param $params
 *   Params to be processed.
 *
 * @return string
 *   Processed params.
 */
function genova_process_array_params($params) {

  if (empty($params)) {
    return NULL;
  }

  if (!is_array($params)) {
    $params = explode(',', $params);
  }

  foreach ($params as &$param) {
    $param = trim($param);
    if (!is_numeric($param) && is_string($param) && strpos('"', $param) === FALSE && strpos("'", $param) === FALSE) {
      $param = "'$param'";
    }
  }

  return implode(', ', $params);
}

/**
 * Build page callback argument names from path and page arguments.
 *
 * @param string $path
 *   Path as defined by menu item.
 * @param array $page_arguments
 *   Page arguments array as defined by menu item.
 *
 * @return array
 *   Array of function argument names.
 * @throws GenException
 *   If Provided path does not contain an element at the position defined by one
 *   of numeric page arguments.
 */
function genova_get_func_args_from_path($path, $page_arguments) {
  $path_parts = explode('/', $path);
  $func_args = array();

  if (!is_array($page_arguments)) {
    $page_arguments = genova_get_options_array($page_arguments, '1D');
  }

  foreach ($page_arguments as $page_argument) {

    $page_argument = trim($page_argument);

    if (is_numeric($page_argument)) {
      if (!isset($path_parts[$page_argument])) {
        throw new GenException('Unable to determine function argument. Provided path does not contain an element at position ' . $page_argument);
      }

      if (strpos($path_parts[$page_argument], '%') !== FALSE && strlen($path_parts[$page_argument]) > 1) {
        $func_args[] = '$' . str_replace('%', '', $path_parts[$page_argument]);
      }
      elseif (strpos($path_parts[$page_argument], '%') !== FALSE && strlen($path_parts[$page_argument]) == 1) {
        $func_args[] = '$arg_' . $page_argument;
      }
      else {
        $func_args[] = '$' . $path_parts[$page_argument];
      }
    }
    elseif (is_string($page_argument) && !empty($page_argument)) {
      // Sanitize input to be used as valid function argument.
      $page_argument = preg_replace('/\s+/', '_', $page_argument);
      $func_args[] = '$' . $page_argument;
    }
  }

  return $func_args;
}

/**
 * Validates a string to be a valid function name.
 *
 * @param string $name
 *   String to be validated.
 *
 * @return bool
 *   Validation result.
 */
function genova_is_valid_func_name($name) {

  $number_of_matches = preg_match('<^[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*$>',
    $name, $match_offset_array, PREG_OFFSET_CAPTURE);

  if ($match_offset_array[0][1] !== 0 || $number_of_matches !== 1) {
    return FALSE;
  }
  else {
    return TRUE;
  }
}

/**
 * Removes line which starts with $line string from a snippet.
 *
 * @param string $line
 *   Starting string of a line to be removed.
 * @param $snippet
 *   Snippet in which the line will be removed.
 * @return string
 *   Snippet with removed line if found.
 */
function genova_remove_line($line, $snippet) {
  return preg_replace("< *($line).*\n>", '', $snippet);
}

/**
 * Gets component class based on provided component name.
 *
 * @param $component
 *   Component name.
 *
 * @return string
 *   Component generator class.
 * @throws GenException
 */
function genova_get_component_class($component) {

  // We have other than genova native component.
  if (strpos($component, '/') !== FALSE) {

    $component_def = explode('/', $component);

    // Pop the last element - it is the component name.
    $component = array_pop($component_def);

    // Check if path ends with module name, if not push back component itself as
    // it is most probably also the module name.
    if (!module_exists($component_def[count($component_def) - 1])) {
      array_push($component_def, $component);
    }

    // Build back the component directory path.
    $component_module_dir = implode('/', $component_def);

    if (!file_exists($component_module_dir)) {
      throw new GenException(strtr('Could not determine component directory at @dir',
        array('@dir' => $component_module_dir)));
    }

    GenLoader::init($component_module_dir);
    spl_autoload_register(array('GenLoader', 'load'));
  }

  $class = 'Gen' . ucfirst($component);

  if (!class_exists($class)) {
    throw new GenException(strtr('Generator for provided component @name does not exist.', array('@name' => $component)));
  }

  return $class;
}

/**
 * Camelize a string.
 *
 * @param $string
 *   String to camelize.
 * @param bool $pascalCase
 *   If to upper the first letter.
 *
 * @return string
 *   Camelized string.
 */
function genova_camelize($string, $pascalCase = false) {
  $string = str_replace(array('-', '_'), ' ', $string);
  $string = ucwords($string);
  $string = str_replace(' ', '', $string);

  if (!$pascalCase) {
    return lcfirst($string);
  }
  return $string;
}

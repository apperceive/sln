<?php

/**
 *
 */
class GenSchema extends GenComponent {

  public $installFilePath;
  public $tableName;
  public $fields;
  public $primaryKey;

  public $allowedFieldTypes = array('int', 'varchar', 'text', 'serial', 'blob', 'numeric', 'float');

  public function drushInit($module_name, $module_path, $params = array()) {
    parent::init($module_name, $module_path, $params);

    $this->installFilePath = genova_get_output_file_path($module_name, 'install');

    // We do not have the module install file yet, create it.
    if (!file_exists($this->installFilePath)) {
      genova_put_code_snippet($this->installFilePath, $this->getCodeSnippet('php', array(), 'general'));
    }

    // If we do not have the schema hook implemented, implement it.
    if (!genova_hook_exists($this->moduleName, 'schema', $this->moduleName . '.install')) {
      $hook_menu_snippet = $this->getCodeSnippet('schema', array('name' => $this->moduleName), 'hooks');
      genova_put_code_snippet($this->installFilePath, $hook_menu_snippet);
    }

    $this->tableName = !empty($params['table_name']) ? $params['table_name'] :
        $this->drushPrompt('Enter table name', NULL, TRUE, 'validateTableName');

    if (!empty($params['fields'])) {
      $this->fields = $params['fields'];

      // Check if we have full field definition. If not prompt for additional
      // info.
      $first_element = array_shift($params['fields']);
      if (!is_array($first_element)) {
        $this->fields = $this->processFlatFields($this->fields);
      }
    }
    else {
      $this->fields = $this->drushPrompt('List schema fields in format "name@type" delimited by a comma (i.e. name@varchar,description@text,price@numeric@16@5)', NULL, TRUE, 'validateFields');
      $this->fields = genova_get_options_array($this->fields, 'first_option_key');
    }

    $this->primaryKey = !empty($params['primary_key']) ? $params['primary_key'] :
        $this->drushPrompt('Enter primary key', NULL, TRUE, 'validatePrimaryKey');
  }

  /**
   * Implements logic to validate all inputs needed to build a component.
   *
   * @throws GenException
   *   On input error.
   */
  public function validateInputs() {
    $this->validateTableName($this->tableName);
    $this->validatePrimaryKey($this->primaryKey);
    $this->validateFields($this->fields);
  }

  /**
   * Implements logic to build component.
   */
  public function buildComponent() {
    $fields_code = '';

    // Generate schema fields and SDBO attributes.
    foreach ($this->fields as $name => $definition) {
      $field_data = array('name' => $name);

      // TODO - implement nicer generic way to add type specific data.
      // TODO - some validation of type specific data would be essential.
      if ($definition[0] == 'numeric') {
        $field_data['precision'] = $definition[1];
        $field_data['scale'] = $definition[2];
      }

      $fields_code .= $this->getCodeSnippet($definition[0], $field_data, 'schema');
    }

    // Build schema code and write it into install file.
    $schema_data = array(
      'table' => $this->tableName,
      'fields' => $fields_code,
      'primary_key' => $this->primaryKey,
    );
    $schema_code = $this->getCodeSnippet('schema_table', $schema_data, 'schema');
    genova_inject_code_snippet($this->installFilePath, $this->moduleName . '_schema', $schema_code, 'return $schema;', "\n  return \$schema;");
  }

  /**
   * Gets component name.
   *
   * @return string
   */
  public function componentName() {
    return 'schema';
  }

  public function validateTableName($table_name) {
    if (!genova_is_valid_func_name($table_name)) {
      throw new GenException('Invalid table name provided.');
    }
  }

  public function validatePrimaryKey($primary_key) {
    if (!isset($this->fields[$primary_key])) {
      throw new GenException(strtr('Field for provided primary key @name is not defined.', array('@name' => $primary_key)));
    }
  }

  public function validateFields($fields) {
    if (!is_array($fields)) {
      $fields = genova_get_options_array($fields, 'first_option_key');
    }

    foreach ($fields as $name => $definition) {
      if (!genova_is_valid_func_name($name)) {
        throw new GenException(strtr('Invalid field name @name.', array('@name' => $name)));
      }

      if (!in_array($definition[0], $this->allowedFieldTypes)) {
        throw new GenException(strtr('Unsupported field type for field @name.', array('@name' => $name)));
      }
    }
  }

  public function validateFieldType($cmd_input) {
    $definition = explode('@', $cmd_input);

    if (!in_array($definition[0], $this->allowedFieldTypes)) {
      throw new GenException('Invalid field type.');
    }
  }

  public function processFlatFields($fields) {

    $processed_fields = array();

    foreach ($fields as $field) {
      $cmd_input = $this->drushPrompt(strtr('Enter field type for "@name" (i.e int, numeric@16@5, varchar, ...)', array('@name' => $field)), NULL, TRUE, 'validateFieldType');
      $definition = explode('@', $cmd_input);
      $processed_fields[$field] = $definition;
    }

    return $processed_fields;
  }

}

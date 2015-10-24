
<?php

/**
 * @file
 * This template is used to print a single field in a view.
 *
 * It is not actually used in default Views, as this is registered as a theme
 * function which has better performance. For single overrides, the template is
 * perfectly okay.
 *
 * Variables available:
 * - $view: The view object
 * - $field: The field handler object that can process the input
 * - $row: The raw SQL result that can be used
 * - $output: The processed output that will normally be used.
 *
 * When fetching output from the $row, this construct should be used:
 * $data = $row->{$field->field_alias}
 *
 *
 * e.g.,
 *
 * $row->nid
 * $row->node_title
 * $row->node_changed // (String, 10 characters ) 1414876661 
 * $row->node_created // (String, 10 characters ) 1414904427 
 * $row->field_data_field_project_status_node_entity_type // (String, 4 characters ) node 
 * $row->field_data_field_project_group_node_entity_type // (String, 4 characters ) node 
 * $row->field_data_field_project_manager_node_entity_type // (String, 4 characters ) node 
 * $row->_field_data['nid]'['entity_type'] // (String, 4 characters ) node 
 * $row->_field_data['nid]'['entity'] // Object
 * $row->field_field_project_status[0]['rendered']['#markup']  // if field has content, then this is the array structure
 * $row->field_field_project_status[0]['rendered']['#access']  // BOOLEAN
 * $row->field_field_project_status[0]['raw']['value']
 * $row->field_field_project_group                            // note that this may be an Array w/ 0 elements
 * $row->field_field_project_manager                          // same

 *
 * The above will guarantee that you'll always get the correct data,
 * regardless of any changes in the aliasing that might happen if
 * the view is modified.
 */
 
 //dpm($row);
 
 $submissions = _slnsite_project_submissions($row->nid);
 $c1 = count($submissions);
 $c2 = 0;        // number of completed submissions
 if ($c1) {
   foreach($submissions as $submission) {
     if (!$submission->is_draft)
       $c2++;
   } // foreach
 } // if c1
 $output = l("$c2 / $c1", "node/" . $row->nid . "/submissions", array('attributes' => array('title' => 'View survey submissions for project ' . $row->node_title . ' ...')));
 
?>
<?php print $output; ?>






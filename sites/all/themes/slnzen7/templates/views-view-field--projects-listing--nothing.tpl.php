
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
 
  $project = node_load($row->nid);

  // skip planning form 
  //$planning_form = is_array($project->field_project_form_planning[LANGUAGE_NONE]) ?  $project->field_project_form_planning[LANGUAGE_NONE][0]['nid'] : 0;
  $planning_form = isset($project->field_project_form_planning[LANGUAGE_NONE]) && is_array($project->field_project_form_planning[LANGUAGE_NONE]) ?  $project->field_project_form_planning[LANGUAGE_NONE][0]['nid'] : 0;
  
  
  //$planning_form =  _slnsite_array_keys_exist(array('field_project_form_planning', LANGUAGE_NONE, 0, 'nid'));
  
  // or _slnsite_get($node->field_project_form_planning, "[LANGUAGE_NONE][0]['nid']");
  
  $c1 = 0;        // wrong, counts plan: $c1 = count($submissions);
  $c2 = 0;        // number of completed submissions

  foreach($submissions as $submission) {
    //dpm(array($submission->nid, $planning_form));
    if ($submission->nid == $planning_form)
      continue;
    if (!$submission->is_draft)
      $c2++;
    $c1++;
  } // foreach

  $output = l("$c2 / $c1", "node/" . $row->nid . "/submissions", array('attributes' => array('title' => 'View survey submissions for project ' . $row->node_title . ' ...')));
 
  $num = _slnsite_count_project_plan_responses($project);
  
  $complete = isset($project->field_project_is_complete) && _slnsite_array_keys_exist($project->field_project_is_complete, LANGUAGE_NONE, 0, 'value') && $project->field_project_is_complete[LANGUAGE_NONE][0]['value'];
  
  if ($c1 == $c2) 
    $output .= "<br/>(<small>Plans: $num</small>)";
  else if ($complete)
    $output .= " (<small>Complete</small>)";
 
 
?>
<?php print $output; ?>






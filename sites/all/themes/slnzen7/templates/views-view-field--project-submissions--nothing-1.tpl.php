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
 * The above will guarantee that you'll always get the correct data,
 * regardless of any changes in the aliasing that might happen if
 * the view is modified.
 */
 
  $output = "to do";
  // load submission data
  // $row has uid, nid, and sid
  //dpm($row);
  //return;
  // change from submissions-based view to project node-based view changes nid and sid property names
  //$sid = $row->sid;
  //$nid = $row->node_nid;
  $sid = $row->webform_submissions_node_sid;
  $nid = $row->nid;                                       // note that this is now the project node id, not the survey node id
  $surveynid = $row->node_field_data_field_project_surveys_nid;
  $submission = webform_get_submission($surveynid, $sid);
  $surveynode = node_load($surveynid);
  $fkcids = array_column($surveynode->webform['components'], 'cid', 'form_key');
  //$cidsfk = array_column($surveynode->webform['components'], 'form_key', 'cid');
  $subject_uid = (array_key_exists('sln_subject_id', $fkcids) && array_key_exists($fkcids['sln_subject_id'], $submission->data))? $submission->data[$fkcids['sln_subject_id']] : 0;
  if (is_array($subject_uid)) {
     //dpm($subject_uid);
    if (array_key_exists('uid', $subject_uid))
      $subject_uid = $subject_uid['uid'];
    else if (array_key_exists(0, $subject_uid))
      $subject_uid = $subject_uid[0];
    else {
      //dpm($subject_uid);        // TODO: remove. replace with what?
    }
  }
  if (!$subject_uid)
    $subject_uid = $row->users_webform_submissions_uid;

  if ($subject_uid > 0) {
    $subject_acct = user_load($subject_uid);
    
    $output =  ($subject_acct && $subject_acct->uid) ? $subject_acct->name : "invalid uid: " . $subject_acct->uid;
  }
  else {
    $output = "subject not set ($subject_uid)";
  }
 //dpm($row);
?>
<?php print $output; ?>


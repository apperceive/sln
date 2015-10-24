<?php

/**
 * @file
 * Page callbacks for admin pages of the slnsite module.
 */

 /**
 * Menu callback; show settings form.
 *
 */
function _slnsite_admin_settings($form, &$form_state)  {

  $form['plan_base']['plan_base_text'] = array(
    '#type' => 'fieldset',
    '#prefix' => '<div id="plan-fieldset-wrapper">',
    '#suffix' => '</div>',
    '#title' => t('Planning survey base form settings'),
  );
  if (!empty($form_state['values']['slnsite_plan_base_node_title'])) {
    $form['plan_base']['plan_base_text']['slnsite_plan_base_node_id'] = array(
      '#type' => 'textfield',
      '#title' => t('Node id of your Planning Survey Base'),
      '#default_value' => variable_get('slnsite_plan_base_node_id', ''),
      '#description' => t('Node <em>id</em> for the planning survey base used to generate plans.'),
    );
    $form['plan_base']['plan_base_text']['plan_base_pick_node_id'] = array(
      '#type' => 'button',
      '#value' => t('I prefer to provide the title of the planning base survey'),
      '#weight' => 10,
      '#ajax' => array(
        //'path' => 'slnsite_base_id/js',
        'callback' => '_slnsite_plan_base_id_js',
        'wrapper' => 'plan-fieldset-wrapper',
      ),
    );
  }
  else {
    $form['plan_base']['plan_base_text']['slnsite_plan_base_node_title'] = array(
      '#type' => 'textfield',
      '#title' => t('Title of your Planning Survey Base'),
      '#default_value' => variable_get('slnsite_plan_base_node_title', ''),
      '#description' => t('Node <em>title</em> for the planning survey base used to generate plans.'),
      '#autocomplete_path' => 'slnsite_base_title/js',
    );
    $form['plan_base']['plan_base_text']['plan_base_pick_node_id'] = array(
      '#type' => 'button',
      '#value' => t('I prefer to specify the planning base survey node id'),
      '#weight' => 10,
      '#ajax' => array(
        //'path' => 'slnsite_base_id/js',
        'callback' => '_slnsite_plan_base_id_js',
        'wrapper' => 'plan-fieldset-wrapper',
      ),
    );

  } //else

  $form['data_entry_base']['data_entry_base_text'] = array(
    '#type' => 'fieldset',
    '#prefix' => '<div id="data-entry-fieldset-wrapper">',
    '#suffix' => '</div>',
    '#title' => t('Data Entry survey base form settings'),
  );
  if (!empty($form_state['values']['slnsite_data_entry_base_node_title'])) {
    $form['data_entry_base']['data_entry_base_text']['slnsite_data_entry_base_node_id'] = array(
      '#type' => 'textfield',
      '#title' => t('Node id of your Data Entry Survey Base'),
      '#default_value' => variable_get('slnsite_data_entry_base_node_id', ''),
      '#description' => t('Node <em>id</em> for the data entry survey base used to generate data entry forms.'),
    );
    $form['data_entry_base']['data_entry_base_text']['data_entry_base_pick_node_id'] = array(
      '#type' => 'button',
      '#value' => t('I prefer to provide the title of the data entry base survey'),
      '#weight' => 10,
      '#ajax' => array(
        //'path' => 'slnsite_base_id/js',
        'callback' => '_slnsite_data_entry_base_id_js',
        'wrapper' => 'data-entry-fieldset-wrapper',
      ),
    );
  }
  else {
    $form['data_entry_base']['data_entry_base_text']['slnsite_data_entry_base_node_title'] = array(
      '#type' => 'textfield',
      '#title' => t('Title of your Data Entry Survey Base'),
      '#default_value' => variable_get('slnsite_data_entry_base_node_title', ''),
      '#description' => t('Node <em>title</em> for the data entry survey base used to generate data entry forms.'),
      '#autocomplete_path' => 'slnsite_base_title/js',
    );
    $form['data_entry_base']['data_entry_base_text']['data_entry_base_data_entry_node_id'] = array(
      '#type' => 'button',
      '#value' => t('I prefer to specify the data entry base survey node id'),
      '#weight' => 10,
      '#ajax' => array(
        //'path' => 'slnsite_base_id/js',
        'callback' => '_slnsite_data_entry_base_id_js',
        'wrapper' => 'data-entry-fieldset-wrapper',
      ),
    );

  } //else

  $form = system_settings_form($form);

  return $form;
} //_slnsite_admin_settings

/**
 * Implements menu callback for AJAX addition.
 */
function _slnsite_plan_base_id_js($form, &$form_state) {
  //watchdog('sln', 'form and form_state: ' . var_export(array($form, &$form_state), 1));
  return $form['plan_base']['plan_base_text'];
} // _slnsite_plan_base_id_js

/**
 * Implements menu callback for AJAX addition.
 */
function _slnsite_data_entry_base_id_js($form, &$form_state) {
  //watchdog('sln', 'form and form_state: ' . var_export(array($form, &$form_state), 1));
  return $form['data_entry_base']['data_entry_base_text'];
} // _slnsite_data_entry_base_id_js

/**
 * Validate the _slnsite_admin_settings form.
 *
 */
function _slnsite_admin_settings_validate($form, &$form_state) {
  //dpm(get_defined_vars());
  // TODO: duplicate for use with data entry base node
  if (array_key_exists('slnsite_plan_base_node_id', $form_state['values']))
    $nid = $form_state['values']['slnsite_plan_base_node_id'];
  if (isset($nid)) {
    if (empty($nid)) {
      form_set_error('slnsite_plan_base_node_id', t('You must specify a node <em>nid</em>.'));
    }
    else {
      $node = node_load($nid);
      if ($node == FALSE) {
        form_set_error('slnsite_plan_base_node_id', t('No post was published with <em>nid</em> !nid.', array('!nid' => $nid)));
      }
      else {
        variable_set('slnsite_plan_base_node_title', $node->title);
      }
    }
  }
  elseif (!empty($form_state['values']['slnsite_plan_base_node_title'])) {
    $query = new EntityFieldQuery();
    $entities = $query->entityCondition('entity_type', 'node')
    ->propertyCondition('title', $form_state['values']['slnsite_plan_base_node_title'])
    ->range(0, 1)
    ->execute();
    if (!empty($entities['node'])) {
      $node = node_load(array_shift(array_keys($entities['node'])));
    }
    if ($node == FALSE) {
      form_set_error('slnsite_plan_base_node_title', t('No post was published with this title.'));
    }
    else {
      variable_set('slnsite_plan_base_node_id', $node->nid);
    }
  }
  else {
    form_set_error('slnsite_plan_base_node_title', t('You must specify a node title.'));
  }

  if (array_key_exists('slnsite_data_entry_base_node_id', $form_state['values']))
    $nid = $form_state['values']['slnsite_data_entry_base_node_id'];
  if (isset($nid)) {
    if (empty($nid)) {
      form_set_error('slnsite_data_entry_base_node_id', t('You must specify a node <em>nid</em>.'));
    }
    else {
      $node = node_load($nid);
      if ($node == FALSE) {
        form_set_error('slnsite_data_entry_base_node_id', t('No post was published with <em>nid</em> !nid.', array('!nid' => $nid)));
      }
      else {
        variable_set('slnsite_data_entry_base_node_title', $node->title);
      }
    }
  }
  elseif (!empty($form_state['values']['slnsite_data_entry_base_node_title'])) {
    $query = new EntityFieldQuery();
    $entities = $query->entityCondition('entity_type', 'node')
    ->propertyCondition('title', $form_state['values']['slnsite_data_entry_base_node_title'])
    ->range(0, 1)
    ->execute();
    if (!empty($entities['node'])) {
      $node = node_load(array_shift(array_keys($entities['node'])));
    }
    if ($node == FALSE) {
      form_set_error('slnsite_data_entry_base_node_title', t('No post was published with this title.'));
    }
    else {
      variable_set('slnsite_data_entry_base_node_id', $node->nid);
    }
  }
  else {
    form_set_error('slnsite_data_entry_base_node_title', t('You must specify a node title.'));
  }

} // _slnsite_admin_settings_validate

/**
 * Implements XXX.
 */
function _slnsite_admin_formgen($form, &$form_state) {
  //drupal_set_title("XXX");
  $form['#attributes']['class'][] = 'sln-formgen';
  $form['introduction'] = array(
    '#type' => 'markup',
    '#markup' => "<p>Select a survey form and generate corresponding planning and data entry forms, using the same questions as the survey.</p>",
  );

  $form['requirednote'] = array(
    '#type' => 'markup',
    '#markup' => "<p><small><span class='form-required'>*</span> Indicates that a field entry is required.</small></p>",
  );

  module_load_include('inc', 'slnsite', 'slnsite.utils');
  $options = _slnsite_surveys_as_options();

  // TODO: select the survey from all sln_surveys as options
  $form['survey'] = array(
    '#type' => 'select',
    '#title' => t('Survey'),
    '#required' => TRUE,
    '#options' => $options,
    //'#default_value' => $category['selected'],
    '#description' => t('Select a survey form to use as a template for new forms checked below.'),
  );

  $form['genforms'] = array(
    '#title' => t('Check which forms you wish to generate:'),
    '#required' => TRUE,
    '#type' => 'checkboxes',
    '#options' => array
      (
      'plan' => 'Planning Form',
      'data' => 'Data Entry Form',
      ),
    //'#default_value'   => variable_get('fd_boxes', 0),
    '#description' => t('Select at least one form type to generate.'),
  );

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title template'),
    '#maxlength' => 60,
    '#required' => TRUE,
    '#default_value' => "[slnsite:node_title] [slnsite:form_type]",
    '#description' => t("The titles for the generated forms. Use [slnsite:node_title] and [slnsite:form_type] as tokens for the current survey title and the selected form within the new title. Form types are 'Planning Form' and 'Data Entry Form'"),
  );

  // $form['nid'] = array(
    // '#type' => 'hidden',
    // '#value' => $nid,
  // );

  $form['submit'] = array(
    '#type' => 'submit',
    '#access' => TRUE,
    '#value' => t('Generate Selected Forms'),
  );


  return $form;
} // _slnsite_admin_formgen

/**
 * Implements XXX.
 */
function _slnsite_admin_formgen_validate($form, &$form_state) {

} // _slnsite_admin_formgen_validate

/**
 * Implements XXX.
 */
function _slnsite_admin_formgen_submit($form, &$form_state) {
  $surveynodeid = $form_state['values']['survey'];
  $surveynode = node_load($surveynodeid);
  $text = "Generated forms based on " . $surveynode->title;
  $titletemplate = $form_state['values']['title'];

  $formnames = array();
  foreach ($form_state['values']['genforms'] as $key => $genform) {
    if ($genform) {
      $formnames[] = $key;
      switch ($key) {
        case 'plan':
          $title = str_replace(array('[slnsite:node_title]', '[slnsite:form_type]'), array($surveynode->title, 'Planning Form'), $titletemplate);
          _slnsite_create_planning_webform($surveynode, $title);
        break;
        case 'data':
          $title = str_replace(array('[slnsite:node_title]', '[slnsite:form_type]'), array($surveynode->title, 'Data Entry Form'), $titletemplate);
          _slnsite_create_data_entry_webform($surveynode, $title);
        break;
      } // switch
    } // if
  } // for
  $text .= ' for: ' . implode(', ', $formnames);
  //dpm(get_defined_vars());
  drupal_set_message(check_plain($text));
} // _slnsite_admin_formgen_submit

/**
 * Implements menu callback for admin content project submissions manager.
 */
function _slnsite_admin_project_submissions() {

  drupal_set_title('Project 360 Submissions');

  // construct project array (nid => array(title))
  
  // TODO: add status to sql based on webform_submission.is_draft
  
  //$sql = "select wsd.sid as submissionid, wsd.data as projectid, n.title as `project title`, wsd.nid as surveynid, n2.title as 'survey title', wsd2.data as `subject` , wsd3.data as `participant` from webform_submitted_data wsd, webform_component wc, node n, node n2, webform_component wc2, webform_submitted_data wsd2, webform_component wc3, webform_submitted_data wsd3 where n.nid = wsd.data and wc.nid = wsd.nid and wc.cid = wsd.cid and wc.form_key = 'sln_project_id' and n2.nid = wsd.nid and wc2.nid = wsd2.nid and wc2.cid = wsd2.cid and wc2.form_key = 'sln_subject_id' and wsd2.sid = wsd.sid and wc3.nid = wsd3.nid and wc3.cid = wsd3.cid and wc3.form_key = 'sln_participant_id' and wsd3.sid = wsd.sid order by 2, 3, 1";

  $sql = "SELECT nid, title FROM node WHERE type = 'sln_project_360' AND status = 1 ORDER BY nid";
  $q = db_query($sql);
  $projectdata = array();  
  foreach ($q as $r) { 
    $projectnode = node_load($r->nid);
    if ($projectnode) {
      $projectdata[$r->nid] = array(
        'title' => $r->title,
        'surveys' => array(
          'self' => array(
            'nid' => $projectnode->field_project_form_self[LANGUAGE_NONE][0]['nid'],
            'title' => 'XXX',
          ),
          'supervisor' => array(
            'nid' => $projectnode->field_project_form_supervisor[LANGUAGE_NONE][0]['nid'],
            'title' => 'XXX',
          ),
          'observer' => array(
            'nid' => $projectnode->field_project_form_observer[LANGUAGE_NONE][0]['nid'],
            'title' => 'XXX',
          ),
          'plan' => array(
            'nid' => $projectnode->field_project_form_planning[LANGUAGE_NONE][0]['nid'],
            'title' => 'XXX',
          ),
          /*
          'self' => $projectnode->field_project_form_self[LANGUAGE_NONE][0]['nid'],
          'supervisor' => $projectnode->field_project_form_supervisor[LANGUAGE_NONE][0]['nid'],
          'observer' => $projectnode->field_project_form_observer[LANGUAGE_NONE][0]['nid'],
          'plan' => $projectnode->field_project_form_planning[LANGUAGE_NONE][0]['nid'],
          */
        ),
        'participants' => array(),
      );
    } // if projectnode
  } // foreach 
  
  $data = array();
  
  foreach ($projectdata as $pid => $pd) {
    foreach ($pd['surveys'] as $surveytype => $surveydata) {

      $survey_nid = $surveydata['nid'];
      $surveynode = node_load($survey_nid);
          
      // map form_keys to cid's
      $fkcids = array_column($surveynode->webform['components'], 'cid', 'form_key');

      // get keys for required/standard sln_survey hidden fields
      $cid_project_id = $fkcids['sln_project_id'];
      $cid_participant_id = array_key_exists('sln_participant_id', $fkcids) ? $fkcids['sln_participant_id'] : 0;
      $cid_subject_id = array_key_exists('sln_subject_id', $fkcids) ? $fkcids['sln_subject_id'] : 0;

      //$submissions = webform_get_submissions(array('nid' => $form_nid, 'uid' => $manager_uid));
      $submissions = webform_get_submissions($survey_nid);
      foreach ($submissions as $submission) {
        $sub_project_id = $submission->data[$cid_project_id][0];
        // $sub_project_id = _slnsite_submission_data('sln_project_id', $submission, $surveynode, -1);        
        if ($sub_project_id == $pid) {
          $sub_subject_id = array_key_exists('sln_subject_id', $fkcids) ? $submission->data[$cid_subject_id][0] : "";
          if ($sub_subject_id)
            $sub_subject_id = _slnsite_project_participant($pid, $sub_subject_id);
          $sub_participant_id = array_key_exists('sln_participant_id', $fkcids) ? $submission->data[$cid_participant_id][0] : "";
          if ($sub_participant_id)
            $sub_participant_id = _slnsite_project_participant($pid, $sub_participant_id);
          $data[$submission->sid] = array(/*'pid' => $pid, */ 'sid' => l($submission->sid, "node/$survey_nid/submission/$submission->sid/edit", array('attributes' => array('target' => '_blank'))),  'title' => l($pd['title'], "node/$pid", array('attributes' => array('target' => '_blank'))), 'type' => $surveytype,  'subject' => $sub_subject_id,  'participant' => $sub_participant_id, 'status' => $submission->is_draft ? "" : "Complete");
        } // if
      } // foreach
    } // pd survey
  } // foreach pd
  
  /*
  $sql = "SELECT '???' AS project, ws.sid, ws.nid, n.title, n.status FROM webform_submissions ws, node n WHERE ws.nid = n.nid ORDER BY nid, sid ";
  $q = db_query($sql);
  $data = array();
  foreach ($q as $r) {  
    $submission = webform_get_submission($r->nid, $r->sid);
    //dpm($submission);
    $data[] = (array) $r;
  }
  $header = array('project id and title', 'submission id', 'survey nid', 'survey title', 'survey status');
  */
  
  $header = array(/*'pid' => 'PID',*/ 'sid' => 'Submission ID',  'title' => 'Project Title', 'type' => 'Survey Type', 'subject' => 'Subject', 'participant' => 'Participant', 'status' => 'Status');
  //$class = "sln-admin-project-submissions";
  //$s = _slnsite_display_table($header, $data, $class);
  
  $f = drupal_get_form('_slnsite_project_submissions_form', $header, $data); 
  $s = drupal_render($f);
  //dpm(get_defined_vars());
  
  return $s;
} // _slnsite_admin_project_submissions

function _slnsite_project_submissions_form($form, &$form_state, $header, $data) {
  //$form = array();
  
  // TODO: add filtering and operations (delete, export, others?)
  
  $form['submissions'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $data,
    '#empty' => t('No submissions available.'),
  );
  $form['pager'] = array('#markup' => theme('pager'));
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  return $form;
  
} // _slnsite_project_submissions_form


function _slnsite_project_submissions_form_submit($form, &$form_state) {

  // how do a confirm for multiple submission deletes
  // how pass op = delete? via submit handler? what if using a select
  // other ops: export?
  

  // NOTE: these are submission ids!!!!
  $results = array_filter($form_state['values']['submissions']);
  //drupal_set_message(print_r($results , 1));
  // outputs: Array ( [224] => 224 [225] => 225 [226] => 226 ) 
  

  foreach ($results as $sid => $result) {
    // $surveynodeid = _slnsite_surveynode_from_submission($submissionid);
    // $submission = webform_get_submission($surveynodeid, $submissionid);
    // $surveynode = node_load($surveynodeid);
    // webform_submission_delete($surveynode, $submission)
  } // foreach results
  
  // Better:?
  // DELETE FROM webform_submissions WHERE sid IN (%p)
  // DELETE FROM webform_submitted_data WHERe sid in (%p)
  
} // _slnsite_project_submissions_form_submit

/**
 * Email settings form for callback.
 */
function _slnsite_email_settings($form, &$form_state) {
  global $user;
  if (in_array("administrator", $user->roles) || in_array("director", $user->roles) || ($user->uid == 1)) {
    drupal_set_title("E-mail Settings");
    $form['#attributes']['class'][] = 'sln-email-settings-form';

//    $form['slnsite_test_emails'] = array(
//      '#type' => 'checkbox',
//      '#title' => t('Send test emails using the addresses below'),
//      '#attributes' => array("checked" => "checked"),
//      '#value' => variable_get('slnsite_test_emails', 0),
//      '#default_value' => variable_get('slnsite_test_emails', 0),
//    );
//    if (variable_get('slnsite_test_emails', 0))

    $form['slnsite_test_emails_list'] = array(
      '#type' => 'textarea',
      '#rows' => 8,
      //'#title' => t('Send test emails to the addresses below'),
      '#description' => "Separate email addresses by a comma. Leave blank to send emails to users.",
      '#maxlength' => 60,
      '#default_value' => variable_get('slnsite_test_emails_list', ""),
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#access' => TRUE,
      '#value' => t('Save Settings'),
      '#weight' => 5,
      //'#submit' => array('slnsite_email_settings_submit'),
    );
  }
  else {
    drupal_set_message("Only administrators and managers can edit e-mail notification templates.", 'warning');
    drupal_goto("<front>");
  }
  return $form;
}  // _slnsite_email_settings

/**
 * Email settings form validation routine.
 */
function _slnsite_email_settings_submit($form, &$form_state) {
  //variable_set('slnsite_email_new_appraisal', $form_state['values']['slnsite_email_new_appraisal']);
  //variable_set('slnsite_email_new_appraisal_new_user', $form_state['values']['slnsite_email_new_appraisal_new_user']);
  //variable_set('slnsite_email_new_access', $form_state['values']['slnsite_email_new_access']);
  //variable_set('slnsite_email_new_access_new_user', $form_state['values']['slnsite_email_new_access_new_user']);
  variable_set('slnsite_test_emails_list', $form_state['values']['slnsite_test_emails_list']);
  drupal_set_message("Email settings have been saved.", "info");
} // _slnsite_email_settings_submit


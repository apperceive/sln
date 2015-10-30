<?php

/**
 * @file
 * Page callbacks for the webform features and hooks in the slnsite module.
 */

module_load_include('inc', 'slnsite', 'slnsite.utils');

/**
 * Allow modules to modify a "display only" webform component.
 *
 * @param array $element
 *   The display element as returned by _webform_display_component().
 * @param array $component
 *   A Webform component array.
 *
 * @see _webform_display_component()
 */
function slnsite_webform_component_display_alter(&$element, &$component) {
  //if ($component['cid'] == 10) {
  //  $element['#title'] = 'My custom title';
  //  $element['#default_value'] = 42;
  //}
  //dpm(get_defined_vars());
} // slnsite_webform_component_display_alter


/**
 * Allow modules to modify a webform component that is going to be rendered in a form.
 *
 * @param array $element
 *   The display element as returned by _webform_render_component().
 * @param array $component
 *   A Webform component array.
 *
 * @see _webform_render_component()
 */
function slnsite_webform_component_render_alter(&$element, &$component) {
  /*** initialize common variables used for altering elements ***/
  // todo: setup clean environment with project, metadata, submission, surveys, fields, constants, etc in object
  // TODO perform sanity checks on project, submission, etc. return early as possible. BE CAREFUL.   NO! do not skip important elements not related to submission (e.g., radios/selects in profile)

  global $user;
  $acct = user_load($user->uid); // load profile vars
  $project_type = SLN_PROJECT_TYPE_DEFAULT;
  $project_lang = '';
  $form_key = array_key_exists('form_key', $component) ? $component['form_key'] : '';
  $extra = array_key_exists('extra', $component) ? $component['extra'] : array(); 
  $etype = array_key_exists('#type', $element) ? $etype = $element['#type'] : '';
  // CONSIDER: use $component['type'] or $element['webform_component']['type'] instead, layouts don't have $element['#type'] defined
  $ctype = array_key_exists('type', $component) ? $component['type'] : '';
  $emarkup = array_key_exists('markup', $element) ? $element['#markup'] : '';
  $translations_text = array_key_exists('translations', $extra) ? trim($extra['translations']) : ''; 
  $surveynodeid = 0;
  $submissionid = 0;
  $subject_cid = 0;
  $surveynode = NULL;
  $submission = NULL;
 
  // on first page, there is no $_REQUEST and so we must use arg 1, which is the participant token  
  if (array_key_exists('details', $_REQUEST) && array_key_exists('sid', $_REQUEST['details']))
    $submissionid = $_REQUEST['details']['sid'];
  elseif ((arg(0) == 'node') && (arg(1) > 0) && (arg(2) == 'submission') && (arg(3) > 0) && (arg(4) == 'edit'))
    $submissionid = arg(3);
  elseif ((arg(0) == 'survey') && (arg(1) > 0)) { 
    // look up submission with token used participant id (uniquely)     
    $sql = "SELECT sid FROM webform_submitted_data d, webform_component wc WHERE d.nid = wc.nid AND d.cid = wc.cid AND wc.form_key = 'sln_participant_id' AND d.data = :uniqid";
    $submissionid = db_query($sql, array(':uniqid' => arg(1)))->fetchField();
  } // elseif
    
  if (array_key_exists('nid', $component));
    $surveynodeid = $component['nid'];
  
  // TODO: move to specific block if this load seems too heavy for just one case
  if ($surveynodeid)
    $surveynode = node_load($surveynodeid);
  $instrument = $surveynode ? $surveynode->field_sln_survey_instrument[LANGUAGE_NONE][0]['tid'] : NULL;
    
  if ($surveynodeid && $submissionid)
    $submission = webform_get_submission($surveynodeid, $submissionid);
  // else return?  NO! do not skip important elements not related to submission (e.g., radios/selects in profile)
  
  // do not bother with these lookups if no submission, since there are no keys in that case
  if ($surveynodeid && $submission) {
    $subject_cid = _slnsite_survey_component_cid($surveynodeid, 'sln_subject_id');
    //$project_cid = _slnsite_survey_component_cid($surveynodeid, 'sln_project_id');  // why do we need this other than to validate?
  } // if submission

  // CONSIDER: why is projid needed here instead of $project_nid?  
  //if ($project_cid && array_key_exists($project_cid, $submission->data) && array_key_exists(0 ,$submission->data[$project_cid])) 
  //  $projid = $submission->data[$project_cid][0];  // todo: why bother with this?
  if ($subject_cid && array_key_exists($subject_cid, $submission->data) && array_key_exists(0 ,$submission->data[$subject_cid]))  
    $subject_token = $submission->data[$subject_cid][0];   

  $project_nid = _slnsite_project_token(arg(1));
  if (!$project_nid)
    $project_nid = _slnsite_submission_data('sln_project_id', $submissionid, $surveynodeid);  // todo: consider: pass survey object instead?

  // home: use  $submission->data[$project_cid][0] if only way to get it? test for it?
  if ($project_nid) {
    $project = node_load($project_nid);
    if ($project) {    
      if (count($project->field_project_survey_language))
        $project_lang = $project->field_project_survey_language[LANGUAGE_NONE][0]['value'];
      if (count($project->field_project_type))
        $project_type = $project->field_project_type[LANGUAGE_NONE][0]['tid'];
    } // project
    // else get project from submission?
  } // project_nid
  
  /*** alter elements with specific form_keys ***/
  switch ($form_key) {
    
    case 'subject_person':
      if ($subject_cid && $project_cid) {
        $name = _slnsite_project_participant($project_nid, $subject_token);
        if ($name) {
          $element['#attributes']['readonly'] = 'readonly';
          $element['#attributes']['disabled'] = 'disabled';
          $element['#default_value'] = $name;
        } // name
      } // subject and project cids
      break;
      
    case 'report_insert':
      if ($project_type == SLN_PROJECT_TYPE_360) 
        $form = drupal_get_form("_slnsite_submission_report_form", $project, $surveynode, $submission);
      if ($project_type == SLN_PROJECT_TYPE_180) 
        $form = drupal_get_form("_slnsite_submission_report_form", $project, $surveynode, $submission, SLN_TEMPLATE_SLQ_SUBMISSION_REPORT);
      $element['#markup'] =  drupal_render($form);
      break;
    
    case 'action_map_insert':
      $skills = _slnsite_skills_definitions($instrument, $all = FALSE);
          $element['#markup'] = "";
      
      if ($project_type == SLN_PROJECT_TYPE_180) {
        //$element['#markup'] .= _slnsite_display_table(array('Skill', 'Score'), array(), 'sln-summary-action-table');      
        // dpm($project->field_project_form_observer);        
        $form_nid = count($project->field_project_form_observer) ? $project->field_project_form_observer[LANGUAGE_NONE][0]['nid'] : 0;
        $submissionid = $submission->sid;
      } // SLN_PROJECT_TYPE_180
      
      if ($project_type == SLN_PROJECT_TYPE_360) {
        //if ($project) {    
        $form_nid = count($project->field_project_form_self) ? $project->field_project_form_self[0]['nid'] : 0;
        $submissionid = _slnsite_project_survey_submission($project, $self_form_nid) ;
      } // SLN_PROJECT_TYPE_360

      $data = _slnsite_submission_actions($form_nid, $submissionid, $project_lang);    
      
      foreach ($skills as $skill) {
        $skillname = $skill[0];
        $element['#markup'] .= _slnsite_display_action_map_table($skillname, $data);
      }  // foreach         
      
      if ($project_type == SLN_PROJECT_TYPE_360) {
        $mcdata = _slnsite_project_submissions_summarize($project);
        $mapchartdataarr = array();
        $mcdata2 = array();
        foreach ($mcdata as $nid => $survey) {
          $t = array();
          foreach ($survey['data'] as $key => $arr) {
            $mcdata2[$key][0] = $key;
            $mcdata2[$key][$nid] = sprintf('%01.2f', round($arr['average'], 2));
            $t[] = round($arr['average'], 2);
          }  // foreach $survey
          $mapchartdataarr[] = $t;
        } // foreach $data
        
        // display radar chart
        $mapchartlabels = array("Managing", "Transforming", "Bonding", "Bridging", "Bartering");
        $element['#markup'] .= _slnsite_project_results_radar_chart($project->nid, $mapchartlabels, $mapchartdataarr);
        
        // display chart data 
        $element['#markup'] .= _slnsite_display_table(array('Skill', 'Observer', 'Supervisor', 'Self'), $mcdata2, 'sln-summary-radar-table');
      //} // project
      } // SLN_PROJECT_TYPE_360
      break;
    
    case 'compare_definitions_insert':
      // get snippet / template using termid
      $rows = _slnsite_skills_definitions(SLN_INSTRUMENT_SLQ);
      $element['#markup'] = _slnsite_display_table(array('Strategic Leader Actions', 'Description'), $rows, "sln-skills-table");
      break;
    
    case 'compare_benchmarks_insert':
      //dpm(array('vars' => get_defined_vars(), '$_REQUEST' => $_REQUEST, 'arg(0)' => arg(0), 'arg(1)' => arg(1)));

      if ($project_type == SLN_PROJECT_TYPE_360) {
        $self_form_nid = _slnsite_get_survey($project, 'self');            
        $selfsubmissionid = _slnsite_project_survey_submission($project, $self_form_nid);
        //$userscores = _slnsite_submission_scores($surveynodeid, $submissionid);
        $userscores = _slnsite_submission_scores($self_form_nid, $selfsubmissionid);
        $benchmarktypes = _slnsite_vocabulary_term_names(SLN_BENCHMARK_TYPES);
        $headers = array_merge(array("Strategic Leader Actions", "Your Score"), $benchmarktypes);
        $rows = _slnsite_comparison_data($instrument, $userscores);
        $element['#markup'] = _slnsite_display_table($headers, $rows, "sln-compare-table");
      } // SLN_PROJECT_TYPE_360
      if ($project_type == SLN_PROJECT_TYPE_180) {
        $obs_form_nid = _slnsite_get_survey($project, 'observer');            
        $obssubmissionid = _slnsite_project_survey_submission($project, $obs_form_nid);
        $userscores = _slnsite_submission_scores($surveynodeid, $submissionid);
        //$userscores = _slnsite_submission_scores($obs_form_nid, $obssubmissionid);
        $benchmarktypes = _slnsite_vocabulary_term_names(SLN_BENCHMARK_TYPES);
        $headers = array_merge(array("Strategic Leader Actions", "Your Score"), $benchmarktypes);
        $rows = _slnsite_comparison_data($instrument, $userscores);
        //dpm(get_defined_vars());
        $element['#markup'] = _slnsite_display_table($headers, $rows, "sln-compare-table");
      } // SLN_PROJECT_TYPE_360
      break;
    
    default:
      break;
  } // switch
  
  /*** modify elements ***/
  
  // allow profile control of selects/radios. check for profile field existence (data may not exist in older profiles).
  if (($etype == 'select') && array_key_exists('aslist', $extra) && $extra['aslist'] && isset($acct->profile_select_as_radio) && $acct->profile_select_as_radio) {
    $element['#type'] = 'radios';
  } // profile_select_as_radio
  
  // translate labels
  if (drupal_strlen($translations_text)) {   
    //dpm(get_defined_vars());
    $translations = array();
    $lines = explode("\n", $translations_text);
    foreach ($lines as $line) {
      $arr = explode("|", $translations_text);
      if (count($arr))
        $translations[$arr[0]] = $arr[1];
    }  // for
    // show translation if it exists
    if (array_key_exists($project_lang, $translations)) {
      $text = $translations[$project_lang];
      if (array_key_exists('#title', $element))
        $element['#title'] .= "<span class='translation ar'><br/>$text</span>";
    }  // translation index exists
  } // translation_text
  
  // use translated select if it exists
  if (($ctype == 'select') && array_key_exists('options_source', $extra) && drupal_strlen($extra['options_source']) && (drupal_strlen($project_lang))) {
    $name = 'sln_select_options';
    $vocabulary = taxonomy_vocabulary_machine_name_load($name);
    $terms = taxonomy_get_tree($vocabulary->vid, 0, NULL, TRUE);
    $theterm = NULL;
    foreach ($terms as $term) {
      if ($term->field_select_options_machinename[LANGUAGE_NONE][0]['value'] == $extra['options_source'] . "_" . $project_lang) {
        $theterm = $term;
        break;
      } // if language matches
    } // foreach term
    if ($theterm)
      $element['#options'] = _slnsite_select_options_from_term($theterm);
  } // select translation

  /*** replace tokens in any markup elements if found ***/  
  
  if (($ctype == 'markup') && (strpos($emarkup, '[department]') !== FALSE)) {
    // todo: change from department to [project]
    $dept = '<em>Department name goes here</em>';
    $element['#markup'] = str_replace('[department]', $dept, $element['#markup']);
  } // [department]

  if (($ctype == 'markup') && (strpos($emarkup, '[subject]') !== FALSE)) {
    if ($subject_token && $project_nid) {
      $name = _slnsite_project_participant($project_nid, $subject_token);
      $element['#markup'] = str_replace('[subject]', $name, $element['#markup']);
    } // $subject_token && $project_nid
  } // [subject]

  //dpm(array('vars' => get_defined_vars(), '$_REQUEST' => $_REQUEST, 'arg(0)' => arg(0), 'arg(1)' => arg(1)));
  //dpm(get_defined_vars());
  //dpm($_REQUEST);
  
} // slnsite_webform_component_render_alter

/**
 * Alter the display of a Webform submission.
 *
 * This function applies to both e-mails sent by Webform and normal display of
 * submissions when viewing through the adminsitrative interface.
 *
 * @param $renderable
 *   The Webform submission in a renderable array, similar to FormAPI's
 *   structure. This variable must be passed in by-reference. Important
 *   properties of this array include #node, #submission, #email, and #format,
 *   which can be used to find the context of the submission that is being
 *   rendered.
 */
function slnsite_webform_submission_render_alter(&$renderable) {

  //dpm(get_defined_vars());
  // Example: Remove page breaks from sent e-mails.
  //if (isset($renderable['#email'])) {
    //dpm(element_children($renderable));
    //$r = $renderable['slq_1'];
    //$r = array_shift(element_children($renderable));
    //$r = NULL;
    foreach (element_children($renderable) as $key) {
      //if ($renderable[$key]['#webform_component']['type'] != 'select') {
      //if (substr($key, 0, 1) !== '#')
        //dpm(array(intval(str_replace("slq_", "", $key))));
        //if (!intval(str_replace("slq_", "", $key)))
        //if ($key !== 'slq_1')
          /*if (!$r && ($renderable[$key]['#webform_component']['type'] == 'markup'))
            $r = $renderable[$key];*/
        if ($renderable[$key]['#webform_component']['type'] == 'pagebreak')
          unset($renderable[$key]);
      //} // if
    } // foreach

  //}

  // check node type for sln_survey_webform
  // check for existing indexes

  // note that markup elements are not displayed
  // hidden are

  /*
  //if (array_key_exists('report_insert', $renderable) && array_key_exists('#value', $renderable['report_insert']) ) {
  if (_slnsite_array_keys_exist($renderable, 'report_insert', '#value')) {
    $renderable['report_insert']['#value'] .= " Some html from <strong>slnsite_webform_submission_render_alter</strong>";
  }
  //if (array_key_exists('fieldset_insert', $renderable) && array_key_exists('#value', $renderable['fieldset_insert'])) {
  // DONE: check for #value key exist (it does not in submission view for non-admin or prived user, even director)
  if (_slnsite_array_keys_exist($renderable, 'fieldset_insert', '#value')) {
    $renderable['fieldset_insert']['#value'] .= " Some html from <strong>slnsite_webform_submission_render_alter</strong>";
  }

  // get all the elements (i.e., everything except ['#xxx'] elements
  foreach (element_children($renderable) as $key) {
    if ($key == 'hidden_test')
      $hidden = $renderable[$key];
    unset($renderable[$key]);
  }
  */

  /*
  $surveynodeid = $renderable['#submission']->nid;
  $submissionid = $renderable['#submission']->sid;
  //$form = drupal_get_form('_slnsite_submission_report_form', $surveynodeid, $submissionid);

  $renderable['status'] = $r;
  $renderable['status']['#webform_component']['type'] = 'markup';
  $renderable['status']['#webform_component']['form_key'] = 'status';
  $renderable['status']['#webform_component']['name'] = 'Status';
  $renderable['status']['#webform_component']['value'] = 'Status goes here?';
  $renderable['status']['#value'] = '';
  $renderable['status']['#title'] = '';
  $renderable['status']['#description'] = _slnsite_submission_report($surveynodeid, $submissionid);
  $renderable['status']['#weight'] = '0';
  */

  /*

  $renderable['test2'] = array(
    '#title' => 'Test title',
    '#value' => '<strong>Test</strong> 2 value',
    '#weight' => '2',
    '#id' => 'test2_id',
    '#format' => 'html',
    //'#webform_component' => $wc,
  );
  */

  //foreach (element_children($renderable) as $key) {
  //  $renderable[$key] = array_merge_recursive ($hidden, $renderable[$key]);
  //}

  /*

  $renderable['test1'] = $hidden;
  $renderable['test1']['#title'] = "<strong>Test</strong> 1 value";
  $renderable['test1']['#weight'] = "1";

  $renderable['test2'] = $hidden;
  $renderable['test2']['#title'] = "<strong>Test</strong> 2 value";
  $renderable['test2']['#weight'] = "2";

  $renderable['test2'] = $hidden;
  $renderable['test2']['#title'] = "<strong>Test</strong> 2 value";
  $renderable['test2']['#weight'] = "2";
 */
  //dpm(get_defined_vars());


 } // slnsite_webform_submission_render_alter

/**
 * Respond to the creation of a new submission from form values.
 *
 * This hook is called when a user has completed a submission to initialize the
 * submission object. After this object has its values populated, it will be
 * saved by webform_submission_insert(). Note that this hook is only called for
 * new submissions, not for submissions being edited. If responding to the
 * saving of all submissions, it's recommended to use
 * hook_webform_submission_presave().
 *
 * @param $submission
 *   The submission object that has been created.
 * @param $node
 *   The Webform node for which this submission is being saved.
 * @param $account
 *   The user account that is creating the submission.
 * @param $form_state
 *   The contents of form state that is the basis for this submission.
 *
 * @see webform_submission_create()
 */
function slnsite_webform_submission_create($submission, $node, $account, $form_state) {

  if (!$submission || !$node)
    return;
  
  if (!isset($submission->completed))
    $submission->completed = _slnsite_submission_is_complete($node->nid, $submission->sid, FALSE);

  //dpm(get_defined_vars());

  //   $submission->new_property = TRUE;
} // slnsite_webform_submission_create

/**
 * Modify a Webform submission, prior to saving it in the database.
 *
 * @param $node
 *   The Webform node on which this submission was made.
 * @param $submission
 *   The Webform submission that is about to be saved to the database.
 */
function slnsite_webform_submission_presave($node, &$submission) {
  //if (in_array($types, array('sln_webform')))

  if (!$submission || !$node)
    return;

  $surveynodeid = $node->nid;
  if (!isset($submission->sid)) {    // return false on any unsaved submission
    $complete = FALSE;
  }
  else {
    $submissionid = $submission->sid;
    $complete = _slnsite_submission_is_complete($surveynodeid, $submissionid, FALSE);
    //drupal_set_message("saved submission with complete: " . ($complete ? 1 : 0));
  }
  $submission->is_draft = $complete ? FALSE : TRUE;

  $submission->completed = $complete;
  

  // set date to now
  $submission->submitted = REQUEST_TIME;


  //dpm(get_defined_vars());

  // Update some component's value before it is saved.
  //$component_id = 4;
  //$submission->data[$component_id][0] = 'foo';

  // another example
  // - See more at: http://www.webomelette.com/drupal-webform-submission-presave-hook

  /*
  // Get the submission user object.
  $user = user_load($submission->uid);

  // Check if $user does have the first name filled.
  if (!empty($user->field_first_name)) {
    $first_name = $user->field_first_name[LANGUAGE_NONE][0]['value'];
    // Add the $user first name to the submission data.
    $submission->data[4]['value'][0] = $first_name;
  }

  // Same as above but for the last name.
  if (!empty($user->field_last_name)) {
    $last_name = $user->field_last_name[LANGUAGE_NONE][0]['value'];
    $submission->data[5]['value'][0] = $last_name;
  }
  */

} // slnsite_webform_submission_presave

/**
 * Respond to a Webform submission being inserted.
 *
 * Note that this hook is called after a submission has already been saved to
 * the database. If needing to modify the submission prior to insertion, use
 * hook_webform_submission_presave().
 *
 * @param $node
 *   The Webform node on which this submission was made.
 * @param $submission
 *   The Webform submission that was just inserted into the database.
 */
function slnsite_webform_submission_insert($node, $submission) {
  
  if (!$submission || !$node)
    return;
  
  if (!isset($submission->completed))
    $submission->completed = _slnsite_submission_is_complete($node->nid, $submission->sid, FALSE);

  //dpm(get_defined_vars());

  // Insert a record into a 3rd-party module table when a submission is added.
  /*
  db_insert('mymodule_table')
    ->fields(array(
      'nid' => $node->nid,
      'sid' => $submission->sid,
      'foo' => 'foo_data',
    ))
    ->execute();
  */
} // slnsite_webform_submission_insert

/**
 * Respond to a Webform submission being updated.
 *
 * Note that this hook is called after a submission has already been saved to
 * the database. If needing to modify the submission prior to updating, use
 * hook_webform_submission_presave().
 *
 * @param $node
 *   The Webform node on which this submission was made.
 * @param $submission
 *   The Webform submission that was just updated in the database.
 */
function slnsite_webform_submission_update($node, $submission) {

  if (!$submission || !$node)
    return;
  
  if (!isset($submission->completed))
    $submission->completed = _slnsite_submission_is_complete($node->nid, $submission->sid, FALSE);
  //dpm(get_defined_vars());

  // Update a record in a 3rd-party module table when a submission is updated.
  /*
  db_update('mymodule_table')
    ->fields(array(
      'foo' => 'foo_data',
    ))
    ->condition('nid', $node->nid)
    ->condition('sid', $submission->sid)
    ->execute();
  */
} // slnsite_webform_submission_update


// TODO: give sln_survey_webforms the defaults that we want:
// save button, redirect, message, save drafts, no draft save button others?
/**
 * Alter default settings for a newly created webform node.
 *
 * @param array $defaults
 *   Default settings for a newly created webform node as defined by webform_node_defaults().
 *
 * @see webform_node_defaults()
 */
/**
 function slnsite_webform_node_defaults_alter(&$defaults) {
  $defaults['allow_draft'] = '1';
}
 */

 /**
 * Alter the list of Webform component default values.
 *
 * @param $defaults
 *   A list of component defaults as defined by _webform_defaults_COMPONENT().
 * @param $type
 *   The component type whose defaults are being provided.
 *
 * @see _webform_defaults_component()
 */
/**
// TODO: give sln_survey_webform components the defaults that we want:
// required, aslist, others?
function slnsite_webform_component_defaults_alter(&$defaults, $type) {
  // Alter a default for all component types.
  $defaults['required'] = 1;

  // Add a default for a new field added via hook_form_alter() or
  // hook_form_FORM_ID_alter() for all component types.
  $defaults['extra']['added_field'] = t('Added default value');

  // Add or alter defaults for specific component types:
  switch ($type) {
    case 'select':
      $defaults['extra']['optrand'] = 1;
      break;

    case 'textfield':
    case 'textarea':
      $defaults['extra']['another_added_field'] = t('Another added default value');
  }
}
 */


/**
 * Implements webform options callback.
 */
function _slnsite_webform_select_options_callback() {
  //$numargs = func_num_args();
  //watchdog('slnsite', var_export(func_get_args(), 1));
  $opts = func_get_args();  // list in forth position
  return $opts[2];    // note: changed from 3 to 2 in webforms 4.x
} // _slnsite_webform_select_options_callback



/**
 * Modify the draft to be presented for editing.
 *
 * When drafts are enabled for the webform, by default, a pre-existig draft is
 * presented when the webform is displayed to that user. To allow multiple
 * drafts, implement this alter function to set the $sid to NULL, or use your
 * application's business logic to determine whether a new draft or which of
 * he pre-existing drafts should be presented.
 *
 * @param integer $sid
 *    The id of the most recent submission to be presented for editing. Change
 *    to a different draft's sid or set to NULL for a new draft.
 * @param array $context
 *    Array of context with indices 'nid' and 'uid'.
 */
function _slnsite_webform_draft_alter(&$sid, $context) {
  if ($_GET['newdraft']) {
    $sid = NULL;
  }
}

/**
 * Alter access to a Webform submission.
 *
 * @param $node
 *   The Webform node on which this submission was made.
 * @param $submission
 *   The Webform submission.
 * @param $op
 *   The operation to be performed on the submission. Possible values are:
 *   - "view"
 *   - "edit"
 *   - "delete"
 *   - "list"
 * @param $account
 *   A user account object.
 * @return
 *   TRUE if the current user has access to submission,
 *   or FALSE otherwise.
 */
function slnsite_webform_submission_access($node, $submission, $op = 'view', $account = NULL) {

  switch ($op) {
    case 'view':
      return TRUE;
      break;
    case 'edit':
      return TRUE;
      break;
    case 'delete':
      return FALSE;
      break;
    case 'list':
      return TRUE;
      break;
  } // switch

} // slnsite_webform_submission_access



<?php

/**
 * @file
 * Page callbacks for non-admin pages of the slnsite module.
 */
////////  MENU CALLBACKS (EXCEPT TESTING)

// access checks, then show surveys, messages or results as appropriate
/**
 * Implements XXX.
 */
function _slnsite_project_page($project = NULL, $leader = NULL , $uid = NULL) {
  global $user;

  //module_load_include('inc', 'slnsite', 'slnsite.webform');
  //module_load_include('inc', 'slnsite', 'slnsite.utils');
  //module_load_include('inc', 'slnsite', 'slnsite.test');

  $s = ""; // output
  $s .= "<ul>";

  _slnsite_validate_current_user();

  // if uid is present, it is $user? if not, deny access or use the logged in user.
  // TODO: ask like Amazon? "are you bob?"
  if (!$uid) {
    $uid = $user->uid;
  }
  else {
    if ($uid != $user->uid) {
      // TODO: check for director, project researcher to see if we should allow review of user data
      drupal_set_message(t("Invalid address (URL). You do not have access to that user's survey."), 'warning');
      drupal_set_message(t("Please contact an administrator."));
      // TODO: how can we improve this message?
      return "Done.";
    } // $uid != $user->uid
  } // !$uid

  // does the survey exist? project? how find from short name? allow project node id?
  // TODO: check that project is either valid object or id
  if (!$project) {
    drupal_set_message(t("Invalid project name in address (URL)."), 'warning');
    drupal_set_message(t("Please contact an administrator."));
    // TODO: how can we improve this message?
    return "Done.";
  } // !$project

  // check for project id or name
  if (is_numeric($project) && ($project > 0))
    $projectnode = node_load($project);
  else {
    $path = drupal_get_normal_path("project/$project");
    $pid = str_replace("node/", "", $path);
    if (!$pid) {
      drupal_set_message(t("Invalid project specified in URL. Unable to locate project path: %path.", array('%path' => $path)), 'warning');
      drupal_set_message(t("Please contact an administrator."));
      // TODO: how can we improve this message?
      return "Done.";
    }  // !$pid
    $projectnode = node_load($pid);
  } // $project > 0

  if (!$projectnode) {
    drupal_set_message(t("Invalid project specified in address (URL). Unable to locate project: %project.", array('%project' => $project)), 'warning');
    drupal_set_message(t("Please contact an administrator."));
    drupal_set_message(t("Returning to your dashboard."));
    drupal_goto("dashboard");
  }  // !$projectnode

  // TODO: remove this eventually or turn into welcome message
  $s .= "<li>Project title is $projectnode->title.</li>";

  // is the current user a member of the project? if not, deny access, present contact form
  // CONSIDER: director, project researcher, assistant access? leader?
  // TODO: add contact form for easy support and problem resolution
  // TODO: use full profile name
  if (_slnsite_user_is_project_participant($projectnode)) {
    $s .= "<li>You ($user->name) are a participant of this project.</li>";
  }
  else {
    if (!_slnsite_user_is_project_administrator($projectnode)) {
      drupal_set_message(t("There is no record of you being a participant of this project."), 'warning');
      drupal_set_message(t("Please contact an administrator."));
      $s .= "<li>Support contact form goes here.</li>";
      $s .= "<li>Done.</li>";
      $s .= "</ul>";
      return $s;
    }
  }  // _slnsite_user_is_project_participant

  // is the leader arg present? if so, test as valid, if not valid do what?
  // if no leader, but leaders exist in project node, assume self?
  // TODO: convert to a utility function
  if (!$leader) {
    $s .= "<li>Note: No leader argument present. Using self-assessment.</li>";
  }
  else {
    $found = FALSE;
    if (is_array($projectnode->field_project_leaders) && array_key_exists(LANGUAGE_NONE, $field_project_leaders->field_project_participants) && is_array($projectnode->field_project_leaders[LANGUAGE_NONE])) {
      $leaders = $projectnode->field_project_leaders[LANGUAGE_NONE];
      foreach ($leaders as $ndx => $l) {
        if ($l[$ndx]['value'] == $leader) {
          $s .= "<li>Note: Leader is valid. '$leader' found in project leaders list.</li>";
          $found = TRUE;
          break;
        } // if $leader
      }  // foreach
    }
    if (!$found) {
      drupal_set_message(t("Invalid leader in address (URL)."), 'warning');
      drupal_set_message(t("Please contact an administrator."));
      $s .= "<li>Note: Leader is NOT valid. Using self-assessment.</li>";
      $s .= "<li>Support contact form goes here.</li>";
      // TODO: should we assume self assessment? perhaps better to ask.
      // TODO: at very least, make it clear that there was no matching leader
    } // !$found
  } // !$leader

  // has the user already taken the survey? if so, allow changes or just review?
  // NOTE: webform_submissions fields are (sid, nid, uid, is_draft, submitted, remote_addr)
  // TODO: change to allow at least two forms for 'self' and 'leader(s)'
  $surveynodeid = $projectnode->field_project_survey_form[LANGUAGE_NONE][0]['nid'];
  $surveynode = node_load($surveynodeid);

  // TODO: CONSIDER: use webform_get_submissions(array(':uid' => $user->uid, ':nid' => $surveynodeid))

  // TODO: factor code blocks out to a separate function _get_leader_name($projectnodeid, $surveynodeid, $userid)
  $sql = "SELECT * FROM {webform_submissions} WHERE uid = :uid AND nid = :nid";
  $q = db_query($sql, array(':uid' => $user->uid, ':nid' => $surveynodeid));
  $found = FALSE;
  // handle multiple surveys for self and one or more leaders?
  // while ($r = db_fetch_array($q)) {
  foreach ($q as $r) {
    $found = TRUE;
    $sid = $r->sid;
    //$cid = $r->cid;
    $submitted_date = date("D, F d, Y g:i A", $r->submitted);
    $cid = _slnsite_survey_leader_component($surveynodeid);
    $leadername = _slnsite_submission_leadername($surveynodeid, $sid, $cid);
    if ($r->is_draft) {
      $s .= "<li>You have a draft of this survey and may continue to fill it out. sid: $sid, last access: $submitted_date. Leader is: $leadername.</li>";
      $s .= "Link to survey: ";
      // TODO: what next?  Display draft.
    }
    else {
      $s .= "<li>You have already taken this survey. sid: $sid, last access: $submitted_date. Leader is: $leadername.</li>";
      // TODO: what next? Allow review? Goto plan?
    } // draft
  } // while
  if (!$found) {
    $s .= "<li>You have not yet taken any surveys in this project.</li>";
    // TODO: what next?  Display survey? set hidden leader field?
  } // !$found


  // TODO: consider: use webform_get_submissions(array(':uid' => $user->uid, ':nid' => $surveynodeid))

  // TODO: do same for demographics
  $demographicsnodeid = $projectnode->field_project_demographics_form[LANGUAGE_NONE][0]['nid'];
  $demographicsnode = node_load($demographicsnodeid);
  $sql = "SELECT * FROM {webform_submissions} WHERE uid = :uid AND nid = :nid";
  $q = db_query($sql, array(':uid' => $user->uid, ':nid' => $demographicsnodeid));
  $found = FALSE;
  // TODO: limit demographics? offer to pre-fill if one already exists?
  //while ($r = db_fetch_array($q)) {
  foreach ($q as $r) {
    $found = TRUE;
    $sid = $r->sid;
    $cid = $r->cid;
    $submitted_date = date("D, F d, Y g:i A", $r->submitted);
    if ($r->is_draft) {
      $s .= "<li>You have a draft of this demographics survey and may continue to fill it out. sid: $sid, last access: $submitted_date.</li>";
      // TODO: what next?  Display draft.
    }
    else {
      $s .= "<li>You have already taken this  demographics survey. sid: $sid, last access: $submitted_date.</li>";
      // TODO: what next? Allow review? Goto plan?
    }  // draft
  } // while
  if (!$found) {
    $s .= "<li>You have not yet taken this survey's demographics section.</li>";
    // TODO: what next?  Display demographics? offer to reuse if exists already?
  } // found

  // TODO: check for plan

  // TODO: present the form, starting at front or where left off?

  // NOTE: build content so as to render body content as HTML
  // not rendering html in D7
  node_build_content($projectnode);
  $s .= "<li>Project body: " . $projectnode->body[LANGUAGE_NONE][0]['value'] . "</li>";
  // D7, but does not work
  //$s .= "<li>Project body: " . $projectnode->body[0]['#markup'] . "</li>";
  if ($s == "<ul>")
    $s .= "<li>Done.</li>";
  $s .= "</ul>";
  return $s;
}  // _slnsite_project_page

// view dashboard for given account, default to current user
// NOTE: $acct is always either null or a userid (possibly invalid)
/**
 * Implements XXX.
 */
function _slnsite_dashboard_page($acct = NULL) {
  global $user;
  $s = "";
  //$file1 = module_load_include('inc', 'slnsite', 'slnsite.webform');
  //$file2 = module_load_include('inc', 'slnsite', 'slnsite.utils');

  _slnsite_validate_current_user();
  $acctobj = _slnsite_account_object($acct);
  if (!is_object($acctobj)) {
    if (is_numeric($acct) && ($acct > 0)) {
      drupal_set_message(t("Invalid account: %acct. Displaying dashboard for current user.", array('%acct' => $acct)), "warning");
    }
    else {
      // user is viewing their own dashboard, do nothing special
    } // (!is_object($acctobj))
    $acct = $user;
  }
  else {
    $acct = $acctobj;
  } // (!is_object($acctobj))

  if (!_slnsite_user_has_director_role() && ($acct->uid != $user->uid)) {
    drupal_set_message(t("You do not have permission to view that user's dashboard. Displaying dashboard for current user."));
    $acct = $user;
  }  // (!_slnsite_user_has_director_role() && ($acct->uid != $user->uid))


  //drupal_set_message("Welcome to your Strategic Leader Network dashboard.");
  //drupal_set_message("Please complete any surveys in your inbox.");
  //drupal_set_message("You can view reports from previously completed self-assessments from within the 'Completed Items' tab.");

  drupal_set_title(t("Dashboard for %name", array('%name' => $acct->name)), PASS_THROUGH);
  // show projects (open only if non director?)

  //$s = t("<span style='display: inline-block;' class='current-user-email'>%mail</span>", array('%mail' => $acct->mail)); // output
  //$s .= t("<small>userid: %uid, username: %uname</small>", array('%uname' => $acct->name, '%uid' => $acct->uid)); // output
  //$s .= _slnsite_show_roles($acct);
  // show submissions (inbox) for all users

  if (arg(1) == 'inbox') {
    $s .= views_embed_view('user_survey_submissions', 'block_1', $acct->uid);
  }
  else if (arg(1) == 'completed') {
    $s .= views_embed_view('user_survey_submissions', 'block_2', $acct->uid);
  }
  else if (arg(1) == 'projects') {
    //$s .= views_embed_view('user_survey_submissions', 'block_2', $acct->uid);
    if (_slnsite_user_has_director_role() && ($user->uid == $acct->uid)) {
      //$s .= "<h3>All Projects</h3>";
      $s .= views_embed_view('projects_listing', 'block_1');
    } // user is director
    else {
      //if (_slnsite_user_has_manager_role()) {
        //$s .= "<h3>Projects you manage</h3>";
        $s .= views_embed_view('projects_listing', 'block_4', $acct->uid);
      //}
      // participants block
      //$s .= "<h3>Projects you participate in</h3>";
      $s .= views_embed_view('projects_listing', 'block_3', $acct->uid);

      // researchers block
      //$s .= "<h3>Your research projects</h3>";
      $s .= views_embed_view('projects_listing', 'block_5', $acct->uid);

    } // user is director
  }
  else { //default
    if (_slnsite_user_has_director_role() && ($user->uid == $acct->uid)) {
      //$s .= "<h3>All Projects</h3>";
      $s .= views_embed_view('projects_listing', 'block_1');
    } // user is director
    else {
      // $s .= views_embed_view('user_survey_submissions', 'block', $acct->uid);
      //$s .= views_embed_view('user_survey_submissions', 'block_1', $acct->uid);
      $s .= views_embed_view('projects_listing', 'block_4', $acct->uid);
    }
  }



  //$s .= _slnsite_show_account_projects_summary($acct);

  //if (_slnsite_user_has_director_role() || in_array("project researcher", $acct->roles)) {
    // only show "all projects" if user is on their own dashboard (and has proper role)
    //if ($user->uid == $acct->uid) {
      // let view handle filtering through query hook
    //}  // user is acct



  //$s .= _slnsite_show_account_projects_details($acct);

  // what if other roles? research assistants who are also survey takers
  // what about a project researcher?
  // if director, just show all projects, with status',
  // if role is director: also quick links to archive, download, graph - or should these just be on project pages? I think so
  // in general, show surveys, with status and links (take, print)
  // use embedded views?
  //if (drupal_strlen($s) === 0)
  //  $s .= "No projects.";
  return $s;
} // _slnsite_dashboard

////////  PROJECT SUBROUTINES

/**
 *
 * Display view of project participants
 */
function _slnsite_project_participants_listing($project) {
  $s = "";

  //dpm(get_defined_vars());

  // if director researcher or admin
  $s .= "<strong class='sln-project-participants'>". t("Participants:") . "</strong>";
  $s .=  views_embed_view('project_participants', 'block', $project->nid);


  return $s;
} // _slnsite_project_participants_listing

/**
 *
 * Display view of project submissions
 */
function _slnsite_project_survey_submissions($project) {
  $s = "";
  //dpm(get_defined_vars());

  // if director researcher or admin
  $s .= "<strong class='sln-project-submissions'>". t("Submissions for all survey forms in this project:") . "</strong>";
  $s .=  views_embed_view('project_submissions', 'default', $project->nid);

  return $s;
} // _slnsite_project_survey_submissions

function _slnsite_project_assignment_form($form, &$form_state, $project) {

  //dpm(get_defined_vars());

  $form['projectid'] = array(
    '#type' => 'hidden',
    '#value' => $project->nid,
  );

  $form['instructions'] = array(
    '#type' => 'markup',
    '#markup' => "<p>Use this form to assign surveys about subjects to project participants.</p>",
  );


  $participantsopts = _slnsite_project_participants_as_options($project->nid);
  $surveyopts = _slnsite_project_surveys_as_options($project->nid);

  $form['participantid'] = array(
    '#title' => t('Assign Participant'),
    '#required' => TRUE,
    '#type' => 'select',
    '#options' => $participantsopts,
    '#default_value' => isset($form_state['input']['participantid']) ? array($form_state['input']['participantid']) : "",
    '#description' => t("Choose the participant to assign this survey to."),
  );

  $form['subjectid'] = array(
    '#title' => t('Assess Subject'),
    '#required' => TRUE,
    '#type' => 'select',
    '#options' => $participantsopts,
    '#default_value' => isset($form_state['input']['subjectid']) ? array($form_state['input']['subjectid']) : "",
    '#description' => t("Choose the person who is the subject of this survey."),
  );

  $form['surveyid'] = array(
    '#title' => t('Survey'),
    '#required' => TRUE,
    '#type' => 'select',
    '#options' => $surveyopts,
    '#default_value' => isset($form_state['input']['surveyid']) ? array($form_state['input']['surveyid']) : "",
    '#description' => t("Choose the project survey to use for this assignment or choose 'No Survey Assigned' to remove an existing assignment."),
 );

  // todo: make this ajax
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Set',
  );

  return $form;
} // _slnsite_project_assignment_form

function _slnsite_project_assignment_form_validate($form, &$form_state) {

  //dpm(get_defined_vars());
  $form_state['storage']['projectid'] = $form_state['values']['projectid'];
  $form_state['storage']['participantid'] = $form_state['values']['participantid'];
  $form_state['storage']['subjectid'] = $form_state['values']['subjectid'];
  $form_state['storage']['surveyid'] = $form_state['values']['surveyid'];

  // todo: ensure required fields do not have 0 values
  // participant and subject

  // check for submission, create one as needed
  //drupal_set_message("Making change participantid: $participantid and subjectid: $subjectid for surveyid: $surveyid.");

} // _slnsite_project_assignment_form_validate

function _slnsite_project_assignment_form_submit($form, &$form_state) {

  $projectid = $form_state['values']['projectid'];            // the context project
  $participantid = $form_state['values']['participantid'];    // the assignee, the user who will get the assignment
  $subjectid = $form_state['values']['subjectid'];            // the subject of the survey, may be same as participantid
  $surveyid = $form_state['values']['surveyid'];              // the assigned survey, NOTE: This may be none (0)

  // check for 0 surveyid, meaning to delete any existing submission for this form, projectid, participantid and subject

  //if (!($surveyid > 0)) {
  //  drupal_set_message("Non-numeric survey node id: $surveyid");
  //  return;
  //}

  $surveynode = NULL;
  if ($surveyid > 0) {
    $surveynode = node_load($surveyid);
    // test for valid surveynode
    if (!is_object($surveynode) || ($surveynode->type != "sln_survey")) {
      drupal_set_message("Invalid survey node id or type: $surveyid");
      return;
    }
  } // ($surveyid > 0)

  $projectnode = node_load($projectid);
  // test for valid projectnode
  if (!is_object($projectnode) || ($projectnode->type != "sln_project")) {
    drupal_set_message("Invalid project node id or type: $projectid");
    return;
  }

  // map form_keys to cid's
  $fkcids = array_column($surveynode->webform['components'], 'cid', 'form_key');

  // get keys for required/standard sln_survey hidden fields
  $cid_project_id = $fkcids['sln_project_id'];
  $cid_participant_id = $fkcids['sln_participant_id'];
  $cid_subject_id = $fkcids['sln_subject_id'];

  // get all form submissions for same participant and subject and project
  $results = array();
  // foreach ($projectnode->field_project_survey_forms[LANGUAGE_NONE] as $surveyref) {
  foreach ($projectnode->field_project_surveys[LANGUAGE_NONE] as $surveyref) {
    $filters = array('nid' => $surveyref['nid'], 'uid' => $participantid);
    $submissions = webform_get_submissions($filters);
    foreach ($submissions as $submission) {
      // consider: these are not defined indexes?
      // $sub_project_id = array_key_exists($cid_project, $submission->data) ? $submission->data[$cid_project_id][0] : -1;
      $sub_project_id = _slnsite_submission_data('sln_project_id', $submission, $$surveyref['nid'], -1);
      // $sub_participant_id = array_key_exists($cid_participant_id, $submission->data) ? $submission->data[$cid_participant_id][0] : -1;
      $sub_participant_id = _slnsite_submission_data('sln_participant_id', $submission, $$surveyref['nid'], -1);
      // $sub_subject_id = array_key_exists($cid_subject_id, $submission->data) ? $submission->data[$cid_subject_id][0] : -1;
      $sub_subject_id = _slnsite_submission_data('sln_subject_id', $submission, $$surveyref['nid'], -1);
      if (($sub_project_id == $projectid) && ($sub_participant_id == $participantid) && ($sub_subject_id == $subjectid))
        $results[$surveyref['nid']] = $submission;
    } // submissions
  } // foreach survey

  $found = FALSE;   // does submission already exist, if so, keep it, but clear out others for same participant and subject
  if (count($results)) {
    foreach ($results as $nid => $result) {
      if ($surveyid != $nid) {

        // TODO: potentially dangerous action. should we prompt user for delete confirmation?

        webform_submission_delete($nid, $result);
        drupal_set_message("Removing submission for survey: $nid and sid: $result->nid");
      }
      else {
        $found = TRUE;
        $sid = $results[0]->sid;
        drupal_set_message("Submission $sid already exists for projectid: $projectid, participantid: $participantid, subjectid: $subjectid for surveyid: $surveyid.");
      } // ($surveyid != $nid)
    } // foreach result  submission
   } // pre-existing submission

  if (!$found && $surveyid) {
    // create a new submission
    $form_state['values']['submitted'] = array(
      $cid_project_id => $projectid,
      $cid_participant_id => $participantid,
      $cid_subject_id => $subjectid,
    );

    $submission = (object) array(
      'nid' => $surveyid,
      'uid' => $participantid,
      'submitted' => REQUEST_TIME,
      'remote_addr' => ip_address(),
      'is_draft' => TRUE,
      'preview' => FALSE,
      'serial' => _webform_submission_serial_next_value($surveyid, TRUE),
      'data' => webform_submission_data($surveynode, $form_state['values']['submitted']),
    );

    $sid = webform_submission_insert($surveynode, $submission);

    drupal_set_message("Submission $sid created for projectid: $projectid, participantid: $participantid, subjectid: $subjectid for surveyid: $surveyid.");

  } // submission not found

  //dpm(get_defined_vars());

} // _slnsite_project_assignment_form_submit

/**
 * Menu access callback. Ensure a user both access and node 'view' permission.
 */
function _slnsite_project_survey_submissions_access($node, $account = NULL) {
  /*
  global $user;
  $account = isset($account) ? $account : $user;

  $module_access = count(array_filter(module_invoke_all('webform_results_access', $node, $account))) > 0;

  return node_access('view', $node, $account) && ($module_access || user_access('access all webform results', $account) || (user_access('access own webform results', $account) && $account->uid == $node->uid));
  */
  //dpm(get_defined_vars());

  if (_slnsite_user_is_project_administrator($node) )
    return TRUE;
  return FALSE;
} // _slnsite_project_survey_submissions_access

/**
 * Menu access callback. Ensure a user both access and node 'view' permission.
 */
function _slnsite_project_survey_assignments_access($node, $account = NULL) {

  if (_slnsite_user_is_project_administrator($node) )
    return TRUE;
  return FALSE;
} // _slnsite_project_survey_assignments_access


/**
 * Menu access callback. Ensure a user both access and node 'view' permission.
 */
function _slnsite_project_participants_listing_access($node, $account = NULL) {
  // TODO: limit this to director, admin and researcher. also, researchers?

  if (_slnsite_user_is_project_administrator($node) )
    return TRUE;
  return FALSE;
} // _slnsite_project_participants_listing_access

function _slnsite_dashboard_page_projects_access() {
  if (_slnsite_user_is_project_administrator() )
    return TRUE;
  return FALSE;
}  // _slnsite_dashboard_page_projects_access


// handler to check for score selection
/**
 * Implements XXX.
 */
function _slnsite_component_scoring_category_submit($form, &$form_state) {
  //dpm($form_state['values']);
  $nid = $form['nid']['#value'];
  $cid = $form['cid']['#value'];
  $cat = $form_state['values']['scoring'];  // note that this is the text of the category name
  $form_key = $form['form_key']['#value'];
  
  //dpm(get_defined_vars());
  
  $num_deleted = db_delete('slnsite_component_scoring')
    ->condition('nid', $nid)
    ->condition('cid', $cid)
    ->execute();

  if ($cat) {
    $num_inserted = db_insert('slnsite_component_scoring')
      ->fields(array(
        'nid' => $nid,
        'cid' => $cid,
        'sid' => $cat,
        'form_key' => $form_key,
      ))
      ->execute();
  }

} // _slnsite_component_scoring_category_submit

/**
 * Implements XXX
 */
function _slnsite_create_dependent_webform($surveynode, $title,  $parentfieldname, $basevariablename, $component_callback, $save = TRUE) {

  //module_load_include('inc', 'slnsite', 'slnsite.webform');
  //module_load_include('inc', 'slnsite', 'slnsite.utils');
  // ASSERT $surveynode is a valid sln_survey

  // TODO: what if no base node? display message and return null or "empty" planning_survey for $plan
  // TODO: "empty" means with no components and few, if any, basic settings
  //$basetitle = variable_get('slnsite_plan_base_node_title', 'no title');
  $baseid = variable_get($basevariablename, '-1');

  $basenode = node_load($baseid);
  if (!is_object($basenode)) {
    drupal_set_message("Unable to load base node in _slnsite_create_dependent_webform()");
    return;
  }

  if (function_exists('node_class'))
    $nodeclass = node_class($basenode);

  $basecomponents = $basenode->webform['components'];

  //dpm($basecomponents);
  //return;

  $node = new stdClass();
  $node->type = 'sln_survey';
  node_object_prepare($node);
  $node->webform = webform_node_defaults();
  $node->title = $title;
  $node->language = LANGUAGE_NONE;
  $node->body = $basenode->body;       // NOTE: this is now an array in D7, possibly NULL
  $node->uid = $surveynode->uid;
  $node->promote = 0;
  $node->comment = 0;

  // TODO: add these to base classes and copy (or allow for SLQ and Data Entry Form)
  $node->field_sln_survey_instrument = array(LANGUAGE_NONE => array(0 => array('tid' => SLN_INSTRUMENT_SLQ)));
  $node->field_sln_survey_subject = array(LANGUAGE_NONE => array(0 => array('tid' => SLN_SUBJECT_SELF)));
  //$node->field_sln_survey_section = array(LANGUAGE_NONE => array(0 => array('tid' => SLN_SURVERY_SECTION_ACTION_PLAN)));


  // TODO: do we need callbacks for other, type specific, $node properties?
  // if so, add a parameter for that (or just pass one that is used in building all callbacks)

  // TODO: handle case where no parent form node ref field is present
  $node->{$parentfieldname}[LANGUAGE_NONE][0]['nid'] = $surveynode->nid;

  // TODO: pass this array as a parameter to this function so can control order ****************************
  // TODO: pass extra param for $order using 'weight' for survey and 'sid, weight' for planning
  $surveyquestions = _slnsite_survey_questions_enumeration($surveynode->nid, 'sid, weight');

  //dpm(get_defined_vars());

  $wccnt = 0; // web component count
  $components = array();
  $weightoffset = 1000;     // add 1000 in the assumption that form may be appended/imported to another
                            // TODO: define constant or get from formgen settings form

  // ****************************************************************************
  // TODO: how control weight assignments or loop processing so that (1) data entry form is in question order and (2) planning form is ordered by skill taxonomy (for instrument) and question number, should this encoded into _slnsite_survey_questions_enumeration?

  // loop over header components in base class (sln-formgen-start sln-formgen-no-repeat classes or use form_key suffixes)
  foreach ($basecomponents as $bc) {
    // dpm(array($bc, _slnsite_webform_component_has_class($bc, 'sln-formgen-start') , _slnsite_webform_component_has_class($bc, 'sln-formgen-end')));

    // base this on extra class, value class or form_key (just look for "head"?)
    if (_slnsite_webform_component_has_class($bc, 'sln-formgen-start')) {
      $wccnt++;
      $c = $bc;
      $c['cid'] = $wccnt;
      $c['weight'] = $wccnt + $weightoffset;
      // TODO: modify classes or form_key?
      $components[$wccnt] = $c;
    } // if
  } // foreach

  $fscnt = 0; // count fieldsets or layout_box's, one per question
  foreach ($surveyquestions as $ndx => $sq) {
    $pid = 0;
    foreach ($basecomponents as $bc) {

      // check that $bc is a repeatable element (i.e., not header or footer), in which case skip it in this loop
      if (_slnsite_webform_component_has_class($bc, 'sln-formgen-start') || _slnsite_webform_component_has_class($bc, 'sln-formgen-end'))
        continue;

      $c = array();
      // these attributes are the same for all components or are overridden in the component callback
      // TODO: CONSIDER: why not just clone entire webform component to start with?
      $wccnt++;
      $c['weight'] = $wccnt + $weightoffset;
      $c['cid'] = $wccnt;
      $c['type'] = $bc['type'];
      $c['required'] = $bc['required'];
      $c['value'] = NULL;
      $c['extra'] = $bc['extra'];

      //dpm(array($c['extra'],  $bc['extra'], $sq->extra));

      // call component building function
      $c = $component_callback($c, $bc, $pid, $wccnt, $fscnt, $sq);

      $components[$wccnt] = $c;
      // TODO: is this better than a node save?
      // possibly, as it allows other modules to be called pre and post save
      // it also assigns cid
      // but also assumes prior node save to get nid?
      //webform_component_insert($c);

    } // foreach $bc

  } // foreach

  // loop over footer components in base class (sln-formgen-end sln-formgen-no-repeat classes or use form_key suffixes)
  foreach ($basecomponents as $bc) {
    // base this on extra class, value class or form_key (just look for "foot"?)
    if (_slnsite_webform_component_has_class($bc, 'sln-formgen-end')) {
      $wccnt++;
      $c = $bc;
      $c['cid'] = $wccnt;
      $c['weight'] = $wccnt + $weightoffset;
      // TODO: modify classes or form_key?
      $components[$wccnt] = $c;
    } // if
  } // foreach

  // obsolete:
  // now done below:
  //$node->webform['components'] = $components;

  // Setup notification email.
  $emails = array(
   /*
     array(
      'email' => '', // 'somebody@example.tld',
      'subject' => 'default',
      'from_name' => 'default',
      'from_address' => 'default',
      'template' => 'default',
      'excluded_components' => array(),
    ),
   */
  );

  // TODO: use base node for these attributes ****************************
  // NOTE: some already done abouve

  // Attach the webform to the node.
  $node->webform = array(
    'confirmation' => '',                            // TODO: set this?
    'confirmation_format' => 1,
    'redirect_url' => '<confirmation>',              // TODO: change this?
    'status' => '1',
    'block' => '0',
    'teaser' => '0',
    'allow_draft' => '0',
    'auto_save' => '1',
    'submit_notice' => '0',
    'submit_text' => 'Save',
    'submit_limit' => '-1',                         // User can submit more than once.
    'submit_interval' => '-1',
    'total_submit_limit' => '-1',
    'total_submit_interval' => '-1',
    'record_exists' => TRUE,
    'roles' => array(
      //0 => '1',                                     // Anonymous user can submit this webform.
      1 => '2',
    ),
    'emails' => $emails,
    'components' => $components,
  );

  if ($save)
    node_save($node);


  if (drupal_strlen($nodeclass) && function_exists('node_class'))
    node_class_upsert($node->nid, $nodeclass);

  //dpm(get_defined_vars());

  return $node;
}

// create plan survey from a sln_survey
// allow save option for testing
/**
 * Implements XXX.
 */
function _slnsite_create_planning_webform($surveynode, $title, $save = TRUE) {

  return _slnsite_create_dependent_webform($surveynode, $title, 'field_sln_survey_parent', 'slnsite_plan_base_node_id', '_slnsite_create_planning_webform_component_set');

}  // _slnsite_create_planning_webform

/**
 * Implements hook_views_query_alter(&$view, &$query).
 */
function _slnsite_create_data_entry_webform($surveynode, $title, $save = TRUE) {

  return _slnsite_create_dependent_webform($surveynode, $title, 'field_sln_survey_parent', 'slnsite_data_entry_base_node_id', '_slnsite_create_data_entry_webform_component_set');

} // _slnsite_create_data_entry_webform

/**
 * Implements XXX.
 */
function _slnsite_create_planning_webform_component_set($c, $bc, &$pid, $wccnt, &$fscnt, $sq) {

  //dpm(get_defined_vars());

  if (!$sq->sid)
    return $c;

  // todo: set class on fieldset/layout-box so we can style
  // done for layout-box: sln-planning-question
  // note: assume question number in question label
  // assume fieldset? or other grouping field (e.g., layout)
  switch ($bc['type']) {
    case 'layout_box':
      $fscnt++;
      $pid = $wccnt;
      $c['name'] = $sq->name;
      $c['form_key'] = 'lb' . $fscnt;
      $c['pid'] = 0;
      break;
    case 'fieldset':            // the label
      $fscnt++;                 // CONSIDER: how do we know that this is always processed before children?
      $pid = $wccnt;
      $c['name'] = $sq->name;
      $c['form_key'] = 'fs' . $fscnt;
      $c['pid'] = 0;
      $sqextra = unserialize($sq->extra);
      if (array_key_exists('css_classes', $extra))
        $extra['css_classes'] = $sqextra['css_classes'];      // is this necessary? won't it happen from just main function copy?
      break;
    case 'textfield':            // the plan
    case 'textarea':            // the plan
      $c['name'] =  $sq->name;
      $c['form_key'] = 'ta' . $fscnt;
      $c['pid'] = $pid;
      break;
    case 'hidden':   // store original question cid
      if ($bc['form_key'] == 'question_cid') {
        $c['name'] = 'cid_' . $fscnt;
        $c['value'] = $sq->cid;
        $c['form_key'] = 'cid_' . $sq->cid;
      }
      if ($bc['form_key'] == 'response_value') {
        $c['name'] = 'response_' . $fscnt;
        $c['value'] = "RESPONSE";
        $c['form_key'] = 'response_' . $sq->cid;
      }
      $c['pid'] = $pid;
      break;
    case 'markup':
      $c['name'] = $sq->skill;   // this should be scoring category/skill
      $c['value'] = $sq->skill;
      $c['form_key'] = 'm' . $fscnt;
      $c['pid'] = $pid;
      break;
    case 'select':
      $c['name'] = $bc['name'];  // term
      $c['form_key'] = 's' . $fscnt;
      $c['pid'] = $pid;
      break;
    default:
      drupal_set_message(t('skipping unexpected planning base element type: %type', array('%type' => $bc['type'])));
  } // switch

  return $c;
} // _slnsite_create_planning_webform_component_set

/**
 * Implements XXX.
 */
function _slnsite_create_data_entry_webform_component_set($c, $bc, &$pid, $wccnt, &$fscnt, $sq) {

  //dpm(get_defined_vars());
  //return $c;

  switch ($bc['type']) {
    case 'layout_box':            // the label
      $fscnt++;                 // CONSIDER: how do we know that this is always processed before children?
      $pid = $wccnt;
      $c['name'] = $fscnt++ . '. ' . $sq->name;
      $c['form_key'] = 'lb' . $fscnt;
      $c['pid'] = 0;
      break;
    case 'hidden':              // question number, is this really needed? No  // also, ordering should be by scoring type and grouped. Why are these not showing up?
      $c['name'] = $wccnt;
      $c['value'] = $wccnt;
      $c['form_key'] = 'h' . $fscnt;
      $c['pid'] = $pid;
      break;
    case 'markup':
      if ($bc['name'] == 'm1') {  // question text
        $c['name'] = 'm' . $fscnt; // $bc['name'];   // this should be scoring category/skill
        $c['value'] = $sq->name;    // this should be scoring category/skill
        $c['form_key'] = 'm' . $fscnt;
        $c['pid'] = $pid;
      }
      if ($bc['name'] == 'q1') {    // question number
        $c['name'] = 'q' . $fscnt; // $bc['name'];   // this should be scoring category/skill
        $c['value'] = $fscnt . '.'; // $bc['name'];   // this should be scoring category/skill
        $c['form_key'] = 'q' . $fscnt;
        $c['pid'] = $pid;
      }
      break;
    case 'select':
      $c['name'] = $bc['name'];  // term
      $c['form_key'] = 's' . $fscnt;
      $c['pid'] = $pid;
      // NOTE: ensure that proper select options (inverted or not) are passed from $sq ???
      $sqextra = unserialize($sq->extra);
      $extra['options_source'] = $sqextra['options_source'];
      $extra['items'] = $sqextra['items'];
      break;
    default:
      drupal_set_message(t('skipping unexpected data entry base element type: %type', array('%type' => $bc['type'])));
  } // switch

  return $c;
} // _slnsite_create_data_entry_webform_component_set

/**
 * return data array used for project report (full listing)
 */
function _slnsite_project_submissions_process($project, $showcases = FALSE) {
  $data = array('headers' => array(), 'data' => array());

  if (is_numeric($project)) {
    $projectid = $project;
    $project = node_load($projectid);
    if (!is_object($project)) {
      watchdog('mss', "unable to load project in _slnsite_project_submissions_process: $projectid");
      return $data;
    } // not object
  }  // numeric project

  $surveys = array();
  if (count($project->field_project_form_self))
    $surveys[] = $project->field_project_form_self[LANGUAGE_NONE][0]['nid'];
  if (count($project->field_project_form_supervisor)) 
    $surveys[] = $project->field_project_form_supervisor[LANGUAGE_NONE][0]['nid'];
  if (count($project->field_project_form_observer)) 
    $surveys[] = $project->field_project_form_observer[LANGUAGE_NONE][0]['nid'];
  if (count($project->field_project_form_planning)) 
    $surveys[] = $project->field_project_form_planning[LANGUAGE_NONE][0]['nid'];
  
  
  // only count completed surveys for this project
  $obscnt = count($project->field_project_form_observer)? _slnsite_project_completed_submissions_count($project, $project->field_project_form_observer[LANGUAGE_NONE][0]['nid']) : 0; 
  
  // set table headings
  if ($showcases) {
    $data['headers'][] = "Skill";
    $data['headers'][] = "Q#";
    for ($i = 1; $i <= $obscnt; $i++)
      $data['headers'][] = "Case $i";
    $data['headers'][] = "Self";
    $data['headers'][] = "Observers";
    $data['headers'][] = "Supervisor";
  } else {
    $data['headers'][] = "Skill";
    $data['headers'][] = "Item";
    $data['headers'][] = "Actions<p class='important'> When I am performing my duties ...<p/>";
    $data['headers'][] = "Self";
    $data['headers'][] = "Obs";
    $data['headers'][] = "Sup";
  } // if showcases

  $casenum = 0;   // observer index

  foreach ($surveys as $surveynoderef) {
    //$surveynodeid = $surveynoderef['nid'];
    if (is_array($surveynoderef))     // handle noderefs
      $surveynodeid = $surveynoderef['nid'];
    else
      $surveynodeid = $surveynoderef;

    $surveynode = node_load($surveynodeid);

    if (!$surveynode) {
      //drupal_set_message("Invalid surveynodeid in _slnsite_project_submissions_process: $surveynodeid");
      watchdog('slnsite', "Invalid surveynodeid in _slnsite_project_submissions_process: $surveynodeid", array(), WATCHDOG_WARNING);
      return $data;
    }

    $submissions = webform_get_submissions($surveynodeid);

    foreach ($submissions as $submission) {
      // loop if submission not for this project

      $sub_project_id = _slnsite_submission_data('sln_project_id', $submission, $surveynode, -1);

      if ($sub_project_id != $project->nid)
        continue;

      if ($surveynodeid && ($surveynodeid == _slnsite_get_survey($project, 'observer'))) {
        // DONE: check that submission is complete before incrementing. DONE
        // DONE: if not complete, loop to next submission? DONE
        // CONSIDER: better to use function?
        //   $is_complete = _slnsite_submission_is_complete($surveynodeid, $submissionid);
        if (!$submission->is_draft) {
          $casenum++;
          $colkey = "case $casenum";
        }
        else {
          continue;   // loop to next submission
        }
      } // obs
      elseif ($surveynodeid && ($surveynodeid == _slnsite_get_survey($project, 'supervisor')))
        $colkey = "supervisor";
      elseif ($surveynodeid && ($surveynodeid == _slnsite_get_survey($project, 'self')))
        $colkey = "self";
      else
        $colkey = "invalid key";

      // process data for each question
      $qnum = 0;
      // load component scoring into array, indexed by cid
      $scoring = _slnsite_component_scoring($surveynodeid);  // array[cid]=>array('cid', 'sid', 'form_key')
      $instrument = $surveynode->field_sln_survey_instrument[LANGUAGE_NONE][0]['tid'];
      $skilltermopts = _slnsite_skills_definitions($instrument, $all = FALSE);
      foreach($scoring as $cid => $score) {
          $form_key = $score['form_key'];
          // TODO: make independent of SLQ, generic instrument (slq_). use regex for ^s(l|t)q_9+$
          //$questionnum = intval(str_replace("slq_", "", $form_key));
          $scoreid = $score['sid'];
          if ($scoreid > 0) {
            $qnum++;
            $value = array_key_exists($cid, $submission->data) ? $submission->data[$cid][0] : "";
            $question = $surveynode->webform['components'][$cid]['name'];
            $skill = array_key_exists($scoreid, $skilltermopts) ? $skilltermopts[$scoreid][0] : "NA";
            $data['data'][$qnum]['skill'] = $skill;
            if ($showcases) {
              //$data['data'][$qnum]['num'] = $qnum;        // TODO: this is not the proper question number
               $data['data'][$qnum]['num'] = substr($question, 0, strpos($question, '. '));
            }
            else {
              //$data['data'][$qnum]['item'] = $qnum . ".";
              $data['data'][$qnum]['item'] = substr($question, 0, strpos($question, '. ') + 1);
              if ($surveynodeid && ($surveynodeid == _slnsite_get_survey($project, 'self')))
                $data['data'][$qnum]['question'] = substr($question, strpos($question, '. ') + 1);
            }

            $data['data'][$qnum][$colkey] = $value;
          } // $scoreid
      } // foreach score
    } // foreach submission
  } // foreach project survey

  foreach ($data['data'] as $ndx => $row) {
    // calculate averages
  
    if (!array_key_exists('self', $row) )
      $data['data'][$ndx]['self'] = "&nbsp;";
    else {
      $temp = $row['self'];
      unset($data['data'][$ndx]['self']);
      $data['data'][$ndx]['self'] = $temp;
    } // if self
      
    $sum = 0;
    for ($i = 1; $i <= $obscnt; $i++) {
      if (array_key_exists("case $i", $row)) {
        $sum += $row["case $i"];
        if (!$showcases)
          unset($data['data'][$ndx]["case $i"]);
      } // if
    } // for
    $data['data'][$ndx]['observers'] = $obscnt ? round($sum / $obscnt, 2) : "NAN";

    if (!array_key_exists('supervisor', $row) )
      $data['data'][$ndx]['supervisor'] = "&nbsp;";
    else {
      $temp = $row['supervisor'];
      unset($data['data'][$ndx]['supervisor']);
      $data['data'][$ndx]['supervisor'] = $temp;
    } // if supervisor
      
  } // foreach data row
  
  //dpm(get_defined_vars());

  return $data;
} // _slnsite_project_submissions_process

/**
 * return data array used for project report (summary)
 */
function _slnsite_project_submissions_summarize($project) {

  $data = array();
  $initsubdata = array(
    'sum' => 0,
    'count' => 0,
    'average' => 'NAN',
    'questions' => array()
  );

  if (is_numeric($project)) {
    $projectid = $project;
    $project = node_load($projectid);
    if (!is_object($project)) {
      watchdog('mss', "unable to load project in _slnsite_project_submissions_summarize: $projectid");
      return $data;
    } // not object
  }  // numeric project

  $surveys = array();
  if (count($project->field_project_form_self))
    $surveys[] = $project->field_project_form_self[LANGUAGE_NONE][0]['nid'];
  if (count($project->field_project_form_supervisor)) 
    $surveys[] = $project->field_project_form_supervisor[LANGUAGE_NONE][0]['nid'];
  if (count($project->field_project_form_observer)) 
    $surveys[] = $project->field_project_form_observer[LANGUAGE_NONE][0]['nid'];
  if (count($project->field_project_form_planning)) 
    $surveys[] = $project->field_project_form_planning[LANGUAGE_NONE][0]['nid'];

  foreach ($surveys as $surveynoderef) {
    if (is_array($surveynoderef))     // handle noderefs
      $surveynodeid = $surveynoderef['nid'];
    else
      $surveynodeid = $surveynoderef;

    $surveynode = node_load($surveynodeid);

    if (!$surveynode) {
      drupal_set_message("Invalid surveynodeid in _slnsite_project_report_process_submissions: $surveynodeid");
    }

    // map form_keys to cid's
    $fkcids = array_column($surveynode->webform['components'], 'cid', 'form_key');

    // get keys for required/standard sln_survey hidden fields
    $cid_project_id = $fkcids['sln_project_id'];
    $cid_participant_id = $fkcids['sln_participant_id'];
    $cid_subject_id = $fkcids['sln_subject_id'];

    $instrument = $surveynode->field_sln_survey_instrument[LANGUAGE_NONE][0]['tid'];
    $skilltermopts = _slnsite_skills_definitions($instrument, $all = FALSE);
    $submissions = webform_get_submissions($surveynodeid);

    $data[$surveynodeid] = array (
      'title' => $surveynode->title,
      'count' => 0,
      'data' => array(),
    );

    foreach ($skilltermopts as $tid => $termarr) {
      $data[$surveynodeid]['data'][$termarr[0]] = $initsubdata;
    }

    foreach ($submissions as $submission) {

      // loop if submission not for this project

      $sub_project_id = _slnsite_submission_data('sln_project_id', $submission, $surveynode, -1);
      //if ($submission->data[$cid_project_id][0] != $project->nid)
      if ($sub_project_id != $project->nid)
        continue;

      $data[$surveynodeid]['count']++;

      // load component scoring into array, indexed by cid
      $scoring = _slnsite_component_scoring($surveynodeid);

      // process data for each SLQ question
      // TODO: make independent of SLQ, generic instrument (slq_). use regex for ^s(l|t)q_9+$

      foreach($scoring as $cid => $score) {

          $form_key = $score['form_key'];
          $questionnum = intval(str_replace("slq_", "", $form_key));
          if (array_key_exists('items', $surveynode->webform['components'][$cid]['extra']))
            $selectitems = $surveynode->webform['components'][$cid]['extra']['items'];  // why no items key in 180?
          $skillopts = _slnsite_select_options_from_string($selectitems);
          $scoreid = $score['sid'];
          $value = array_key_exists($cid, $submission->data) ? $submission->data[$cid][0] : "";
          //if (is_numeric($value))
          //  $value = 1.00000001 * $value;
          $question = $surveynode->webform['components'][$cid]['name'];
          $skill = array_key_exists($scoreid, $skilltermopts) ? $skilltermopts[$scoreid][0] : "NA";
          $response = ((is_string($value) || is_integer($value)) && array_key_exists($value, $skillopts)) ? $skillopts[$value] : "";

          if (($skill != 'NA') && ($value != "")) {
            $data[$surveynodeid]['data'][$skill]['count']++;
            $data[$surveynodeid]['data'][$skill]['sum'] += $value;
            if ($data[$surveynodeid]['data'][$skill]['count'] > 0)
              $data[$surveynodeid]['data'][$skill]['average'] = $data[$surveynodeid]['data'][$skill]['sum'] / $data[$surveynodeid]['data'][$skill]['count'];
            $data[$surveynodeid]['data'][$skill]['questions'][$question] = '';
          }



      } // foreach score

    } // foreach submission
  } // foreach project survey

  //dpm(get_defined_vars());

  return $data;
} // _slnsite_project_report_process_submissions




/**
 * process data into arrays from submission
 * no user output from this as it will be used in several places
 * so assume valid params from callers
 * returns one big array of processed submission data
 *
 *
 */
//function _slnsite_submission_report_process_data_self($surveynodeid, $submissionid) {
function _slnsite_submission_report_process_data_self($project) {

  // todo: query for sid for self for this project
  // TODO: move to utils?  Also, describe returned array keys in doc above
  // consider: pass form id to eliminate "self"

  //$surveynodeid = count($project->field_project_form_self[LANGUAGE_NONE]) ? $project->field_project_form_self[LANGUAGE_NONE][0]['nid'] : 0;
  
  $surveynodeid = _slnsite_get_survey($project, 'self');

  $submissionid = _slnsite_project_survey_submission($project, $surveynodeid); 
  
  $data = array(
    'surveynodeid' => $surveynodeid,
    'submissionid' => $submissionid,
  );
  if (!(_slnsite_check_valid_id($surveynodeid, 'survey') && _slnsite_check_valid_id($submissionid, 'submission') ))
    return $data;

  // load survey and submission
  $submission = webform_get_submission($surveynodeid, $submissionid);
  $surveynode = node_load($surveynodeid);

  // todo: check for valid, unblocked account
  $submitteracctid = $submission->uid;
  $submitteracct = user_load($submitteracctid);

  // TODO: check that node is sln_survey
  // TODO: consider differences in output for self vs observer and supervisor
  // only show a summary for observer and supervisor?

  // load component scoring into array, indexed by cid
  $scoring = _slnsite_component_scoring($surveynodeid);

  // note that this loads skills indexed by names, not by taxonomy id
  // note that submission value is based on name offset

  $skilltermopts = _slnsite_select_options_from_vocabulary(SLN_SKILLS, array(0 => 'None'));

  // todo: use taxonomy for keys so as to work with STQ
  // define and init common subarray and copy to each skill
  $skillscoresub = array('count' => 0, 'sum' => 0, 'percent' => 0, 'questions' => array('strength' => array(), 'weakness' => array(), 'neutral' => array(), 'all' => array()));
  $skillscores = array(
    'Transforming' => $skillscoresub,
    'Bonding' => $skillscoresub,
    'Managing' => $skillscoresub,
    'Bartering' => $skillscoresub,
    'Bridging' => $skillscoresub,
    'All' => $skillscoresub,
    /*'None' => $skillscoresub,*/
    'Unscored' => array('count' => 0, 'questions' => array()),
  );

  // for testing and debugging
  $testitems = array();

  // process data for each score
  foreach($scoring as $cid => $score) {
    $form_key = $score['form_key'];
    $questionnum = intval(str_replace("slq_", "", $form_key));
    $selectitems = $surveynode->webform['components'][$cid]['extra']['items'];
    $skillopts = _slnsite_select_options_from_string($selectitems);
    //$skilloptskeys = array_keys($skillopts);
    $scoreid = $score['sid'];
    // TODO: this index may be invalid in a draft
    $value = array_key_exists($cid, $submission->data) ? $submission->data[$cid][0] : "";
    $question = $surveynode->webform['components'][$cid]['name'];
    $skill = array_key_exists($scoreid, $skilltermopts) ? $skilltermopts[$scoreid] : "NA";
    //$response = array_key_exists($value, $skilloptskeys) && array_key_exists($skilloptskeys[$value], $skillopts) ? $skillopts[$skilloptskeys[$value]] : "";

    $response = array_key_exists($value, $skillopts) ? $skillopts[$value] : "";

    // TODO: set up a better test for unscored?
    // TODO: in the meantime, define constant for 36 or create an array of unscored values?
    // TODO: create a taxonomy with 'scored' and 'unscored' and then create a wc extra attribute like score type to allow form designer to select unscored questions. add a css class too? default to scored
    // TODO: define constants for 'All' and 'Unscored'?

    $class = 'unscored';
    if ($questionnum < 36) {
      $class = ($value > 3) ? "strength" : (($value < 3) ? "weakness" : "neutral");
      $skillscores[$skill]['count']++;
      $skillscores[$skill]['sum'] += $value;
      $skillscores['All']['count']++;
      $skillscores['All']['sum'] += $value;
      $skillscores[$skill]['questions']['all'][] = $question;
      $skillscores[$skill]['questions'][$class][] = $question;
      $skillscores['All']['questions']['all'][] = $question;
      $skillscores['All']['questions'][$class][] = $question;
    }
    else {
      $skillscores['Unscored']['count']++;
      $skillscores['Unscored']['questions'][] = $question;
    } // if

    // put important data into a test dump for listing via dpm() or in content
    $testitems[$cid] = array(
      "form_key: $form_key",
      "scoreid: $scoreid",
      "skill(scoreid): $skill",
      "value: $value",
      "response(value): $response",
      "question: $question",
      "class: $class",
    );

  } // foreach scoring

  // sum up totals
  // todo: eventually check webform_component extra sub array for unscored?
  foreach ($skillscores as $key => $skillscore) {
    if ($skillscores[$key]['count'] && ($key != 'Unscored'))
      $skillscores[$key]['percent'] = ceil(100 * ($skillscores[$key]['sum'] / $skillscores[$key]['count']));
  } // foreach

  // return data array
  $data = array(
    'surveynode' => $surveynode,
    'submission' => $submission,
    'submitteracct' => $submitteracct,
    'scoring' => $scoring,
    'skilltermopts' => $skilltermopts,
    'skillscores' => $skillscores,
    'testitems' => $testitems,
  );

  //dpm(get_defined_vars());

  return $data;
}  // _slnsite_submission_report_process_data_self

/**
 * Implements submission report output for use in forms or nodes. Returns html for report.
 */
// function _slnsite_submission_report($surveynodeid, $submissionid, $templatetid = SLN_TEMPLATE_SLQ_SUBMISSION_REPORT) {
function _slnsite_submission_report($project, $surveynode, $submission, $templatetid = SLN_TEMPLATE_SLQ360_SUBMISSION_REPORT, $button = "") {
  
  //dpm(get_defined_vars());
 
  // todo: query for sid for self for this project

  //$surveynodeid = count($project->field_project_form_self[LANGUAGE_NONE]) ? $project->field_project_form_self[LANGUAGE_NONE][0]['nid'] : 0;
  //$surveynodeid = _slnsite_get_survey($project, 'self');

  $surveynodeid = $surveynode->nid;
  $submissionid = $submission->sid;
  
  //$submissionid = _slnsite_project_survey_submission($project, $surveynodeid); 
  
  // TODO: change $template default (remove it?) so as to be instrument independent

  // dpm(array($surveynodeid, $submissionid));

  // TODO: also check user is submitter or that user is director, researcher or administrator
  // TODO: redirect to form version so that user can select valid submission and survey
  // if all else fails, issue message and return to dashboard
  /*if (!(_slnsite_check_valid_id($surveynodeid, 'survey') && _slnsite_check_valid_id($submissionid, 'submission') )) {
    return ""; // "Invalid surveyid: $surveynodeid or submissionid: $submissionid";
  } // invalid ids*/

  //$surveynode = node_load($surveynodeid); // IS THIS NECESSARY performance
  $instrument = $surveynode->field_sln_survey_instrument[LANGUAGE_NONE][0]['tid'];
  
  $fkcids = array_column($surveynode->webform['components'], 'cid', 'form_key');

  $project_type = count($project->field_project_type) ? $project->field_project_type[LANGUAGE_NONE][0]['tid'] : 0;
  
  // only show data for 360 participants other than the subject, then return
  if ($project_type == SLN_PROJECT_TYPE_360)  {
    $subject = $surveynode->field_sln_survey_subject[LANGUAGE_NONE][0]['tid'];
    //$submission = webform_get_submission($surveynodeid, $submissionid);
    // TODO: add code to handle STQ and STQ generically (e.g., via $instrument variable)
    if (($subject != SLN_SUBJECT_SELF) || ($instrument != SLN_INSTRUMENT_SLQ)) {
      $rows = array();
      $cidsfk = array_column($surveynode->webform['components'], 'form_key', 'cid');

      foreach($submission->data as $cid => $r) {
        $rows[] = array('data' => array('form_key' => $cidsfk[$cid], 'data' => $r[0]));
      }
      //dpm(get_defined_vars());
      $header = array('form_key', 'data');

      return _slnsite_display_table($header, $rows, 'sln-submission-data-table');
    } // if
  } // if ($project_type == SLN_PROJECT_TYPE_360) 

  // ASSSUMPTION: $data has all the variables needed for template


  $templateterm = taxonomy_term_load($templatetid);
  $template = $templateterm ? $templateterm->description : "";
  
  $submission_data = _slnsite_submission_report_process_data_self($project);
  
  if (array_key_exists('testitems', $submission_data)) {
    // create a ul list for each entry in testitems array
    $items = array();
    foreach ($submission_data['testitems'] as $cid => $testitem) {
      $items[] = theme_item_list(array(
        'items' => $testitem,
        'title' => "cid: $cid",
        'type' => 'ul',
        'attributes' => array(),
      ));
    } // foreach testitem

    // list dump of test items
    $listvars = array(
      'items' => $items,
      'title' => 'Test Data',
      'type' => 'ol',
      'attributes' => array('class' => 'test-list-class'),
    );

    // put test items in a fieldset
    // TODO: remove or hide in final output
    // see: https://www.drupal.org/node/1099132#comment-5045000
    // for explanation of why library needs to added here
    drupal_add_library('system', 'drupal.collapse');
    $fieldsetvars = array(
      '#value' => theme_item_list($listvars),
      '#collapsed' => TRUE,
      '#collapsible' => TRUE,
      '#description' => "Description goes here.",
      '#id' => "question_data",
      '#title' => "Question Data",
      // this causes an error: arg 1 needs to be an array
      // see: https://www.drupal.org/node/1099132#comment-8009943
      // for explanation of why classes are duplicated here
      '#attributes' => array('class' => array('test-fieldset-class', 'collapsible', 'collapsed')),
      '#children' => "",
    );

    // TODO: set survey data to "" in production
    $surveydata = theme_fieldset(array('element' => $fieldsetvars));
  } // key exists testitems
  
  $position = _slnsite_submission_position($surveynodeid, $submissionid);

  //$submitterdata = _slnsite_display_submitter_info_table($submission->uid, $position);
  // TODO: get projectid from submission data, load $project for here and radarchart
  if (array_key_exists('sln_project_id', $fkcids)) {
    $cid_project_id = $fkcids['sln_project_id'];
    $projectid = $submission->data[$cid_project_id][0];
    // TODO: $projectid = _slnsite_submission_data('sln_project_id', $submission, $surveynode, -1);
    $project = node_load($projectid);
    //$submitterdata = _slnsite_display_submitter_info_table($project->field_research_subject[LANGUAGE_NONE][0]['value'], $position);
    
    $subjectname = "";
    if (array_key_exists(LANGUAGE_NONE, $project->field_project_subject) && array_key_exists(LANGUAGE_NONE, $project->field_project_subject[LANGUAGE_NONE][0]['field_project_subject_name']))
      $subjectname = $project->field_project_subject[LANGUAGE_NONE][0]['field_project_subject_name'][LANGUAGE_NONE][0]['safe_value'];
    $submitterdata = "";
    if (drupal_strlen($subjectname))
      $submitterdata = _slnsite_display_submitter_info_table($subjectname, $position);
  }
  else
    $submitterdata = "Unable to determine project id";

  $skillsdefndata = _slnsite_display_table(
    array('Strategic Leader Influence Actions', 'Description'),
    _slnsite_skills_definitions($instrument),
    "sln-skills-table"
  );

  
  // TODO: only show if subject data exists
  $comparedata = _slnsite_display_table(
    array_merge(
      array("Strategic Leader Actions", "Your Score"),
      _slnsite_vocabulary_term_names(SLN_BENCHMARK_TYPES)
    ),
    _slnsite_comparison_data(
      SLN_INSTRUMENT_SLQ,
      _slnsite_submission_scores($surveynodeid, $submissionid)
    ),
    "sln-compare-table"
  );

  $plandata = "";
  $mapdata = "";
  $mapchart = "";
  $mapchartdata = "";
  if ($project_type == SLN_PROJECT_TYPE_360)  {  
    $plansurveynodeid = _slnsite_get_survey($project, 'planning');
    $plansubmissionid = _slnsite_project_survey_submission($project, $plansurveynodeid); 
    $plandata = _slnsite_display_action_plan_table($plansurveynodeid, $plansubmissionid);
  } // SLN_PROJECT_TYPE_360
  if ($project_type == SLN_PROJECT_TYPE_180)  {  
    $plansurveynodeid = _slnsite_get_survey($project, 'obs');
    $plansubmissionid = $submission->sid;     
    $plandata = _slnsite_display_action_plan_table($plansurveynodeid, $plansubmissionid);
  } // SLN_PROJECT_TYPE_180
  
    $skills = _slnsite_skills_definitions($instrument, $all = FALSE);
    $project_lang = count($project->field_project_survey_language) ? $project->field_project_survey_language[LANGUAGE_NONE][0]['value'] : 0;
    $actions = _slnsite_submission_actions($surveynodeid, $submissionid, $project_lang);
    $mapdata = "";
    //$cnt = 1;
    foreach ($skills as $skill) {
      $skillname = $skill[0];
      $mapdata .= _slnsite_display_action_map_table($skillname, $actions);
      //if (!($cnt % 2))
        //$mapdata .= "<div class='emptypagebreak' style='PAGE-BREAK-BEFORE: always'>&nbsp;</div>";
      //$cnt++;
    }  // foreach $skill


    if ($templatetid == SLN_TEMPLATE_SLQ360_SUBMISSION_REPORT) {
      $mcdata = _slnsite_project_submissions_summarize($project);
      $planning_form = is_array($project->field_project_form_planning[LANGUAGE_NONE]) ? $project->field_project_form_planning[LANGUAGE_NONE][0]['nid'] : 0;
      $mapchartdataarr = array();
      $mcdata2 = array();
      foreach ($mcdata as $nid => $survey) {
        $t = array();
        foreach ($survey['data'] as $key => $arr) {
          $mcdata2[$key][0] = $key;
          if ($nid != $planning_form)
            $mcdata2[$key][$nid] = sprintf('%01.2f', round($arr['average'], 2));
          //$t[] = round($arr['average'], 2);
          $t[] = sprintf('%01.2f', round($arr['average'], 2));
        }  // foreach $survey
        $mapchartdataarr[] = $t;
      } // foreach $data

      // display radar chart
      // $mapchartlabels = array("Transforming", "Bonding", "Managing", "Bartering", "Bridging");
      $mapchartlabels = array("Managing", "Transforming", "Bonding", "Bridging", "Bartering");
      $mapchart = _slnsite_project_results_radar_chart($project->nid, $mapchartlabels, $mapchartdataarr);
      // display chart data 
      
      // avearage scores
      $obs = is_array($project->field_project_form_observer[LANGUAGE_NONE]) ? $project->field_project_form_observer[LANGUAGE_NONE][0]['nid'] : 0;
      $sup = is_array($project->field_project_form_supervisor[LANGUAGE_NONE]) ? $project->field_project_form_supervisor[LANGUAGE_NONE][0]['nid'] : 0;
      $sub = is_array($project->field_project_form_self[LANGUAGE_NONE]) ? $project->field_project_form_self[LANGUAGE_NONE][0]['nid'] : 0;
      
      $mcdata2['Behavioral Agility'][0] = t('Behavioral Agility');
      $mcdata2['Behavioral Agility'][$obs] = _sln_average_col($mcdata2, $obs);
      $mcdata2['Behavioral Agility'][$sup] = _sln_average_col($mcdata2, $sup);
      $mcdata2['Behavioral Agility'][$sub] = _sln_average_col($mcdata2, $sub);
            
      //dpm(get_defined_vars());
      
      /*
      foreach ($survey['data'] as $key => $arr) {
        if ($nid != $planning_form) {
          
          foreach ($skills as $skill) {
            $mcdata2['Behavioral Agility'][$nid]
          }  // foreach $skill
                
        } // if
          $mcdata2[$key][$nid] = sprintf('%01.2f', round($arr['average'], 2));
          //$t[] = round($arr['average'], 2);
          $t[] = sprintf('%01.2f', round($arr['average'], 2));
      }  // foreach $survey
      */

      $mapchartdata = _slnsite_display_table(array('Skill', 'Observer', 'Supervisor', 'Self'), $mcdata2, 'sln-summary-radar-table');
    } // $templatetid == SLN_TEMPLATE_SLQ360_SUBMISSION_REPORT


  // load free resources from a term
  $resourcesterm = taxonomy_term_load(SLN_TEMPLATE_SLQ_FREE_RESOURCES);
  $resourcesdata = $resourcesterm ? $resourcesterm->description : "";

  // logo, instrument description (instrument name), instrument tagline
  $instrument_term = taxonomy_term_load($instrument);
  $desc = $instrument_term->description; //  . " (" . $instrument_term->name .")";
  $tagline = $instrument_term->field_tagline[LANGUAGE_NONE][0]['value'];
  $headerdata = "<h1 class='description'>$desc</h1><p class='tagline'>$tagline</p>";

  $is_complete = _slnsite_submission_is_complete($surveynodeid, $submissionid);
  //$messagedata = $is_complete ? "" : "<div class='draft warning'>" . t("Warning: this survey is not yet complete!  You can complete the survey by clicking") . " <a href='/node/$surveynodeid/submission/$submissionid/edit'>" . t("here") . ".</a></div>";
  //$messagedata = $is_complete ? "" : "<div class='draft warning'>" . t("Warning: this survey is not yet complete! Not all submissions have been received.") . "</div>";
  $messagedata = "";

  $actiondata = "";
  $actionsummarydata = "";
  // this is all 360 related
  if ($project_type == SLN_PROJECT_TYPE_360) {
    // display results table
    $data3 = _slnsite_project_submissions_process($project);
    $data4 = array('headers' => array('Action', 'Strengths M=>4.1', 'Areas for Improvement M<=3.5'), 'data' => array());
    foreach ($data3['data'] as $key => $d3) {
      foreach ($d3 as $ndx => $attr) {
        //dpm($ndx);
        if (in_array($ndx, array('item', 'self', 'observers', 'supervisor'))) {
          $classes = array('number');
          if (is_numeric($attr) && ($attr < 3.1) && in_array($ndx, array('self', 'observers', 'supervisor'))) {
            $classes[] = 'weakness';
            $data4['data'][$d3['skill']]['weakness'][$d3['item']] = $d3['question'];
          }
          if (is_numeric($attr) && ($attr > 3.9) && in_array($ndx, array('self', 'observers', 'supervisor'))) {
            $classes[] = 'strength';
            $data4['data'][$d3['skill']]['strength'][$d3['item']] = $d3['question'];
          }
          if ($ndx == 'observers')
            $data3['data'][$key][$ndx] = array('data' => sprintf("%01.2f", $attr), 'class' => $classes);
          else
            $data3['data'][$key][$ndx] = array('data' => $attr, 'class' => $classes);
        } // if
      } // foreach
    } // foreach
    $actiondata = _slnsite_display_table($data3['headers'], $data3['data'], "sln-360-project-action-data");
    
    $actionsummarydata = _slnsite_display_table($data4['headers'], $data4['data'], "sln-360-project-action-summary-data");
  } // 360
  
  $actionsummarydata = "";
  
  $downloadbutton = $button;

  // data array used by template
  // TODO: document tokens on node by making it it's own content type (sln_report_template)
  $data = array(
    '!messagedata' => $messagedata,
    '!headerdata' => $headerdata,
    '!surveydata' => "", //$surveydata,   // for debugging and testing only
    '!submitterdata' => $submitterdata,
    '!skillsdefndata' => $skillsdefndata,
    '!comparedata' => $comparedata,
    '!mapdata' => $mapdata,
    '!mapchart' => $mapchart,
    '!mapchartdata' => $mapchartdata,
    '!actiondata' => $actiondata,
    '!actionsummarydata' => $actionsummarydata,
    '!plandata' => $plandata,
    '!downloadbutton' => $downloadbutton,
    '!resourcesdata' => $resourcesdata,
  );

  $s = format_string($template, $data);

  //dpm(get_defined_vars());

  return $s;
 } // _slnsite_submission_report

/**
 * Implements submission report callback used to display report and call pdf print
 */
//function _slnsite_submission_report_form($form, &$form_state, $surveynodeid, $submissionid, $templatetid = SLN_TEMPLATE_SLQ_SUBMISSION_REPORT) {
function _slnsite_submission_report_form($form, &$form_state, $project, $surveynode, $submission, $templatetid = SLN_TEMPLATE_SLQ360_SUBMISSION_REPORT) {
  
  //dpm(get_defined_vars());
  
  global $user;
  
  $project_type = 0;
  if (count($project->field_project_type))
    $project_type = $project->field_project_type[LANGUAGE_NONE][0]['tid'];
  
/*  if ($project_type == SLN_PROJECT_TYPE_360) 
    $surveynodeid = _slnsite_get_survey($project, 'self');

  if ($project_type == SLN_PROJECT_TYPE_180) 
    $surveynodeid = _slnsite_get_survey($project, 'observer');

  // this cant work unless assumption is that there is only one survey submission 9maybe works for 360 subject
  $submissionid = _slnsite_project_survey_submission($project, $surveynodeid); 
*/
  //dpm(get_defined_vars());
  //dpm($project);

/*
  // TODO: check here and present form with appropriate survey and submission choices for the user (especially director) or figure them out if there is only 'self' survey and participant
  if (!(_slnsite_check_valid_id($surveynodeid, 'survey') && _slnsite_check_valid_id($submissionid, 'submission') )) {
    $form['output'] = array(
      '#type' => 'markup',
      '#markup' => "", //"Invalid surveyid: $surveynodeid or submissionid: $submissionid",
    );
    return $form;
  } // invalid ids
*/

  //$surveynode = node_load($surveynodeid); // IS THIS NECESSARY performance
  
  // TODO: field_sln_survey_subject field is only in sln-survey type
  //if ($project_type == SLN_PROJECT_TYPE_360) {
  if ($project_type != SLN_PROJECT_TYPE_360) {
    $subject = $surveynode->field_sln_survey_subject[LANGUAGE_NONE][0]['tid'];
    if ($subject != SLN_SUBJECT_SELF) {
      $form['output'] = array(
        '#type' => 'markup',
        '#markup' => "<p class='notice'>Reports are only for Self Assessments at this time.</p>",
      );
      return $form;
    } // not self assessment survey
  }

  // todo: consider: move to _slnsite_submission_report()
  $button = "";
  if (!isset($_REQUEST['pdf'])) {
    $query_string = str_replace('q=', '', $_SERVER['QUERY_STRING']);
    $button = array(
      '#type' => 'markup',
      '#markup' => "<p class='sln-pdf-link'><a href='/printpdf/$query_string?pdf=yes' target='_blank'>Download PDF version of this report</a></p><p class='sln-pdf-link-description'>Please note that the PDF version may take a minute or so to display in another browser tab or window. Once displayed, you can use the browser's File|Save As menu option to save a copy on your desktop computer. Additional plug-ins or applications from Adobe may be required for viewing PDF documents on mobile devices such as an iPad or iPhone.</p>",
    );
  }

  $form['output'] = array(
    '#type' => 'markup',
    //'#markup' => _slnsite_submission_report($surveynodeid, $submissionid, $templatetid),
    '#markup' => _slnsite_submission_report($project, $surveynode, $submission, $templatetid, drupal_render($button)),
  );

  // display on form page but not on report page node used for printing
  // moved to inside report 
  /*if (!isset($_REQUEST['pdf'])) {
    $query_string = str_replace('q=', '', $_SERVER['QUERY_STRING']);
    $form['printpdf'] = $button;
  }*/

  /*
  // potential elements for this form
  $form['group1'] = array(
    '#type' => 'fieldset',
    '#prefix' => '<div id="group1-fieldset-wrapper">',
    '#suffix' => '</div>',
    '#title' => t('Group 1 fieldset label'),
  );

  $form['nid'] = array(
    '#type' => 'hidden',
    '#value' => $nid,
  );

  $form['download'] = array(
    '#type' => 'button',
    '#value' => t('Download PDF version of this report.'),
    //'#weight' => 10,
    '#ajax' => array(
      //'path' => 'slnsite_base_id/js',
      'path' => "printpdf/122/?nid=$surveynodeid&sid=$submissionid",   // 122 is the node that has report
      //'callback' => '_slnsite_plan_base_id_js',
      //'wrapper' => 'plan-fieldset-wrapper',
    ),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#access' => TRUE,
    '#value' => t('Generate Selected Forms'),
    //'#submit' => array('ja_accessadd_submit'),
  );
  */
  
  //dpm(array($surveynodeid, $submissionid));
  //dpm(get_defined_vars());
  return $form;
  
} // _slnsite_submission_report_form

/**
 * submit handler for participant import button on project form
 *
 */
function _slnsite_batch_import_users_submit($form, &$form_state) {

  //drupal_set_message('next goto simple import form');

  $nid = $form['nid']['#value'];
  drupal_goto("node/$nid/import/participants");

} // _slnsite_batch_import_users_submit

function _slnsite_project_import_participants($project) {
  // todo: use a simpler form and processing?
  //dpm($project);
  
  // validate email addresses,
  
  // submit: loop, add field_project_observers (check for dups)
  // use what for name?
  // allow a variable? department?
  // report errors
  
  // add instructions
  
  $form = drupal_get_form('_slnsite_project_import', $project);
  //$s = drupal_render($form);
  return $form;  
} // _slnsite_project_import_participants

/**
 * menu callback for project imports to list fields of users
 *
 */
function _slnsite_project_import($form, &$form_state, $project) {

  // todo: add instructions
  $form['instructions'] = array(
   '#markup' => "Use this tab to manage bulk imports and deletes of project participants",
   );
   
 $form['clear_participants'] = array(
    '#type' => 'checkbox',
    '#title' => t('Remove existing participants.'),
    '#description' => t('Warning! Checking this box will remove all project participants AND their corresponding survey submissions from this project.'),
    /*'#required' => TRUE,*/
    '#default_value' => FALSE,
  );
  
  // add textarea and submit button
  $form['import'] = array(
    '#type' => 'textarea',
    '#title' => t('Import participants'),
    /*'#required' => true,*/
    '#rows' => '8',
    '#columns' => '40',
    '#default_value' => '',
    '#description' => "Enter email addresses, one per line. Optionally include a comma and a single name field. For example, joe@example.com,Joe Smith",
   );
  
  // DONE: add hidden values for the other settings
  //     'field-delimiter' => ',', 'notify', 'preview_count', 'value_delimiter'

  $form['project_id'] = array(
    '#type' => 'hidden',
    '#name' => 'project_id',
    '#value' => $project->nid,
  );

  /*
  $form['field_delimiter'] = array(
    '#type' => 'hidden',
    '#name' => 'field-delimiter',
    '#value' => ',',
  );

  $form['notify'] = array(
    '#type' => 'hidden',
    '#name' => 'notify',
    '#value' => TRUE,
  );

  $form['preview_count'] = array(
    '#type' => 'hidden',
    '#name' => 'preview_count',
    '#value' => 0,
  );

  $form['value_delimiter'] = array(
    '#type' => 'hidden',
    '#name' => 'value_delimiter',
    '#value' => '|',
  );
  */
  
  

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Apply',
  );

  //dpm(get_defined_vars());

  return $form;
} //_slnsite__project_import

function _slnsite_project_import_validate($form, &$form_state) {
  $import = trim($form_state['values']['import']);
  $project_id = $form_state['values']['project_id'];
  $project = node_load($project_id);
  if ($form_state['values']['clear_participants']) {
    // remove participants
    unset($project->field_project_observers);
    $project->field_project_observers = array(LANGUAGE_NONE => array());
    drupal_set_message('Removing existing participants.');
  }
  $additions = array();
  $errors = array();
  $participants = array();
  $rows = explode("\n", $import);
  $cnt = 0;
  // parse rows of text into name and email fields, validate and check for dups
  foreach ($rows as $row) {
    // parse row into 'email' and 'name' 
    // todo: allow for extra field(s)?
    $data = explode(',', trim($row));
    if (!$data || !is_array($data) || !count($data))
      continue;
    $obsname = '';
    $obsemail = '';
    //dpm($data);
    foreach($data as $d) {
      if (!drupal_strlen(trim($d)))
        continue;
      $d = trim(str_replace(array(',,', '"'), array(',', ''), $d));
      if (strpos($d, '@'))
        $obsemail = $d;
      else {
        if (drupal_strlen($obsname))
          $obsname .= ", ";
        $obsname .= $d;
      }
    } // foreach
    if (drupal_strlen($obsemail)) {
      if (valid_email_address($obsemail)) {
        // check for dups
        // TODO: also check for sup or sub in 360
        $found = false;
        $observers = (is_array($project->field_project_observers) && array_key_exists(LANGUAGE_NONE, $project->field_project_observers) && is_array($project->field_project_observers[LANGUAGE_NONE]) ) ? $project->field_project_observers[LANGUAGE_NONE] : array();
        foreach ($observers as $ndx => $observer) {
          $pemail = "";
          if (array_key_exists(LANGUAGE_NONE, $observer['field_project_observer_email']) && count($observer['field_project_observer_email'][LANGUAGE_NONE])) 
            $pemail = $observer['field_project_observer_email'][LANGUAGE_NONE][0]['value'];
          if ($obsemail == $pemail) {
            $found = TRUE;
            break;
          } // if
        } // foreach
        if (!$found) {
          $cnt++;
          if (!drupal_strlen($obsname)) {
            $pos = strpos($obsemail, '@');
            $obsname = str_replace(array('.', '-', '_',), ' ', substr($obsemail, 0, $pos));
          } // if
          $project->field_project_observers[LANGUAGE_NONE][] = array(
            'field_project_observer_email' => array (LANGUAGE_NONE => array(0 => array('value' => $obsemail))),
            'field_project_observer_name' => array (LANGUAGE_NONE => array(0 => array('value' => $obsname))),
          );
          $additions[] = "Adding: $row";
        }
        else {
          $errors[] = "Error, row skipped (duplicate): $row";
        }
      }
      else {
        $errors[] = "Error, row skipped (invalid email): $row";
      }
    }
    else {
      $errors[] = "Error, row skipped (no email): $row";
    }
  } // foreach
   
  foreach ($additions as $addition) {
    drupal_set_message($addition);
  } // foreach
  foreach ($errors as $error) {
    drupal_set_message($error, array(), 'warning');
  } // foreach

  node_save($project);
  
  if ($cnt)
    drupal_set_message(t("%d imports processed.", array('%d' => $cnt)));
  /*else
    drupal_set_message(t("Nothing to import.");*/

  //dpm(get_defined_vars());
} //_slnsite_project_import_submit


/**
 * Batch import all users.
 */
function _slnsite_batch_import_users($form_state) {
  $batch = array(
    'title' => t('Importing participants'),
    'operations' => array(
      array('_slnsite_batch_import_users_process', array($form_state))
    ),
    'progress_message' => '', // uses count(operations) which is irrelevant in this case
    'finished' => '_slnsite_batch_import_users_finished',
    //'file' => drupal_get_path('module', 'uif') . '/uif.admin.inc',
  );
  batch_set($batch);
} // _slnsite_batch_import_users

/**
 * User import batch processing.
 */
function _slnsite_batch_import_users_process($form_state, &$context) {
  // Initialize
  if (empty($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max'] = count($form_state['storage']['data']['user']);
    $context['results']['created'] = 0;
    $context['results']['updated'] = 0;
  }

  //dpm(get_defined_vars());
  watchdog('mss', var_export($form_state['storage'], 1));
  $project_id = $form_state['values']['project_id'];
  $emails = $form_state['storage']['data']['user'];
  $projectnode = node_load($project_id);


  // Process max 20 users at a time
  $processed = 0;
  $notify = $form_state['storage']['notify'];
  while ($context['sandbox']['progress'] < $context['sandbox']['max'] && $processed < 20) {
    $index = $context['sandbox']['progress'];

    //dpm(get_defined_vars());
    // watchdog('mss', var_export(get_defined_vars(), 1));

    uif_import_user($form_state['storage']['data']['user'][$index], $notify, $context['results'], $form_state);

    // now add to project
    if ($projectnode) {
      $email = $form_state['storage']['data']['user'][$index];
      $acct = user_load_by_mail($email);
      if ($acct && $acct->uid) {
        $projectnode->field_project_participants[LANGUAGE_NONE][] = array('uid' => $acct->uid);
        drupal_set_message($acct->uid . " added to project ");
      }
      else {
        drupal_set_message($email . " not found as a user");
      }
    }

    $context['sandbox']['progress']++;
    $processed++;
  }

  // Finished yet?
  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
  else {
    node_save($projectnode);
  }
}

/**
 * User import batch completion.
 */
function _slnsite_batch_import_users_finished($success, $results, $operations) {
  if ($success) {
    global $user;
    if (isset($results['self'])) {
      uif_update_user($results['self'], $user->uid);
      $results['updated']++;
      unset($results['self']);
    }
    $done = t('Participant import complete.');
    $created = $results['created'] ?
      ' ' . format_plural($results['created'], 'One user was created.', '@count users were created.') . ' ' :
      '';
    $updated = $results['updated'] ?
      ' ' . format_plural($results['updated'], 'One user was updated.', '@count users were updated.') . ' ' :
      '';
    $more = t('View the <a href="@url">user list</a>.', array('@url' => url('admin/people')));
    drupal_set_message($done . $created . $updated . $more);
  }
  else {
    drupal_set_message(t('An error occurred and processing did not complete.'), 'error');
  }
} // _slnsite_batch_import_users_finished

function _slnsite_project_messaging($project) {
  $form = drupal_get_form('_slnsite_project_messaging_form', $project);
  $s = drupal_render($form);
  return $s;
}

// form for _slnsite_project_messaging
function _slnsite_project_messaging_form($form, &$form_state, $project) {

  if (!_slnsite_project_status_report($project)) {
    //drupal_set_message("Note that messages can not be sent until subject, supervisor and observers are defined.", 'warning');
  }
  
  $test = trim(variable_get('slnsite_test_emails_list', ""));
  if (drupal_strlen($test)) {
    $repeat = FALSE;
    drupal_set_message("<strong>NOTE:</strong> e-mail testing is on. All emails will be sent to $test. Visit <a href='/admin/config/slnsite/email'>admin/config/slnsite/email</a> to change this.", "warning", $repeat);
  }

  global $user;
  $options = array();
  $checked = array();
  $cbs = array();     // checkboxes
  $recipients = _slnsite_project_participants($project);

  $project_type = SLN_PROJECT_TYPE_360;
  if (array_key_exists('#node', $form)) {
    $node = $form['#node'];  
    if (isset($node->field_project_type) && count($node->field_project_type))
      $project_type = $node->field_project_type[LANGUAGE_NONE][0]['tid'];
  }

  if (array_key_exists('values', $form_state) && array_key_exists('recipients', $form_state['values']))
    $cbs = $form_state['values']['recipients'];

  foreach($recipients as $ndx => $r) {
    //$ndx = trim($ndx);                    // TODO: clean up emails at their source in presave
    if (valid_email_address(trim($ndx))) {
      $recipients[$ndx]['selected'] = (is_array($cbs) && array_key_exists($ndx, $cbs) ) ? $cbs[$ndx] : 1;
      $options[$ndx] = $r['name'] . " (" . $ndx . ") - " . $r['type'];

      $checked[$ndx] = (is_array($cbs) && array_key_exists($ndx, $cbs) ) ? $cbs[$ndx] : $ndx;
      
      // TODO: why two ways of setting/indexing checked[]?
      /*if (is_array($cbs)) {
        if (array_key_exists($ndx, $cbs) && $cbs[$ndx])
          $checked[] = $ndx;
      }
      else {
        $checked[] = $ndx;
      }*/

    } // if valid email
    //else
      //drupal_set_message("invalid email: [$ndx]");
  } // foreach

  if (!is_array($form))
    $form = array();

  $form['project_nid'] = array(
   '#type' => 'hidden',
   '#value' => is_object($project) ? $project->nid : NULL,
   );

  $form['preview'] = array(
   '#markup' => isset($form_state['values']['show_preview']) ? _slnsite_message_preview($project, $recipients, $form_state['values']) : "",
   );

   $form['messagingbookmark'] = array(
   '#markup' => "<div><a name='messagingbookmark'></a></div>",
   );

  $form['instructions'] = array(
   '#markup' => "Fill out the form and press send. Use 'Preview' to view e-mails. Use 'Send' to mail the messages to the selected recipients.",
   );

  $form['message'] = array(
    '#type' => 'fieldset',
    '#title' => t('Messages'),
    '#description' => "Set the message body for the participants. Use the following tokens for replacement values: %project, %survey, %name, %email, %subject, %supervisor, %researcher. Web page addresses and e-mail addresses turn into links automatically. Lines and paragraphs break automatically.",
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    //'#group' => 'additional_settings',
    '#tree' => TRUE,
  );

  if ($project_type == SLN_PROJECT_TYPE_360) {
    
    $v = count($project->field_messaging_subject_message) ? $project->field_messaging_subject_message[LANGUAGE_NONE][0]['value']: NULL;
    if (!drupal_strlen($v))
      $v = variable_get('sln_project_messaging_message_subject', "Please take the survey at <a href='%survey'>%survey</a> to evaluate %subject.");
    $form['message']['subject'] = array(
     '#type' => 'text_format',
     '#title' => t('Subject Message'),
     '#required' => TRUE,
     '#default_value' => $v,
     '#rows' => 8,
     '#description' => "",
      '#tree' => TRUE,
    );

    $v = count($project->field_messaging_sup_message) ? $project->field_messaging_sup_message[LANGUAGE_NONE][0]['value']: NULL;
    if (!drupal_strlen($v))
      $v = variable_get('sln_project_messaging_message_supervisor', "Please take the survey at <a href='%survey'>%survey</a> to evaluate %subject.");
    $form['message']['supervisor'] = array(
     '#type' => 'text_format',
     '#title' => t('Supervisor Message'),
     '#required' => TRUE,
     '#default_value' => $v,
     '#rows' => 8,
     '#description' => "",
     '#tree' => TRUE,
    );
  
  }

  $v = count($project->field_messaging_obs_message) ? $project->field_messaging_obs_message[LANGUAGE_NONE][0]['value']: NULL;
  if (!drupal_strlen($v))
    $v = variable_get('sln_project_messaging_message_observer', "Please take the survey at <a href='%survey'>%survey</a> to evaluate %subject.");
  $form['message']['observer'] = array(
   '#type' => 'text_format',
   '#title' => t('Observer/Participant Message'),
   '#required' => TRUE,
   '#default_value' => $v,
   '#rows' => 8,
   '#description' => "",
   '#tree' => TRUE,
  );

  $v = count($project->field_project_messaging_topic) ? $project->field_project_messaging_topic[LANGUAGE_NONE][0]['value']: NULL;
  if (!drupal_strlen($v))
    $v = variable_get('sln_project_messaging_topic', "%project");
   $form['subject'] = array(
   '#type' => 'textfield',
   '#title' => t('Subject or Topic'),
   '#required' => TRUE,
   '#default_value' => $v,
   '#maxlength' => 255,
  );

  $v = count($project->field_project_messaging_from) ? $project->field_project_messaging_from[LANGUAGE_NONE][0]['value']: NULL;
  if (!drupal_strlen($v))
    $v = variable_get('sln_project_messaging_from', "%researcher");
  $form['from'] = array(
   '#type' => 'textfield',
   '#title' => t('From'),
   '#required' => TRUE,
   '#default_value' => $v,
   '#maxlength' => 255,
  );

  /*
  $v = count($project->field_project_messaging_reply_to) ? $project->field_project_messaging_reply_to[LANGUAGE_NONE][0]['value']: NULL;
  if (!drupal_strlen($v))
    $v = variable_get('sln_project_messaging_reply_to', "%researcher");
  $form['reply_to'] = array(
   '#type' => 'textfield',
   '#title' => t('Reply-To'),
   '#required' => TRUE,
   '#default_value' => $v,
   '#maxlength' => 255,
  );
  */
  
  $form['to'] = array(
    '#type' => 'fieldset',
    '#prefix' => '<div id="project-messaging-recipient-wrapper">',
    '#suffix' => '</div>',
    '#title' => t('Recipients'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => "Select e-mail recipients. <small>Note that only project participants with valid e-mail addresses entered into project form may be selected.</small>",
  );

  $form['to']['recipients'] = array(
   '#type' => 'checkboxes',
   '#options' => $options,
   '#title' => "Check to select recipients.",
   //'#description' => t('Select All/None'),
   '#attributes' => array('class' => array('sln-recipient-checkbox')),
   '#default_value' => $checked,
   //'#prefix' => '<small>Click <a href="javascript:void(0);" onclick="$(\'.sln-recipient-checkbox\').toggle()" title=\'Toggle selection\'>here</a> to select all/none.</small> ',
   //'#suffix' => 'sss',
  );

  $form['actions'] = array(
    '#type' => 'actions'
  );

  $form['actions']['send'] = array(
   '#type' => 'submit',
   '#value' => t('Send'),
   //'#attributes' => array('disabled' => 'disabled'),
  );

  $form['actions']['preview'] = array(
   '#type' => 'submit',
   '#value' => t('Preview'),
   '#submit' => array('_slnsite_project_messaging_form_preview'),
  );

  //dpm($project);
  //dpm(get_defined_vars());

  return $form;
} // _slnsite_project_messaging_form

function _slnsite_project_messaging_form_validate($form, &$form_state) {

  // check for valid emails
  // warn of invalid tokens?

  $project_nid = $form_state['values']['project_nid'];
  $project = node_load($project_nid);
  if ($project) {
    $project->field_messaging_subject_message[LANGUAGE_NONE][0]['value'] = $form_state['values']['message']['subject']['value'];
    $project->field_messaging_sup_message[LANGUAGE_NONE][0]['value'] = $form_state['values']['message']['supervisor']['value'];
    $project->field_messaging_obs_message[LANGUAGE_NONE][0]['value'] = $form_state['values']['message']['observer']['value'];
    $project->field_project_messaging_topic[LANGUAGE_NONE][0]['value'] = $form_state['values']['subject'];
    $project->field_project_messaging_from[LANGUAGE_NONE][0]['value'] = $form_state['values']['from'];
    //$project->field_project_messaging_reply_to[LANGUAGE_NONE][0]['value'] = $form_state['values']['reply_to'];

    node_save($project);
  } // if project
  //dpm(get_defined_vars());

} // _slnsite_project_messaging_form

function _slnsite_project_messaging_form_submit($form, &$form_state) {
  global $user;
  $form_state['rebuild'] = TRUE;
  
  if (!(array_key_exists('values', $form_state) && array_key_exists('recipients', $form_state['values']) && is_array($form_state['values']['recipients']))) {
    drupal_set_message("No email recipients selected.");
    return;
  }
  
  $project_nid = $form_state['values']['project_nid'];
  $project = node_load($project_nid);
  if (!$project) {
    drupal_set_message("Not a valid project id: $project_nid.");
    return;
  }
  
  $cbs = array();     // checkboxes
  $recipients = _slnsite_project_participants($project);

  if (array_key_exists('values', $form_state) && array_key_exists('recipients', $form_state['values']))
    $cbs = $form_state['values']['recipients'];
 
  foreach($recipients as $ndx => $r) {
    if (valid_email_address($ndx)) 
      $recipients[$ndx]['selected'] = array_key_exists($ndx, $cbs) ? $cbs[$ndx] : 1;
  } // foreach

  $data = _slnsite_message_data($project, $recipients, $form_state['values']);
  foreach($recipients as $ndx => $r) {
    if (valid_email_address($ndx)) {
      // why is this necessary?
      $recipients[$ndx]['selected'] = array_key_exists($ndx, $cbs) ? $cbs[$ndx] : 1;
      if ($recipients[$ndx]['selected']) {
        // old init code:
        // what account being used here? manager? director? global $user?
        // $acct = user_load_by_mail($email);
        //  $params = array("acct" => $acct);
        
        // similar, but just return emailparts
        // _slnsite_message_preview($project, $recipients, $form_state['values'])
        
        $params = $data[$ndx];
        $params['project_nid'] = $project_nid;
        //$params["acct"] = $user;
        
        $key = "survey";
        $message = drupal_mail('slnsite', $key, $ndx, LANGUAGE_NONE, $params);
        drupal_set_message('Sending e-mail to ' . $ndx);
      } // if selected 
      //else
        //drupal_set_message("skipping unselected: $ndx");
    } // if valid 
    //else
      //drupal_set_message("invalid email: $ndx"); 
  } // foreach
  
  //dpm(get_defined_vars());
} // _slnsite_project_messaging_form

function _slnsite_project_messaging_form_preview($form, &$form_state) {
  // allow cut and paste from 'preview' to own emails in easy to use format
  $form_state['rebuild'] = TRUE;
  $form_state['values']['show_preview'] = TRUE;

  //dpm(get_defined_vars());
} // _slnsite_project_messaging_form_preview

// obsolete?
function _slnsite_project_plan($project) {

  if (is_object($project))
    $projectid = $project->nid;
  else
    $projectid = $project;

  $s = ""; // "Coming soon"; //"<h2>Plan</h2>\n";  
 
  //$surveynodeid = count($project->field_project_form_planning[LANGUAGE_NONE]) ? $project->field_project_form_planning[LANGUAGE_NONE][0]['nid'] : 0; 
  $surveynodeid = _slnsite_get_survey($project, 'planning');

  
  $mgrplannode = node_load($mgrplanid);
  $submissionid = 141; //_slnsite_project_360_plan_submission($mgrplanid, $projectid);
  $submission = webform_get_submission($mgrplanid, $submissionid);

  $s .= drupal_render(drupal_get_form('webform_client_form_' . $mgrplannode->nid, $mgrplannode, $submission));

  //dpm(get_defined_vars());

  return $s;
} // _slnsite_project_plan


/**
 * Draw a radar chart
 *
 * see http://wiki.pchart.net/doc.draw.radar.html
 *
 */
function _slnsite_project_results_radar_chart($projectid, $labels, $data) {
  $path = '/var/www/sln.thestrategicleader.org/public/sites/all/libraries/pchart';
  include("$path/class/pData.class.php");
  include("$path/class/pDraw.class.php");
  include("$path/class/pImage.class.php");
  include("$path/class/pRadar.class.php");

  $height = 650;
  $width = 1000;
  $hmargin = 180;
  $vmargin = 0;
  $fontsize = 24;

  $MyData = new pData();

  $MyData->addPoints($data[0], "Observers");
  $MyData->setSerieDescription("Observers", "Observers");
  $MyData->setPalette("Observers", array("R" => 0, "G" => 176, "B" => 80)); // #00B050 = green
  //$MyData->setSerieWeight("Observers", 4);

  $MyData->addPoints($data[1], "Supervisor");
  $MyData->setSerieDescription("Supervisor", "Supervisor");
  $MyData->setPalette("Supervisor", array("R" => 0, "G" => 0, "B" => 255));
  //$MyData->setSerieWeight("Supervisor", 4);

  $MyData->addPoints($data[2], "Self");
  $MyData->setSerieDescription("Self", "Self");
  $MyData->setPalette("Self", array("R" => 255, "G" => 0, "B" => 0));
  //$MyData->setSerieWeight("Self", 4);

  $MyData->addPoints($labels, "Skills");
  $MyData->setAbscissa("Skills");

  $myPicture = new pImage($width, $height, $MyData, TRUE);
  $myPicture->setFontProperties(
    array(
     //  "FontName" => "$path/fonts/advent_light.ttf",
      // "FontName" => "$path/fonts/Bedizen.ttf",
      // "FontName" => "$path/fonts/calibri.ttf",
      // "FontName" => "$path/fonts/Forgotte.ttf",
      // "FontName" => "$path/fonts/GeosansLight.ttf",
      // "FontName" => "$path/fonts/MankSans.ttf",
       "FontName" => "$path/fonts/OpenSans-Bold.ttf",
      // "FontName" => "$path/fonts/pf_arma_five.ttf",
      // "FontName" => "$path/fonts/verdana.ttf",
      // "FontName" => "$path/fonts/Ubuntu-R.ttf",
      // "FontName" => "$path/fonts/Architex.ttf",
      // "FontName" => "$path/fonts/Quikhand.ttf",
      // "FontName" => "$path/fonts/AlegreyaSans-Medium.ttf",
      "FontSize" => $fontsize,
      "R" => 0, "G" => 0, "B" => 0
    )
  );
  $myPicture->setGraphArea($hmargin, $vmargin, $width-$hmargin-1, $height-$vmargin-1);
  //$myPicture->setGraphArea($hmargin, $vmargin, $width-1, $height-1);
  //$myPicture->setShadow(TRUE,array("X" => 2, "Y" => 2, "R" => 0, "G" => 0, "B" => 0, "Alpha" => 10));

  //$bgGradient = array('StartR' => 255, 'StartG' => 0, 'StartB' => 0, 'StartAlpha' => 100, 'EndR' => 0, 'EndG' => 255, 'EndB' => 0, 'EndAlpha' => 100);

  $Options = array(
    'DrawPoly' => FALSE,
    'WriteValues' => FALSE,
    'LabelsBackground' => FALSE,
    'DrawBackground' => FALSE,
    'WriteValues' => TRUE,
    'DrawAxisValues' => FALSE,
    'WriteValuesInBubble' => TRUE,
    'Layout' => RADAR_LAYOUT_STAR,
    'LabelPos' => RADAR_LABELS_HORIZONTAL,
    'SegmentHeight' => 1, //SEGMENT_HEIGHT_AUTO,
    'Segments' => 5,
    'FixedMax' => 5,
    'AxisRotation' => -90,
    'PointRadius' => 4,
    //'PolyAlpha' => 50,
    'ValueFontSize' => $fontsize / 2 ,
    //'BackgroundGradient' => $bgGradient,
  );

  //$legend_options = array("Style"=>LEGEND_NOBORDER, "Mode"=>LEGEND_HORIZONTAL, "Family" => LEGEND_FAMILY_CIRCLE);
  //$legend_size = $myPicture->getLegendSize($legend_options);
  //$myPicture->drawLegend(($size - $legend_size['Width'])/2 + 10, $size - $legend_size['Height'], $legend_options);

  $MyChart = new pRadar();
  $MyChart->drawRadar($myPicture, $MyData, $Options);

  // todo: chart changes:
  // set min to 1, not 0
  // inc by 1 always (increasing chart size causes units to move to 0.5), use SegmentHeight ? setFixedScale(-12,12)?

  $output = file_create_filename("sln-radar-$projectid.png", 'sites/default/files/sln-charts');
  $myPicture->Render($output);
  
  // TODO: SLQ independent and chart independent solution needed here for area map   
  // coords are for rectangles around vertex labels for skills
  $coordsarr = array('377,0,645,106', '730,165,998,286', '638,544,827,625', '168,544,357,625', '0,164,268,270');
  $areas = "";
  foreach ($labels as $ndx => $skill) {
    $obs = $data[0][$ndx];
    $sup = $data[1][$ndx];
    $self = $data[2][$ndx];
    $title = "$skill: Obs: $obs, Sup: $sup, Self: $self";  
    $coords = $coordsarr[$ndx];
    $href = "javascript: alert(\"$title\")";
    $areas .= "<area class='tipsy' title='$title' href='$href' shape='rect' coords='$coords'/>\n";
  } // foreach
  
/*  
  $obs1 = $data[0][1];
  $sup1 = $data[1][1];
  $self1 = $data[2][1];
  $title1 = "Transforming: Obs: $obs1, Sup: $sup1, Self: $self1";
  
  $obs2 = $data[0][0];
  $sup2 = $data[1][0];
  $self2 = $data[2][0];
  $title2 = "Managing: Obs: $obs2, Sup: $sup2, Self: $self2";

  $areas =<<< EOS

<area  alt="" title="$title1" href="#" shape="rect" coords="730,165,998,286" style="outline:none;" target="_self"     />
<area  alt="" title="$title2" href="#" shape="rect" coords="377,0,645,106" style="outline:none;" target="_self"     />
<area  alt="" title="3" href="#" shape="rect" coords="0,164,268,270" style="outline:none;" target="_self"     />
<area  alt="" title="4" href="#" shape="rect" coords="168,544,357,625" style="outline:none;" target="_self"     />
<area  alt="" title="5" href="#" shape="rect" coords="638,544,827,625" style="outline:none;" target="_self"     />
EOS;
*/  
  
  return "<div id='pchart_ajax'><img src='" . file_create_url($output) . "' usemap='#chartmap'/><map name='chartmap'>$areas</map></div>";
} // _slnsite_project_results_radar_chart

function _slnsite_project_report($project) {

  $s = '';
  
  _slnsite_project_status_report($project);

  // 180
  if (count($project->field_project_type) && array_key_exists(LANGUAGE_NONE, $project->field_project_type) && $project->field_project_type[LANGUAGE_NONE][0]['tid'] == SLN_PROJECT_TYPE_180) {    
  
    $s .= "There is currently no output report for a 180-type project.";

  } // 180
  
  // 360
  //if (count($project->field_project_type) && array_key_exists(LANGUAGE_NONE, $project->field_project_type) && $project->field_project_type[LANGUAGE_NONE][0]['tid'] == SLN_PROJECT_TYPE_360) {    
  // do not assume project type is filled in.
  else {
    // todo: check that all required submissions are complete else say "report not available yet" and explain why

    $surveynodeid = _slnsite_get_survey($project, 'self');

    $submissionid = _slnsite_project_survey_submission($project, $surveynodeid); 
    // $sql = "select sid from webform_submitted_data inner join webform_component using (nid,cid) where data = :projectid and nid = :nid";
    // $submissionid = db_query($sql, array(':projectid' => $project->nid, ':nid' => $surveynodeid))->fetchField();
    
    //$form = drupal_get_form("_slnsite_submission_report_form", $surveynodeid, $submissionid, SLN_TEMPLATE_SLQ360_SUBMISSION_REPORT);
    
    // TODO: missing params???
    
    $surveynode = node_load($surveynodeid);
    $submission = webform_get_submission($surveynodeid, $submissionid);
    
    $form = drupal_get_form("_slnsite_submission_report_form", $project, $surveynode, $submission, SLN_TEMPLATE_SLQ360_SUBMISSION_REPORT);
    $s = drupal_render($form);
   
  } // 360
  
   return $s;
} // _slnsite_project_report

/* obsolete? */
function _slnsite_project_results($project) {
  $s = ""; //"<h2>Results</h2>\n";

  if (is_numeric($project))
    $project = node_load($project);

  // this is taken care of by hook_menu access callback
  // if ($project->type == 'sln_project_180') {
    // return "180 projects do not display results reports at this time.";
  // }

  if ($project->type == 'sln_project_360') {
    
    // 360
    if (count($project->field_project_type) && array_key_exists(LANGUAGE_NONE, $project->field_project_type) && $project->field_project_type[LANGUAGE_NONE][0]['tid'] == SLN_PROJECT_TYPE_360) {
    
     
      $settings = array(
        'slnsite' => array(
          'dataurl' => 'http://' . $_SERVER['HTTP_HOST'] .  '/sln/project/compare/json/' . $project->nid,
        ),
      );
      drupal_add_js($settings, 'setting');
      drupal_add_js('sites/all/libraries/Chart.js-master/Chart.js');
      drupal_add_js(drupal_get_path('module', 'slnsite') . '/slnsite-chart.js', array('defer' => TRUE, 'type' => 'file'));

      $s .= "<canvas id='sln-radar-canvas' height='500' width='500'></canvas>\n";
      //$s .= "<img id='sln-radar-img' src='http://sln.thestrategicleader.org/sites/default/files/XXXlogo.png' style='display: none;' height='500' width='500'/>\n";
      $s .= "<div id='sln-radar-legend'></div>\n";
      

      $data = _slnsite_project_submissions_summarize($project);

      $data1 = array(); // chart data
      $data2 = array(); // compare table data
      foreach ($data as $nid => $survey) {
        $t = array();
        foreach ($survey['data'] as $key => $arr) {
          $data2[$key][0] = $key;
          $data2[$key][$nid] = round($arr['average'], 2);
          //$t[] = round($arr['average'], 2);
          $t[] = sprintf('%01.2f', round($arr['average'], 2));
        }  // foreach $survey
        $data1[] = $t;
      } // foreach $data

      // display radar chart
      $labels = array("Transforming", "Bonding", "Managing", "Bartering", "Bridging");
      //$s .= _slnsite_project_results_radar_chart($project->nid, $labels, $data1);


      // display summary table
      // note: header order must match $data column ordering via _slnsite_project_submissions_summarize
      $headers2 = array('Skill', 'Observers', 'Supervisor', 'Self');
      //$s .= _slnsite_display_table($headers2, $data2, "sln-360-report-compare");


      // display results table
      $data3 = _slnsite_project_submissions_process($project, TRUE);    // TRUE for showcases
      //$s .= _slnsite_display_table($data3['headers'], $data3['data'], "sln-360-report-results");

      // display action plan table
      //$s .= _slnsite_display_action_plan_table(140, 141);
    } // 360
    
    // 180
    if (count($project->field_project_type) && array_key_exists(LANGUAGE_NONE, $project->field_project_type) && $project->field_project_type[LANGUAGE_NONE][0]['tid'] == SLN_PROJECT_TYPE_180) {    
    
      $s .= "There is currently no output report for a 180-type project.";
    
    
    } // 180
    
  } // 360 project
  //dpm(get_defined_vars());

  return $s;

} // _slnsite_project_results

/**
 * Menu access callback. Ensure a user both access and node 'view' permission.
 */
function _slnsite_project_360_access($node, $account = NULL) {
  if ($node->type != "sln_project_360")
    return FALSE;

  // TODO: limit this to director, admin and researcher. also, researchers?
  if (_slnsite_user_is_project_administrator($node) )
    return TRUE;

  return FALSE;
} // _slnsite_project_360_access

function _slnsite_project_submissions_listing($project) {

  $s = ''; // '<h2>Surveys</h2>';

  if (is_numeric($project))
    $project = node_load($project);

  _slnsite_project_status_report($project);

  $surveys = array();
  if (count($project->field_project_form_self))
    $surveys[] = $project->field_project_form_self[LANGUAGE_NONE][0]['nid'];
  if (count($project->field_project_form_supervisor)) 
    $surveys[] = $project->field_project_form_supervisor[LANGUAGE_NONE][0]['nid'];
  if (count($project->field_project_form_observer)) 
    $surveys[] = $project->field_project_form_observer[LANGUAGE_NONE][0]['nid'];
  // do not include plans
  //if (count($project->field_project_form_planning)) 
  //  $surveys[] = $project->field_project_form_planning[LANGUAGE_NONE][0]['nid'];

  //dpm($surveys);
  
  $data = array();
  foreach($surveys as $survey_nid) {
    $surveynode = node_load($survey_nid);

    // map form_keys to cid's
    $fkcids = array_column($surveynode->webform['components'], 'cid', 'form_key');

    // TODO: test that these indexes exist in surveys. they may not if sln standard fields are missing
    if (!(array_key_exists('sln_project_id', $fkcids) && array_key_exists('sln_participant_id', $fkcids) && array_key_exists('sln_subject_id', $fkcids))) {
      drupal_set_message("Warning: missing sln metadata in survey in _slnsite_project_submission_listing");
      watchdog('slnsite', "warning: missing sln metadata in survey in _slnsite_project_submission_listing", array(), WATCHDOG_WARNING);
      //dpm(get_defined_vars());
      // return " ";
      break;
    }

    // get keys for required/standard sln_survey hidden fields
    $cid_project_id = $fkcids['sln_project_id'];
    $cid_participant_id = $fkcids['sln_participant_id'];
    $cid_subject_id = $fkcids['sln_subject_id'];

    //$submissions = webform_get_submissions(array('nid' => $survey_nid, 'uid' => $manager_uid));
    $submissions = webform_get_submissions($survey_nid);
    
    //dpm($submissions);

    //dpm(get_defined_vars());
    //return "No surveys yet.";

    //dpm(array('survey'=>$surveynode->title,'cid_project_id'=>$cid_project_id, 'cid_participant_id'=>$cid_participant_id, 'cid_subject_id'=>$cid_subject_id));
    
    foreach ($submissions as $submission) {
      
      // TODO: check that these offset's exist DONE
      if (array_key_exists($cid_project_id, $submission->data) && array_key_exists($cid_participant_id, $submission->data) && array_key_exists($cid_subject_id, $submission->data)) {
        $sub_project_id = _slnsite_submission_data('sln_project_id', $submission, $surveynode, -1);
        $sub_participant_id = _slnsite_submission_data('sln_participant_id', $submission, $surveynode, -1);
        $sub_subject_id = _slnsite_submission_data('sln_subject_id', $submission, $surveynode, -1);
      } else {
        $sub_project_id = -1;
      }
     
      if ($sub_project_id == $project->nid) {

        if ($project->field_project_form_planning[LANGUAGE_NONE][0]['nid'] == $survey_nid) 
          dpm($submission);
        
        //dpm($submission);
        $arr = array();
        //$arr['sid'] = $submission->sid;
        //$arr['nid'] = $project->nid;
        if ($project->type == 'sln_project_360')
          $arr['rator'] = _slnsite_project_participant($sub_project_id, $sub_participant_id);  // LOOKUP NAME OR EMAIL
        $arr['survey'] = $surveynode->title;
        $arr['ratee'] = _slnsite_project_participant($sub_project_id, $sub_subject_id);  // LOOKUP NAME OR EMAIL
        $arr['status'] = $submission->is_draft ? " - " : "Complete";
        $arr['link'] = l('form', "survey/$sub_participant_id");     
        $arr['dlink'] = $submission->is_draft ? "&nbsp;" : l('data', "node/$survey_nid/submission/$submission->sid");      
        
        // ignore invalid tokens for subject or participant
        // TODO: delete old surveys
        
        //if ($submission->sid == '384')
          //dpm(array($submission, $arr, $sub_project_id, $sub_participant_id));

        //dpm($arr);
        // what is this? rators and ratees are strings so how can they be id's (return vals?)
        if ((strpos($arr['rator'], "unknown uniquid") === FALSE) && (strpos($arr['ratee'], "unknown uniquid") === FALSE) && ($arr['rator'] != $sub_participant_id) && ($arr['ratee'] != $sub_subject_id))
          $data[] = $arr;
      }
    } // foreach submission
  } // foreach form
  if ($project->type == 'sln_project_360')
    $headers = array(/*'sid', 'nid',*/ 'Rater', 'Survey', 'Subject', 'Status', 'Link', 'Data');
  else
    $headers = array(/*'sid', 'nid', 'Rater', */'survey', 'Subject', 'Status', 'Link', 'Data');

  if (count($data))
    $s .= _slnsite_display_table($headers, $data, 'sln-project-surveys-listing');

  //dpm(get_defined_vars());

  return $s;

} // _slnsite_project_submissions_listing


/**
 * Menu access callback. Ensure a user both access and node 'view' permission.
 */
function _slnsite_project_submissions_listing_access($node, $account = NULL) {
  // TODO: limit this to director, admin and researcher. also, researchers?

  if (_slnsite_user_is_project_administrator($node) )
    return TRUE;
  return FALSE;
} // _slnsite_project_submissions_listing_access

// route project short codes
// we got here via hook init, using a lookup table in vars
function _slnsite_project_router($uniqid) {
  $s = "";
  global $user;
  $codes = variable_get("sln_project_short_codes", array());
  $uniqid = strtolower($uniqid);
  $nid = 0;
  if (array_key_exists($uniqid, $codes))
    $nid = $codes[$uniqid];
  
  // todo show form _slnsite_project_signin_form
  // _slnsite_project_signin_form_validate: email
  // _slnsite_project_signin_form-submit: trim email of spaces, lookup token, drupal_goto survey/$token
  // how is access to survey, demo, planning, report controlled?

  //return "Future login page. This page is under construction. ID: $uniqid, NID: $nid:";
  
  // todo: verify project is open and published?
  $project = node_load($nid);
  if ($project) 
    return drupal_get_form('_slnsite_project_signin_form', $project);
  
  // todo: link to help, this should not happen if variable array is up to date
  return t("We are unable to locate the project with code :code. Please contact an SLN administrator for assistance.", array(':code' => $uniqid));
} // _slnsite_project_router

function _slnsite_project_signin_form($form, &$form_state, $project) {
  
  // title?
  //dpm(get_defined_vars());
    
  // todo: make responsive and html5 with iphone fiendly properties, place holders, etc
    
  // review for 360/180 aspects (e.g, search should include sup and sub, but no add except observer
    
  $form['projectid'] = array(
    '#type' => 'hidden',
    '#value' => $project->nid,
  );

  $form['instructions'] = array(
    '#type' => 'markup',
    '#markup' => "<p>Please enter your email (or an SLN survey coupon code). Your email will not be used to track your survey responses, spammed or shared with others. It is used only to ensure that you can return to review the results.</p>",
  );
  
  $form['email'] = array(
   '#type' => 'textfield',
   '#title' => t('E-mail:'),
   '#required' => TRUE,
   '#default_value' => '',
   '#maxlength' => 255,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Sign In',
  );
  
  return $form;
} // _slnsite_project_signin_form


function _slnsite_project_signin_form_validate($form, &$form_state) {
  // check for valid email if @ present, otherwise assume token
  $email = trim($form_state['values']['email']);
  if (!(strpos($email, '@') === FALSE) && !valid_email_address($email))
    form_set_error('email', t('The email address appears to be invalid.'));  
  //dpm(get_defined_vars());
} // _slnsite_project_signin_form


function _slnsite_project_signin_form_submit($form, &$form_state) {
  $email = trim($form_state['values']['email']);
  $nid = $form_state['values']['projectid'];
  $project = node_load($nid);
  $observers = $project->field_project_observers[LANGUAGE_NONE]; 
  $found = FALSE;
  foreach ($observers as $ndx => $observer) {
    // check for email in project participants as observers (360: test not sup or subject)
    // check for token as an alternative

    $obsemail = "";
    if (array_key_exists(LANGUAGE_NONE, $observer['field_project_observer_email']) && count($observer['field_project_observer_email'][LANGUAGE_NONE])) 
      $obsemail = $observer['field_project_observer_email'][LANGUAGE_NONE][0]['value'];

    $obstoken = "";
    if (array_key_exists(LANGUAGE_NONE, $observer['field_project_observer_token']) && count($observer['field_project_observer_token'][LANGUAGE_NONE])) 
      $obstoken = $observer['field_project_observer_token'][LANGUAGE_NONE][0]['value'];
      
    if (($email == $obsemail) || ($email == $obstoken)){
      $found = TRUE;
      break;
    } // found email
    
  } // foreach observer
  
  // if not found, create observer  and submission, (add to array and save node) set obstoken
  if (!$found) {
    $obstoken = '';
    $obsemail = $email;
    $obsname = $email;
    $pos = strpos($email, '@');
    if ($pos)
      $obsname = str_replace(array('.', '-', '_',), ' ', substr($email, 0, $pos));
    $project->field_project_observers[LANGUAGE_NONE][] = array(
      'field_project_observer_email' => array (LANGUAGE_NONE => array(0 => array('value' => $obsemail))),
      'field_project_observer_name' => array (LANGUAGE_NONE => array(0 => array('value' => $obsname))),
      //'field_project_observer_token' => array (LANGUAGE_NONE => array(0 => array('value' => $obstoken))),
    );
    node_save($project);

    // get new token, generated during save
    $observers = $project->field_project_observers[LANGUAGE_NONE]; 
    $found = FALSE;
    foreach ($observers as $ndx => $observer) {
      $obsemail = "";
      if (array_key_exists(LANGUAGE_NONE, $observer['field_project_observer_email']) && count($observer['field_project_observer_email'][LANGUAGE_NONE])) 
        $obsemail = $observer['field_project_observer_email'][LANGUAGE_NONE][0]['value'];

      $obstoken = "";
      if (array_key_exists(LANGUAGE_NONE, $observer['field_project_observer_token']) && count($observer['field_project_observer_token'][LANGUAGE_NONE])) 
        $obstoken = $observer['field_project_observer_token'][LANGUAGE_NONE][0]['value'];
        
      if (($email == $obsemail) || ($email == $obstoken)){
        $found = TRUE;
        break;
      } // found email
    } // foreach
  } // !$found
  
  // route to submission
  // todo: consider displaying token to user or sending email or noting url in email or drupal_set_message
  // better:? use #redirect or other property of form
  if ($obstoken)
    drupal_goto('survey/' . $obstoken);

  // dpm(get_defined_vars());

  // report error? no email or token found and no submission created
  
  // consider; settings for project that allow/disallow signup
  // consider: what routine controls access to submission in what form (edit, view, plan, report, etc)
} // _slnsite_project_signin_form


// route survey/token urls to appropriate submission, create if needed
// todo: handle PDF, review of report for self/360 or all obs/180
function _slnsite_submission_router($uniqid) {
  $s = "";
  // consider anon vs auth? consider role? director vs researcher vs auth?
  global $user;

  // first look for an existing submission, create one if needed
  $submissionid = _slnsite_submission_token($uniqid);
  if (!$submissionid) {
    // check to see if any project exists that uses this token for a participant (obs, sub or sup)
    $project_nid = _slnsite_project_token($uniqid);
    if ($project_nid) {
      $project = node_load($project_nid);
      if ($project) {
        
        // TODO: test if supervisor and other value exists
        $subject = $project->field_project_subject[LANGUAGE_NONE][0]['field_project_subject_token'][LANGUAGE_NONE][0]['value'];
        $supervisor = $project->field_project_supervisor[LANGUAGE_NONE][0]['field_project_supervisor_token'][LANGUAGE_NONE][0]['value'];
        
        // assume observer form as most likely unless token belongs to subject or supervisor
        $form_nid = 0;
        if (count($project->field_project_form_observer))
          $form_nid = $project->field_project_form_observer[LANGUAGE_NONE][0]['nid'];

        if (($subject == $uniqid) && count($project->field_project_form_self))
          $form_nid = $project->field_project_form_self[LANGUAGE_NONE][0]['nid'];
        
        if (($supervisor == $uniqid) && count($project->field_project_form_supervisor))
          $form_nid = $project->field_project_form_supervisor[LANGUAGE_NONE][0]['nid'];
        
        
         // TODO: ensure $uniqid is safe
        if ($subject && $form_nid) {
          $submissionid = _slnsite_project_submission_create($project_nid, $form_nid, $subject, $uniqid);
        } // subject
        
        if (!$submissionid) {
          // unable to create submission
          // if no project or submission, indicate that no survey corresponds to the url and show form to email person links (assuming anon). return only message and form    
          $s .= "No survey matches this url address (unable to create survey). Please contact an administrator for assistance.";
        }
      }
      else {
        $s .= "No survey matches this url address (unable to load project). Please contact an administrator for assistance.";  
      } // $project
    }
    else {
      $s .= "No survey matches this url address (unable to find project id). Please contact an administrator for assistance.";  
    } // $project_nid
    
  } // !$submissionid
 
  //dpm(get_defined_vars());

  if ($submissionid > 0) {
    $surveynid = _slnsite_surveynode_from_submission($submissionid);
    $surveynode = node_load($surveynid);   
    $submission = webform_get_submission($surveynid, $submissionid);
    
    // todo: check form for title, use instrument? how show version
    if ($surveynode) {
      $instrument = $surveynode ? $surveynode->field_sln_survey_instrument[LANGUAGE_NONE][0]['tid'] : NULL;
      $subjecttype = $surveynode ? $surveynode->field_sln_survey_subject[LANGUAGE_NONE][0]['tid'] : NULL;
      $i = taxonomy_term_load($instrument);
      $st = taxonomy_term_load($subjecttype);
      //drupal_set_title('<center>The ' .$i->name . '&trade; <br/><small>Version 4 ' . $st->name, PASS_THROUGH) . '</small></center>';
    }
    
    if ($submission) {
    
      // TODO: use? _slnsite_submission_is_complete($surveynodeid, $submissionid);
      $is_draft = $submission->is_draft;
      
      
      $project_nid = _slnsite_submission_data('sln_project_id', $submission, $surveynode);
      if ($project_nid) {   
        $project = node_load($project_nid);
        if ($project) {
          // TODO: is project published?, special message for unpublished projects?
          
          $project_submissions_complete = _slnsite_project_submissions_complete($project);
      
          $self_form_nid = 0;
          if (count($project->field_project_form_self))
            $self_form_nid = $project->field_project_form_self[LANGUAGE_NONE][0]['nid'];
        
          $obs_form_nid = 0;
          if (count($project->field_project_form_supervisor))
            $obs_form_nid = $project->field_project_form_supervisor[LANGUAGE_NONE][0]['nid'];
                  
          $sup_form_nid = 0;
          if (count($project->field_project_form_observer))
            $sup_form_nid = $project->field_project_form_observer[LANGUAGE_NONE][0]['nid'];
          
          $plan_form_nid = 0;
          if (count($project->field_project_form_planning))
            $plan_form_nid = $project->field_project_form_planning[LANGUAGE_NONE][0]['nid'];
          
          // TODO: we need to consider access to plans, reports, etc for self type submissions

      //dpm(get_defined_vars());
      
          if ($is_draft) {
            $s = drupal_get_form('webform_client_form_' . $surveynode->nid, $surveynode, $submission);
            //dpm(get_defined_vars());
          }
          else {
            // TODO: check if user is project manager, director or admin.
            // if so, show data view and form view e.g., goto:
            // drupal_goto("node/$surveynid/submission/$submissionid/edit");
            if (($surveynid == $obs_form_nid) || ($surveynid == $sup_form_nid)) {
              // if obs or sup and complete, then display thank you and indicate submission already received          
              drupal_goto("thank-you");
            } 
            else {
              
              //dpm(array($project_submissions_complete));
              if ($project_submissions_complete) {
                // check for plan. if no plan for this project, create it. either way, show plan form.
                $plansubmissionid = _slnsite_project_survey_submission($project, $plan_form_nid) ;
                if (!$plansubmissionid) {
                  $plansubmissionid = _slnsite_project_submission_create($project_nid, $plan_form_nid, $uniqid, $uniqid);
                }
                $plansurveynode = node_load($plan_form_nid);
                $plansubmission = webform_get_submission($plan_form_nid, $plansubmissionid);
                if ($plansurveynode && $plansubmission) {
                  // TODO: check project status too, if project closed, do not allow alterations to plan
                  $plansubmission->is_draft = TRUE;  // force submit button and redirect every time
                  $s = drupal_get_form('webform_client_form_' . $plansurveynode->nid, $plansurveynode, $plansubmission);
                  //dpm(get_defined_vars());
                }
                else {
                  $s .= "Unable to load planning form. Please contact an administrator for assistance.";
                } // if $plansurveynode
              }
              else {
                //$s .= "You will be sent an e-mail notification when all survey responses are submitted and your report is ready.";
                drupal_goto("thank-you-subject");
              }
            } // $surveynid is $self_form_nid

            // NOTE: do not use the following. I can't remember why?
            //$format = "form";
            //$s .= webform_submission_page($node, $submission, $format);
            // same as going to 
            //drupal_goto("node/$surveynid/submission/$submissionid/edit");   
          } // $is_draft
        }
        else {
          $s .= "Submission found, but error reading project. Please contact an administrator for assistance.";  
        } // $project
      }
      else {
        $s .= "Submission found, but unable to find project id in data. Please contact an administrator for assistance.";  
      } // $project_nid
 
    } // if $submission    
  } // $submission id > 0

  return $s;
} // _slnsite_submission_router

// now obsolete
//function _slnsite_submission_router($submissionid) {
function _slnsite_submission_router_original($uniqid) {
  global $user;
  if ($user->uid == 1)
    return _slnsite_submission_router2($uniqid);   
 
  $sql = "SELECT sid FROM webform_submitted_data d, webform_component wc WHERE d.nid = wc.nid AND d.cid = wc.cid AND wc.form_key = 'sln_participant_id' AND d.data = :uniqid";

  //dpm($sql);
  $submissionid = db_query($sql, array(':uniqid' => $uniqid))->fetchField();

  $nid = _slnsite_surveynode_from_submission($submissionid);
  //drupal_goto("node/$nid/submission/$submissionid/edit");
  $node = node_load($nid);
  $submission = webform_get_submission($nid, $submissionid);

  return drupal_get_form('webform_client_form_' . $node->nid, $node, $submission);
  //$format = "form";
  //return webform_submission_page($node, $submission, $format);

} // _slnsite_submission_router


/**
 *
 * output radar chart friendly data for testing
 *
 */
function _slnsite_project_submissions_compare_json($projectid) {
  // TODO: create real data
  // TODO: move to test  // todo: get labels from taxonomy 'SLN Skills', based on instrument parameter
  $labels = array("Transforming", "Bonding", "Managing", "Bartering", "Bridging");

 // todo: get from submission data for subject, else taxonomy 'SLN Benchmark Types' and 'SLN Benchmarks'
  // TODO: just output data for self, framework and peers
  // TODO: put framework benchmarks in taxonomy


  $data = _slnsite_project_submissions_summarize($projectid);

  $d = array();
  foreach ($data as $nid => $survey) {
    $t = array();
    foreach ($survey['data'] as $key => $arr) {
      $t[] = round($arr['average'], 2);
    }
    $d[] = $t;
  }

  $dataset1 = array(
    'label' => "Observers",
    'fillColor' => "rgba(255,255,255,0.25)",
    'strokeColor' => "rgba(0,0,255,1)",
    'pointColor' => "rgba(0,0,255,1)",
    'pointStrokeColor' => "#fff",
    //'data' => array(5,5,5,5,5),
    'data' => $d[0],
  );
  $dataset2 = array(
    'label' => "Supervisor",
    'fillColor' => "rgba(255,255,255,0.25",
    'strokeColor' => "rgba(152,0,67,1)",
    'pointColor' => "rgba(152,0,67,1)",
    'pointStrokeColor' => "#fff",
    //'data' => array(3.8,3.2,4.1,3.5,4.75),
    'data' => $d[1],
  );
  $dataset3 = array(
    'label' => "Self",
    'fillColor' => "rgba(255,255,255,0.25)",
    'strokeColor' => "rgba(44,162,95,1)",
    'pointColor' => "rgba(44,162,95,1)",
    'pointStrokeColor' => "#fff",
    //'data' => array(3.21,3.25,4.50,3.72,3.80),
    'data' => $d[2],
  );

  $datasets = array($dataset1, $dataset2, $dataset3);
  $response = array('labels' => $labels, 'datasets' => $datasets);

  drupal_json_output($response);

  //dpm(get_defined_vars());
  //return "done";
} // _slnsite_project_submissions_compare_json

/**
 * Callback for mandrill.com webhook
 * 
 * See http://help.mandrill.com/entries/58303976-Message-Event-Webhook-format
 * note that we send out mail to mandril with metadata defined items: "token" and possibly others
 */
function _slnsite_project_mandrillapp_webhook() {

  if (array_key_exists('mandrill_events', $_POST))
    $json = $_POST['mandrill_events'];
  else
    $json = "{'error', 'no POST data'}";
    
  $data = drupal_json_decode($json);
  
  if (array_key_exists('event', $data))
    $event = $data['event'];
  else
    $event = "no event key in json";
    
  watchdog('slnsite mandrill', "json: :json", array(':json' => $json));
  
  header('');

} // _slnsite_project_mandrillapp_webhook

define('MANDRILLAPP_API_KEY', 'OMzlug85tEFaoDbj79eiZg');
define('MANDRILLAPP_API_ENDPOINT', 'https://mandrillapp.com/api/1.0/');
function _slnsite_project_messaging_history($project) {
  // TODO: eventually base this on project id metadata, for now, use participant emails
  $rows = array();
  $s = "";
  $participants = _slnsite_project_participants($project->nid);
  $uri = MANDRILLAPP_API_ENDPOINT . "messages/search.json";
  foreach ($participants as $ndx => $p) {
    //$json = "{'key' : '" . MANDRILLAPP_API_KEY . "', 'query' : 'email: $ndx'}";
    $json = array('key' => MANDRILLAPP_API_KEY, 'query' => "email: $ndx", 'date_from' => "2014-09-01",);
    $curl = curl_init();
    curl_setopt($curl, CURLOPT_POST, 1);
    curl_setopt($curl, CURLOPT_POSTFIELDS, $json);
    curl_setopt($curl, CURLOPT_URL, $uri);
    curl_setopt($curl, CURLOPT_RETURNTRANSFER, 1);
    $response = curl_exec($curl);
    curl_close($curl);    
     $data = drupal_json_decode($response);
    
    // TODO: check for error $data['status'] == "error" 
        
    foreach ($data as $event) {
      $project = $token = "";
      if (array_key_exists('metadata', $event) && array_key_exists('project', $event['metadata'])) 
        $project = $event['metadata']['project'];
      if (array_key_exists('metadata', $event) && array_key_exists('token', $event['metadata'])) 
        $token = $event['metadata']['token'];
      $id = "<small>" . $event['_id'] . "</small>";
      // TODO: use API call to message.info(string key, string id)  or message.content(string key, string id)
      $link = l("view", "messaging/history/" . $event['_id'] );
      $rows[] = array(date('M d, Y h:i a', $event['ts']), /*$id,*/ $event['email'], $event['sender'], $event['subject'], $event['state'], $event['opens'], $event['clicks'], /*$link, $token, $project */);
    } // foreach data
  } // foreach participant
  $headers = array('date time', /*'msg id',*/ 'to', 'from', 'topic', 'state', 'opens', 'clicks', /*'link to email', 'token', 'project'*/);
  $s .= _slnsite_display_table($headers, $rows, "sln-project-messaging-history");
  
  //dpm(get_defined_vars());
  return $s;
} //

/**
 * callback to send survey links
 */
function _slnsite_survey_email($form, &$form_state) {

  //dpm(get_defined_vars());

  $form['instructions'] = array(
    '#type' => 'markup',
    '#markup' => "<p>Enter an email address and we will send you links to any SLN surveys waiting for you.</p>",
  );

  $form['email'] = array(
   '#type' => 'textfield',
   '#title' => t('E-mail:'),
   '#required' => TRUE,
   '#default_value' => '',
   '#maxlength' => 255,
  );

  // todo: make this ajax
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Send',
  );

  return $form;
} // _slnsite_survey_email


function _slnsite_survey_email_validate($form, &$form_state) {
  $email = $form_state['values']['email'];
  if (!valid_email_address($email))
    form_set_error('email', 'Please enter a valid e-mail address.');
} // _slnsite_survey_email_validate

function _slnsite_survey_email_submit($form, &$form_state) {
  $email = $form_state['values']['email'];
  $params["to"] = $email;
  $params["from"] = 'no-reply@thestrategicleader.org';
  $params["subject"] = "Your surveys at The Strategic Leader Network";
  $data = _slnsite_participant_projects($email); // array of $projects
  if (count($data)) {
    $params["message"] = "<p>Here are the links to your surveys:</p>\n";  // TODO: search for all projects using email address
    $cnt = 0;
    foreach ($data as $index => $row) {
      $token = $row['token'];
      $title = $row['title'];
      $cnt++;
      $params["message"] .=  "<p>$cnt. " . l($title, "survey/$token") . "</p>\n"; 
    } // foreach
    //$params["message"] .= "</ul>";
  } 
  else { 
    $params["message"] = "You have no open surveys at this time. If you have further questions, please contact john@thestrategicleader.org";
  } // if
  
  $key = "surveys";
  $message = drupal_mail('slnsite', $key, $email, LANGUAGE_NONE, $params);  
  
  drupal_set_message("An e-mail has been sent to $email");
  dpm(get_defined_vars());
} // _slnsite_survey_email_submit






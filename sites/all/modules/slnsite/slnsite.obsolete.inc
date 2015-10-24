<?php


/**
 * Implements XXX.
 */
function _slnsite_set_planning_form_properties(&$plan) {
  // TODO: do we still need this? YES: sort by skill and question number
  return $plan;
} // _slnsite_set_planning_form_properties


/**
 * Implements XXX.
 */
function _slnsite_set_data_entry_form_properties(&$data) {
  // TODO: do we still need this?
  return $data;
} // _slnsite_set_data_entry_form_properties



/**
 * Implements XXX. Obsolete
 */
function _slnsite_show_account_projects_summary($acct) {
  global $user;

  //module_load_include('inc', 'slnsite', 'slnsite.webform');
  //module_load_include('inc', 'slnsite', 'slnsite.utils');


  // TODO: validate acct
  $s = "";
  //dpm(get_defined_vars());
  if (_slnsite_user_has_director_role() || in_array("project manager", $acct->roles)) {
    // only show "all projects" if user is on their own dashboard (and has proper role)
    if ($user->uid == $acct->uid) {
      // let view handle filtering through query hook
      $s .= "<h3>Projects</h3>";
      $s .= views_embed_view('projects_listing', 'block_1');
    }
  }
  else {
    // get all projects participated in by user and pass nid's to view
    // TODO: factor this out to get_account_projects($acct)
    /*
    $args = array();
    // TODO: filter projects? published only? open only
    $sql = "SELECT DISTINCT nid FROM {content_field_project_participants} WHERE field_project_participants_uid = :uid";
    $q = db_query($sql, array(':uid' => $acct->uid));
    foreach ($q as $r) {
      $args[] = $r->nid;
    }
    if (count($args)) {
      //$s .= "<p>args:" . implode("+", $args) . "</p>";
      // show all project info for each arg (project id)
      // TODO: replace with more details for each project
      // TODO: recreate D7 version of this view?
      // $s .= views_embed_view('projects_listing', 'block_1', implode("+", $args));
    }
    else {
      $s .= "You are not a participant in any projects yet.";
    }
    */
  } // director or project manager

  return $s;
} // _slnsite_show_account_projects_summary



/**
 * Implements XXX. Obsolete
 */
function _slnsite_show_account_projects_details($acct = NULL, $project = NULL) {
  global $user;

  //module_load_include('inc', 'slnsite', 'slnsite.webform');
  //module_load_include('inc', 'slnsite', 'slnsite.utils');


  _slnsite_validate_current_user();
  $acctobj = _slnsite_account_object($acct);
  if (!is_object($acctobj)) {
    if (is_numeric($acct) && ($acct > 0)) {
      drupal_set_message(t("Invalid account: %acct. Displaying dashboard for current user.", array('%acct' => $acct)), "warning");
    } // (is_numeric($acct) && ($acct > 0))
    else {
      // user is viewing their own dashboard, do nothing special
    } // (is_numeric($acct) && ($acct > 0))
    $acct = $user;
  } // (!is_object($acctobj))
  else {
    $acct = $acctobj;
  } // (!is_object($acctobj))

  // TODO: check for director vs no access

  $args = array();
  if (is_null($project)) {
    // get all projects participated in by user and pass nid's to view
    // TODO: factor this out to get_account_projects($acct)

    // TODO: filter projects? published only? open only
    // REDO: sql for D7
    $sql = "SELECT DISTINCT nid FROM {content_field_project_participants} WHERE field_project_participants_uid = :uid";
    $q = db_query($sql, array(':uid' => $acct->uid));
    foreach ($q as $r) {
      $args[] = $r->nid;
    } // while
  } // (is_null($project))
  else {
    if (is_object($project)) {
      $args[] = $project->nid;
    }
    else {
      if (is_numeric($project) && ($project > 0)) {
        $args[] = $project;
      } // (is_numeric($project) && ($project > 0))
    } // is_object project
  } // is_null project
  $s = "";
  if (count($args)) {
    //$s .= "<p>args:" . implode("+", $args) . "</p>";
    // show all project info for each arg (project id)
    // TODO: replace with more details for each project
    // TODO: recreate D7 version of this view?
    //$s .= views_embed_view('project_details', 'default', implode("+", $args), $acct->uid);
  }
  else {
    if (!_slnsite_user_has_director_role())
      $s .= "You are not a participant in any projects yet.";
  } //(count($args))
  $s .= "<h3>xxxSurvey Submissions</h3>";
  if (_slnsite_user_has_director_role())
    $s .= views_embed_view('user_survey_submissions', 'page_2');
  else
    $s .= views_embed_view('user_survey_submissions', 'block', $acct->uid);
  return $s;
} // _slnsite_show_projects_details


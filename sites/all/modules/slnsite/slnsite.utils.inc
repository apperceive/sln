<?php

/**
 * @file
 * Page callbacks for the utility functions in the slnsite module.
 */


////////  USER ACCOUNT AND ACCESS UTILITIES

/**
 * Implements XXX.
 */
function _slnsite_user_has_director_role() {
  global $user;
  //$acct = _slnsite_account_object($acct);
  return (in_array("director", $user->roles) || in_array("administrator", $user->roles) || ($user->uid == 1));
} // _slnsite_user_has_director_role

/**
 * Implements XXX.
 */
function _slnsite_user_has_manager_role() {
  global $user;
  //$acct = _slnsite_account_object($acct);
  return in_array("researcher", $user->roles);
} // _slnsite_user_has_manager_role

/**
 * Implements XXX.
 */
function _slnsite_user_is_project_participant($projectnode) {
  global $user;
  if (!is_object($projectnode) || ($projectnode->type != 'project'))
    return FALSE;
  if (is_array($projectnode->field_project_participants) && array_key_exists(LANGUAGE_NONE, $projectnode->field_project_participants) && is_array($projectnode->field_project_participants[LANGUAGE_NONE])) {
    foreach ($projectnode->field_project_participants[LANGUAGE_NONE] as $ndx => $p) {
      //dpm($p);
      if ($p['uid'] == $user->uid)
        return TRUE;
    }
  }
  return FALSE;
} // _slnsite_user_is_project_participant

/**
 * Implements XXX.
 */
function _slnsite_user_is_project_administrator($project = NULL) {
  global $user;
  //$acct = _slnsite_account_object($acct);
  $acct = $user;
  if (_slnsite_user_has_director_role())
    return TRUE;
  //if (!_slnsite_user_has_manager_role())
  //  return FALSE;
  if (is_null($project))
    return FALSE;
  if (is_numeric($project) && ($project > 0))
    node_load($project);
  if (is_object($project)) {
    //if ($project->type == "project")
    if (isset($project->field_project_manager) && is_array($project->field_project_manager[LANGUAGE_NONE]))    
      return ($project->field_project_manager[LANGUAGE_NONE][0]['uid'] == $acct->uid);
  } // is_object
  
    //dpm(get_defined_vars());
  
  return FALSE;
} // _slnsite_is_director

/**
 * Implements XXX.
 */
function _slnsite_init_array($obj_or_id, $field = 'nid') {
  // init array with number or object field
  $arr = array();
  if (is_numeric($obj_or_id) && ($obj_or_id > 0)) {
    $arr[] = $obj_or_id;
  }
  else {
    $arr[] = $obj_or_id->{$field};
  } // is_numeric($obj_or_id) && ($obj_or_id > 0))
  return $arr;
} // _slnsite_init_array

// return Drupal object with indicated id, use type to distinguish between user and node
// NOTE: type is not the node type
/**
 * Implements XXX.
 */
function _slnsite_object($obj_or_id, $type = 'node') {
  if (is_object($obj_or_id))
    return $obj_or_id;
  if (is_numeric($obj_or_id) && ($obj_or_id > 0)) {
    switch ($type) {
      case 'node':
        $obj = node_load($obj_or_id);
        break;
      case 'user':
        $obj =  user_load($obj_or_id);
        break;
      default:
        break;
    } // switch
    if (is_object($obj)) {
        return $obj;
    } // is_object
  } // (is_numeric($obj_or_id) && ($obj_or_id > 0))
  return NULL;
} // _slnsite_user_is_project_participant


// check that account is valid object, load it if passed valid uid, else return NULL
/**
 * Implements XXX.
 */
function _slnsite_account_object($acct = NULL) {
  if (is_null($acct))
    return NULL;
  if (is_numeric($acct) && ($acct > 0))
    $acctobj = user_load($acct);
  else
    $acctobj = $acct;
  if (is_object($acctobj) && $acctobj->uid)
    return $acctobj;
  return NULL;
} // _slnsite_account_object


// make sure user is logged in, otherwise goto login page and display message
/**
 * Implements XXX.
 */
function _slnsite_validate_current_user() {
  global $user;
  if (!$user->uid) {
    // TODO: set destination?
    drupal_set_message(t("Please sign in."));
    //drupal_access_denied();
    //drupal_goto("user");
    //drupal_goto("user/surveys");
    drupal_goto("user");
  } // !$user->uid
} // _slnsite_validate_current_user

/**
 * Implements XXX.
 */
function _slnsite_show_roles($acct) {
  // TODO: validate acct
  //dpm($acct);
  //$acct = _slnsite_account_object($acct);
  if (!is_object($acct))
    return t("<p>Invalid account (not an object)</p>");
  $s = "";
  /*
  if ($acct->uid == 1)
    $s .= "site administrator";
  */
  foreach ($acct->roles as $role) {
    if ($role != "authenticated user") {
      if (drupal_strlen($s))
        $s .= ", ";
      $s .= $role;
    } // if
  } // foreach
  return "<p>". t("Roles: ") . "$s</p>";
} // _slnsite_show_roles

////////  SURVEY UTILITIES

/**
 * Implements XXX.
 */
function _slnsite_survey_question_count($surveynodeid) {
  $sql = "SELECT COUNT(*) FROM {webform_component} wc, {node} n WHERE wc.nid = n.nid AND n.type LIKE 'sln_survey' AND wc.type = 'select' AND n.nid = :nid";
  return db_query($sql, array(':nid' => $surveynodeid))->fetchField();
}

/**
 * Implements XXX.
 */
function _slnsite_survey_questions_enumeration($surveynodeid, $order = 'weight') {
  return _slnsite_survey_components_enumeration('sln_survey', $surveynodeid, $order, array('select'));
}

/**
 * Implements XXX.  OBSOLETE? **********************
 */
function _slnsite_survey_demographic_enumeration($demographicsnodeid, $order = 'weight') {
  return _slnsite_survey_components_enumeration('sln_demographics_webform', $demographicsnodeid, $order);
}

/**
 * Implements XXX. OBSOLETE? **********************
 */
function _slnsite_survey_planning_enumeration($planningnodeid, $order = 'weight') {
  return _slnsite_survey_components_enumeration('sln_planning_webform', $planningnodeid, $order, array('fieldset'), array('hidden', 'markup', 'pagebreak'));
}


/**
 * Get survey components filtered by type and ordered by weight
 * TODO: how allow sort by score type?
 * add join with slnsite_component_scoring until scoring type in component's extra data?
 * NOTE: be sure to pass $include and $exclude as SQL sets
 */
function _slnsite_survey_components_enumeration($type, $nodeid, $order = 'weight', $include = array('select', 'date', 'time', 'textarea', 'grid', 'textfield', 'number', 'file', 'email'), $exclude = array('hidden', 'markup', 'pagebreak', 'fieldset')) {
  $arr = array();
  $sql = "SELECT wc.form_key, wc.name, wc.type, wc.weight, wc.extra, wc.cid, sc.sid, t.name as skill FROM {webform_component} wc, {node} n, {slnsite_component_scoring} sc, {taxonomy_term_data} t WHERE t.tid = sc.sid AND wc.nid = n.nid AND sc.nid = n.nid AND wc.cid = sc.cid AND NOT wc.extra LIKE '%css_classes%no-number%' AND n.type = :type AND wc.type IN (:include) AND NOT wc.type IN (:exclude) AND n.nid = :nid ORDER BY $order";
  $args = array_merge(array(':type' => $type), array(':include' => $include), array(':exclude' => $exclude), array(':nid' => $nodeid));
  $q = db_query($sql, $args);
  foreach ($q as $r) {
    $arr[$r->form_key] = $r;
  } // foreach
  // dpm(get_defined_vars());
  return $arr;
}

/**
 * Get all published survey form node ids and titles
 */
function _slnsite_surveys_as_options() {
  $sql = "SELECT nid, title FROM {node} WHERE type = 'sln_survey' AND status = 1 ORDER BY title";
  $q = db_query($sql);
  $options = array();
  foreach ($q as $r) {
    $options[$r->nid] = $r->title;
  } // foreach
  //dpm(get_defined_vars());
  return $options;
} // _slnsite_surveys_as_options

/**
 * Get all published project surveys form node ids and titles
 */
function _slnsite_project_surveys_as_options($projectid) {
  // $sql = "SELECT n.nid, n.title FROM node n, field_data_field_project_survey_forms psf WHERE psf.field_project_survey_forms_nid = n.nid AND psf.entity_id = :projectid";
  $sql = "SELECT n.nid, n.title FROM node n, field_data_field_project_surveys ps WHERE ps.field_project_surveys_nid = n.nid AND ps.entity_id = :projectid";
  $q = db_query($sql, array(':projectid' => $projectid));
  $options = array(0 => ' - No Survey Assigned - ');
  foreach ($q as $r) {
    $options[$r->nid] = $r->title;
  } // foreach
  //dpm(get_defined_vars());
  return $options;
} // _slnsite_surveys_as_options


/**
 * Get all published project surveys form node ids and titles
 */
function _slnsite_project_participants_as_options($projectid) {
  $sql = "SELECT u.uid, u.name FROM field_data_field_project_participants pp, users u WHERE u.uid = pp.field_project_participants_uid AND pp.entity_id = :projectid";
  $q = db_query($sql, array(':projectid' => $projectid));
  $options = array(0 => ' - Please Select - ');
  foreach ($q as $r) {
    $options[$r->uid] = $r->name;
  } // foreach
  //dpm(get_defined_vars());
  return $options;
} // _slnsite_surveys_as_options


/**
 * Get all scoring component cid, sid and form_keys for a given node id
 */
function _slnsite_component_scoring($nid) {
  $sql = "SELECT scs.cid, scs.sid, scs.form_key FROM {slnsite_component_scoring} scs, {webform_component} wc WHERE wc.nid = scs.nid AND wc.cid = scs.cid AND scs.nid = :nid ORDER BY sid, weight";
  $q = db_query($sql, array(':nid' => $nid));
  $options = array();
  foreach ($q as $r) {
    $options[$r->cid] = array(  // consider: use form_key instead?
      'form_key' => $r->form_key,   // form_key is no longer maintained
      'sid' => $r->sid,
      'cid' => $r->cid,
    );
  } // foreach
  //dpm(get_defined_vars());
  return $options;
} // _slnsite_component_scoring

////////  SUBMISSION PROCEDURES

/**
 * Implements XXX.
 */
function _slnsite_submission_position($surveynodeid, $submissionid) {
  $position = "";
  $cid = 0;
  // NOTE: webform_submitted_data fields are (nid, sid, cid, no, data)
  $sql = "SELECT wsd.data, wsd.cid FROM {webform_submitted_data} wsd, {webform_component} wc WHERE wsd.sid = :sid AND wsd.nid = :nid AND wsd.nid = wc.nid AND wsd.cid = wc.cid AND wc.form_key = :form_key";
  $q = db_query($sql, array(':sid' => $submissionid, ':nid' => $surveynodeid, ':form_key' => 'org_role'));
  foreach ($q as $r) {
    $position = $r->data;
    $cid = $r->cid;
  } // foreach
  if (($position === FALSE) || (drupal_strlen($position) == 0))
    $position = "";
  if ($position > -1) {
    $surveynode = node_load($surveynodeid);     // can we use this to avoid query?
    if (array_key_exists($cid, $surveynode->webform['components'])) {
      $selectitems = $surveynode->webform['components'][$cid]['extra']['items'];
      $positions = _slnsite_select_options_from_string($selectitems);
    }
    if (isset($positions) && is_array($positions) && array_key_exists($position, $positions))
      $position = array_key_exists($position, $positions) ? $positions[$position] : "";
    else
      $position = "";
  }
  //dpm(get_defined_vars());
  return $position;
} // _slnsite_submission_leadername


/**
 * Implements XXX.
 */
function _slnsite_submission_leadername($surveynodeid, $submissionid, $componentid) {
  // NOTE: webform_submitted_data fields are (nid, sid, cid, no, data)
  $sql = "SELECT data FROM {webform_submitted_data} WHERE cid = :cid AND sid = :sid AND nid = :nid";
  $q = db_query($sql, array(':cid' => $componentid, ':sid' => $submissionid, ':nid' => $surveynodeid));
  $leadername = $q->fetchField();
  if (drupal_strlen($leadername) == 0)
    $leadername = "self";
  return $leadername;
} // _slnsite_submission_leadername

////////  SURVEY PROCEDURES

// look up leader field in webform_components
/**
 * Implements XXX.
 */
function _slnsite_survey_leader_component($surveynodeid) {
  $sql = "SELECT cid FROM {webform_component} WHERE name = 'leader' AND nid = :nid";
  return db_query($sql, array(':nid' => $surveynodeid))->fetchField();
} // _slnsite_survey_leader_component

// pass either voc object, id or name, pass preloaded options array to include 'none'
function _slnsite_select_options_from_vocabulary($voc, $options = array()) {
  if (is_string($voc))
    $voc = taxonomy_vocabulary_machine_name_load($voc);
  elseif (is_numeric($voc))
    $voc = taxonomy_vocabulary_load($voc);
  if (!is_object($voc))
    return $options;
  $voctree = taxonomy_get_tree($voc->vid, 0, NULL, TRUE);
  foreach ($voctree as $term) {
    $options[$term->tid] = t($term->name);
  } // foreach
  return $options;
} // _slnsite_select_options_from_vocabulary

/**
 * Get select options from a term or termid.  Return as array.
 */
function _slnsite_select_options_from_term($term) {
  $options = array();
  if (is_numeric($term))
    $term = taxonomy_term_load($term);
  if (!is_object($term))
    return $options;
  //watchdog('mss', var_export($term, 1));
  //dpm(array($term->name => $term->description));
  $opttext = $term->description;
  return _slnsite_select_options_from_string($opttext);
} //_slnsite_select_options_from_term

/**
 * Get select options from a node or nodeid.  Return as array.
 */
// TODO: DONE: add version that works with terms
// THIS IS NOW OBSOLETE
function _slnsite_select_options_from_node($node) {
  $options = array();
  if (is_numeric($node))
    $node = node_load($node);
  if (!is_object($node))
    return $options;
  $opttext = $node->body[LANGUAGE_NONE][0]['value'];
  return _slnsite_select_options_from_string($opttext);
} //_slnsite_select_options_from_node

/**
 * Get select options from a string.  Return as array.
 */
function _slnsite_select_options_from_string($opttext) {
  $options = array();
  $opttexts = explode("\n", $opttext);
  foreach ($opttexts as $ndx => $opttext) {
    if (drupal_strlen(trim($opttext))) {
      if (strpos($opttext, "|") != FALSE) {
        list($value, $label) = explode("|", $opttext);
        // CONSIDER: disallow blank lines???  ***************************
        // treat blank lines as a space " "
        if (!drupal_strlen(trim($label)))
          $label = " ";
      }
      else {
        $value = $ndx + 1;
        $label = $opttext;
      } // if strpos
      $options[$value] = t($label);
    } // strlen
  } // foreach
  return $options;
} //_slnsite_select_options_from_node

/**
 * get skills names and descriptions from taxonomy for a given instrument
 *
 * use second param to include or exclude "Strategic Leadership"
 */
function _slnsite_skills_definitions($instrument_termid, $include_all = FALSE) {
  $voc = taxonomy_vocabulary_machine_name_load(SLN_SKILLS);
  $skillterms = taxonomy_get_tree($voc->vid, 0, NULL, TRUE);
  $rows = array();
  foreach ($skillterms as $skillterm) {
    if ($skillterm->field_sln_instrument[LANGUAGE_NONE][0]['tid'] == $instrument_termid) {
      // test for existence of value field since some taxonomy terms did not have it defined originally
      if (!$include_all && _slnsite_array_keys_exist($skillterm->field_sln_summary_field, LANGUAGE_NONE, 0, 'value') && ($skillterm->field_sln_summary_field[LANGUAGE_NONE][0]['value'] == 1))
        continue;
      $rows[$skillterm->tid] = array($skillterm->name, $skillterm->description);
    }  // if instrument
  }  // foreach $skilterm
  return $rows;
} // _slnsite_skills_definitions

/**
 * return array of term names
 */
function _slnsite_vocabulary_term_names($vocabulary_machine_name) {
  $voc = taxonomy_vocabulary_machine_name_load($vocabulary_machine_name);
  $terms = taxonomy_get_tree($voc->vid, 0, NULL, TRUE);
  $rows = array();
  foreach ($terms as $term) {
    $rows[$term->tid] = $term->name;
  }  // foreach $term
  return $rows;
} // _slnsite_vocabulary_term_names

/**
 *
 * Display submitter information used in reports
 *
 */
function _slnsite_display_submitter_info_table($uid, $position = "") {

  $acct = user_load($uid);
  
  if ($acct && $acct->uid) {
    $first = $acct->profile_first_name;
    $last = $acct->profile_last_name;
    
    $name = "";
    if ($first && $last)
      $name = $first . " " . $last;
    if (!drupal_strlen(trim($name))) 
      $name = $acct->mail;
  }
  else {
    //$name = "Unknown. User id is not valid ($uid)";
    // pass thru uid (it's an email or name)
    $name = check_plain(is_string($uid) ? $uid : "");
  }

  $header = array();
  $rows = array();
  $rows[] = array('data' => array(t('Name'), $name), 'class' => array('name'));
  $rows[] = array('data' => array(t('Date Time'), date("m/d/Y g:i a T")), 'class' => array('date'));
  if (drupal_strlen($position))
    $rows[] = array('data' => array(t('Position'), $position), 'class' => array('position'));
  $class = 'sln-submitter-info';
  $vars = array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('class' => array($class)),
    'sticky' => FALSE,
    'caption' => "",
    'colgroups' => array(), //array($colgroup),
    'empty' => 'No data',
  );
  
  return theme_table($vars);
} // _slnsite_display_submitter_info_table

/**
 * Display a simple table with headings and a class
 */
function _slnsite_display_table($header, $rows, $class, $colgroups = array()) {
  $vars = array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('class' => array($class)),
    'sticky' => FALSE,
    'caption' => "",
    'colgroups' => $colgroups,
    'empty' => 'No data',
  );
  return theme_table($vars);
} // _sln_display_table

function _slnsite_comparison_data($instrument, $userscores) {
  $rowlabels = _slnsite_skills_definitions($instrument, $all = TRUE);
  $voc = taxonomy_vocabulary_machine_name_load(SLN_BENCHMARKS);
  $terms = taxonomy_get_tree($voc->vid, 0, NULL, TRUE);
  $i = 0;
  $data = array();
  foreach ($rowlabels as $ndx => $rl) {
    $data[$i] = array();
    $data[$i][0] = $rl[0];
    $userscore = 0;
    if (array_key_exists($i, $userscores))
      $userscore = $userscores[$i];
    $data[$i][1] = array('data' => $userscore, 'class'=>array('number'));// $userscores[$i];
    foreach ($terms as $term) {
      if ($term->field_sln_instrument[LANGUAGE_NONE][0]['tid'] == $instrument) {
        $r = $term->field_sln_skill[LANGUAGE_NONE][0]['tid'];
        $c = $term->field_sln_benchmark_type[LANGUAGE_NONE][0]['tid'];
        $v = $term->field_benchmark_value[LANGUAGE_NONE][0]['value'];
        if ($r== $ndx)
          $data[$i][$c] = array('data' => $v, 'class'=>array('number'));//$v;
     }
    }  // foreach
    $i++;
  }
  return $data;
} // _sln_get_comparison_data

/**
 * Check for the existence of keys in an array.
 */
function _slnsite_array_keys_exist($arr) {
  $keys = array_values(func_get_args());
  $parent = $arr;
  if (!is_array($parent))
    return FALSE;
  // remove the $arr argument, the rest are keys to test
  array_shift($keys);
  //dpm($keys, $parent);
  if (!(is_array($keys) && ($cnt = count($keys))))
    return FALSE;
  foreach ($keys as $ndx => $key) {
    if (is_array($parent)) {
      if(!array_key_exists($key, $parent))
        return FALSE;
      $parent = $parent[$key];
    }
    else {
      return $ndx == $cnt;
    } // if
  } // foreach
  return TRUE;
} // _slnsite_array_keys_exist


/**
 * Check for valid node, user or other id, return boolean indicating if found or not
 */
function _slnsite_check_valid_id($id, $type = 'survey', $tablename = 'node', $idfieldname = 'nid') {
  if (!(is_numeric($id) && ($id > 0)))
    return FALSE;
  if (!drupal_strlen($type))
    $type = 'survey';
  $nodetype = "";
  switch ($type) {
    case 'submission':
        $idfieldname = 'sid';
        $tablename = "{webform_submissions}";
        $nodetype = "";
      break;
    case 'user':
    case 'users':
        $idfieldname = 'uid';
        $tablename = "{users}";
        $nodetype = "";
      break;
    case 'node':
        $idfieldname = 'nid';
        $tablename = "{node}";
        $nodetype = "";
        break;
    case 'page':
    case 'story':
    case 'webform':
        $idfieldname = 'nid';
        $tablename = "{node}";
        $nodetype = $type;
        break;
    case 'term':
        $idfieldname = 'tid';
        $tablename = "{taxonomy_term_data}";
        $nodetype = "";
        break;
    default:
        if (!drupal_strlen($tablename))
          $tablename = 'node';
        if ($tablename == "node") {
          $idfieldname = 'nid';
          if ($type != 'IGNORETYPE')
            $nodetype = 'sln_survey';
        } // if
        $tablename = str_replace(array('{{', '}}'), array('{', '}'), "{$tablename}");
        // use $idname as supplied
      break;
  } // switch
  $args = array(':id' => $id);
  $sql = "SELECT COUNT(*) FROM $tablename WHERE $idfieldname = :id ";
  if (drupal_strlen($nodetype)) {
    if ($type != 'IGNORETYPE') {
      $sql .= " AND type = :nodetype";
      $args[':nodetype'] = $nodetype;
    } // if not IGNORETYPE
  } // if strlen

  // return false on bad sql
  try {
    $q = db_query($sql, $args);
  }
  catch (Exception $e) {
    return FALSE;
  }
  if (!$q || !is_object($q))
    return FALSE;
  return $q->fetchField() ? TRUE : FALSE;
} // _slnsite_check_valid_id

/**
 * for class in either $webform_component['extra']['css_classes'] or $webform_component['value']
 * return boolean
 */
function _slnsite_webform_component_has_class($webform_component, $classname) {
  $retval = FALSE;
  if (_slnsite_array_keys_exist($webform_component, 'extra', 'css_classes'))
    $retval = strpos($webform_component['extra']['css_classes'], $classname);
  if (!$retval && _slnsite_array_keys_exist($webform_component, 'value'))
    $retval = strpos($webform_component['value'], $classname);
  return ($retval > 0) ? TRUE : FALSE;
} // _slnsite_webform_component_has_class

/**
 * return array of questions, responses (as both text and values), skill tid, skill name, class
 *
 */
function _slnsite_submission_plans($surveynodeid, $submissionid) {
  $data = array();
  if (!(_slnsite_check_valid_id($surveynodeid, 'survey') && _slnsite_check_valid_id($submissionid, 'submission') ))
    return $data;

    // load survey and submission
  $submission = webform_get_submission($surveynodeid, $submissionid);
  //dpm($submission);
  $surveynode = node_load($surveynodeid);
  
  
  $self_form_nid = 0;
  $project_nid = _slnsite_submission_data('sln_project_id', $submissionid, $surveynodeid);
  if ($project_nid) {
    $project = node_load($project_nid);
    if ($project) {    
      // TODO: test index? log showed an 'Undefined offset: 0' for next line
      $self_form_nid = (count($project->field_project_form_self)) ? $project->field_project_form_self[0]['nid'] : 0;  
    }
    else {
    
    } // if $project
  }
  else {
    
  } // $project_nid

  

/*  $sql = <<<EOS
  select ttd.name as skill, wc1.name as question, wsd1.data as action, wsd2.data as term, wsd3.data as response, wc1.cid from slnsite_component_scoring scs, taxonomy_term_data ttd, webform_component wc1, webform_component wc2, webform_submitted_data wsd1, webform_submitted_data wsd2, webform_submitted_data wsd3 where wc1.name = wc2.name and wc1.form_key like 'slq_%' and wc2.form_key like 'ta_%' and scs.nid = wc1.nid and scs.cid = wc1.cid and ttd.tid = scs.sid and wsd1.cid = wc2.cid and wsd1.nid = wc2.nid and wc2.nid = wc1.nid and wsd2.cid = wsd1.cid + 1 and wsd3.nid = wsd1.nid and wsd3.cid = wc1.cid and wc2.nid = wc1.nid and wsd2.sid = wsd1.sid and wsd3.sid = wsd1.sid and wc1.nid = :nid and wsd1.sid = :sid order by ttd.weight, wc1.weight

EOS;
 */
 
  $sql = <<<EOS
select ttd.name as skill, wc1.name as question, wsd1.data as action, wsd2.data as term, wsd3.data as response, wc1.cid from slnsite_component_scoring scs, taxonomy_term_data ttd, webform_component wc1, webform_component wc2, webform_submitted_data wsd1, webform_submitted_data wsd2, webform_submitted_data wsd3 where wc1.name = wc2.name and wc1.form_key like 'slq_%' and wc2.form_key like 'ta_%' and scs.nid = wc1.nid and scs.cid = wc1.cid and ttd.tid = scs.sid and wsd1.cid = wc2.cid and wsd1.nid = wc2.nid and wsd2.cid = wsd1.cid + 1 and wsd3.nid = wsd1.nid and wsd3.cid = wc1.cid and wsd2.sid = wsd1.sid and wsd3.sid = wsd1.sid and wc1.nid = :selfnid and wc2.nid = :nid and wsd1.sid = :sid order by ttd.weight, wc1.weight
EOS;
  
  $data = array();
  $sqldebug = strtr($sql, array(':nid' => $surveynodeid, ':sid' => $submissionid, ':selfnid' => $self_form_nid));
  $q = db_query($sql, array(':nid' => $surveynodeid, ':sid' => $submissionid, ':selfnid' => $self_form_nid));
  foreach ($q as $r) {
    $data[] = $r;
  }
  
  $label = t('Your response');
  $rows = array();
  foreach ($data as $ndx => $d) {
    $rows[] = (array) $d;
    $cid = $rows[$ndx]['cid'];
    $response = $rows[$ndx]['response'];
    $selectitems = $surveynode->webform['components'][$cid]['extra']['items'];
    $responseopts = _slnsite_select_options_from_string($selectitems);
    $rows[$ndx]['response'] = array_key_exists($response, $responseopts) ? $responseopts[$response] : "no response for $response";
    
    $action = drupal_strlen($rows[$ndx]['action']) ? $rows[$ndx]['action'] : '&nbsp;';
    $rows[$ndx]['question'] = "<small><small><span class='question'>" . $rows[$ndx]['question'] . "</span>";
    $rows[$ndx]['question'] .= "<span class='response'>$label: " . $rows[$ndx]['response'] . " (" . $response . ").</span></small></small>";
    $rows[$ndx]['question'] .= "<p class='action-plan'>" . $action . "</p>";
    
    //$rows[$ndx]['skill'] = array('data' => $rows[$ndx]['skill'], 'class' => array('action-skill'));
    $rows[$ndx]['question'] = array('data' => $rows[$ndx]['question'], 'class' => array('question-action-plan'));
    //$rows[$ndx]['term'] = array('data' => $rows[$ndx]['term'], 'class' => array('action-term'));
    
    unset ($rows[$ndx]['action']);
    unset ($rows[$ndx]['response']);
    unset ($rows[$ndx]['cid']);
  }

  //dpm(get_defined_vars());
  return $rows;
} // _slnsite_submission_plans

/**
 * return array of questions, responses (as both text and values), skill tid, skill name, class
 *
 */
function _slnsite_submission_actions($surveynodeid, $submissionid, $lang = '') {
  $data = array();
  if (!(_slnsite_check_valid_id($surveynodeid, 'survey') && _slnsite_check_valid_id($submissionid, 'submission') ))
    return $data;

    // load survey and submission
  $submission = webform_get_submission($surveynodeid, $submissionid);
  $surveynode = node_load($surveynodeid);

  // load component scoring into array, indexed by cid
  $scoring = _slnsite_component_scoring($surveynodeid);     // Performance NOTE: this is a db query
  $skilltermopts = _slnsite_select_options_from_vocabulary(SLN_SKILLS, array(0 => 'None'));  // Performance NOTE: calls to tax load and tree

  $instrument_termid = $surveynode->field_sln_survey_instrument[LANGUAGE_NONE][0]['tid'];
  //$voc = taxonomy_vocabulary_machine_name_load(SLN_SKILLS);       // REPEAT???
  //$skillterms = taxonomy_get_tree($voc->vid, 0, NULL, TRUE);      // REPEAT?

  $firstunscored = ($instrument_termid == SLN_INSTRUMENT_SLQ)? SLN_SLQ_UNSCORED_START : SLN_STQ_UNSCORED_START;
  $allscorestid = ($instrument_termid == SLN_INSTRUMENT_SLQ)? SLN_SLQ_ALL_SCORES : SLN_STQ_ALL_SCORES;
  // process data for each score
  foreach($scoring as $cid => $score) {
    $form_key = $score['form_key']; 
    $questionnum = intval(str_replace("slq_", "", $form_key));
    $skilltid = $scoring[$cid]['sid'];
    
    // todo: if lang then append xlate to $selectitems
    
    $selectitems = array_key_exists('items', $surveynode->webform['components'][$cid]['extra']) ? $surveynode->webform['components'][$cid]['extra']['items'] : '';
    
    $name = 'sln_select_options';
    $vocabulary = taxonomy_vocabulary_machine_name_load($name);
    $terms = taxonomy_get_tree($vocabulary->vid, 0, NULL, TRUE);
    $theterm = NULL;
    foreach ($terms as $term) {
      if ($term->field_select_options_machinename[LANGUAGE_NONE][0]['value'] == $selectitems . "_" . $lang) {
        $theterm = $term;
        break;
      } // if language matches
    } // foreach term
    if ($theterm)
      $skillopts = _slnsite_select_options_from_term($theterm);
    else
      $skillopts = _slnsite_select_options_from_string($selectitems);
   
    //$skilloptskeys = array_keys($skillopts);
    $scoreid = $score['sid'];
    $value = array_key_exists($cid, $submission->data) ? $submission->data[$cid][0] : "";
    $question = $surveynode->webform['components'][$cid]['name'];
    
    // todo: if lang then append xlate to question
    
    $skill = array_key_exists($scoreid, $skilltermopts) ? $skilltermopts[$scoreid] : "NA";
    //$response = array_key_exists($value, $skillopts) && array_key_exists($skillopts[$value], $skillopts) ? $skillopts[$skillopts[$value]] : "no response for $value";
    $response = array_key_exists($value, $skillopts) ? $skillopts[$value] : "no response for $value";
    $class = ($value > SLN_UNSCORED_NEUTRAL) ? "strength" : (($value < SLN_UNSCORED_NEUTRAL) ? "weakness" : "neutral");

    if ($questionnum < $firstunscored) {
      $r = array();
      $r['question'] = $question;
      $r['response'] = $response;
      $r['value'] = $value;
      $r['class'] = $class;
      $r['skilltid'] =$skilltid;
      $r['skillname'] = $skill;
      $data[$questionnum] = $r;
    } // if < $firstunscored
  } // foreach scoring

  //dpm(get_defined_vars());
  return $data;
} // _slnsite_submission_actions

/**
 *
 * return array of submission scores for use in analysis, mapping and reports
 *
 * array is indexed by skill tids, which are filtered by surveynode instrument
 */
function _slnsite_submission_scores($surveynodeid, $submissionid) {
  $data = array();
  //if (!(_slnsite_check_valid_id($surveynodeid, 'survey') && _slnsite_check_valid_id($submissionid, 'submission') ))
  //  return $data;

  // load survey and submission
  $submission = webform_get_submission($surveynodeid, $submissionid);
  $surveynode = node_load($surveynodeid);

  // load component scoring into array, indexed by cid
  $scoring = _slnsite_component_scoring($surveynodeid);  // this is empty for 180

  $instrument_termid = 0;
  if ($surveynode)
    $instrument_termid = $surveynode->field_sln_survey_instrument[LANGUAGE_NONE][0]['tid'];
  $voc = taxonomy_vocabulary_machine_name_load(SLN_SKILLS);
  $skillterms = taxonomy_get_tree($voc->vid, 0, NULL, TRUE);

  // define and init common subarray and copy to each skill
  $skillscoresub = array('count' => 0, 'sum' => 0);
  $skillscores = array();
  foreach ($skillterms as $skillterm) {
    if ($skillterm->field_sln_instrument[LANGUAGE_NONE][0]['tid'] == $instrument_termid)
      $skillscores[$skillterm->tid] = $skillscoresub;
  } // foreach skillterm

  $firstunscored = ($instrument_termid == SLN_INSTRUMENT_SLQ)? SLN_SLQ_UNSCORED_START : SLN_STQ_UNSCORED_START;
  $allscorestid = ($instrument_termid == SLN_INSTRUMENT_SLQ)? SLN_SLQ_ALL_SCORES : SLN_STQ_ALL_SCORES;
  // process data for each score
  foreach($scoring as $cid => $score) {
    $form_key = $score['form_key'];
    $questionnum = intval(str_replace("slq_", "", $form_key));
    $value = array_key_exists($cid, $submission->data) ? $submission->data[$cid][0] : "";
    $skilltid = $scoring[$cid]['sid'];

    if ($questionnum < $firstunscored) {
      //$class = ($value > SLN_UNSCORED_NEUTRAL) ? "strength" : (($value < SLN_UNSCORED_NEUTRAL) ? "weakness" : "neutral");
      $skillscores[$skilltid]['count']++;
      $skillscores[$skilltid]['sum'] += $value;
      $skillscores[$allscorestid]['count']++;
      $skillscores[$allscorestid]['sum'] += $value;
    } // if < $firstunscored
  } // foreach scoring

  // sum up totals
  $i = 0;
  foreach ($skillscores as $key => $skillscore) {
    if ($skillscores[$key]['count'])
      $data[$i] = ceil(100 * ($skillscores[$key]['sum'] / $skillscores[$key]['count']));
    else
      $data[$i] = "NAN";
    $i++;
  } // foreach skillscores

  //dpm(get_defined_vars());
  return $data;
} // _slnsite_submission_scores

function _slnsite_display_action_plan_table($surveynodeid, $submissionid) {
  $surveynode = node_load($surveynodeid);
  $rows = _slnsite_submission_plans($surveynodeid, $submissionid);
  $header = array(t('Influence Action'), 'data' => array('data' => t('Actions You Will Take to Develop Your Strategic Leader Skills'), 'class' => array('question-action-plan')), t('Term of Action'));
  $class = 'sln-submission-plans-table';
  $colgroups = array(
    array(array('class' => array('skill'))),
    array(array('class' => array('action'))),
    array(array('class' => array('term'))),
  );
  
  //dpm(get_defined_vars());
  return _slnsite_display_table($header, $rows, $class, $colgroups);
} // _sln_site_display_action_plan_table

function _slnsite_display_action_map_table($skillname, $data)  {
  $header = array(
    'data' => array(
      'data' => $skillname,
      'colspan' => 2
    ),
  );

 // TODO: move text to constants and use t();
  $rows[] = array(
    'data' => array(
      array('data' => "Question", 'class' => array('question')), 
      array('data' => "Your Response (value)", 'class' => array('response')),
    ), 
    'class' => array('headings'),
  );
  
  // TODO: move text to constants and use t();
  $subheader = array(
    'data' => array(
      array(
        'data' => "<span class='weakness'>Red</span> = Needs Improvement <span class='strength'>Green</span> = Strength",
        'colspan' => 2
      )
    ),
    'class' => array('sln-action-plan-legend', 'subheading')
  );
  $rows[] = $subheader;

  $total = 0;
  $cnt = 0;
  $scores = array();
  $qs = array();
  $rs = array();
  foreach ($data as $ndx => $row) {
    if ($row['skillname'] == $skillname) {
      $qs[$ndx] = $row['question'];
      $rs[$ndx] = $row['response'];
      $scores[$ndx] = $row['value'];
      $total += $row['value'];
      $cnt++;
    }
  } // foreach

  // use two passes/loops so as to group questions by strength and weakness
  // NOTE: this skips items with scores of SLN_UNSCORED_NEUTRAL
  // TODO: change to use two constants SLN_SQL_STRENGTH and SLN_SLQ_WEAKNESS and three loops for >, < and between (neutral)
  foreach ($qs as $i => $q) {
    if ($scores[$i] > SLN_UNSCORED_NEUTRAL) {
      $rows[] = array(
        'data' => array(
          array('data' => $q, 'class' => 'question'),
          array('data' => "$rs[$i] <span class='number'>($scores[$i])</span>", 'class' => 'response'),
        ),
        'class' => array('strength'),
      );
    }
  } // foreach

  foreach ($qs as $i => $q) {
    if ($scores[$i] == SLN_UNSCORED_NEUTRAL) {
      $rows[] = array(
        'data' => array(
          array('data' => $q, 'class' => 'question'),
          array('data' => "$rs[$i] <span class='number'>($scores[$i])</span>", 'class' => 'response'),
        ),
        'class' => array('neutral'),
      );
    }
  } // foreach

  foreach ($qs as $i => $q) {
    if ($scores[$i] < SLN_UNSCORED_NEUTRAL) {
      $rows[] = array(
        'data' => array(
          array('data' => $q, 'class' => 'question'),
          array('data' => "$rs[$i] <span class='number'>($scores[$i])</span>", 'class' => 'response'),
        ),
        'class' => array('weakness'),
      );
    }
  } // foreach

  if ($cnt) {
    $total = ceil(100 * $total / $cnt);
    $class = 'number';
  }
  else {
    $total = "NAN";
    $class = 'nan error';
  }
  $subfooter = array(
    'data' => array(
      array(
        'data' => "$skillname total: <span class='$class'>$total</span>",
        'colspan' => 2
      )
    ),
    'class' => array('sln-action-plan-total-score')
  );

  $rows[] = $subfooter;
  $class = 'sln-action-map-table';

  $vars = array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('class' => array($class)),
    'sticky' => FALSE,
    'caption' => "",
    'colgroups' => array(), //array($colgroup),
    'empty' => 'No data',
  );

  return theme_table($vars);

} // _slnsite_display_action_map_table

/**
 *
 *  lookup taxonomy term name by tid
 *
 */
function _slnsite_term_name($tid) {
  $term = taxonomy_term_load($tid);
  if ($term)
    return $term->name;
  return "no term";
} // _slnsite_term_name


/**
 *
 *  return instrument name for a survey node
 *
 */
function _slnsite_survey_instrument_name($surveynodeid) {
  $node = node_load($surveynodeid);
  if ($node && ($node->type == 'sln_survey')) {
    $tid = $node->field_sln_survey_instrument[LANGUAGE_NONE][0]['tid'];
    if ($tid)
      return _slnsite_term_name($tid);
  } // if sln_survey
  return "no instrument for $surveynodeid";
} // _slnsite_survey_instrument_name


/**
 *
 *  return subject name for a survey node
 *
 */
function _slnsite_survey_subject_name($surveynodeid) {
  $node = node_load($surveynodeid);
  if ($node && ($node->type == 'sln_survey')) {
    $tid = $node->field_sln_survey_subject[LANGUAGE_NONE][0]['tid'];
    if ($tid)
      return _slnsite_term_name($tid);
  } // if sln_survey
  return "no subject for $surveynodeid";
} // _slnsite_survey_subject_name


/**
 *
 *  return submission based only on submissionid
 *
 */
function _slnsite_surveynode_from_submission($submissionid) {
  $sql = "SELECT nid FROM {webform_submissions} WHERE sid = :submissionid";
  $q = db_query($sql, array(':submissionid' => $submissionid));
  if ($q)
    return $q->fetchField();
  return 0;
} // _slnsite_surveynode_from_submission


/**
 *
 *  return a specific node's webform component cid based on the form_key
 *
 * return 0 on error
 *
 */
function _slnsite_survey_component_cid($surveynodeid, $form_key) {
  $sql = "SELECT cid FROM {webform_component} WHERE nid = :surveynodeid AND form_key = :form_key";
  $q = db_query($sql, array(':surveynodeid' => $surveynodeid, ':form_key' => $form_key));
  if ($q)
    return $q->fetchField();
  return 0;
} // _slnsite_survey_component_cid


/**
 *
 * return TRUE if (1) all required fields are present and (2) at least 10% of action plans for weaknesses are filled in.
 *
 * note that condition (2) only applies to self assessments and not to observer, RDE or supervisor forms
 *
 * data is an array of submission data, indexed by cid. if empty, this array will be loaded from database
 *
 */
function _slnsite_submission_is_complete($surveynodeid, $submissionid, $messages = FALSE, $data = array()) {

  $submission = webform_get_submission($surveynodeid, $submissionid);
  $surveynode = node_load($surveynodeid);
  
  $subjecttid = $surveynode->field_sln_survey_subject[LANGUAGE_NONE][0]['tid'];
  
  // NOTE: survey node types are obsolete at this time:
  // TODO: switch on node type to allow for differing survey types (e.g., RDE vs sln_survey)
  //if (!$submission || !$surveynode || ($surveynode->type != "sln_survey")) {
  if (!$submission || !$surveynode || !in_array($surveynode->type , array("sln_survey", "sln_survey360", "sln_survey_180"))) {
    if ($messages)
      drupal_set_message("No submission, survey node or not of type sln_survey");
    return FALSE;
  }
  
  // CONSIDER: why query db? Isn't the data in the submission?
  if (!count($data))
    $data = $submission->data;

  //allow empty $data array default to preloading it from database
  // this is wrong - fix it
  /*
  if (!count($data)) {
   $sql = "SELECT cid, data FROM {webform_submissions} WHERE nid = :surveynodeid AND sid = :submissionid ORDER BY cid";
   $q = db_query($sql, array(':surveynodeid' => $surveynodeid, ':submissionid' => $submissionid));
   foreach ($q as $r) 
     $data[$r->cid] = $r->data;
  } // if no data 
  */
  
  if (!count($data)) {
    if ($messages)
      drupal_set_message("no data in array");
    return FALSE;
  }
  
  $cntplans = 0;  // count action plans if self-assessment
  foreach ($surveynode->webform['components'] as $cid => $wc) {
    $form_key = $wc['form_key'];
      
    
    // first, check required fields
    if ($wc['required']) {
      //$cid = $wc['cid'];
      if (!array_key_exists($cid, $data)) {
        if ($messages)
          drupal_set_message("no data for a required field, cid: $cid, form_key: $form_key");
        return FALSE;
      }
      
      if (array_key_exists($cid, $data)) {
        $val = $data[$cid];
        if (is_array($val) && array_key_exists(0, $val))
          $val = $val[0];
        else {
          if ($messages)
            drupal_set_message("value is array, but no item at index zero");
          return FALSE;
        }
      }
      else {
        if ($messages)
          drupal_set_message("array key cid ($cid) does not exist in data");
        return FALSE;
      }
      
      if ((is_string($val) && trim($val) == "") || is_null($val)) {
        if ($messages)
          drupal_set_message("no value for a required field, cid: $cid, form_key: $form_key");
        return FALSE;
      }
    } // if required field
    
    // ASSUMPTION: ta9999 are form keys for plan textarea's
    if (($subjecttid == SLN_SUBJECT_SELF) && ($form_key == "ta$cid"))  {
    
      $val = $data[$cid][0];
      if (is_string($val) && drupal_strlen(trim($val)))
         $cntplans++;
         
         
      //dpm(get_defined_vars());
      
    } // self plan
    
  } // foreach wc
  
  // count and compare number of plans written
  if (($subjecttid == SLN_SUBJECT_SELF) && ($cntplans < SLN_MIN_ACTION_PLANS_REQUIRED)) {
    $minplans = SLN_MIN_ACTION_PLANS_REQUIRED;
    //drupal_set_message("less than $minplans action plans ($cntplans)");
    //dpm(get_defined_vars());
    return TRUE;  // TODO: return TRUE for now, but eventually FALSE;
  } // if self and insufficient plans
  return TRUE;
} // _slnsite_submission_complete

// TODO: similar function to return array of error messages that indicate missing items
// e.g.: required questions in part 1 of the instrument, required questions in the demographics, no subject


function _slnsite_project_participant_emails_csv($projectid) {
  $sql = "SELECT u.mail FROM field_data_field_project_participants pp, users u WHERE u.uid = pp.field_project_participants_uid AND pp.entity_id = :projectid";
  $q = db_query($sql, array(':projectid' => $projectid));
  $csv = "";
  foreach ($q as $r) {
    if (strlen($csv))
      $csv .= ",";
    $csv .= $r->mail;
  } // foreach
  //dpm(get_defined_vars());
  return $csv;
} // _slnsite_project_participant_emails_csv

/**
 * presave helper for sln_project
 *
 */
function _slnsite_project_presave($node) {
  // obsolete?
  $projectid = $node->nid;
  
  // do not process project unless it is published
  // TODO: do not make assignments unless project is (1) published and (2) open?
  if (!$node->status)
    return;

  // check for self assigns and ensure each participant has a submission for each self survey
  // note: self assign value may not exist in all survey nodes yet
  // TODO: REFACTOR this out of presave into one or more functions

  $hasvalue = _slnsite_array_keys_exist($node->field_project_self_assign, LANGUAGE_NONE, 0, 'value');
  $selfassign = $hasvalue ? ($node->field_project_self_assign[LANGUAGE_NONE][0]['value']) : FALSE;
  // foreach ($node->field_project_survey_forms[LANGUAGE_NONE] as $ndx => $surveyidarr) {
  foreach ($node->field_project_surveys[LANGUAGE_NONE] as $ndx => $surveyidarr) {
    $surveyid = $surveyidarr['nid'];
    $surveynode = node_load($surveyid);
    $survey_subject_tid = $surveynode->field_sln_survey_subject[LANGUAGE_NONE][0]['tid'];
    if ($survey_subject_tid == SLN_SUBJECT_SELF) {
      // map form_keys to cid's
      $fkcids = array_column($surveynode->webform['components'], 'cid', 'form_key');
      // test that these indexes exist in surveys. they may not if sln standard fields are missing
      if (array_key_exists('sln_project_id', $fkcids) && array_key_exists('sln_participant_id', $fkcids) && array_key_exists('sln_subject_id', $fkcids)) {
        // get keys for required/standard sln_survey hidden fields
        // todo: avoid? at least test for existence  (use array_key_exists)
        $cid_project_id = $fkcids['sln_project_id'];
        $cid_participant_id = $fkcids['sln_participant_id'];
        $cid_subject_id = $fkcids['sln_subject_id'];
        foreach ($node->field_project_participants[LANGUAGE_NONE] as $participantidarr) {
          //dpm($participantid);
          $participantid = $participantidarr['uid'];
          $subjectid = $participantid;
          $submissions = webform_get_submissions(array('nid' => $surveyid, 'uid' => $participantid));
          $found = FALSE;
          foreach ($submissions as $submission) {
            // $sub_project_id = $submission->data[$cid_project_id][0];
            $sub_project_id = _slnsite_submission_data('sln_project_id', $submission, $surveynode, -1);
            // $sub_participant_id = $submission->data[$cid_participant_id][0];
            $sub_participant_id = _slnsite_submission_data('sln_participant_id', $submission, $surveynode, -1);
            // $sub_subject_id = $submission->data[$cid_subject_id][0];
            $sub_subject_id = _slnsite_submission_data('sln_subject_id', $submission, $surveynode, -1);
            if (($sub_project_id == $projectid) && ($sub_participant_id == $participantid) && ($sub_subject_id == $subjectid)) {
              $found = TRUE;
              break;
            }
          } // foreach submission
          if (!$found) {
            // create a new submission
            // TODO: only include valid cid's (use array_key_exists)
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
            watchdog('slnsite', "Submission $sid created for projectid: $projectid, participantid: $participantid, subjectid: $subjectid for surveyid: $surveyid.");
          } // not found, create submission
        } // foreach participant
      } // survey has sln fields
    } // if SLN_SUBJECT_SELF
  } // foreach surveyform
  
} // _slnsite_project_presave

/**
 * validation for sln_project_360 node type
 *
 */
function _slnsite_project_360_validate($node, $form, &$form_state) {

  // use form_set_error($element_name, $message);   // name has format 'foo][bar][baz'

  // check for valid emails
  // check for email dups
  // require names and emails
  // require subject
  // require supervisor?
  // require at least one observer
  
  
  // issue warnings, but not form_set_error for some things?

} // _slnsite_project_360_validate

/**
 * common save function - create surveys if necessary
 *
 */
function _slnsite_project_360_insert_update($node) {

  //dpm(get_defined_vars());

// TODO: need form id's - replace with adv settings fields
  
  $self_form_nid = _slnsite_get_survey($node, 'self');
  $obs_form_nid = _slnsite_get_survey($node, 'observer');
  $sup_form_nid = _slnsite_get_survey($node,  'supervisor');

  //if ($node->nid)
  $project_nid = $node->nid;
  $project_type = SLN_PROJECT_TYPE_DEFAULT;
  if (count($node->field_project_type))
    $project_type = $node->field_project_type[LANGUAGE_NONE][0]['tid'];
  
  //else {      // TODO: TEMP HACK!!!!!
    //$sql = "SELECT AUTO_INCREMENT FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA = 'sln' AND TABLE_NAME = 'node'";
    //$project_nid = db_query($sql)->fetchField();
  //}   
  //drupal_set_message("project_nid: $project_nid");
  
  // NOTE: not really needed any longer. used for submission create, which now uses uid of 0
  //$manager_uid = $node->field_project_manager[LANGUAGE_NONE][0]['uid'];
  
  // note (done): value may not exist if empty!!! test for this. first init values
  $subject = "";
  $supervisor = ""; 
  $observers = $node->field_project_observers[LANGUAGE_NONE]; 
  
  // ONLY 360, NOT 180:
  if ($project_type == SLN_PROJECT_TYPE_360) {
    // first check if subfield has any elements (value, format, safe_value_
    if (array_key_exists(LANGUAGE_NONE, $node->field_project_subject) && count($node->field_project_subject[LANGUAGE_NONE]))
      $subject = $node->field_project_subject[LANGUAGE_NONE][0]['field_project_subject_token'][LANGUAGE_NONE][0]['value'];
    if (array_key_exists(LANGUAGE_NONE, $node->field_project_supervisor) && count($node->field_project_supervisor[LANGUAGE_NONE]))
      $supervisor = $node->field_project_supervisor[LANGUAGE_NONE][0]['field_project_supervisor_token'][LANGUAGE_NONE][0]['value'];
      
    // ONLY 360, NOT 180:
    if (drupal_strlen($subject))
      _slnsite_project_submission_create($project_nid, $self_form_nid, $subject, $subject);
   
    // ONLY 360, NOT 180:
    if (drupal_strlen($subject) && drupal_strlen($supervisor))
      _slnsite_project_submission_create($project_nid, $sup_form_nid, $subject, $supervisor);
  } // SLN_PROJECT_TYPE_360
  
  foreach ($observers as $ndx => $observer) {
    $obstok = "";
    if (array_key_exists('field_project_observer_token', $observer) && array_key_exists(LANGUAGE_NONE, $observer['field_project_observer_token']) && count($observer['field_project_observer_token'][LANGUAGE_NONE]))
      $obstok = $observer['field_project_observer_token'][LANGUAGE_NONE][0]['value'];
    
    // for 180, set $subject to $obstok
    if ($project_type == SLN_PROJECT_TYPE_180) 
      $subject = $obstok;
    
    if (drupal_strlen($subject) && drupal_strlen($obstok))
      $retval = _slnsite_project_submission_create($project_nid, $obs_form_nid, $subject, $obstok); 
    
    //dpm(get_defined_vars());
    
  } // foreach observer
  
  
  
  
  $codes = variable_get("sln_project_short_codes", array());
  // delete old code if there
  foreach ($codes as $ndx => $c) {
    if ($codes[$ndx] == $project_nid)
      unset($codes[$ndx]);
  } // foreach
  // add short code if appropriate
  if (isset($node->field_project_short_code) && is_array($node->field_project_short_code) && array_key_exists(LANGUAGE_NONE, $node->field_project_short_code) && count($node->field_project_short_code[LANGUAGE_NONE])) {
    $code = strtolower(trim($node->field_project_short_code[LANGUAGE_NONE][0]['value']));
    if (drupal_strlen($code)) {
      $codes[$code] = $node->nid;    // how do we know this exists?
    } // $code
  } // short_codes
  variable_set("sln_project_short_codes", $codes);
  
  //dpm(get_defined_vars());

}  // _slnsite_project_360_insert_update



/**
 * node_submit for sln_project_360 node type
 *
 */
function _slnsite_project_360_node_submit($node) {

} // _slnsite_project_360_node_submit

/**
 * insert for sln_project_360 node type
 *
 */
function _slnsite_project_360_insert($node) {
  _slnsite_project_360_insert_update($node);
} // _slnsite_project_360_insert

/**
 * update for sln_project_360 node type
 *
 */
function _slnsite_project_360_update($node) {
  _slnsite_project_360_insert_update($node);
} // _slnsite_project_360_update

/**
 * presave for sln_project_360 node type
 *
 */
function _slnsite_project_360_presave($node) {
  
  //dpm(get_defined_vars());
  
  // do not process project unless it is published
  if (!$node->status) {
    drupal_set_message("Note Surveys will not be created/updated until this project is 'published'.");
    return;
  } // status
  
  // reassign ownership to manager/researcher 
  // assert manager_uid field exists (it's required)
  $manager_uid = $node->field_project_manager[LANGUAGE_NONE][0]['uid'];
  $node->uid = $manager_uid;

  // set any missing tokens
  // first check if subfield has any elements (value, format, safe_value_)
  // assert field always exists and is an array, but has no items before initialization
  $token = "";
  if (array_key_exists(LANGUAGE_NONE, $node->field_project_subject) && count($node->field_project_subject[LANGUAGE_NONE])) {
    $token = $node->field_project_subject[LANGUAGE_NONE][0]['field_project_subject_token'][LANGUAGE_NONE][0]['value'];  
    if (!drupal_strlen($token))
      $node->field_project_subject[LANGUAGE_NONE][0]['field_project_subject_token'][LANGUAGE_NONE][0]['value'] = uniqid();
  } // if count
    
  //dpm(get_defined_vars());    

  $token = "";
  if (array_key_exists(LANGUAGE_NONE, $node->field_project_supervisor) && count($node->field_project_supervisor[LANGUAGE_NONE])) {
    $token = $node->field_project_supervisor[LANGUAGE_NONE][0]['field_project_supervisor_token'][LANGUAGE_NONE][0]['value'];  
    if (!drupal_strlen($token))
      $node->field_project_supervisor[LANGUAGE_NONE][0]['field_project_supervisor_token'][LANGUAGE_NONE][0]['value'] = uniqid();
  } // if count
  
  $observers = $node->field_project_observers[LANGUAGE_NONE]; 
  foreach ($observers as $ndx => $observer) {
    $token = "";
    if (array_key_exists('field_project_observer_token', $observer) && array_key_exists(LANGUAGE_NONE, $observer['field_project_observer_token']) && count($observer['field_project_observer_token'][LANGUAGE_NONE])) {
      $token = $observer['field_project_observer_token'][LANGUAGE_NONE][0]['value'];
      if (!drupal_strlen($token)) 
        $node->field_project_observers[LANGUAGE_NONE][$ndx]['field_project_observer_token'][LANGUAGE_NONE][0]['value'] = uniqid();
    } // if count
    // dpm(get_defined_vars());
  } // foreach observer

  return;
    
    
   //drupal_set_message("project_nid: $project_nid");
   
  // TODO: check if form adv settings fields are empty. if so, fill with default survey form ids
  
  
} // _slnsite_project_360_presave
 
function _slnsite_project_submission_exists($project_nid, $surveyform_nid, $ratee, $rator) {
  $surveynode = node_load($surveyform_nid);
  
  if (!is_object($surveynode))
    return FALSE;
    
  // map form_keys to cid's
  $fkcids = array_column($surveynode->webform['components'], 'cid', 'form_key');
  
  // TODO: test that these indexes exist in surveys. they may not if sln standard fields are missing
  // turn this into a user friendly message about required fields missing in a specific survey
  if (!(array_key_exists('sln_project_id', $fkcids) && array_key_exists('sln_participant_id', $fkcids) && array_key_exists('sln_subject_id', $fkcids))) {
    //drupal_set_message("warning: missing sln metadata in submission in _slnsite_project_submission_exists");
    watchdog('slnsite', "SLN metadata not found in submission in _slnsite_project_submission_exists()", array(), WATCHDOG_WARNING);
    return FALSE;
  }
  
  // get keys for required/standard sln_survey hidden fields
  $cid_project_id = $fkcids['sln_project_id'];
  $cid_participant_id = $fkcids['sln_participant_id'];
  $cid_subject_id = $fkcids['sln_subject_id'];

  
  // TODO: check for existence - DONE
  //$submissions = webform_get_submissions(array('nid' => $surveyform_nid, 'uid' => $manager_uid));
  $submissions = webform_get_submissions($surveyform_nid);
  foreach ($submissions as $submission) {
    // $sub_project_id = $submission->data[$cid_project_id][0];
    $sub_project_id = _slnsite_submission_data('sln_project_id', $submission, $surveynode, -1);
    // $sub_participant_id = $submission->data[$cid_participant_id][0];
    $sub_participant_id = _slnsite_submission_data('sln_participant_id', $submission, $surveynode, -1);
    //$sub_subject_id = $submission->data[$cid_subject_id][0];
    $sub_subject_id = _slnsite_submission_data('sln_subject_id', $submission, $surveynode, -1);
    if (($sub_project_id == $project_nid) && ($sub_participant_id == $rator) && ($sub_subject_id == $ratee)) {
      //dpm(get_defined_vars());
      return TRUE;
    }
  } // foreach submission
  return FALSE;
} // _slnsite_project_submission_exists

/**
 *
 * create a submission, setting metadata values for sln_project_id, etc.
 *
 * checks for existence of submission with same metadata values
 *
 * returns TRUE for successful submission creation, FALSE if already exists or invalid survey (one w/o metadata webform components)
 *
 */
function _slnsite_project_submission_create($project_nid, $form_nid, $ratee, $rator) {

  if (_slnsite_project_submission_exists($project_nid, $form_nid, $ratee, $rator)) 
    return FALSE;
  
  //drupal_set_message("_slnsite_project_submission_create wants to create submission for ratee: $ratee, rator: $rator");
  //return;     // TODO: remove after testing !!!!!!!!!!!!!!!!
  
  $surveynode = node_load($form_nid);
  
  //dpm(get_defined_vars());
  
  if (!is_object($surveynode))      // null or bad form_id
    return FALSE;  
    
  // map form_keys to cid's
  $fkcids = array_column($surveynode->webform['components'], 'cid', 'form_key');
  
  // TODO: test that these indexes exist in surveys. they may not if sln standard fields are missing
  if (!(array_key_exists('sln_project_id', $fkcids) && array_key_exists('sln_participant_id', $fkcids) && array_key_exists('sln_subject_id', $fkcids))) {
    //drupal_set_message("warning: missing sln metadata in submission in _slnsite_project_submission_create");
    //watchdog('slnsite', "SLN metadata not found in submission in _slnsite_project_submission_create()", array(), WATCHDOG_WARNING);
   
    return FALSE;
  }
  
  // get keys for required/standard sln_survey hidden fields
  $cid_project_id = $fkcids['sln_project_id'];
  $cid_participant_id = $fkcids['sln_participant_id'];
  $cid_subject_id = $fkcids['sln_subject_id'];

  $form_state['values']['submitted'] = array(
    $cid_project_id => $project_nid,
    $cid_participant_id => $rator,
    $cid_subject_id => $ratee,
  );
  $submission = (object) array(
    'nid' => $form_nid,
    'uid' => 0,   // anon user for slnsite
    'submitted' => REQUEST_TIME,
    'remote_addr' => ip_address(),
    'is_draft' => TRUE,
    'preview' => FALSE,
    'serial' => _webform_submission_serial_next_value($form_nid, TRUE),
    'data' => webform_submission_data($surveynode, $form_state['values']['submitted']),
  );
  $sid = webform_submission_insert($surveynode, $submission);
  //drupal_set_message("Submission $sid created for project_nid: $project_nid, rator: $rator, ratee: $ratee for surveyid: $form_nid.");
  
  return $sid;
} // _slnsite_project_submission_create

/**
 *
 *  return project participant name, based on token. if name is blank, return email
 *
 * NOTE: currently designed for 360 projects
 */
function _slnsite_project_participant($project_or_id, $uniqid) { 
  $participant = $uniqid;
  $node = is_object($project_or_id) ? $project_or_id : (is_numeric($project_or_id) ? node_load($project_or_id) : NULL) ;
  if (is_object($node)) {
    //dpm($node);
    $subject = "";
    $supervisor = ""; 
    $observers = count($node->field_project_observers) ? $node->field_project_observers[LANGUAGE_NONE] : array(); 
    
    if (count($node->field_project_subject) && count($node->field_project_subject[LANGUAGE_NONE]))
      $subject = $node->field_project_subject[LANGUAGE_NONE][0]['field_project_subject_token'][LANGUAGE_NONE][0]['value'];
    if (drupal_strlen($subject)) {
      if ($subject == $uniqid) {
        $participant = $node->field_project_subject[LANGUAGE_NONE][0]['field_project_subject_name'][LANGUAGE_NONE][0]['value'];
        if (!drupal_strlen(trim($participant)))
          $participant = $node->field_project_subject[LANGUAGE_NONE][0]['field_project_subject_email'][LANGUAGE_NONE][0]['value'];
        return $participant;
      }
    } // if
    
    // todo: simplify tests using functions and apply to subject and observers too
    // note: applying extra tests here in response to missing supervisor in projects
    if (count($node->field_project_supervisor) && count($node->field_project_supervisor[LANGUAGE_NONE]))
      $supervisor = $node->field_project_supervisor[LANGUAGE_NONE][0]['field_project_supervisor_token'][LANGUAGE_NONE][0]['value'];
    if (drupal_strlen($supervisor)) {
      if ($supervisor == $uniqid) {
        if (count($node->field_project_supervisor[LANGUAGE_NONE][0]['field_project_supervisor_name']) && count($node->field_project_supervisor[LANGUAGE_NONE][0]['field_project_supervisor_name'][LANGUAGE_NONE]))
          $participant = $node->field_project_supervisor[LANGUAGE_NONE][0]['field_project_supervisor_name'][LANGUAGE_NONE][0]['value'];
        if (!drupal_strlen(trim($participant)) && count($node->field_project_supervisor[LANGUAGE_NONE][0]['field_project_supervisor_email']) && count($node->field_project_supervisor[LANGUAGE_NONE][0]['field_project_supervisor_email'][LANGUAGE_NONE]))
          $participant = $node->field_project_supervisor[LANGUAGE_NONE][0]['field_project_supervisor_email'][LANGUAGE_NONE][0]['value'];
        return $participant;
      }
    } // if
    
    foreach ($observers as $ndx => $observer) {
      $obstok = "";
      if (count($observer['field_project_observer_token']) && count($observer['field_project_observer_token'][LANGUAGE_NONE]))
        $obstok = $observer['field_project_observer_token'][LANGUAGE_NONE][0]['value'];
      if (drupal_strlen($obstok)) {
        if ($obstok  == $uniqid) {
          // note that observer name can be blank, same for email?
          $participant = count($node->field_project_observers[LANGUAGE_NONE][$ndx]['field_project_observer_name'][LANGUAGE_NONE]) ? $node->field_project_observers[LANGUAGE_NONE][$ndx]['field_project_observer_name'][LANGUAGE_NONE][0]['value'] : '';
          if (!drupal_strlen(trim($participant)))
            $participant = count($node->field_project_observers[LANGUAGE_NONE][$ndx]['field_project_observer_email'][LANGUAGE_NONE]) ? $node->field_project_observers[LANGUAGE_NONE][$ndx]['field_project_observer_email'][LANGUAGE_NONE][0]['value'] : '';
          return $participant;
        }
      }  // if
    } // foreach observer
  } // if    
  //dpm(get_defined_vars());
  //return "unknown uniquid: $uniqid for project $project_or_id";  
  return $participant;  
} // _slnsite_project_participant

/**
 * return array of all project participants, including name, email, token and type (Subject, Supervisor, Observer)
 *
 */
function _slnsite_project_participants($project_or_id) { 
  $node = is_object($project_or_id) ? $project_or_id : (is_numeric($project_or_id) ? node_load($project_or_id) : NULL) ;
  $arr = array();
  if (is_object($node)) {
    // TODO: check type?
    // todo: check for valid indexes
    if (count($node->field_project_subject) && count($node->field_project_subject[LANGUAGE_NONE])) {
      $subject = $node->field_project_subject[LANGUAGE_NONE][0];
      $name = $subject['field_project_subject_name'][LANGUAGE_NONE][0]['value'];
      $email = $subject['field_project_subject_email'][LANGUAGE_NONE][0]['value'];
      $token = $subject['field_project_subject_token'][LANGUAGE_NONE][0]['value'];    
      //$arr['subject'] =  array('name' => $name, 'email' => $email, 'token' => $token, 'type' => 'Subject');
      $arr[$email] =  array('name' => $name, 'email' => $email, 'token' => $token, 'type' => 'Subject', 'project' => $node->nid);
    } // if
    if (count($node->field_project_supervisor) && count($node->field_project_supervisor[LANGUAGE_NONE])) {
      $supervisor = $node->field_project_supervisor[LANGUAGE_NONE][0];
      // TODO: potential warning: index 0 undefined
      $name = $supervisor['field_project_supervisor_name'][LANGUAGE_NONE][0]['value'];
      $email = $supervisor['field_project_supervisor_email'][LANGUAGE_NONE][0]['value'];
      $token = $supervisor['field_project_supervisor_token'][LANGUAGE_NONE][0]['value'];    
      //$arr['supervisor'] =  array('name' => $name, 'email' => $email, 'token' => $token, 'type' => 'Supervisor');
      $arr[$email] =  array('name' => $name, 'email' => $email, 'token' => $token, 'type' => 'Supervisor', 'project' => $node->nid);
    } // if
    if (count($node->field_project_observers) && count($node->field_project_observers[LANGUAGE_NONE])) {
      $observers = $node->field_project_observers[LANGUAGE_NONE];
      $cnt = 1;
      foreach($observers as $ndx => $observer) {
        $name = $observer['field_project_observer_name'][LANGUAGE_NONE][0]['value'];
        $email = $observer['field_project_observer_email'][LANGUAGE_NONE][0]['value'];
        $token = $observer['field_project_observer_token'][LANGUAGE_NONE][0]['value'];
        //$arr["case_" . $cnt++] = array('name' => $name, 'email' => $email, 'token' => $token, 'type' => 'Observer');
        $arr[$email] = array('name' => $name, 'email' => $email, 'token' => $token, 'type' => 'Observer', 'project' => $node->nid);
      } // foreach 
    } // if
  } // if object
  //dpm(get_defined_vars());
  return $arr;
} // _slnsite_project_participants

function _slnsite_message_data($project, $recipients, $values) {

  // assume each recipient has a 'selected' boolean field
  // TODO: check that all fields exist
  $subject = $project->field_project_subject[LANGUAGE_NONE][0]['field_project_subject_name'][LANGUAGE_NONE][0]['value'];
  if (!drupal_strlen(trim($subject)))
    $subject =  $project->field_project_subject[LANGUAGE_NONE][0]['field_project_subject_token'][LANGUAGE_NONE][0]['value'];
  if (!drupal_strlen(trim($subject)))
    $subject =  "'unknown subject'";
  
  // TODO: allow for missing supervisor
  $supervisor = "";
  if (count($project->field_project_supervisor) && count($project->field_project_supervisor[LANGUAGE_NONE]))   
    $supervisor = $project->field_project_supervisor[LANGUAGE_NONE][0]['field_project_supervisor_name'][LANGUAGE_NONE][0]['value'];
  
  $researcheruid = $project->field_project_manager[LANGUAGE_NONE][0]['uid'];
  $researcher = user_load($researcheruid);
  $researchername = "unknown (id: " . check_plain(is_string($researcheruid) ? $researcheruid : "") . ")";
  if ($researcher && $researcher->uid) {
    $first = $researcher->profile_first_name ? $researcher->profile_first_name : "";
    $last = $researcher->profile_last_name ? $researcher->profile_last_name : "";
    $mail = $researcher->mail ? $researcher->mail : "";
    $name = $researcher->name ? $researcher->name : "";
    $researchername = $mail ? $mail : "";
    //$researchername = '"' . $name . '" &lt;' . $mail .'&gt;';
    //if ($first && $last)
      //$researchername = '"' . $first . " " . $last . ' "&lt;' . $mail .'&gt;';
  }
  else {
    $researchername = check_plain(is_string($researcheruid) ? $researcheruid : "");
  } // researcher

  $data = array();
  foreach($recipients as $ndx => $r) {
    $survey = 'http://' . $_SERVER['HTTP_HOST'] .  '/survey/' . $r['token'];
    $email = $r['email'];     // TODO: consider if blank
    $name = $r['name'];
    if (!(drupal_strlen(trim($name))))
      $name = "'name not specified'";
    $vars = array('%survey' => $survey, '%name' => $name, '%email' => $email, '%project' => $project->title, '%subject' => $subject, '%supervisor' => $supervisor, '%researcher' => $researchername);
    $mailparts = array();
    if (array_key_exists('selected', $r) && $r['selected']) {
      $mailparts['to'] = $r['email'];
      $mailparts['from'] = strtr($values['from'], $vars);
      //$mailparts['reply_to'] = strtr($values['reply_to'], $vars);
      $mailparts['subject'] = strtr($values['subject'], $vars);
      $type = strtolower($r['type']);
      // TODO: no value index possible?
      //dpm($values['message'][$type]);
      if (isset($values['message'][$type]['value']))
        $mailparts['message'] = strtr($values['message'][$type]['value'], $vars);
      else if (isset($values['message'][$type]) && is_string($values['message'][$type]))
        $mailparts['message'] = strtr($values['message'][$type], $vars);
      else
        $mailparts['message'] = "Error: unable to load message.";
      $mailparts['token'] = $r['token'];
      $data[$ndx] = $mailparts;

    } // if selected
  } // foreach

  //dpm(get_defined_vars());

  return $data;
} // _slnsite_message_data

function _slnsite_message_preview($project, $recipients, $values) {
  // assume each recipient has a 'selected' boolean field

  /*
  $subject = $project->field_project_subject[LANGUAGE_NONE][0]['field_project_subject_name'][LANGUAGE_NONE][0]['value'];
  $supervisor = $project->field_project_supervisor[LANGUAGE_NONE][0]['field_project_supervisor_name'][LANGUAGE_NONE][0]['value'];
  $reserceheruid = $project->field_project_manager[LANGUAGE_NONE][0]['uid'];
  $researcher = user_load($reserceheruid);
  $researchername = "unknown (id: " . check_plain($reserceheruid) . ")";
  if ($researcher && $researcher->uid) {
    $first = $researcher->profile_first_name;
    $last = $researcher->profile_last_name;
    $mail = $researcher->mail;
    $name = $researcher->name;
    $researchername = '"' . $name . '" &lt;' . $mail .'&gt;';
    if ($first && $last)
      $researchername = '"' . $first . " " . $last . ' "&lt;' . $mail .'&gt;';

    //dpm(get_defined_vars());
  }
  else {
    $researchername = check_plain($reserceheruid);
  }
  */
  $s = "";
  $data = _slnsite_message_data($project, $recipients, $values);  // todo: convert code here to use this 
  
  $link = "<a href='#messagingbookmark'>click here</a>";
  
  $s .= "<div class = 'sln-project-message-preview'>";
  $s .= "<p class = 'sln-instructions'>Scroll to the bottom of the page to access project messaging form or $link.</p>";
  $items = array();
  foreach($recipients as $ndx => $r) {
    //$survey = (array_key _exists($_SERVER, 'HTTPS')  ? 'https://' : 'http://') . $_SERVER['HTTP_HOST'] .  "/survey/" . $r['token'];
    //$survey = 'http://' . $_SERVER['HTTP_HOST'] .  '/survey/' . $r['token'];
    //$vars = array('%survey' => $survey, '%name' => $r['name'], '%email' => $r['email'], '%project' => $project->title, '%subject' => $subject, '%supervisor' => $supervisor, '%researcher' => $researchername);
    $mailparts = array();
    if (array_key_exists('selected', $r) && $r['selected']) {
      $mailparts[] = 'To: ' . $data[$ndx]['to'];
      $mailparts[] = 'From: ' . $data[$ndx]['from'];
      //$mailparts[] = 'Reply-To: ' . $data[$ndx]['reply_to'];
      $mailparts[] = 'Subject: ' . $data[$ndx]['subject'];
      $type = strtolower($r['type']);
      $mailparts[] = 'Message: ' . $data[$ndx]['message'];
      $items[] = theme_item_list(array(
        'items' => $mailparts,
        'title' => 'Message for ' . $r['name'],
        'type' => 'ul',
        'attributes' => array('class' => array('sln-project-message-preview-item')),
      ));
    } // if selected
  } // foreach
  $listvars = array(
    'items' => $items,
    'title' => 'Preview',
    'type' => 'ul',
    'attributes' => array('class' => 'sln-project-message-preview-items'),
  );  
  $s .= theme_item_list($listvars);
  $s .= "</div>";
  
  //dpm(get_defined_vars());
  
  return $s;
}

/**
 * Issue project completeness issues. Return TRUE if all info required to generate surveys is available
 *
 */
function _slnsite_project_status_report($project_or_id) {

  // TODO: better test for empty / blank values / also test for valid emails
  $s = "<ul>";
  $node = is_object($project_or_id) ? $project_or_id : (is_numeric($project_or_id) ? node_load($project_or_id) : NULL) ;
  if (is_object($node)) {
    // special handling for 360 projects
    //dpm($node);
    if (isset($node->field_project_type) && count($node->field_project_type) && array_key_exists(LANGUAGE_NONE, $node->field_project_type) && $node->field_project_type[LANGUAGE_NONE][0]['tid'] == SLN_PROJECT_TYPE_360) {
      if (!isset($node->field_project_subject) || !count($node->field_project_subject) || !count($node->field_project_subject[LANGUAGE_NONE])) {
        $s .= "<li>You must define a subject for the project.</li>";
      } 
      else {
        if (!drupal_strlen(trim($node->field_project_subject[LANGUAGE_NONE][0]['field_project_subject_name'][LANGUAGE_NONE][0]['value'])))
          $s .= "<li>Please enter a name for the subject.</li>";
        //if (!drupal_strlen($node->field_project_subject[LANGUAGE_NONE][0]['field_project_subject_email'][LANGUAGE_NONE][0]['value']))
        //  $s .= "<li>Please enter an e-mail address for the subject.</li>";
      }
      /*
      if (!count($node->field_project_supervisor) || !count($node->field_project_supervisor[LANGUAGE_NONE])) {
        $s .= "<li>You must define a supervisor for the project.</li>";
      } 
      else {
        if (!drupal_strlen(trim($node->field_project_supervisor[LANGUAGE_NONE][0]['field_project_supervisor_name'][LANGUAGE_NONE][0]['value'])))
          $s .= "<li>Please enter a name for the supervisor.</li>";
        if (!drupal_strlen($node->field_project_supervisor[LANGUAGE_NONE][0]['field_project_supervisor_email'][LANGUAGE_NONE][0]['value']))
          $s .= "<li>Please enter an e-mail address for the supervisor.</li>";
      }
      */
      if (!isset($node->field_project_observers) || !count($node->field_project_observers) || !count($node->field_project_observers[LANGUAGE_NONE])) {
        $s .= "<li>You must define some observers/participants for the project.</li>";
      } 
      else {
        $observers = $node->field_project_observers[LANGUAGE_NONE];
        $cnt = 0;
        foreach ($observers as $ndx => $observer) {    
          $cnt++;
          if (!drupal_strlen($observer['field_project_observer_email'][LANGUAGE_NONE][0]['value']) && !drupal_strlen(trim($observer['field_project_observer_name'][LANGUAGE_NONE][0]['value'])))
            $s .= "<li>Please enter at least one of a name or e-mail addresses for each observer or remove blank rows (see observer # $cnt).</li>";
          // if (!drupal_strlen(trim($observer['field_project_observer_name'][LANGUAGE_NONE][0]['value'])))
          //   $s .= "<li>Please enter a name for each observer or remove blank rows (see observer # $cnt).</li>";
        }  // foreach
      } // if observers
    } // if 360
  } // if node
  if ($s != "<ul>") {
    $s .= "</ul>";
    drupal_set_message($s);
    return FALSE;
  }
  return TRUE;
} // _slnsite_project_status_report

/**
 * return submission data, based on form_key name
 *
 * return $retval for non-existent form_key or cid indicies, also for object errors
 */
function _slnsite_submission_data($form_key, $submission, $survey, $retval = NULL) {
  if (is_numeric($survey))
    $survey = node_load($survey);
    
  if (!is_object($survey))
    return $retval;
    
  if (is_numeric($submission))
    $submission = webform_get_submission($survey->nid, $submission);
  
  if (!is_object($submission))
    return $retval;
    
  // todo: test that node has webform, test that $submission->nid = $survey->nid
  // todo: allow load with only submission but no survey. why bother?
  
  // map form_keys to cid's - is there a faster way to do this
  $fkcids = array_column($survey->webform['components'], 'cid', 'form_key');

  if (!array_key_exists($form_key, $fkcids)) 
    return $retval;
  
  $cid = $fkcids[$form_key];
  if (!array_key_exists($cid, $submission->data)) 
    return $retval;

  $value = $submission->data[$cid];
  if (is_array($value))
    $value = $value[0];
  
  //dpm(get_defined_vars());
  
  return $value;   
    
} // _slnsite_submission_data

/**
 * return array of project submissions (including plan!!!)
 *
 *
 */
function _slnsite_project_submissions($project_nid) {
  $data = array();
  
  // TODO: allow second param of nid's to ignore (e.g., plan)
  
  
  $sql = "SELECT wsd.nid, wsd.sid FROM webform_submitted_data wsd, webform_component wc WHERE wc.nid = wsd.nid AND wc.cid = wsd.cid AND wc.form_key = 'sln_project_id' AND wsd.data = :project_nid";
  
  $q = db_query($sql, array(':project_nid' => $project_nid));
  foreach ($q as $r) {
    $submission = webform_get_submission($r->nid, $r->sid);
    $data[$r->sid] = $submission;
  }
  
  //dpm(get_defined_vars());
  
  return $data;
} //_slnsite_project_submissions

/**
 *
 *  return project based on token.
 *
 */
function _slnsite_project_token($uniqid) { 
  $sql = <<<EOS
SELECT entity_id FROM sln.field_data_field_project_observers WHERE field_project_observers_field_project_observer_token_value = :uniqid
UNION ALL
SELECT entity_id FROM sln.field_data_field_project_subject WHERE field_project_subject_field_project_subject_token_value = :uniqid
UNION ALL
SELECT entity_id FROM sln.field_data_field_project_supervisor WHERE field_project_supervisor_field_project_supervisor_token_value = :uniqid
EOS;
  return db_query($sql, array(':uniqid' => $uniqid))->fetchField();
} // _slnsite_project_token

/**
 *
 *  return project based on token.
 *
 */
function _slnsite_submission_token($uniqid) { 
  $sql = "SELECT sid FROM webform_submitted_data d, webform_component wc WHERE d.nid = wc.nid AND d.cid = wc.cid AND wc.form_key = 'sln_participant_id' AND d.data = :uniqid";
  return db_query($sql, array(':uniqid' => $uniqid))->fetchField();
} // _slnsite_project_token

// return boolean indicating if projects are all complete or project completion field checked
function _slnsite_project_submissions_complete($project) {
  if (is_numeric($project))
    $project = node_load($project);
    
  if (!is_object($project))
    return FALSE;
  
  if (isset($project->field_project_is_complete) && _slnsite_array_keys_exist($project->field_project_is_complete, LANGUAGE_NONE, 0, 'value') && $project->field_project_is_complete[LANGUAGE_NONE][0]['value'])
    return TRUE;

  $planning_form = is_array($project->field_project_form_planning[LANGUAGE_NONE]) ? $project->field_project_form_planning[LANGUAGE_NONE][0]['nid'] : 0;
  
  $submissions = _slnsite_project_submissions($project->nid);
  foreach ($submissions as $submission) {
    //dpm(array($submission->is_draft, $submission));
    // avoid plan!
    if ($submission->nid == $planning_form)
      continue;
    if ($submission->is_draft)
      return FALSE;
  } // foreach
  return TRUE;
} // _slnsite_project_submissions_complete

// return last submission for a given project and survey
function _slnsite_project_survey_submission($project, $surveynodeid) {
  $sql = "select sid from webform_submitted_data inner join webform_component using (nid,cid) where data = :projectid and nid = :nid ORDER BY sid DESC";
  return db_query($sql, array(':projectid' => $project->nid, ':nid' => $surveynodeid))->fetchField();
} //_slnsite_project_survey_submission

function _slnsite_project_completed_submissions_count($project, $surveynodeid) {
  $submissions = webform_get_submissions($surveynodeid);
  $surveynode = node_load($surveynodeid);
  $cnt = 0;
  foreach ($submissions as $submission) {
    // loop if submission not for this project
    $sub_project_id = _slnsite_submission_data('sln_project_id', $submission, $surveynode, -1);
    if ($sub_project_id != $project->nid)
      continue;
    if ($submission->is_draft)
      continue;
    $cnt++;
  } // foreach
  return $cnt;
} //_slnsite_project_completed_submissions_count

/**
 *
 * return array of projects and tokens
 * array(token, project title, topic, body, from)
 */
function _slnsite_participant_projects($email) {

  $data = array();
  
  $sql = <<<EOS
SELECT entity_id, 'observer' AS survey FROM sln.field_data_field_project_observers WHERE field_project_observers_field_project_observer_email_value = :email
UNION
SELECT entity_id, 'subject' AS survey FROM sln.field_data_field_project_subject WHERE field_project_subject_field_project_subject_email_value = :email
UNION
SELECT entity_id, 'supervisor' AS survey FROM sln.field_data_field_project_supervisor WHERE field_project_supervisor_field_project_supervisor_email_value = :email
EOS;

  // TOOD: test for survey completeness
  
  $q = db_query($sql, array(':email' => $email));
  foreach ($q as $r) {
    $project = node_load($r->entity_id);
    // get project participants indexed by email
    $participants = _slnsite_project_participants($project);
    if (array_key_exists($email, $participants)) {
      $from = "";
      $message = "";
      $survey = $r->survey;
      $token = $participants[$email]['token'];
      if (drupal_strlen($token)) 
        $data[$r->entity_id] = array(
          'title' => $project->title, 
          'token' => $token, 
          'message' => $message, 
          'from' => $from, 
          'survey' => $survey,
        );
    } // array_key_exists
  } // foreach
  
  return $data;

} // _slnsite_participant_projects
  
// get survey nid from project based on name
function _slnsite_get_survey($projectnode, $id) {
  // todo: validate node is project ?

  switch ($id) {
    case 'observer':
      // check for field in project node with valid data (a node ref to a survey)
      if (count($projectnode->field_project_form_observer) && count($projectnode->field_project_form_observer[LANGUAGE_NONE])) {
        return $projectnode->field_project_form_observer[LANGUAGE_NONE][0]['nid'];
      }
      break;
    case 'supervisor':
      if (count($projectnode->field_project_form_supervisor) && count($projectnode->field_project_form_supervisor[LANGUAGE_NONE])) {
        return $projectnode->field_project_form_supervisor[LANGUAGE_NONE][0]['nid'];
      }     
      break;
    case 'self':      
    case 'subject':
      if (count($projectnode->field_project_form_self) && count($projectnode->field_project_form_self[LANGUAGE_NONE])) {
        return $projectnode->field_project_form_self[LANGUAGE_NONE][0]['nid'];
      }
      break;
    case 'planning':
      if (count($projectnode->field_project_form_planning) && count($projectnode->field_project_form_planning[LANGUAGE_NONE])) {
        return $projectnode->field_project_form_planning[LANGUAGE_NONE][0]['nid'];
      }
      break;
    
    default:
        // output error to watchdog?
  } // switch
  
  return 0;
  
  // legacy var name support 
  //return variable_get($id, 0);
  
} // _slnsite_get_survey


// generate a unique token
// see http://stackoverflow.com/questions/9543715/generating-human-readable-usable-short-but-unique-ids
function _sln_gen_token($prefix = "", $tokenLen = 7, $base = 0) {
  $baseChars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  $baseCharsLen = drupal_strlen($baseChars);
  $baseCharsLen = (($base > 0) && ($base < $baseCharsLen)) ? $base-1 : $baseCharsLen;
  if (!($tokenLen > 0))
    $tokenLen = 7;
  srand();
  do {
    $retval = "";
    for ($i = 0; $i < $tokenLen; $i++) {
      $p = rand(0, $baseCharsLen);
      $retval .= substr($baseChars, $p, 1);
    } // for
    
    // TODO: db check for dup - use view or union or just multiple queries?
    $onerow = db_select('field_data_field_project_observers', 'f')->fields('f', array('entity_id'))->condition('field_project_observers_field_project_observer_token_value', $retval)->execute()->fetchAssoc();
  } while ($onerow);      // TODO: limit number of iterations and return false or null?
  
  return $prefix . $retval;
} // _sln_gen_token

function _sln_average_col($data, $ndx) {
  if (!is_array($data))
    return NULL;
  $sum = 0;
  $cnt = 0;
  foreach ($data as $row) {
    if (array_key_exists($ndx, $row)) {
      $sum += $row[$ndx];
      $cnt++;
    } // if
  } // foreach
  if ($cnt)
    return sprintf('%01.2f', ($cnt ? round($sum/$cnt, 2) : ""));
  return 0;
} // _sln_average_col
  
function _slnsite_count_project_plan_responses($project) {

  $project_type = count($project->field_project_type) ? $project->field_project_type[LANGUAGE_NONE][0]['tid'] : 0;

  //$plansurveynodeid = is_array($project->field_project_form_planning[LANGUAGE_NONE]) ? $project->field_project_form_planning[LANGUAGE_NONE][0]['nid'] : 0;
  $plansurveynodeid = isset($project->field_project_form_planning[LANGUAGE_NONE]) && is_array($project->field_project_form_planning[LANGUAGE_NONE]) ? $project->field_project_form_planning[LANGUAGE_NONE][0]['nid'] : 0;
  
  $surveynode = node_load($plansurveynodeid);
  $submissionid = _slnsite_project_survey_submission($project, $plansurveynodeid);
  $submission = webform_get_submission($plansurveynodeid, $submissionid);
  
  //dpm(array($surveynode, $submission));
  
  return _slnsite_count_plan_submission_responses($surveynode, $submission);
} // _slnsite_count_project_plan_responses

// count project plan responses
function _slnsite_count_plan_submission_responses($surveynode, $submission) {
  $cntplans = 0;  // count action plans if self-assessment
  $data = array();
  if ($submission && isset($submission->data) && is_array($submission->data))
    $data = $submission->data;
  // foreach ($surveynode->webform['components'] as $cid => $wc) {
  //$cnt = 0;
  foreach($data as $cid => $value) {
    //$cnt++;
    //$form_key = $wc['form_key'];
    $form_key = $surveynode->webform['components'][$cid]['form_key'];
    //$val = is_array($data[$cid]) ? $data[$cid][0] : $data[$cid];
    $val = $value[0];
    
    //if (is_string($val) && strlen($val) && (substr($val,0,3) == 'aaa')) 
    //  dpm(array($form_key, $cid, "ta$cnt", $val));
    
    // ASSUMPTION: ta9999 are form keys for plan textarea's
    //if ($form_key == "ta$cnt")  {
    if (preg_match('~ta[0-9]+~', $form_key))  {
      if (is_string($val) && drupal_strlen(trim($val)))
        $cntplans++;
    } // self plan 
  }
  return $cntplans;
} // _slnsite_count_plan_submission_responses

function _slnsite_messages_save($form_state) {
  if (!(array_key_exists('values', $form_state) && array_key_exists('project_nid', $form_state['values'])))
    return;
  $projectid = $form_state['values']['project_nid'];
  $project = NULL;
  if ($projectid > 0)
    $project = node_load($projectid);
  if ($project) {
    $values = $form_state['values'];
    $project->field_messaging_subject_message = _slnsite_field_init($values['message']['subject']);
    $project->field_messaging_sup_message = _slnsite_field_init($values['message']['supervisor']);
    $project->field_messaging_obs_message = _slnsite_field_init($values['message']['observer']);
    $project->field_project_messaging_topic = _slnsite_field_init($values['subject']);
    $project->field_project_messaging_from = _slnsite_field_init($values['from']);
    node_save($project);
    drupal_set_message("Project messages updated.", 'info');
  } // project
} // _slnsite_messages_save

function _slnsite_field_init($value = "", $retval = NULL, $key = 'value') {  
  if (!is_string($key)) return $retval;
  return array(LANGUAGE_NONE => array(0 => array($key => $value)));
} // 

function _slnsite_field_init_var($varname = '', $retval = '', $key = 'value') {
  if (!is_string($varname) || !is_string($key)) return $retval;
  return _slnsite_field_init(variable_get($varname, $retval), $key, $retval);
} // _slnsite_field_init_var

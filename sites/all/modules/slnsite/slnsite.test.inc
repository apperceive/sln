<?php

/**
 * @file
 * Page callbacks for the testing features of slnsite module.
 */

module_load_include('inc', 'slnsite', 'slnsite.utils');
module_load_include('inc', 'slnsite', 'slnsite.webform');


// for quick testing of functions
/**
 * Implements simple test page.
 */
function _slnsite_msstestpage() {
 //return drupal_get_form('_slnsite_pchart_ajax_callback3');
   $errors = array();
  $surveynode = node_load(193);
  $components = $surveynode->webform['components'];
  $q = 1;
  foreach ($components as $cid => $c) {
   $form_key = $c['form_key'];
   
    if (strpos($form_key, "slq_") === 0) {
      //$cid = $c['cid'];
      //todo need cid/index of ta.$cid form_key, loop components for search or call function?
      $cidplan = _slnsite_survey_component_cid(193, 'cid_' . $q) - 2;  // slow db lookup
      $pc = $surveynode->webform['components'][$cidplan];
      //dpm(array($q, $form_key, $c['name'], $cid, $cidplan, $surveynode->webform['components'], $pc, $pc['name'] ));
      $extra = $c['extra'];
      $pextra = $pc['extra'];
      $translations_text = array_key_exists('translations', $extra) ? trim($extra['translations']) : ''; 
      $ptranslations_text = array_key_exists('translations', $pextra) ? trim($pextra['translations']) : '';       
      dpm(array($q, $form_key, $c['name'], $translations_text, $pc['name'], $ptranslations_text ));
      if (!drupal_strlen($ptranslations_text) && drupal_strlen($translations_text)) {
        $surveynode->webform['components'][$cidplan]['extra']['translations'] = $translations_text;
        drupal_set_message("question $q set");
      }
      if ($c['name'] != $pc['name'])
        $errors[] = "text for question $q and plan differ: " . $c['name'] . " VS " . $pc['name'];
      
      $q++;
   }

  }
  //node_save($surveynode);
  $s = '';
  foreach ($errors as $e)
    $s .= $e . "<br/>";
  return $s . "done";
} // _slnsite_msstestpage
 
 
function _slnsite_msstestpage14() {
  header( 'Content-type: text/html; charset=utf-8' );
  //if (ob_get_level() == 0) 
    //ob_start();

  $s = "";
  $arr = array();
  $max = 1000000;
  $maxp = 1;
  for ($n = 1; $n <= $max; $n++) {
    $p = mss_get_persistence($n);
    if ($p > $maxp) {
      $maxp = $p;
      //$s .= "$maxp min at $n<br/>\n";
      echo "$maxp min at $n<br/>\n";
      /*ob_end_flush(); 
      ob_flush(); 
      flush(); 
      ob_start(); */
    }
  } // for

  //dpm(get_defined_vars());

  $s .= "MSS test page complete.";
  //return $s;
}  // _slnsite_msstestpage13

// just math fun
function mss_get_persistence($n) {
  set_time_limit(30);
  if ($n < 10) 
    return 0;
  $l = strlen($n);
  $p = 1;
  for ($i = 0; $i < $l; $i++) {
    $p *= substr($n, $i, 1);
  } // for
  return 1 + mss_get_persistence($p);
} // mss_get_persistence


function _slnsite_msstestpage13() {
  $s = "";

  $form = drupal_get_form('_slnsite_pchart_test');
  $s .= drupal_render($form);

  //dpm(get_defined_vars());

  //$s .= "MSS test page complete.";
  return $s;
}  // _slnsite_msstestpage13

function _slnsite_pchart_test($form, &$form_state) {
  $form = array();
   // Item for return schedule.
   $form['replace_textfield'] = array(
     '#markup' => '<div id="pchart_ajax">'. _slnsite_pchart_ajax_callback3($form = array(), $form_state = array())  .'</div>',
   );
   /*
   $form['first_values'] = array(
     '#type' => 'textfield',
     '#title' => t('Enter values ​​for first schedule'),
     '#size' => 60,
     '#maxlength' => 128,
     '#required' => TRUE,
     '#description' => t('For fractional numbers use sign ".". Use the sign ";" to separate digits.'),
     '#value' => '1;2;3;3;4;5;1.5',
   );
   $form['second_values'] = array(
     '#type' => 'textfield',
     '#title' => t('Enter values ​​for second schedule'),
     '#required' => TRUE,
     '#description' => t('For fractional numbers use sign ".". Use the sign ";" to separate digits.'),
     '#value' => '5;4;3;3;1.5;2.5;5',
   );
   $form['confirm'] = array(
     '#type' => 'submit',
     '#value' => 'show image',
     '#ajax' => array(
       'callback' => '_slnsite_pchart_ajax_callback3',
       'wrapper' => 'pchart_ajax',
       'effect' => 'fade',
     ),
   );
   */
   return $form;
} // _slnsite_pchart_test


/**
  * Construction schedule to the entered value.
  */
function _slnsite_pchart_ajax_callback($form, &$form_state) {

  //return "<h1>HEY!</h1>";

  // Create an array of values.
  $first_values  = explode(";", $form_state['input']['first_values']);
  $second_values = explode(";", $form_state['input']['second_values']);

  // Include required files with pChart library.
  $name = "pChart";
  //if ($path = libraries_get_path($name)) {
  $path = '/var/www/sln.thestrategicleader.org/public/sites/all/libraries/pchart';
   include("{$path}/class/pData.class.php");
   include("{$path}/class/pDraw.class.php");
   include("{$path}/class/pImage.class.php");
  //}

  // Create and populate the pData object.
  $myData = new pData();

  // Build your data series.
  $myData->addPoints($first_values, 'first');
  $myData->addPoints($second_values, 'second');

  // Draw series with specified weight.
  $myData->setSerieWeight("first", 2);
  $myData->setSerieWeight("second", 2);

  // Create a pChart object.
  $myPicture = new pImage(700, 210, $myData);

  // Rectangular gradient area.
  $myPicture->drawGradientArea(0, 0, 700, 230, DIRECTION_VERTICAL, array("StartR" => 075, "StartG" => 0, "StartB" => 0, "EndR" => 50, "EndG" => 50, "EndB" => 50, "Alpha" => 80));

  // Font and background of the schedule.
  $myPicture->setFontProperties(array("FontName" => "{$path}/fonts/Forgotte.ttf", "FontSize" => 11));
  $myPicture->setGraphArea(20, 10, 700, 190);

  // Draw scale, spline chart and legend of the schedule.
  $myPicture->drawScale();
  $myPicture->drawSplineChart();
  $myPicture->drawLegend(600, 12, array("Style" => LEGEND_NOBORDER, "Mode" => LEGEND_HORIZONTAL));

  // Creates a full file path from a directory and filename for render and view picture.
  $output = file_create_filename('example.png', 'sites/default/files/pictures');
  $myPicture->Render($output);
  
  return '<div id="pchart_ajax"><img src="' . file_create_url($output) . '" /></div>';
} // _slnsite_pchart_ajax_callback

/** 
  * Construction schedule to the entered value. 
  */ 
function _slnsite_pchart_ajax_callback2($form, &$form_state) { 
  $first_values = explode(";", $form_state['input']['first_values']); 
  $second_values = explode(";", $form_state['input']['second_values']); 
  $name = "pChart"; 
  //if ($path = libraries_get_path($name)) {
  $path = '/var/www/sln.thestrategicleader.org/public/sites/all/libraries/pchart';
   include("{$path}/class/pData.class.php");
   include("{$path}/class/pDraw.class.php");
   include("{$path}/class/pImage.class.php");
  //}

  // Create and populate the pData object. 
  $myData = new pData(); 
  $myData->addPoints($first_values, "First value"); 
  $myData->addPoints($second_values, "Second value"); 
  $myData->addPoints(array('Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday', 'Sunday'), "Day"); 

  // Add a description to one serie. 
  $myData->setSerieDescription("Days", "Day"); 

  // Define the abscissa axis labels serie. 
  $myData->setAbscissa("Day"); 
  $myPicture = new pImage(700, 230, $myData); 
  $myPicture->setFontProperties(array("FontName" => "{$path}/fonts/Forgotte.ttf", "FontSize" => 11)); 
  $myPicture->setGraphArea(50, 30, 680, 200);  $myPicture->drawGradientArea(0, 0, 700, 230, DIRECTION_VERTICAL, array("StartR" => 075, "StartG" => 0, "StartB" => 0, "EndR" => 50, "EndG" => 50, "EndB" => 50, "Alpha" => 80)); 
  $myPicture->drawScale(array("CycleBackground" => TRUE, "DrawSubTicks" => TRUE, "GridR" => 185, "GridG" => 240, "GridB" => 120, "GridAlpha" => 10)); 
  $settings = array("Gradient" => TRUE, "GradientMode" => GRADIENT_EFFECT_CAN); 

  // Draw a bar chart. 
  $myPicture->drawBarChart($settings); 
  $myPicture->drawLegend(540, 12, array("Style" => LEGEND_NOBORDER, "Mode" => LEGEND_HORIZONTAL)); 
  $output = file_create_filename('example.png', 'sites/default/files/pictures'); 
  $myPicture->Render($output); 
  return '<div id="pchart_ajax"><img src="' . file_create_url($output) . '" /></div>'; 
} // _slnsite_pchart_ajax_callback2


/** 
  * Construction schedule to the entered value. 
  */ 
function _slnsite_pchart_ajax_callback3($form, &$form_state) { 
  $projectid = 145;
  
  $data = _slnsite_project_submissions_summarize($projectid);
  $d = array();
  foreach ($data as $nid => $survey) {
    $t = array();
    foreach ($survey['data'] as $key => $arr) {
      $t[] = round($arr['average'], 2);    
    } // survey
    $d[] = $t;
  }  // foreach data as survey
  
  $labels = array("Transforming", "Bonding", "Managing", "Bartering", "Bridging");
  
  return _slnsite_project_results_radar_chart($projectid, $labels, $d);
} // _slnsite_pchart_ajax_callback3





function _slnsite_msstestpage_12() {
  $s = "";

  $projectid = 1;  // sln_project
  $data = _slnsite_project_report_process_submissions($projectid);

  $d = array();
  foreach ($data as $nid => $survey) {
    foreach ($survey['data'] as $key => $arr) {
     $d[$key][0] = $key;
     $d[$key][$nid] = round($arr['average'], 2);
    }
  }

  $headers = array('Skill', 'Peers', 'Self', 'Boss');
  $s .= _slnsite_display_table($headers, $d, "sln-360-report");


  $projectid2 = 145;     // sln_project_360
  $data2 = _slnsite_project_submissions_listing($projectid2);

  $projectid3 = 146;     // sln_project_180
  $data3 = _slnsite_project_submissions_listing($projectid3);

  dpm(get_defined_vars());

  $s .= "MSS test page complete.";
  return $s;
}  // _slnsite_msstestpage_12

function _slnsite_msstestpage_11() {
  $s = "";
  $s .= "MSS test page complete.";

  // get the projects that i particpate in
  // for each project, get any self surveys, are any submitted and complete
  // for each project, get the subjects I am assigned, are any submitted and complete

  global $user;
  $acct = $user;

  $data = array();

  // get all projects that user is participating in
  $projects = array();
  $sql = "SELECT np.nid, np.vid, np.title FROM {content_field_project_participants} cfpp, {node} np WHERE np.nid = cfpp.nid AND np.vid = cfpp.vid AND cfpp.field_project_participants_uid = :uid";
  $q = db_query($sql, array(':uid' => $acct->uid));

  $surveys = array();

  $submissions = array();

  // for each project that has acct as participant
  foreach ($q as $r) {

    $projectnode = node_load($r->nid);

    $row = array(
      'projectid' => $r->nid,
      'projectname' => $r->title,
      'project' => $projectnode,
      'surveys' => array(),
    );

    // for each project survey
    // foreach ($projectnode->field_project_survey_forms[LANGUAGE_NONE] as $ndx => $survey) {
    foreach ($projectnode->field_project_surveys[LANGUAGE_NONE] as $ndx => $survey) {

      $surveynode = node_load($survey['nid']);
      $surveys[$ndx] = $surveynode;

      // does this user have any submissions?
      // note: having a submission means you are either the author (uid) of the submission or the $acct uid is the same as the data for form_key "participant_id"
      $submissions = webform_get_submissions($survey['nid']);

      $row['surveys'][] = array(
        'node' => $surveynode,
        'submissions' => $submissions,
      );

      foreach ($submissions as $submission) {
        $drow = array(
          $r->nid,
          $projectnode->title,
          $survey['nid'],
          $surveynode->title,
          $submission->sid,
          $submission->is_draft,
          // $submission->data['999'][0]
        );
        $data[] = $drow;
      } // foreach submission

    } // foreach survey

    $projects[$r->nid] = $row;
  } // foreach


  $headers = array('projectid', 'project name', 'surveyid', 'survey name', 'submission id', 'is_draft');
  $s .= _slnsite_display_table($headers, $data, "sln-submissions-table");

  dpm(get_defined_vars());
  return $s;
} // _slnsite_msstestpage_11

function _slnsite_msstestpage_10() {
  $project = 1;

  $header = array(
    'sid' => array('data' => t('SID'), 'field' => 'ws.sid'),
    'nid' => array('data' => t('NID'), 'field' => 'ws.nid'),
    'uid' => array('data' => t('UID'), 'field' => 'ws.uid'),
    'is_draft' => array('data' => t('Status'), 'field' => 'ws.is_draft'),
    'submitted' => array('data' => t('Submitted'), 'field' => 'ws.submitted', 'sort' => 'desc'),
  );

  $query = db_select('webform_submissions', 'ws');
  //$query->condition('u.uid', 0, '<>');
  //user_build_filter_query($query);

  $count_query = clone $query;
  $count_query->addExpression('COUNT(ws.sid)');

  $query = $query->extend('PagerDefault')->extend('TableSort');
  $query
    ->fields('ws', array('sid', 'nid', 'uid', 'is_draft', 'submitted'))
    ->limit(20)
    ->orderByHeader($header)
    ->setCountQuery($count_query);
  $result = $query->execute();

  /*
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Update options'),
    '#attributes' => array('class' => array('container-inline')),
  );
  $options = array();
  foreach (module_invoke_all('user_operations') as $operation => $array) {
    $options[$operation] = $array['label'];
  }


  $form['options']['operation'] = array(
    '#type' => 'select',
    '#title' => t('Operation'),
    '#title_display' => 'invisible',
    '#options' => $options,
    '#default_value' => 'unblock',
  );
  $options = array();
  $form['options']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );

  $destination = drupal_get_destination();

  $status = array(t('blocked'), t('active'));
  $roles = array_map('check_plain', user_roles(TRUE));

  */
  $rows = array();
  foreach ($result as $row) {
    //$users_roles = array();
    //$roles_result = db_query('SELECT rid FROM {users_roles} WHERE uid = :uid', array(':uid' => $account->uid));
    //foreach ($roles_result as $user_role) {
    //  $users_roles[] = $roles[$user_role->rid];
    //}
    //asort($users_roles);

    $rows[$row->sid] = array(
      'sid' => $row->sid,
      'nid' => $row->nid,
      'uid' => $row->uid,
      'is_draft' => $row->is_draft ? 'Complete' : '',
      'submitted' => format_interval(REQUEST_TIME - $row->submitted),
    );
  }

  $form['submissions'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $rows,
    '#empty' => t('No submissions available.'),
  );
  $form['pager'] = array('#markup' => theme('pager'));

  // return $form;
  return drupal_get_form($form);    // this line untested after move to msstest funtion

} // _slnsite_msstestpage_10


function _slnsite_msstestpage_9() {

  $submissionid = 45;
  $surveynodeid = 117;


  $surveynode = node_load($surveynodeid);
  $rows = _slnsite_submission_plans($surveynodeid, $submissionid);
  $header = array('Skill', 'data' => array('data' => 'Actions You Will Take to Develop Develop Your Strategic Leader Skills', 'class' => array('question-action-plan')), 'Term of Action');
  $class = 'sln-submission-plans-table';

  $colgroups = array(
    array(
      array(
        'class' => array('skill'), // Attribute for the COL element.
      ),
    ),
    array(
      array(
        'class' => array('action'), // Attribute for the COL element.
      ),
    ),
    array(
      array(
        'class' => array('term'), // Attribute for the COL element.
      ),
    ),
  );

  $s .= _slnsite_display_table($header, $rows, $class, $colgroups);


  $s .= "MSS test page complete.";

  //dpm(get_defined_vars());
  return $s;

} // _slnsite_msstestpage_9

function _slnsite_msstestpage_8() {
  $s = "";

  $submissionid = 46;
  $surveynodeid = 117;

  $submission = webform_get_submission($surveynodeid, $submissionid);
  //$surveynode = node_load($surveynodeid);

  $position = _slnsite_submission_position($surveynodeid, $submissionid);
  $s .= _slnsite_display_submitter_info_table($submission->uid, $position);

  $s .= "MSS test page complete.";

  //dpm(get_defined_vars());
  return $s;

} // _slnsite_msstestpage_8

function _slnsite_msstestpage_7() {
  $instrument = SLN_INSTRUMENT_SLQ;
  $skills = _slnsite_skills_definitions($instrument, $all = FALSE);

  $data = _slnsite_submission_actions(117, 46);  // completed self
  //$data = _slnsite_submission_actions(117, 45);  // all 5's draft self
  //$data = _slnsite_submission_actions(114, 57);  // sup draft
  //$data = _slnsite_submission_actions(104, 35);  // researcher de completed
  //$data = _slnsite_submission_actions(116, 56);  // completed obs

  foreach ($skills as $skill) {
    $skillname = $skill[0];
    $s .= _slnsite_display_action_map_table($skillname, $data);
  }  // foreach

} // _slnsite_msstestpage_7

function _slnsite_msstestpage_6() {
  $s = "";

  $rows = sln_get_skills_definitions(SLN_INSTRUMENT_SLQ);
  $s .= _sln_display_table(array('Strategic Leader Actions', 'Description'), $rows, "sln-skills-table");

  $rows = sln_get_skills_definitions(SLN_INSTRUMENT_STQ);
  $s .= _sln_display_table(array('Strategic Leader Actions', 'Description'), $rows, "sln-skills-table");

  // TODO temp data - later get from submission
  $userscores = array(100, 200, 300, 400, 500, 350);
  $benchmarktypes = sln_get_vocabulary_term_names(SLN_BENCHMARK_TYPES);
  $headers = array_merge(array("Strategic Leader Actions", "Your Score"), $benchmarktypes);

  $data1 = _slnsite_comparison_data(SLN_INSTRUMENT_SLQ, $userscores);
  $s .= _sln_display_table($headers, $data1, "sln-compare-table");

  $data2 = _slnsite_comparison_data(SLN_INSTRUMENT_STQ, $userscores);
  $s .= _sln_display_table($headers, $data2, "sln-compare-table");

  $s .= "MSS test page complete.";

  dpm(get_defined_vars());
  return $s;
} // _slnsite_msstestpage_6

function _slnsite_msstestpage_5() {
  //module_load_include('inc', 'slnsite', 'slnsite.webform');
  //module_load_include('inc', 'slnsite', 'slnsite.utils');

  $s = "";
  // test calls to
  // _slnsite_check_valid_id($id, $type = 'survey', $tablename = 'node', $idfieldname = 'nid')

  $validsurveynodeid = 117;
  $invalidsurveynodeid = 999117;
  $validpageid = 122;
  $invalidpageid = 122999;
  $validuserid = 3;
  $invaliduserid = -1;
  $nonexistantuserid = 123999;
  $validsubmissionid = 45;
  $invalidsubmissionid = 12345;
  $invalidnodeid = 99999;
  $validtid = 24;
  $invalidtid = 999924;

  $results = array(
    _slnsite_check_valid_id($validsurveynodeid),  // #0 true - ok
    _slnsite_check_valid_id("$validsurveynodeid"), // true - ok
    _slnsite_check_valid_id($invalidsurveynodeid),  // false - ok
    _slnsite_check_valid_id($invalidsurveynodeid, 'survey'),  // false - ok
    _slnsite_check_valid_id($invalidsurveynodeid, 'survey', 'node'),   // false - ok
    _slnsite_check_valid_id($invalidsurveynodeid, 'survey', 'node', 'tid'), // TRUE - OK
    _slnsite_check_valid_id($validsurveynodeid, 'xxx'), // bad type  #6 - true because node & nid assumed
    _slnsite_check_valid_id($invalidsurveynodeid, 'xxx'), // bad type and id -- false - ok

    _slnsite_check_valid_id($validsubmissionid, 'submission'), // TRUE - ok
    _slnsite_check_valid_id($validsubmissionid, 'submission', 'frank zappa'), // FALSE - OK
    _slnsite_check_valid_id($validsubmissionid, 'survey'), // #10  FALSE - OK
    _slnsite_check_valid_id($validsubmissionid, 'survey', 'node'), // false - ok
    _slnsite_check_valid_id($validsubmissionid, 'survey', 'xxxnode'),  // false - ok
    _slnsite_check_valid_id($validsubmissionid, 'survey', 'node', 'nid'), //false - ok
    _slnsite_check_valid_id($invalidsubmissionid, 'submission'),   // false - ok

    _slnsite_check_valid_id($validuserid, 'user'), // valid user #15 - true - ok
    _slnsite_check_valid_id($invaliduserid, 'user'), // invalid user  - false - ok
    _slnsite_check_valid_id($nonexistantuserid, 'user'),  // non-existent user id - false - ok

    _slnsite_check_valid_id($validuserid, 'user', 'node'), // wrong table name #18 - TRUE OK tblname ignored
    _slnsite_check_valid_id($validuserid, 'user', 'xxxnode'), // non-existent table name - TRUE - OK
    _slnsite_check_valid_id($validuserid, 'user', 'user', 'tid'), // wrong field name - TRUE - OK
    _slnsite_check_valid_id($validuserid, 'user', 'userxxx', 'tid'), // wrong field name - TRUE - OK

    _slnsite_check_valid_id($validsurveynodeid, '', '', ''), // valid survey id all blanks #22 - true ok

    _slnsite_check_valid_id($validuserid, 'user', 'user', 'nnnnnnid'),  // true - ok
    _slnsite_check_valid_id($validuserid, 'user', 'node', 'nnnnnnid'),  // true - ok

    _slnsite_check_valid_id($validpageid, 'page', 'node'), // #25  true - ok
    _slnsite_check_valid_id($validpageid, 'node', 'page'), // type and node reversed - true - ok
    _slnsite_check_valid_id($validpageid, 'page'),    // true - ok
    _slnsite_check_valid_id($validpageid, 'node'),  //  true - ok
    _slnsite_check_valid_id($invalidpageid, 'page'), // 29 - FALSE - OK
    _slnsite_check_valid_id($validpageid, 'page', 'node', 'nnnnnnid'), // - FALSE - ok
    _slnsite_check_valid_id($validpageid, 'page', 'node', 'nid'), // - FALSE - wrong node type
    _slnsite_check_valid_id($validpageid, 'page', 'page', 'nid'), // 32 - FALSE - OK

    _slnsite_check_valid_id($validpageid, 'IGNORETYPE'), // #33 - TRUE - OK

    _slnsite_check_valid_id($validtid, 'IGNORETYPE', 'taxonomy_term_data'), // #34 FALSE - OK
    _slnsite_check_valid_id($validtid, 'IGNORETYPE', 'taxonomy_term_data', 'tid'), // #35 TRUE - OK
    _slnsite_check_valid_id($invalidtid, 'IGNORETYPE', 'taxonomy_term_data'), // false - ok
    _slnsite_check_valid_id($invalidtid, 'IGNORETYPE', 'taxonomy_term_data', 'xxxxtid'), // false ok

    _slnsite_check_valid_id($validtid, 'taxonomy', 'taxonomy_term_data', 'tid'),// #38 TRUE - OK
    _slnsite_check_valid_id($invalidtid, 'taxonomy', 'taxonomy_term_data', 'tid'), // #39 FALSE - OK

    _slnsite_check_valid_id($invalidsurveynodeid, '', '', ''), // valid survey id all blanks #22 - false ok

    ); // array

  $s .= "MSS test page complete.";

  dpm(get_defined_vars());
  return $s;
} // _slnsite_msstestpage_5
/**
 * Implements XXX.
 */
function _slnsite_msstestpage_4() {
  $testarr = array('one' => array('one_one' => array('one_one_one' => '1.1.1'), 'one_two' => NULL), 'two' => '2', 'three' => array('three_one' => '3.1', 'three_two' => array('three_two_one' => '3.2.1')));

  dpm(array(
    $testarr,
    _slnsite_array_keys_exist($testarr),
    _slnsite_array_keys_exist($testarr, 'one'),
    _slnsite_array_keys_exist($testarr, 'three', 'three_two', 'three_two_one'),
    _slnsite_array_keys_exist($testarr, 'three', 'three_two', 'three_two_one', 'xxx'),
    _slnsite_array_keys_exist($testarr, 'three', 'three_two', 'three_two_one', 'xxx', 'yyy'),
    _slnsite_array_keys_exist($testarr, 'three', 'three_one', 'three_two_one'),
    _slnsite_array_keys_exist($testarr, 'four'),
    _slnsite_array_keys_exist($testarr, 'one', 'one_one_one', 'one_one'),
    _slnsite_array_keys_exist($testarr, 'one', 'one_one', 'one_one_one'),
    _slnsite_array_keys_exist($testarr, 'one', 'one_one_one'),
  ));
} // _slnsite_msstestpage_4

/**
 * Implements XXX.
 */
function _slnsite_msstestpage_3() {
  $opt1 = _slnsite_select_options_from_node(11);
  $opt2 = _slnsite_select_options_from_node(20);

  $termopt1 = _slnsite_select_options_from_term(78);
  $termopt2 = _slnsite_select_options_from_term(79);

  $voc1 = _slnsite_select_options_from_vocabulary('sln_skills');
  $voc2 = array_merge(array(0 => t("None")), _slnsite_select_options_from_vocabulary('sln_skills'));
  $voc3 = _slnsite_select_options_from_vocabulary(SLN_SKILLS_VOCABULARY, array(0 => 'none'));

} // _slnsite_msstestpage_3

/**
 * Implements XXX.
 */
function _slnsite_msstestpage_2() {
  $surveynode = node_load(30);  // for testing
  $plannode = _slnsite_create_planning_webform($surveynode);
  dpm(get_defined_vars());
} // _slnsite_msstestpage_2

/**
 * Implements XXX.
 */
function _slnsite_msstestpage_1() {
  $include = array('a', 'b', 'c');
  $exclude = array('xxx');
  $type = 'sln_planning_webform';
  $nodeid = 100;
  $pi = db_placeholders($include, "text");
  $pe = db_placeholders($exclude, "text");
  $sql = "SELECT wc.form_key, wc.cid, wc.pid, wc.type, wc.weight FROM {webform_component} wc, {node} n WHERE wc.nid = n.nid AND n.type = '%s' AND wc.type IN (" . $pi . ") AND NOT wc.type IN (" . $pe . ") AND n.nid = %d ORDER BY weight";
  $arr = array_merge(array($type), $include, $exclude, array($nodeid));
  $t1 = _slnsite_survey_question_count(23);
  $t2 = _slnsite_survey_questions_enumeration(23);
  $t3 = _slnsite_survey_questions_enumeration(24);
  $t4 = _slnsite_survey_demographic_enumeration(6);
  $t5 = _slnsite_survey_planning_enumeration(27);
  dpm(get_defined_vars());
} // _sqlsite_msstest1




////////  TEST AND DEBUG UTILITIES

// TODO: for testing. eventually remove function and call in nodeapi
/**
 * Implements XXX.
 */
function _slnsite_extra_content($projectnode) {
  global $user;
  $s = "";
  $s .= "<h3>You are:</h3>";
  $s .= "<ul>";
  $s .= "<li><b>$user->mail</b>";
  $s .= "<ul>";
  foreach ($user->roles as $role)
    $s .= "<li>$role</li>";
  /*
  if ($user->uid == 1)
    $s .= "<li>admin</li>";
  */
  if (_slnsite_user_is_project_participant($projectnode))
    $s .= "<li>You are a participant of this project</li>";
  else
    $s .= "<li>You are NOT a participant of this project</li>";
  $s .= "</ul>";
  $s .= "</li>";
  $s .= "</ul>";
  $s .= "<h3>Page content: </h3>";
  return $s;
} // _slnsite_extra_content

// temp fix for entity token + reference issues
// https://www.drupal.org/node/1058912#comment-4801902
// see also token_tweaks module
// also https://www.drupal.org/node/1334456
// also https://www.drupal.org/node/1058912#comment-4878408
// also https://www.drupal.org/node/1058912
// also https://www.drupal.org/node/1115324
// also (but this is about the nginx error resulting from token issues):
// http://serverfault.com/questions/587386/an-upstream-response-is-buffered-to-a-temporary-file
// http://mailman.nginx.org/pipermail/nginx/2012-March/032608.html
/**
function slnsite_preprocess_token_tree(&$variables) {
  if (array_intersect(array('node', 'user'), $variables['token_types'])) {
    $variables['recursion_limit'] = 2;
  }
}
 */


/**
 * Implements XXX.
 */
function _slnsite_process_error_backtrace($errno, $errstr, $errfile, $errline, $errcontext) {
    if (!(error_reporting() & $errno))
        return;
    switch ($errno) {
    case E_WARNING      :
    case E_USER_WARNING :
    case E_STRICT       :
    case E_NOTICE       :
    case E_USER_NOTICE  :
        $type = 'warning';
        $fatal = FALSE;
        break;
    default             :
        $type = 'fatal error';
        $fatal = TRUE;
        break;
    }
    $trace = array_reverse(debug_backtrace());
    array_pop($trace);
    echo "<small><pre>";
    if (php_sapi_name() == 'cli') {
        echo 'Backtrace from ' . $type . ' \'' . $errstr . '\' at ' . $errfile . ' ' . $errline . ':' . "\n";
        foreach ($trace as $item)
            echo '  ' . (isset($item['file']) ? $item['file'] : '<unknown file>') . ' ' . (isset($item['line']) ? $item['line'] : '<unknown line>') . ' calling ' . $item['function'] . '()' . "\n";
    }
    else {
        echo '<p class="error_backtrace">' . "\n";
        echo '  Backtrace from ' . $type . ' \'' . $errstr . '\' at ' . $errfile . ' ' . $errline . ':' . "\n";
        echo '  <ol>' . "\n";
        foreach ($trace as $item)
            echo '    <li>' . (isset($item['file']) ? $item['file'] : '<unknown file>') . ' ' . (isset($item['line']) ? $item['line'] : '<unknown line>') . ' calling ' . $item['function'] . '()</li>' . "\n";
        echo '  </ol>' . "\n";
        echo '</p>' . "\n";
    }
    echo "</pre></small>";

    /*
    if(ini_get('log_errors')) {
        $items = array();
        foreach($trace as $item)
            $items[] = (isset($item['file']) ? $item['file'] : '<unknown file>') . ' ' . (isset($item['line']) ? $item['line'] : '<unknown line>') . ' calling ' . $item['function'] . '()';
        $message = 'Backtrace from ' . $type . ' \'' . $errstr . '\' at ' . $errfile . ' ' . $errline . ': ' . join(' | ', $items);
        error_log($message);
    }
    if($fatal)
        exit(1);
        */
} // process_error_backtrace

// uncomment to turn on debugging trace. keep off for production
//set_error_handler('_slnsite_process_error_backtrace');


/**

// alternatives for debugging

$e = new Exception;
print "<pre>" . $e->getTraceAsString() . "</pre>";

// or

try {
    test();
} catch(Exception $e) {
    echo $e->getTraceAsString();
}

 */

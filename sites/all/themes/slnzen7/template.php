<?php
/**
 * @file
 * Contains the theme's functions to manipulate Drupal's default markup.
 *
 * Complete documentation for this file is available online.
 * @see https://drupal.org/node/1728096
 */


/**
 * Override or insert variables into the maintenance page template.
 *
 * @param $variables
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("maintenance_page" in this case.)
 */
/* -- Delete this line if you want to use this function
function slnzen7_preprocess_maintenance_page(&$variables, $hook) {
  // When a variable is manipulated or added in preprocess_html or
  // preprocess_page, that same work is probably needed for the maintenance page
  // as well, so we can just re-use those functions to do that work here.
  slnzen7_preprocess_html($variables, $hook);
  slnzen7_preprocess_page($variables, $hook);
}
// */

/**
 * Override or insert variables into the html templates.
 *
 * @param $variables
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("html" in this case.)
 */

function slnzen7_preprocess_html(&$variables, $hook) {
  // $variables['sample_variable'] = t('Lorem ipsum.');

  // The body tag's classes are controlled by the $classes_array variable. To
  // remove a class from $classes_array, use array_diff().
  //$variables['classes_array'] = array_diff($variables['classes_array'], array('class-to-remove'));

  //var_dump(get_defined_vars());

  // not needed with zen - automatic. same for theme_hook_suggestions
  // note: not showing on survey forms
  // add roles as classes
  global $user;
  // if ($variables['user']) {
  if ($user && isset($user->roles) && is_array($user->roles)) {
    foreach($variables['user']->roles as $key => $role){
      $variables['classes_array'][] = 'role-' . drupal_html_class($role);
     } // foreach
  } // if

     
    // todo: check for html__node__999999 and load that node to add type, add title too? - automatic for type
    // if this is an sln_survey node, add subject and instrument to body classes
    // consider: add parent project (title?) if 1-1 and there is a reference
    // todo: check for html__node__submission__999999 and add "webform_submission" to body class, also status (draft, complete)
    // use html__node__submission__edit presence to add "edit"
  $survey_classess_added = FALSE;
  $submission_classess_added = FALSE;
  $subject = "";
  $instrument = "";
  $nodeid = 0;
  $submissionid = 0;
  $surveynode = NULL;
  $submission = NULL;
  if (array_key_exists('theme_hook_suggestions', $variables)) {
    $ths = $variables['theme_hook_suggestions'];
    if (is_array($ths)) {
      foreach($ths as $ndx => $sug) {
    
        if (!$survey_classess_added  && strpos($sug, 'html__node__') !== FALSE) {
          if (!$nodeid)
            $nodeid = str_replace(array('html__node__%', 'html__node__'), '', $sug);
          
          if ($nodeid) {
            $node = node_load($nodeid);
            if ($node)
              $variables['classes_array'][] = drupal_html_class("node-type-" . $node->type);
          }
            
          if ($nodeid && $node && !$survey_classess_added && ($node->type == 'sln_survey')) {
            // TODO: use menu function to get preloaded node and pass object instead of nid to avoid additional node_loads. performance
            if (!$subject)
              $subject = _slnsite_survey_subject_name($nodeid);
            if (!$instrument)
              $instrument = _slnsite_survey_instrument_name($nodeid);
            $variables['classes_array'][] = drupal_html_class("sln-survey-form");
            if (drupal_strlen($subject))
              $variables['classes_array'][] = drupal_html_class("sln-survey-form-$subject");
            if (drupal_strlen($instrument))
              $variables['classes_array'][] = drupal_html_class("sln-survey-form-$instrument");
            if (drupal_strlen($instrument) && drupal_strlen($subject))
             $variables['classes_array'][] = drupal_html_class("sln-survey-form-$instrument-$subject");
            $survey_classess_added = TRUE;
          } // if surveynodeid
        } // if strpos html__node__
        
        if (!$submission_classess_added && strpos($sug, 'html__node__submission__') !== FALSE) {
          if (!$submissionid)
            $submissionid = str_replace(array('html__node__submission__%', 'html__node__submission__edit', 'html__node__submission__'),  '', $sug);
          if ($submissionid) {
            if ($nodeid) 
              $submission = webform_get_submission($nodeid, $submissionid);
            else {
              $nodeid = _slnsite_node_from_submission($submissionid);
              if ($nodeid)
                $submission = webform_get_submission($nodeid, $submissionid);
            }
            //$surveynode = node_load($nodeid);
            if ($submission && $submission->is_draft)
              $variables['classes_array'][] = drupal_html_class("sln-survey-submission-draft");
            else
              $variables['classes_array'][] = drupal_html_class("sln-survey-submission-completed");
            if (!$subject)
              $subject = _slnsite_survey_subject_name($nodeid);
            if (!$instrument)
              $instrument = _slnsite_survey_instrument_name($nodeid);
            $variables['classes_array'][] = drupal_html_class("sln-survey-submission-form");
            if (drupal_strlen($subject))
              $variables['classes_array'][] = drupal_html_class("sln-survey-submission-form-$subject");
            if (drupal_strlen($instrument))
              $variables['classes_array'][] = drupal_html_class("sln-survey-submission-form-$instrument");
            if (drupal_strlen($instrument) && drupal_strlen($subject))
              $variables['classes_array'][] = drupal_html_class("sln-survey-submission-form-$instrument-$subject");
            $submission_classess_added = TRUE;
            if (!$survey_classess_added  && ($node->type == 'sln_survey')) {
              if (!$subject)
                $subject = _slnsite_survey_subject_name($nodeid);
              if (!$instrument)
                $instrument = _slnsite_survey_instrument_name($nodeid);
              $variables['classes_array'][] = drupal_html_class("sln-survey-form");
              if (drupal_strlen($subject))
                $variables['classes_array'][] = drupal_html_class("sln-survey-form-$subject");
              if (drupal_strlen($instrument))
                $variables['classes_array'][] = drupal_html_class("sln-survey-form-$instrument");
              if (drupal_strlen($instrument) && drupal_strlen($subject))
                $variables['classes_array'][] = drupal_html_class("sln-survey-form-$instrument-$subject");
              $survey_classess_added = TRUE;
            } // survey_classess_added
          } // if submissionid
        } // if strpos html__node__submission__
        
        if (strpos($sug, 'html__node__slnsurvey__')) {
          $class = $str_replace('html__node__slnsurvey__', '', $sug);
          $variables['classes_array'][] = drupal_html_class("sln-survey-formkey-$sug");
        } // if slnsurvey formkey class
        
      }  // foreach
    } // if is array
  } // if key exists
  
  // todo: add args (e.g., slnsurvey/submission/report/%nid/%sid)
  
  //dpm($variables['theme_hook_suggestions']);
  //dpm(get_defined_vars());
} // slnzen7_preprocess_html



/**
 * Override or insert variables into the page templates.
 *
 * @param $variables
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("page" in this case.)
 */

function slnzen7_preprocess_page(&$variables, $hook) {

  global $user;
  $node = array_key_exists('node', $variables) ? $variables['node'] : NULL;
  
  //$arg0 = arg(0);
  //$arg1 = arg(1);
  //$arg2 = arg(2);
  //dpm(get_defined_vars());
 
/* 
  // breadcrumbs for submission edits
  if ((arg(0) == 'node') && (arg(2) == 'submission') && (arg(4) == 'edit') && is_numeric(arg(1)) && is_numeric(arg(3))) {
    $breadcrumb = array();
    $breadcrumb[] = l('Home', '<front>'); 
    drupal_set_breadcrumb($breadcrumb);  
  } // node/%/submission/%/edit
  
  // breadcrumbs for project nodes. assume user has access to projects so no checking here
  if ((arg(0) == 'node') && is_numeric(arg(1)) && $node && ($node->type == 'sln_project')) {
    $breadcrumb = array();
    $breadcrumb[] = l('Home', '<front>'); 
    $breadcrumb[] = l('Projects', 'dashboard/projects');
    drupal_set_breadcrumb($breadcrumb);  
  } // ((arg(0) == 'node') && is_numeric(arg(1))
*/

  
  // control tab visibility on submissions
  if ((arg(0) == 'node') && (arg(2) == 'submission') && is_numeric(arg(1)) && is_numeric(arg(3))) {
    //dpm($variables);
    if (!in_array('administrator', $user->roles)) 
      $variables['tabs'] = array();
  } // node/%/submission
  
  
} // slnzen7_preprocess_page

/**
 * Override or insert variables into the node templates.
 *
 * @param $variables
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("node" in this case.)
 */

function slnzen7_preprocess_node(&$variables, $hook) {
  // $variables['sample_variable'] = t('Lorem ipsum.');

  // Optionally, run node-type-specific preprocess functions, like
  // slnzen7_preprocess_node_page() or slnzen7_preprocess_node_story().
  /*
  $function = __FUNCTION__ . '_' . $variables['node']->type;
  if (function_exists($function)) {
    $function($variables, $hook);
  }
  */
  $node = $variables['node'];

  if ($node->type == 'sln_project') {

  }  // ($node->type == 'sln_project')
  
  if ($node->type == 'sln_survey') {
    if (isset($node->field_sln_survey_subject)) {
      $subject = $node->field_sln_survey_subject;
      $tid = _slnsite_array_keys_exist($subject,'und',0,'tid') ? $subject['und'][0]['tid'] : "unk";
      if ($tid !== "unk") {
        $term = taxonomy_term_load($tid);
        if ($term) {
          $name = $term->name;
          $variables['classes_array'][] = 'sln-survey-subject-' . drupal_html_class($name);
        } // term
      } // if not unk
    } // if isset

    if (isset($node->field_sln_survey_instrument)) {
      $instrument = $node->field_sln_survey_instrument;
      $tid = _slnsite_array_keys_exist($instrument,'und',0,'tid') ? $instrument['und'][0]['tid'] : "unk";
      if ($tid !== "unk") {
        $term = taxonomy_term_load($tid);
        if ($term) {
          $name = $term->name;
          $variables['classes_array'][] = 'sln-survey-instrument-' . drupal_html_class($name);
        } // term
      } // if not unk
    } // if isset
  } // sln_survey
  
  //dpm(get_defined_vars());
  
} // slnzen7_preprocess_node

/**
 * Override or insert variables into the comment templates.
 *
 * @param $variables
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("comment" in this case.)
 */
/* -- Delete this line if you want to use this function
function slnzen7_preprocess_comment(&$variables, $hook) {
  $variables['sample_variable'] = t('Lorem ipsum.');
}
// */

/**
 * Override or insert variables into the region templates.
 *
 * @param $variables
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("region" in this case.)
 */
/* -- Delete this line if you want to use this function
function slnzen7_preprocess_region(&$variables, $hook) {
  // Don't use Zen's region--sidebar.tpl.php template for sidebars.
  //if (strpos($variables['region'], 'sidebar_') === 0) {
  //  $variables['theme_hook_suggestions'] = array_diff($variables['theme_hook_suggestions'], array('region__sidebar'));
  //}
}
// */

/**
 * Override or insert variables into the block templates.
 *
 * @param $variables
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("block" in this case.)
 */
/* -- Delete this line if you want to use this function
function slnzen7_preprocess_block(&$variables, $hook) {
  // Add a count to all the blocks in the region.
  // $variables['classes_array'][] = 'count-' . $variables['block_id'];

  // By default, Zen will use the block--no-wrapper.tpl.php for the main
  // content. This optional bit of code undoes that:
  //if ($variables['block_html_id'] == 'block-system-main') {
  //  $variables['theme_hook_suggestions'] = array_diff($variables['theme_hook_suggestions'], array('block__no_wrapper'));
  //}
}
// */

// preprocessor functions added by mss from webforms and other modules
// see also, numerous theme_ functions in these modules

// webforms
// function slnzen7_preprocess_webform_progressbar(&$variables) {}
// function slnzen7_preprocess_webform_mail_message(&$variables) {}

/**
 * allow webform components with special form_key names to add template suggestions for conversion to classes for body in html preprocessor
 *
 */
function slnzen7_preprocess_webform_element(&$variables) {
  $form_key = $variables['element']['#webform_component']['form_key'];
  if (strpos($form_key, 'sln_body_class_') === 0) {
    $class = str_replace('sln_body_class_', '', $form_key);
    // todo: shouldn't this be $class instead of $form_key???
    // also, why hook suggestion instead of a class addition? because of depth of call?
    $variables['theme_hook_suggestions'][] = "html-node-slnsurvey-" . drupal_html_class($form_key);
    //dpm($variables);

  } // if
} // slnzen7_preprocess_webform_element

// function slnzen7_preprocess_webform_confirmation(&$variables) {}
// see also slnzen7_preprocess_webform_form below

// webform components form
// see also slnzen7_preprocess_webform_components_form below

// submissions
// 
function slnzen7_preprocess_webform_submission(&$variables) {
  // NOTE: this only effects 'view' of submission, not edit
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>'); 
  // todo: add project from sln_project_id data
  // $breadcrumb[] = l('xxx', 'url');
  drupal_set_breadcrumb($breadcrumb);
}
// function slnzen7_preprocess_webform_submission_information(&$variables) {}
// function slnzen7_preprocess_webform_submission_navigation(&$variables) {}

// reports
// function slnzen7_preprocess_webform_analysis(&$variables) {}
// function slnzen7_preprocess_webform_analysis_component(&$variables) {}
// function slnzen7_preprocess_webform_results_submissions(&$variables) {}

// views
// function template_preprocess_views_ui_view_preview_section(&$variables) {}
// function template_preprocess_views_view(&$variables) {}
// function template_preprocess_views_view_fields(&$variables) {}
// function template_preprocess_views_view_grouping(&$variables) {}
// function template_preprocess_views_view_field(&$variables) {}
// function template_preprocess_views_view_summary(&$variables) {}
// function template_preprocess_views_view_summary_unformatted(&$variables) {}
// function template_preprocess_views_view_table(&$variables) {}
// function template_preprocess_views_view_grid(&$variables) {}
// function template_preprocess_views_view_unformatted(&$variables) {}
// function template_preprocess_views_view_list(&$variables) {}
// function template_preprocess_views_view_rss(&$variables) {}
// function template_preprocess_views_view_row_rss(&$variables) {}
// function template_preprocess_views_exposed_form(&$variables) {}



function slnzen7_preprocess_webform_form(&$variables) {

  // NOTE: classes_array items these show up in template's $class string, but are not applied to the form
  // thus we have added a div around the main form content using these classes
  $form = $variables['form'];
  $node = $form['#node'];
  
  if (array_key_exists('details', $form)) {
    $details = $form['details'];
    $page_num = _slnsite_array_keys_exist($details,'page_num','#value') ? $details['page_num']['#value'] : "unk";
    $variables['classes_array'][] = 'sln-survey-page-num-' . drupal_html_class($page_num);
    //$variables['form']['#attributes']['class'][] = 'sln-survey-page-num-' . drupal_html_class($page_num);
  } // details
  
  if (array_key_exists('actions', $form)) {
    $actions = $form['actions'];
    $next_button = _slnsite_array_keys_exist($actions,'next','#value') ? $actions['next']['#value'] : "unk";
    $variables['classes_array'][] = 'sln-survey-next-button-' . drupal_html_class($next_button);
    //$variables['form']['#attributes']['class'][] = 'sln-survey-next-button-' . drupal_html_class($next_button);
  }
  
  /*
  if (isset($node->field_sln_survey_subject)) {
    $subject = $node->field_sln_survey_subject;
    $tid = _slnsite_array_keys_exist($subject,'und',0,'tid') ? $subject['und'][0]['tid'] : "unk";
    if ($tid !== "unk") {
      $term = taxonomy_term_load($tid);
      $name = $term->name;
      $variables['classes_array'][] = 'sln-subject-' . drupal_html_class($name);
      $variables['form']['#attributes']['class'][] = 'sln-subject-' . drupal_html_class($name);
    } // if not unk
  } // subject
  */
  
  // add roles as classes
  //if ($variables['user']) {
  //  foreach($variables['user']->roles as $key => $role){
  //    $variables['classes_array'][] = 'role-' . drupal_html_class($role);
  //  } // foreach
  //} // if
  
  
  //dpm($variables['form']);
  //dpm(get_defined_vars());
  
} // slnzen7_preprocess_webform_form



function slnzen7_preprocess_webform_components_form(&$variables) {
  // variables keys are: form, theme_hook_suggestions, rows, header
  // rows are indexed by integers, starting from 0,
  // rows have keys: data (array), class (array) and data-cid (integer)

  //watchdog('mss', var_export($variables, 1));
  //dpm(array_keys($variables));

  /*
  //dpm($variables['form']);
  //dpm($variables['header']);
  // this is empty
  //dpm($variables['theme_hook_suggestions']);

  $numrows = count($variables['rows']);

  //dpm($variables['rows'][0]);
  //dpm($variables['rows'][1]);
  //dpm($variables['rows'][$numrows-2]);
  //dpm($variables['rows'][$numrows-1]);

  // append to header
  $variables['header'][] = 'X';
  // insert into header
  //array_splice($variables['header'], 3, 0, 'mytitle');

  // TODO: do not show if no components

  foreach ($variables['rows'] as $ndx => $row) {
    if ($ndx != ($numrows - 1)) {
      $input = array('data' => 'x', 'class' => array('myclass'));
      // append to row:
      $variables['rows'][$ndx]['data'][] = $input;
      // insert into row
      //array_splice($variables['rows'][$ndx]['data'], 3, 0, $input);
    } // if
  } // foreach
  */
} // slnzen7_preprocess_webform_components_form




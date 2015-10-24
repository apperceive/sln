<?php

/**
 * @file
 * Page callbacks for the slnsite module.
 */

/**
 * Implements helper function for autocompletion
 */
function _slnsite_base_title_js($string = '') {
  // make this generic for use with any type of content (for example, pass type 'sln_planning_base_webform' or 'sln_ia_base_form')

  $matches = array();
  if ($string != '') {
    $result = db_select('node')
      ->fields('node', array('title'))
      ->condition('type', 'sln_base_form')
      ->condition('title', '%' . db_like($string) . '%', 'LIKE')
      ->range(0, 10)
      ->execute();
    foreach ($result as $node) {
      $matches[$node->title] = $node->title;
    }
  }
  else
    $matches['no string error'] = 'no string error';
  drupal_json_output($matches);
} // _slnsite_plan_base_title_js




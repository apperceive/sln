<?php

/**
 * @file
 *   slnslq360self
 */

/**
 * Entry page for the multi-step form.
 *
 * @return array
 *   Drupal renderable array.
 */
function slnslq360self_slnslq360self_page() {

  $description = '<div class="messages status">' . t('Demonstrates multistep form which uses $_SESSION object as storage. If you refresh the page all filled in data will remain in fields. Same happends if you load the page again. Moreover, the last step you were at is displayed. This approach is suitable for complex forms, where it is important to provide users with more time to fill in all data and prevent data loss in case of page reload or accidental window close, etc.') . '</div>';

  // Just an example how to pass additional parameters into mforms.
  $params = array('param1', 'objectN');

  // Return Drupal renderable array.
  return array(
    'message' => array(
      '#markup' => $description,
    ),
    'mform' => drupal_get_form('slnslq360self_slnslq360self', $params),
  );
}

/**
 * Callback function to generate form.
 *
 * @param array $form
 *   Drupal form array.
 * @param array $form_state
 *   Drupal form_state array.
 * @param array $params
 *   Optional params passed into form.
 *
 * @return array
 *   Drupal form array.
 */
function slnslq360self_slnslq360self($form, &$form_state, $params) {

  dpm(get_defined_vars());

  return mforms_build('slnslq360self', '_slnslq360self_step1', $form_state);
}

/**
 * Callback function to validate form inputs
 *
 * @param array $form
 *   Drupal form array.
 * @param array $form_state
 *   Drupal form_state array.
 */
function slnslq360self_slnslq360self_validate($form, &$form_state) {
  mforms_validate('slnslq360self', $form, $form_state);
}

/**
 * Callback function to process the form inputs.
 *
 * @param array $form
 *   Drupal form array.
 * @param array $form_state
 *   Drupal form_state array.
 */
function slnslq360self_slnslq360self_submit($form, &$form_state) {

  dpm(get_defined_vars());

  mforms_submit('slnslq360self', $form, $form_state);
}

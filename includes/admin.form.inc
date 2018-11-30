<?php

/**
 * @file
 * Administration forms and submit handling for Islandora Digitization Requests module.
 */

/**
 * Administration and configuration form for islandora_digitization_requests.
 *
 * @ingroup forms
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_digitization_requests_admin_form() {
  module_load_include('inc', 'islandora_digitization_requests', 'includes/db');
  module_load_include('inc', 'islandora_digitization_requests', 'includes/utilities');

  // Needed for collapsible fieldsets
  drupal_add_js('misc/collapse.js');

  $title = 'Islandora Digitization Requests - Configuration';
  drupal_set_title(t($title), CHECK_PLAIN);
  $webforms = islandora_digitization_requests_get_webforms();
  $webform_options = variable_get('islandora_digitization_requests_webform_forms', array());

  $form = array();
  // Selected Webform forms
  $form['webform_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('"Digitization Request" Webforms'),
    '#description' => t('From all possible Webform forms, which ones are used as ' .
        'various types of "Digitization Request" forms.') . "<br><br>",
    'webforms' => array(),
  );

  $link_attributes = array('attributes'=>array('target' => '_blank'));
  foreach ($webforms as $nid => $webform_arr) {
    $form_components = islandora_digitization_requests_webform_components($nid);
    $form['webform_options']['webform_' . $nid] = array(
      '#type' => 'fieldset',
      '#title' => '<b>' . $webform_arr['title'] . '</b>',
      'webform_' . $nid . '_is_digitization_request' => array(
        '#title' => '"' . $webform_arr['title'] . '" is a Digitization Request?',
        '#type' => 'checkbox',
        '#prefix' => '<ul class="action-links"><li>' . l('Create new "' . $webform_arr['title'] . '"', '/node/' . $nid, $link_attributes) . '</li></ul>',
        '#default_value' => $webform_options[$nid]['selected'],
      ),
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Settings'),
  );
  $form['reset'] = array(
    '#type' => 'submit',
    '#prefix' => '&nbsp; ',
    '#value' => t('Reset'),
  );
  return $form;
}

/**
 * To store the Drupal variables based on user's input.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_digitization_requests_admin_form_submit(array $form, array &$form_state) {
  $id = $form_state['triggering_element']['#id'];
  switch ($id) {
    case 'edit-submit':
      // Loop through the available webforms and make an array of values that
      // corresponds to the checkbox names and their values.
      module_load_include('inc', 'islandora_digitization_requests', 'includes/db');
      $webforms = islandora_digitization_requests_get_webforms();
      $digitization_request_options = array();
      foreach ($webforms as $nid => $webform_arr) {
        $id = 'webform_' . $nid . '_is_digitization_request';
        $digitization_request_options[$nid]['selected'] = $form_state['values'][$id];
      }
      variable_set('islandora_digitization_requests_webform_forms', $digitization_request_options);

      drupal_set_message('[' . date('H:i:s') . '] The settings have been saved.');
      break;
    case 'edit-reset':
      variable_del('islandora_digitization_requests_foo');
      variable_del('islandora_digitization_requests_webforms');
      drupal_set_message('[' . date('H:i:s') . '] The settings have been reset.');
      break;
  }
}

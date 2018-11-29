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

  $mapping_to_fieldnames_for_textfields = islandora_digitization_requests_batch_field_mappings(TRUE, FALSE);
  $mapping_to_fieldnames_for_intfields = islandora_digitization_requests_batch_field_mappings(FALSE, TRUE);

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
        'various types of "Digitization Request" forms and what are their field ' . 
        'mappings for converting them into a batch, which fields are able to be ' .
        'searched, etc.') . "<br><br>",
    'webforms' => array(),
  );

  $link_attributes = array('attributes'=>array('target' => '_blank'));
  foreach ($webforms as $nid => $webform_arr) {
    $searchable_fields = islandora_digitization_requests_test_search($nid);
    $search_options = array();
    $form_components = islandora_digitization_requests_webform_components($nid);

    $form['webform_options']['webform_' . $nid] = array(
      '#type' => 'fieldset',
      '#title' => '<b>' . $webform_arr['title'] . '</b> Options and mappings',
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      'webform_' . $nid . '_is_digitization_request' => array(
        '#title' => '"' . $webform_arr['title'] . '" is a Digitization Request?',
        '#type' => 'checkbox',
        '#prefix' => '<ul class="action-links"><li>' . l('Create new "' . $webform_arr['title'] . '"', '/node/' . $nid, $link_attributes) . '</li></ul>',
        '#default_value' => $webform_options[$nid]['selected'],
      ),
      'webform_' . $nid . '_searchfields_wrapper' => array(
        '#type' => 'fieldset',
        '#title' => 'Search fields',
        '#description' => '<p>Select which fields from this Webform are include in the search operation. ' .
          '</p><span><b>Configured search fields: </b>' . implode(', ', $searchable_fields) . '</span>',
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        // The individual form field components are enumerated in the loop below.
      ),
    );
    if (module_exists('islandora_digital_workflow')) {
      $form['webform_options']['webform_' . $nid]['webform_' . $nid . '_mappings_wrapper'] = array(
        '#type' => 'fieldset',
        '#title' => 'Mappings',
        '#description' => 'Select the mappings for fields converted into a Digital Workflow batch records.',
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        // The individual form field components are enumerated in the loop below.
      );
    }

    $allowed_field_types_for_text_mapping = array('textarea', 'textfield');

    foreach ($form_components as $cid => $components_arr) {
      $form['webform_options']['webform_' . $nid]['webform_' . $nid . '_searchfields_wrapper']['webform_' . $nid . '_searchfields_' . $cid] = array(
        '#type' => 'checkbox',
        '#title' => $components_arr['name'],
        '#default_value' => (array_key_exists('search_fields', $webform_options[$nid]) ? $webform_options[$nid]['search_fields'][$cid]: 0),
      );
      if (module_exists('islandora_digital_workflow')) {
        $form['webform_options']['webform_' . $nid]['webform_' . $nid . '_mappings_wrapper']['webform_' . $nid . '_mappings_' . $cid] = array(
          '#type' => 'select',
          '#title' => $components_arr['name'] . '<span class="light_text"> - (' . $components_arr['type'] . ')</span>',
          '#options' => ((!(array_search($components_arr['type'], $allowed_field_types_for_text_mapping) === FALSE))
              ? $mapping_to_fieldnames_for_textfields : array_merge($mapping_to_fieldnames_for_textfields, $mapping_to_fieldnames_for_intfields)),
          '#default_value' => (array_key_exists('mapped_fields', $webform_options[$nid]) ? $webform_options[$nid]['mapped_fields'][$cid]: ''),
        );
      }
    }
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
        $form_components = islandora_digitization_requests_webform_components($nid);
        foreach ($form_components as $cid => $components_arr) {
          $digitization_request_options[$nid]['search_fields'][$cid] = $form_state['values']['webform_' . $nid . '_searchfields_' . $cid];
        }

        // The mapped field depends on whether or not the Islandora Digital
        // Workflow module is installed.
        if (module_exists('islandora_digital_workflow')) {
          foreach ($form_components as $cid => $components_arr) {
            $digitization_request_options[$nid]['mapped_fields'][$cid] = $form_state['values']['webform_' . $nid . '_mappings_' . $cid];
          }
        }
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

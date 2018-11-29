<?php

/**
 * @file
 * Set of utility functions that are widely used throughout the module code.
 */

/**
 * Does the mapping between a selected "Digitization Request" and a "Digital
 * Workflow batch record".
 *
 * @global object $user
 *   Drupal user object.
 * @return mixed
 *   Either redurns the batch record OR NULL if there is not enough info to
 * create the record
 */
function islandora_digitization_requests_do_mapping() {
  module_load_include('inc', 'islandora_digitization_requests', 'includes/db');
  module_load_include('inc', 'islandora_digitization_requests', 'includes/search');

  global $user;

}

function islandora_digitization_requests_batch_field_mappings($text_fields = TRUE, $integer_fields = TRUE) {
  module_load_include('install', 'islandora_digital_workflow', 'islandora_digital_workflow');

  $schema = islandora_digital_workflow_schema();
  $allowed_field_types_for_mapping = $mapping_to_fieldnames = array();
  $mapping_to_fieldnames[] = 'Select field';
  if ($text_fields) {
    $allowed_field_types_for_mapping = array('varchar', 'text');
  }
  if ($integer_fields) {
    $allowed_field_types_for_mapping = array('int');
  }

  foreach ($schema['islandora_digital_workflow_batch']['fields'] as $fieldname => $field) {
    if (!(array_search($field['type'], $allowed_field_types_for_mapping) === FALSE) ) {
      $desc = array_key_exists('description', $field) ? ' - ' . $field['description'] : '';
      $mapping_to_fieldnames[$fieldname] = $fieldname . ' (' . $field['type'] . ')' . $desc;
    }
  }
  return $mapping_to_fieldnames;
}

function islandora_digitization_requests_test_search($nid) {
  module_load_include('inc', 'islandora_digitization_requests', 'includes/db');
  $webform_options = variable_get('islandora_digitization_requests_webform_forms', array());
  $searchable_fields = array();
  $components = islandora_digitization_requests_webform_components($nid);
  foreach ($webform_options as $options_nid => $options) {
    if ($nid == $options_nid) {
      foreach ($options['search_fields'] as $cid => $setting) {
        if ($setting) {
          $searchable_fields[$cid] = $components[$cid]['name'];
        }
      }
    }
  }
  return $searchable_fields;
}
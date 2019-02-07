<?php

/**
 * @file
 * Set of database functions that used throughout the module code.
 */

/**
 * This returns all Webforms within the current site - regardless of whether 
 * or not they are configured to be "Digitization Requests"
 *
 * @return array of key-value pairs where each has each Webform reference as the
 * node's nid value stores that array:
 *   $node->nid => array('title' => $webform_title).
 */
function islandora_digitization_requests_get_webforms() {
  module_load_include('module', 'webform', 'webform');
  $webforms = array();

  $webform_node_types = webform_node_types();
  if (!empty($webform_node_types)) {
    $query = db_select('webform', 'w')->fields('w')->fields('n', array('title'));
    $query->leftJoin('node', 'n', 'w.nid = n.nid');
    $query->condition('n.type', $webform_node_types, 'IN');
    $query->orderBy('n.title');
    $result = $query->execute();
    foreach ($result as $data) {
      $webforms[$data->nid] = array(
        'title' => $data->title,
      );
    }
  }
  return $webforms;
}

/**
 * Returns which Webforms have been configured to be "Digitization Requests".
 *
 * @param array $webforms
 *   Array of key-value pairs where each has each Webform reference as the
 * node's nid value stores that array:
 *   $node->nid => array('title' => $webform_title).
 * @return array of key-value
 *   $nid => $title
 */
function islandora_digitization_requests_get_digitization_request_webforms($webforms) {
  // Get the configuration -- to check each of the webform nodes to see whether
  // or not they are selected as "Digitization Request Webforms".
  $webform_options = variable_get('islandora_digitization_requests_webform_forms', array());
  $filtered_webforms = array();
  foreach ($webforms as $nid => $title_arr) {
    if (isset($webform_options[$nid]) && $webform_options[$nid]['selected']) {
      $filtered_webforms[$nid] = $title_arr;
    }
  }
  return $filtered_webforms;
}

/**
 * Returns the components (fields) that are used for a given Webform.
 *
 * The given Webform is referenced by its node->nid value.  The fieldset
 * components are filtered out.
 *
 * @param integer $nid
 *   The related $nid for the Webform in question
 * @return array
 *   Webform component objects
 */
function islandora_digitization_requests_webform_components($nid) {
  $webform_components = array();
  $query = db_select('webform_component', 'wc')->fields('wc');
  $query->condition('wc.nid', $nid);
  // Do not need to pull in the fieldset components
  $query->condition('wc.type', 'fieldset', '!=');
  $query->orderBy('wc.weight');
  $result = $query->execute();
  foreach ($result as $data) {
    $webform_components[$data->cid] = (array)$data;
  }
  return $webform_components;
}

/**
 * This will return the data for a single Webform submission.
 *
 * @param type $sid
 *   The submission record's identifier value.
 * @return array of webform submission data for the given $sid.
 */
function islandora_digitization_requests_get_webform_submission($sid, $include_component_fieldname = FALSE) {
  $webform_submission = array();
  $query = db_select('webform_submitted_data', 'wsd')->fields('wsd');
  $query->condition('wsd.sid', $sid);
  if ($include_component_fieldname) {
    $query->leftJoin('webform_component', 'wc', 'wsd.nid = wc.nid AND wsd.cid = wc.cid');
    $query->addField('wc', 'form_key', 'form_key');
    $query->addField('wc', 'name', 'component_name');
  }
  $query->leftJoin('webform_submissions', 'ws', 'ws.sid = wsd.sid');
  $query->leftJoin('users', 'u', 'u.uid = ws.uid');
  $query->addField('u', 'name', 'drupal_username');
  $query->orderBy('wsd.cid');
  $result = $query->execute();
  foreach ($result as $data) {
    $webform_submission[$data->cid][] = (array)$data;
  }
  return $webform_submission;
}


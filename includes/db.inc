<?php

/**
 * @file
 * Set of database functions that used throughout the module code.
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
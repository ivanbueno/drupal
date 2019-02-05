<?php
/**
 * @file
 * search_api_merchandizing.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function search_api_merchandizing_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "current_search" && $api == "current_search") {
    return array("version" => "1");
  }
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
  if ($module == "feeds" && $api == "feeds_importer_default") {
    return array("version" => "1");
  }
  if ($module == "feeds_tamper" && $api == "feeds_tamper_default") {
    return array("version" => "2");
  }
}

/**
 * Implements hook_views_api().
 */
function search_api_merchandizing_views_api() {
  return array("version" => "3.0");
}

/**
 * Implements hook_node_info().
 */
function search_api_merchandizing_node_info() {
  $items = array(
    'search_merchandizing' => array(
      'name' => t('Search Merchandizing'),
      'base' => 'node_content',
      'description' => t('Merchandizing page components based on search criteria'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}

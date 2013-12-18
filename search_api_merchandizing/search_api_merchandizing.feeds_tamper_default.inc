<?php
/**
 * @file
 * search_api_merchandizing.feeds_tamper_default.inc
 */

/**
 * Implements hook_feeds_tamper_default().
 */
function search_api_merchandizing_feeds_tamper_default() {
  $export = array();

  $feeds_tamper = new stdClass();
  $feeds_tamper->disabled = FALSE; /* Edit this to true to make a default feeds_tamper disabled initially */
  $feeds_tamper->api_version = 2;
  $feeds_tamper->id = 'search_merchandizing_importer-banner-html_entity_decode';
  $feeds_tamper->importer = 'search_merchandizing_importer';
  $feeds_tamper->source = 'banner';
  $feeds_tamper->plugin_id = 'html_entity_decode';
  $feeds_tamper->settings = '';
  $feeds_tamper->weight = 0;
  $feeds_tamper->description = 'HTML entity decode';
  $export['search_merchandizing_importer-banner-html_entity_decode'] = $feeds_tamper;

  return $export;
}

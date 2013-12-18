<?php
/**
 * @file
 * search_api_merchandizing.feeds_importer_default.inc
 */

/**
 * Implements hook_feeds_importer_default().
 */
function search_api_merchandizing_feeds_importer_default() {
  $export = array();

  $feeds_importer = new stdClass();
  $feeds_importer->disabled = FALSE; /* Edit this to true to make a default feeds_importer disabled initially */
  $feeds_importer->api_version = 1;
  $feeds_importer->id = 'search_merchandizing_importer';
  $feeds_importer->config = array(
    'name' => 'Search Merchandizing Importer',
    'description' => 'Import search merchandizing banners',
    'fetcher' => array(
      'plugin_key' => 'FeedsHTTPFetcher',
      'config' => array(
        'auto_detect_feeds' => FALSE,
        'use_pubsubhubbub' => FALSE,
        'designated_hub' => '',
      ),
    ),
    'parser' => array(
      'plugin_key' => 'FeedsCSVParser',
      'config' => array(
        'delimiter' => ',',
        'no_headers' => 0,
      ),
    ),
    'processor' => array(
      'plugin_key' => 'FeedsNodeProcessor',
      'config' => array(
        'content_type' => 'search_merchandizing',
        'expire' => '-1',
        'author' => 0,
        'authorize' => 0,
        'mappings' => array(
          0 => array(
            'source' => 'id',
            'target' => 'title',
            'unique' => 1,
          ),
          1 => array(
            'source' => 'target',
            'target' => 'field_merchandizing_target',
            'unique' => FALSE,
          ),
          2 => array(
            'source' => 'banner',
            'target' => 'field_merchandizing_content',
            'unique' => FALSE,
          ),
          3 => array(
            'source' => 'keyword',
            'target' => 'entityreference:field_merchandizing_trigger_fac',
            'unique' => FALSE,
          ),
          4 => array(
            'source' => 'keyword',
            'target' => 'field_merchandizing_trigger_key',
            'unique' => FALSE,
          ),
        ),
        'update_existing' => '1',
        'input_format' => 'full_html',
        'skip_hash_check' => 0,
      ),
    ),
    'content_type' => '',
    'update' => 0,
    'import_period' => '-1',
    'expire_period' => 3600,
    'import_on_create' => 1,
    'process_in_background' => 0,
  );
  $export['search_merchandizing_importer'] = $feeds_importer;

  return $export;
}

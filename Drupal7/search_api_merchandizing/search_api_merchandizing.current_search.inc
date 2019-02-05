<?php
/**
 * @file
 * search_api_merchandizing.current_search.inc
 */

/**
 * Implements hook_current_search_default_items().
 */
function search_api_merchandizing_current_search_default_items() {
  $export = array();

  $item = new stdClass();
  $item->disabled = FALSE; /* Edit this to true to make a default item disabled initially */
  $item->api_version = 1;
  $item->name = 'merchandizing_target_1';
  $item->label = 'Merchandizing Target 1';
  $item->settings = array(
    'items' => array(
      'merchandizing' => array(
        'id' => 'merchandizing',
        'label' => 'Merchandizing',
        'merchandizing_type' => 'search_merchandizing_block_1',
        'wrapper' => 0,
        'element' => '0',
        'css' => 0,
        'classes' => '',
        'weight' => '0',
      ),
    ),
    'advanced' => array(
      'empty_searches' => '1',
    ),
  );
  $export['merchandizing_target_1'] = $item;

  $item = new stdClass();
  $item->disabled = FALSE; /* Edit this to true to make a default item disabled initially */
  $item->api_version = 1;
  $item->name = 'merchandizing_target_2';
  $item->label = 'Merchandizing Target 2';
  $item->settings = array(
    'items' => array(
      'merchandizing' => array(
        'id' => 'merchandizing',
        'label' => 'Merchandizing',
        'merchandizing_type' => 'search_merchandizing_block_2',
        'wrapper' => 0,
        'element' => '0',
        'css' => 0,
        'classes' => '',
        'weight' => '0',
      ),
    ),
    'advanced' => array(
      'empty_searches' => '1',
    ),
  );
  $export['merchandizing_target_2'] = $item;

  $item = new stdClass();
  $item->disabled = FALSE; /* Edit this to true to make a default item disabled initially */
  $item->api_version = 1;
  $item->name = 'merchandizing_target_3';
  $item->label = 'Merchandizing Target 3';
  $item->settings = array(
    'items' => array(
      'merchandizing' => array(
        'id' => 'merchandizing',
        'label' => 'Merchandizing',
        'merchandizing_type' => 'search_merchandizing_block_3',
        'wrapper' => 0,
        'element' => '0',
        'css' => 0,
        'classes' => '',
        'weight' => '0',
      ),
    ),
    'advanced' => array(
      'empty_searches' => '1',
    ),
  );
  $export['merchandizing_target_3'] = $item;

  $item = new stdClass();
  $item->disabled = FALSE; /* Edit this to true to make a default item disabled initially */
  $item->api_version = 1;
  $item->name = 'merchandizing_target_4';
  $item->label = 'Merchandizing Target 4';
  $item->settings = array(
    'items' => array(
      'merchandizing' => array(
        'id' => 'merchandizing',
        'label' => 'Merchandizing',
        'merchandizing_type' => 'search_merchandizing_block_4',
        'wrapper' => 0,
        'element' => '0',
        'css' => 0,
        'classes' => '',
        'weight' => '0',
      ),
    ),
    'advanced' => array(
      'empty_searches' => '1',
    ),
  );
  $export['merchandizing_target_4'] = $item;

  $item = new stdClass();
  $item->disabled = FALSE; /* Edit this to true to make a default item disabled initially */
  $item->api_version = 1;
  $item->name = 'merchandizing_header';
  $item->label = 'Merchandizing Header';
  $item->settings = array(
    'items' => array(
      'merchandizing' => array(
        'id' => 'merchandizing',
        'label' => 'Merchandizing',
        'merchandizing_type' => 'search_merchandizing_header',
        'wrapper' => 0,
        'element' => '0',
        'css' => 0,
        'classes' => '',
        'weight' => '0',
      ),
    ),
    'advanced' => array(
      'empty_searches' => '1',
    ),
  );
  $export['merchandizing_header'] = $item;

  $item = new stdClass();
  $item->disabled = FALSE; /* Edit this to true to make a default item disabled initially */
  $item->api_version = 1;
  $item->name = 'merchandizing_content_top';
  $item->label = 'Merchandizing Content Top';
  $item->settings = array(
    'items' => array(
      'merchandizing' => array(
        'id' => 'merchandizing',
        'label' => 'Merchandizing',
        'merchandizing_type' => 'search_merchandizing_content_top',
        'wrapper' => 0,
        'element' => '0',
        'css' => 0,
        'classes' => '',
        'weight' => '0',
      ),
    ),
    'advanced' => array(
      'empty_searches' => '1',
    ),
  );
  $export['merchandizing_content_top'] = $item;

  return $export;
}

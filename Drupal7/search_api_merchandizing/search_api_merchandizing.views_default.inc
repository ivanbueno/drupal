<?php
/**
 * @file
 * search_api_merchandizing.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function search_api_merchandizing_views_default_views() {
  $export = array();
$view = new view();
$view->name = 'search_merchandizing';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Search Merchandising';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Search Merchandising';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'role';
$handler->display->display_options['access']['role'] = array(
  3 => '3',
  4 => '4',
  5 => '5',
  6 => '6',
  7 => '7',
  8 => '8',
  9 => '9',
);
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '50';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['pager']['options']['id'] = '0';
$handler->display->display_options['pager']['options']['quantity'] = '9';
$handler->display->display_options['style_plugin'] = 'table';
/* Header: Global: Text area */
$handler->display->display_options['header']['area']['id'] = 'area';
$handler->display->display_options['header']['area']['table'] = 'views';
$handler->display->display_options['header']['area']['field'] = 'area';
$handler->display->display_options['header']['area']['empty'] = TRUE;
$handler->display->display_options['header']['area']['content'] = '<ul class="action-links"><li><a href="/node/add/search-merchandizing?destination=admin/config/search/search_api/merchandising">Add a search merchandizing</a></li><li><a href="/admin/config/varnish/clear?destination=admin/config/search/search_api/merchandising">Clear varnish cache</a></li></ul>';
$handler->display->display_options['header']['area']['format'] = 'full_html';
/* Relationship: Content: Author */
$handler->display->display_options['relationships']['uid']['id'] = 'uid';
$handler->display->display_options['relationships']['uid']['table'] = 'node';
$handler->display->display_options['relationships']['uid']['field'] = 'uid';
/* Field: User: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'users';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['relationship'] = 'uid';
$handler->display->display_options['fields']['name']['label'] = 'Created by';
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
$handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
/* Field: Content: Facet Trigger */
$handler->display->display_options['fields']['field_merchandizing_trigger_fac']['id'] = 'field_merchandizing_trigger_fac';
$handler->display->display_options['fields']['field_merchandizing_trigger_fac']['table'] = 'field_data_field_merchandizing_trigger_fac';
$handler->display->display_options['fields']['field_merchandizing_trigger_fac']['field'] = 'field_merchandizing_trigger_fac';
$handler->display->display_options['fields']['field_merchandizing_trigger_fac']['element_type'] = '0';
$handler->display->display_options['fields']['field_merchandizing_trigger_fac']['element_wrapper_type'] = '0';
$handler->display->display_options['fields']['field_merchandizing_trigger_fac']['element_default_classes'] = FALSE;
$handler->display->display_options['fields']['field_merchandizing_trigger_fac']['field_api_classes'] = TRUE;
/* Field: Content: Keyword Trigger */
$handler->display->display_options['fields']['field_merchandizing_trigger_key']['id'] = 'field_merchandizing_trigger_key';
$handler->display->display_options['fields']['field_merchandizing_trigger_key']['table'] = 'field_data_field_merchandizing_trigger_key';
$handler->display->display_options['fields']['field_merchandizing_trigger_key']['field'] = 'field_merchandizing_trigger_key';
$handler->display->display_options['fields']['field_merchandizing_trigger_key']['element_type'] = '0';
$handler->display->display_options['fields']['field_merchandizing_trigger_key']['element_wrapper_type'] = '0';
$handler->display->display_options['fields']['field_merchandizing_trigger_key']['element_default_classes'] = FALSE;
$handler->display->display_options['fields']['field_merchandizing_trigger_key']['field_api_classes'] = TRUE;
/* Field: Content: Target Block */
$handler->display->display_options['fields']['field_merchandizing_target']['id'] = 'field_merchandizing_target';
$handler->display->display_options['fields']['field_merchandizing_target']['table'] = 'field_data_field_merchandizing_target';
$handler->display->display_options['fields']['field_merchandizing_target']['field'] = 'field_merchandizing_target';
$handler->display->display_options['fields']['field_merchandizing_target']['element_type'] = '0';
$handler->display->display_options['fields']['field_merchandizing_target']['element_wrapper_type'] = '0';
$handler->display->display_options['fields']['field_merchandizing_target']['element_default_classes'] = FALSE;
$handler->display->display_options['fields']['field_merchandizing_target']['field_api_classes'] = TRUE;
/* Field: Content: Duration */
$handler->display->display_options['fields']['field_merchandizing_start_date']['id'] = 'field_merchandizing_start_date';
$handler->display->display_options['fields']['field_merchandizing_start_date']['table'] = 'field_data_field_merchandizing_start_date';
$handler->display->display_options['fields']['field_merchandizing_start_date']['field'] = 'field_merchandizing_start_date';
$handler->display->display_options['fields']['field_merchandizing_start_date']['element_type'] = '0';
$handler->display->display_options['fields']['field_merchandizing_start_date']['element_wrapper_type'] = '0';
$handler->display->display_options['fields']['field_merchandizing_start_date']['element_default_classes'] = FALSE;
$handler->display->display_options['fields']['field_merchandizing_start_date']['settings'] = array(
  'format_type' => 'medium',
  'fromto' => 'both',
  'multiple_number' => '',
  'multiple_from' => '',
  'multiple_to' => '',
);
$handler->display->display_options['fields']['field_merchandizing_start_date']['field_api_classes'] = TRUE;
/* Field: Content: Edit link */
$handler->display->display_options['fields']['edit_node']['id'] = 'edit_node';
$handler->display->display_options['fields']['edit_node']['table'] = 'views_entity_node';
$handler->display->display_options['fields']['edit_node']['field'] = 'edit_node';
$handler->display->display_options['fields']['edit_node']['label'] = '';
$handler->display->display_options['fields']['edit_node']['element_label_colon'] = FALSE;
/* Field: Content: Delete link */
$handler->display->display_options['fields']['delete_node']['id'] = 'delete_node';
$handler->display->display_options['fields']['delete_node']['table'] = 'views_entity_node';
$handler->display->display_options['fields']['delete_node']['field'] = 'delete_node';
$handler->display->display_options['fields']['delete_node']['label'] = '';
$handler->display->display_options['fields']['delete_node']['element_label_colon'] = FALSE;
/* Sort criterion: Content: Post date */
$handler->display->display_options['sorts']['created']['id'] = 'created';
$handler->display->display_options['sorts']['created']['table'] = 'node';
$handler->display->display_options['sorts']['created']['field'] = 'created';
$handler->display->display_options['sorts']['created']['order'] = 'DESC';
/* Filter criterion: Content: Published */
$handler->display->display_options['filters']['status']['id'] = 'status';
$handler->display->display_options['filters']['status']['table'] = 'node';
$handler->display->display_options['filters']['status']['field'] = 'status';
$handler->display->display_options['filters']['status']['value'] = 1;
$handler->display->display_options['filters']['status']['group'] = 1;
$handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
/* Filter criterion: Content: Type */
$handler->display->display_options['filters']['type']['id'] = 'type';
$handler->display->display_options['filters']['type']['table'] = 'node';
$handler->display->display_options['filters']['type']['field'] = 'type';
$handler->display->display_options['filters']['type']['value'] = array(
  'search_merchandizing' => 'search_merchandizing',
);

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'admin/config/search/search_api/merchandising';
$handler->display->display_options['menu']['type'] = 'tab';
$handler->display->display_options['menu']['title'] = 'Merchandising';
$handler->display->display_options['menu']['weight'] = '0';
$handler->display->display_options['menu']['context'] = 0;
$handler->display->display_options['menu']['context_only_inline'] = 0;
$translatables['search_merchandizing'] = array(
  t('Master'),
  t('Search Merchandising'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('Items per page'),
  t('- All -'),
  t('Offset'),
  t('« first'),
  t('. previous'),
  t('next .'),
  t('last »'),
  t('<ul class="action-links"><li><a href="/node/add/search-merchandizing?destination=admin/config/search/search_api/merchandizing">Add a search merchandizing</a></li></ul>'),
  t('author'),
  t('Created by'),
  t('Title'),
  t('Facet Trigger'),
  t('Keyword Trigger'),
  t('Target Block'),
  t('Duration'),
  t('Page'),
);

  $export['search_merchandizing'] = $view;

  return $export;
}

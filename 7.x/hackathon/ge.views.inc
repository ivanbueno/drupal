<?php
/**
 * Implements of hook_views_data().
 */
function ge_views_data() {
  return array(
    'node' => array(
      'author_select' => array(
        'group' => t('Content'),
        'title' => t('Author UID (select list)'),
        'help' => t('Filter by author, choosing from dropdown list.'),
        'filter' => array('handler' => 'ge_handler_filter_author_select'),
        'real field' => 'uid',
      )
    )
  );
}
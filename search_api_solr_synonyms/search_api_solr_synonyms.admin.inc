<?php

/**
 * Settings form for rearranging the order of facet blocks within the container
 * block.
 */
function search_api_solr_synonyms_settings_form($form, &$form_state) {
  $form = array();
  $include_current_search = variable_get('search_api_solr_synonyms_current_search', FALSE);
  $facet_enabled = variable_get('search_api_solr_synonyms_facet_enabled', array());
  $facet_weights = variable_get('search_api_solr_synonyms_facet_weights', array());

  $form['alias_facets'] = array(
    '#type' => 'fieldset',
    '#title' => 'Facet Alias',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['alias_facets']['search_api_solr_synonyms_facet_rename'] = array(
    '#type' => 'textarea',
    '#title' => t('Rename facets'),
    '#default_value' => variable_get('search_api_solr_synonyms_facet_rename', ''),
  );

  $form['facet_key_map'] = array(
    '#type' => 'fieldset',
    '#title' => 'Facet Key Mapping',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['facet_key_map']['search_api_solr_synonyms_facet_key_map_voc'] = array(
    '#type' => 'textarea',
    '#title' => t('Vocabularies'),
    '#default_value' => variable_get('search_api_solr_synonyms_facet_key_map_voc', ''),
  );
  $form['facet_key_map']['search_api_solr_synonyms_facet_synonym'] = array(
    '#type' => 'textarea',
    '#title' => t('Synonym'),
    '#default_value' => variable_get('search_api_solr_synonyms_facet_synonym', ''),
  );
  $form['facet_key_map']['search_api_solr_synonyms_facet_keyword_redirect'] = array(
    '#type' => 'textarea',
    '#title' => t('Keyword Redirect'),
    '#default_value' => variable_get('search_api_solr_synonyms_facet_keyword_redirect', ''),
  );

  $form['facet_boost_items'] = array(
    '#type' => 'fieldset',
    '#title' => 'Boost Items',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['facet_boost_items']['search_api_solr_synonyms_boost_items'] = array(
    '#type' => 'textarea',
    '#title' => t('Boost items'),
    '#default_value' => variable_get('search_api_solr_synonyms_boost_items', ''),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

function search_api_solr_synonyms_settings_form_submit(&$form, &$form_state) {
  // Invalidate the facet weight cache.
  cache_set('search_api_solr_synonyms_weighted_facets', NULL);

  variable_set('search_api_solr_synonyms_current_search', $form_state['values']['current_search']);
  variable_set('search_api_solr_synonyms_facet_title', $form_state['values']['search_api_solr_synonyms_facet_title']);
  variable_set('search_api_solr_synonyms_facet_rename', $form_state['values']['search_api_solr_synonyms_facet_rename']);
  variable_set('search_api_solr_synonyms_boost_items', $form_state['values']['search_api_solr_synonyms_boost_items']);

  variable_set('search_api_solr_synonyms_facet_key_map_voc', $form_state['values']['search_api_solr_synonyms_facet_key_map_voc']);
  variable_set('search_api_solr_synonyms_facet_synonym', $form_state['values']['search_api_solr_synonyms_facet_synonym']);
  variable_set('search_api_solr_synonyms_facet_keyword_redirect', $form_state['values']['search_api_solr_synonyms_facet_keyword_redirect']);
  drupal_set_message(t('Facet weights have been saved.'));
}

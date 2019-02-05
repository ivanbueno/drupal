<?php

/**
 * @file
 * The text current search item plugin class.
 */

/**
 * Current search item plugin that displays banners and other merchandizing assets
 * based on the given date, search filter, or search text.
 */
class CurrentSearchItemMerchandizing extends CurrentSearchItem {

  /**
   * Implements CurrentSearchItem::execute().
   */
  public function execute(FacetapiAdapter $adapter) {
    $data = array('facetapi_results' => array('facetapi_adapter' => $adapter));
    $merchandizing_options = array(
      'facets' => $adapter->getAllActiveItems(),
      'keyword' => $adapter->getSearchKeys(),
      'target' => $this->settings['merchandizing_type'],
    );
    $merchandizing_elements = $this->getMerchandizingNodes($merchandizing_options);
    $merchandizing_render = '';

    // add Home link in the merchandizing header
    $home_link = variable_get('commerce_cart_home_url', 'http://www.yliving.com');
    if ($this->settings['merchandizing_type'] == 'search_merchandizing_header') {
      $search_breadcrumb_link = variable_get('commerce_cart_search_breadcrumb', '');
      $merchandizing_render .= '<div class="ydesign-header-home">' . l(t('Home'), $home_link) . $search_breadcrumb_link . '</div>';
    }
    if (count($merchandizing_elements)) {
      foreach ($merchandizing_elements as $merch) {
        $merchandizing_render .= render(node_view($merch, 'default'));
      }
    }
    elseif ($this->settings['merchandizing_type'] == 'search_merchandizing_header') {
      $free_shipping_image = theme('image', array('path' => $home_link == 'http://www.yliving.com' ? 
                                                              '/sites/all/themes/ydesign/images/free-shipping.png' :
                                                              '/sites/all/themes/ydesign/images/free-shipping-yl.png'
                             ));
      $merchandizing_render .= '<div class="node-search-merchandizing">' . l($free_shipping_image, $home_link . '/info.html', array('html' => TRUE)) . '</div>';
    }

    // Translates text, returns themed output.
    $variables = array(
      'text' => $merchandizing_render,
      'wrapper' => $this->settings['wrapper'],
      'element' => $this->settings['element'],
      'css' => $this->settings['css'],
      'class' => current_search_get_classes($this->settings['classes'], $data),
      'options' => array('html' => TRUE),
    );
    return array('#markup' => theme('current_search_text', $variables));
  }

  /**
   * Implements CurrentSearchItem::settingsForm().
   */
  public function settingsForm(&$form, &$form_state) {

    $form['merchandizing_type'] = array(
      '#type' => 'select',
      '#title' => t('Merchandizing Block'),
      '#default_value' => $this->settings['merchandizing_type'],
      '#options' => array(
        'search_merchandizing_block_1' => 'Search Block 1',
        'search_merchandizing_block_2' => 'Search Block 2',
        'search_merchandizing_block_3' => 'Search Block 3',
        'search_merchandizing_block_4' => 'Search Block 4',
        'search_merchandizing_header' => 'Search Merchandizing Header',
        'search_merchandizing_content_top' => 'Search Merchandizing Content Top',
      ),
      '#description' => t('Custom text displayed in the text box. Token replacement patterns are allowed.'),
    );

    // Adds HTML wrapper elements.
    $this->wrapperForm($form, $form_state);
  }

  /**
   * Implements CurrentSearchItem::getDefaultSettings().
   */
  public function getDefaultSettings() {
    $defaults = array(
      'merchandizing_type' => 'search_merchandizing_block_1',
    );
    return $defaults + $this->wrapperDefaults();
  }

  protected function getMerchandizingNodes($options = array()) {
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'node')
      ->entityCondition('bundle', 'search_merchandizing')
      ->propertyCondition('status', 1)
      ->fieldCondition('field_merchandizing_target', 'value', $options['target'], '=');

    $keyword = trim($options['keyword']);

    $facets = array();
    if (count($options['facets'])) {
      foreach ($options['facets'] as $facet) {
        $facets[] = $facet['value'];
      }
    }

    $result = $query->execute();
    $merchandizing = array();
    if (isset($result['node'])) {
      $merchandizing_nids = array_keys($result['node']);
      $merchandizing_nodes = entity_load('node', $merchandizing_nids);

      foreach ($merchandizing_nodes as $merch) {
        $merch_wrapper = entity_metadata_wrapper('node', $merch);
        $merch_front_page_only = $merch_wrapper->field_merchandizing_front_page->value();
        if ($merch_front_page_only) {
          $selected_facets = array();
          if (isset($_GET['f'])) {
            $selected_facets = array_diff($_GET['f'], array('field_item_on_sale:1'));
          }
          if (!empty($keyword) || (count($selected_facets))) {
            continue;
          }
        }

        $on_sale_only = $merch_wrapper->field_merchandizing_on_sale->value();
        if ($on_sale_only) {
          $has_on_sale_facet_only = FALSE;
          if (isset($_GET['f'])) {
            if (count($_GET['f']) == 1 && in_array('field_item_on_sale:1', $_GET['f'])) {
              $has_on_sale_facet_only = TRUE;
            }
          }
          if (!empty($keyword) || !$has_on_sale_facet_only) {
            continue;
          }
        }
        
        $merch_trigger_keyword = $merch_wrapper->field_merchandizing_trigger_key->value();
        $merch_trigger_facet_terms = $merch_wrapper->field_merchandizing_trigger_fac->value();
        $merch_trigger_facet = array();
        if (count($merch_trigger_facet_terms)) {
          foreach ($merch_trigger_facet_terms as $facet_term) {
            $merch_trigger_facet[] = $facet_term->tid;
          }
        }

        try {
          $merch_duration_start = $merch_wrapper->field_merchandizing_start_date->value->value();
        }
        catch (Exception $e) {
          $merch_duration_start = REQUEST_TIME;
        }

        try {
          $merch_duration_end = $merch_wrapper->field_merchandizing_start_date->value2->value();
        }
        catch (Exception $e) {
          $merch_duration_end = REQUEST_TIME;
        }

        try {
          $merch_redirect = trim($merch_wrapper->field_merchandizing_redirect->value());
        }
        catch (Exception $e) {
          $merch_redirect = '';
        }

        // Show ads that are in the right date duration
        if ($merch_duration_start == $merch_duration_end
          && REQUEST_TIME < $merch_duration_start) {
          continue;
        }
        elseif ($merch_duration_start <> $merch_duration_end
          && !(REQUEST_TIME > $merch_duration_start && REQUEST_TIME <= $merch_duration_end)) {
          continue;
        }

        if (!count($merch_trigger_keyword) && !count($merch_trigger_facet)) {
          // Display all ads that have no criteria
          if ($merch_redirect) {
            drupal_goto($merch_redirect);
            return;
          }
          else {
            $merchandizing[] = $merch;
          }
        }
        elseif (count($merch_trigger_keyword) && count($merch_trigger_facet)) {
          if (count(array_intersect($merch_trigger_facet, $facets)) || in_array($keyword, $merch_trigger_keyword)) {

            if ($merch_redirect) {
              drupal_goto($merch_redirect);
              return;
            }
            else {
              // Display ads that match both criteria
              $merchandizing[] = $merch;
            }
          }
        }
        else {
          if (count($merch_trigger_keyword) && in_array($keyword, $merch_trigger_keyword)) {

            if ($merch_redirect) {
              drupal_goto($merch_redirect);
              return;
            }
            else {
              // Display ads that match on keyword only
              $merchandizing[] = $merch;
            }
          }
          elseif (count($merch_trigger_facet) && count(array_intersect($merch_trigger_facet, $facets))) {
            if ($merch_redirect) {
              drupal_goto($merch_redirect);
              return;
            }
            else {
              // Display ads that match on facets only
              $merchandizing[] = $merch;
            }
          }
        }
      }
    }
    return $merchandizing;
  }
}

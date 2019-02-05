<?php

/**
 * @file
 * Product display data alteration callback.
 */

/**
 * Data alteration callback that filters out nodes that
 * do not have an lf_id
 */
class CommerceSearchApiEmptyProductDisplayFilter extends SearchApiAbstractAlterCallback {

  /**
   * Check whether this data-alter callback is applicable for a certain index.
   *
   * @param SearchApiIndex $index
   *   The SearchApiIndex this data alteration callback resides on.
   *
   * @return bool
   *   A boolean whether or not this index is supported.
   */
  public function supportsIndex(SearchApiIndex $index) {
    $bundles = commerce_product_reference_node_types();
    return ($index->item_type == 'node' && !empty($bundles));
  }

  /**
   * Exclude nodes that do not have a field_item_id.
   * Remove duplicate older nodes.
   */
  public function alterItems(array &$items) {
    if ($bundles = commerce_product_reference_node_types()) {
      $most_recent_item = array();
      if ($most_recent_item_cache = cache_get('search_indexing_most_recent_nodes')) {
        $most_recent_item = $most_recent_item_cache->data;
      }

      foreach ($items as $id => $item) {
        if (!isset($bundles[$item->type])) {
          continue;
        }
        
        $node_wrapper = entity_metadata_wrapper('node', $item);
        $field_id = trim($node_wrapper->field_item_id->value());  
        if (empty($field_id)) {
          // Remove nodes that do not have an id
          unset($items[$id]);
        }
        else {
          // Remove older nodes that are duplicates
          if (!isset($most_recent_item[$field_id])) {
            $most_recent_item[$field_id] = $id;
          }
          else {
            if ($id > $most_recent_item[$field_id]) {
              unset($items[$most_recent_item[$field_id]]);
              // delete the older item in the index
              search_api_track_item_delete('node', array($most_recent_item[$field_id]));
              $most_recent_item[$field_id] = $id;
            }
            elseif ($id < $most_recent_item[$field_id]) {
              unset($items[$id]);
            }
          }
        }

        // Don't index items with $0 price
        if (empty($item->field_item_price)) {
          unset($items[$id]);
        }
        elseif ($node_wrapper->field_item_price->amount->value() == 0) {
          unset($items[$id]);
        }
        
        // don't index items that are not orderable
        $orderable = trim($node_wrapper->field_item_orderable->value()); 
        if (!$orderable) {
          unset($items[$id]);
        }
      }

      // Preserve the collection of most recent items
      cache_set('search_indexing_most_recent_nodes', $most_recent_item);
    }
  }

}

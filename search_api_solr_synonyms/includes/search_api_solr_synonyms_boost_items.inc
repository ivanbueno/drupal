<?php

/**
 * Boost the score of an item based on a given numerical field.
 */
class SearchApiBoostItemsAlter extends SearchApiAbstractAlterCallback {

  public function alterItems(array &$items) {
    if (!$items) {
      return;
    }
    
    $boosted_items = &drupal_static(__FUNCTION__);
    if (!isset($boosted_items)) {
      $boost_raw = array_filter(array_map('trim', explode("\n", variable_get('facetapi_block_boost_items', array()))));
      if (count($boost_raw)) {
        foreach ($boost_raw as $fa) {
          list($boost_id, $boost_score) = explode(',', $fa);
          $boosted_items[$boost_id] = $boost_score;
        }
      }
    }

    if (count($boosted_items)) {
      foreach ($items as $item) {
        $wrapper = $this->index->entityWrapper($item);
        $id = $wrapper->field_item_id->value();
        $item->{'item_boost'} = isset($boosted_items[$id]) ? $boosted_items[$id] : 0;
      }
    }
  }

  public function propertyInfo() {
    $ret = array();
    $ret['item_boost'] = array(
      'label' => 'Item Boost',
      'description' => 'A field used for boosting an item score',
      'type' => 'decimal',
    );
    return $ret;
  }

}


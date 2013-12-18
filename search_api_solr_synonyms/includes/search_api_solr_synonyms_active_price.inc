<?php

/**
 * Search API ranges data alteration callback that uses the active price
 * as a facet, eg. an item on sale needs to be filtered based on its sale price
 * instead of base price.
 */
class SearchApiActivePriceAlter extends SearchApiAbstractAlterCallback {

  public function alterItems(array &$items) {
    if (!$items) {
      return;
    }
    $required_fields = array(
      'field_item_price:amount_decimal' => array('type' => 'decimal'),
      'field_item_sale_price:amount_decimal' => array('type' => 'decimal'),
    );
    foreach ($items as $item) {
      $wrapper = $this->index->entityWrapper($item);
      $fields = search_api_extract_fields($wrapper, $required_fields);
      $active_price = 0;
      foreach ($fields as $name => $f) {
        if (!empty($f['value']) && ($active_price == 0 || $f['value'] < $active_price)) {
          $active_price = $f['value'];
        }
      }
      $item->{'item_active_price'} = $active_price;
    }
  }

  public function propertyInfo() {
    $ret = array();
    $ret['item_active_price'] = array(
      'label' => 'Active Price',
      'description' => 'Indexes the active price of a product',
      'type' => 'decimal',
    );
    return $ret;
  }

}


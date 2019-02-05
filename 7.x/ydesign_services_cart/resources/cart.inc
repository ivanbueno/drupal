<?php
/**
 * @file
 */
/**
 * Resource callback to return an orderxml of an order.
 * 
 * @param type $order_id
 * @return string 
 */
function ydesign_services_cart_get_orderxml($order_id) {
  $order = commerce_order_load($order_id);
  if ($order->order_id) {
    $order_data = ydesign_order_post_xml_get_order_details($order);
    $order_xml = theme('ydesign_order_post_xml', $order_data);
  }
  else {
    $order_xml = services_error(t('Order not found'), 404);
  }
  
  return $order_xml;
}

/**
 * Resource callback to add items to the cart from an external form.
 * 
 * @param type $signature
 * @return type 
 */
function ydesign_services_cart_addtocart($signature) {  
  $parsed_products = ydesign_services_cart_parse_products($signature);
  $products = array();
  if ($parsed_products) {
    foreach ($parsed_products as $item) {
      if (!isset($item['quantity']) || $item['quantity'] < 1 || 
         !isset($item['lf_id']) || empty($item['lf_id'])) {
        continue;
      }

      // get item data
      $item['data'] = ydesign_services_cart_get_item($item['lf_id']);
      if (!empty($item['data'])) {
        $product_id = ydesign_services_cart_get_product($item);
        if ($product_id) {
          $cart = commerce_cart_product_add_by_id($product_id, $item['quantity']);
        }
        else {
          if ($product_id = ydesign_services_cart_create_product($item)) {
            $cart = commerce_cart_product_add_by_id($product_id, $item['quantity']);
          }
          else {
            drupal_set_message(t('Product %id is not available at the moment. Please call 1-800-236-9100 to inquire about the item.', array('%id' => $item['lf_id'])));
          }
        }
      }
      else {
        drupal_set_message(t('Product %id is not available at the moment. Please call 1-800-236-9100 to inquire about the item.', array('%id' => $item['lf_id'])));
      }
    }
  }
  
  drupal_goto('cart');
  return;
}

/**
 * Resource callback to add items to the wishlist from an external form.
 * 
 * @param type $signature
 * @return type 
 */
function ydesign_services_cart_addtowishlist($signature) {  
  global $user;
    $parsed_products = ydesign_services_cart_parse_products($signature);
    $products = array();
    if ($parsed_products) {
      foreach ($parsed_products as $item) {
        $item['data'] = ydesign_services_cart_get_item($item['lf_id']);
        if (!empty($item['data'])) {
          if ($product_id = ydesign_services_cart_get_product($item)) {
            $products[] = $product_id;
          }
          else {
            if ($product_id = ydesign_services_cart_create_product($item)) {
              $products[] = $product_id;
            }
            else {
              drupal_set_message(t('Product %id is not available at the moment. Please call 1-800-236-9100 to inquire about the item.', array('%id' => $item['lf_id'])));
            }
          }
        }
      }
    }

    if (count($products)) {
      if (!$user->uid) {
        $params['query']['add-to-wishlist'] = implode(',', $products);
        $params['query']['original_url'] = 'user/wishlist';
        $url = 'user/login';

        drupal_set_message(t('Sign in to add the item to your wishlist.'));
        drupal_goto($url, $params);
        return;
      }
      else {
        foreach ($products as $product_id) {
          if (!commerce_wishlist_in_wishlist($user->uid, $product_id)) {
            $fields = array('uid' => $user->uid, 'product_id' => $product_id, 'quantity' => 1);
            $cart = commerce_wishlist_add_product($fields);
          }
        }
        drupal_goto('user/' . $user->uid . '/wishlist');
      }
    }
    else {
      drupal_goto('cart');
    }
}

/**
 * Callback function to group and structure products from a given flat array.
 * 
 * @param type $payload
 * @return type 
 */
function ydesign_services_cart_parse_products($payload = array()) {
  $products = array();
  if (count($payload)) {
    foreach ($payload as $key => $value) {
      $key_group = explode('_', $key);
      
      // ignore form elements that are not items
      if (stripos($key_group[0], 'item', 0) === 0) {
        $key_name = str_replace($key_group[0] . '_', '', $key);
        
        // put all option elements into one bucket
        if (stripos($key_name, 'option_', 0) === 0) {
          $key_name = str_replace('option_', '', $key_name);
          $products[$key_group[0]]['options'][$key_name] = $value;
        }
        else {
          $products[$key_group[0]][$key_name] = $value;
        }
      }
    }
  }
  return $products;
}

/**
 * Callback function to resolve the commerce_product equivalent of the given
 * sku.
 * 
 * @param type $item
 * @return boolean 
 */
function ydesign_services_cart_get_product($item = array(), $get_sku = FALSE) {
  
  // Parse options of the added item to the cart
  $item_options = isset($item['options']) ? $item['options'] : array();
  $option_result = ydesign_services_cart_parse_options($item_options);
  $products = ydesign_services_cart_get_all_products($item['lf_id'], $option_result['signature']);

  $product_id = $product_sku = FALSE;

  // Return if there is only one product
  if (count($products) == 1) {
    $product_id = $products[0]['product_id'];
    $product_sku = $products[0]['sku'];
  }
  elseif (count($products) > 1) {
    // Look for the product in the item set
    $greater_sku = 0;
    foreach ($products as $p) {
      if ($p['sku'] > $greater_sku) {
        $greater_sku = $p['sku'];
        $product_id = $p['product_id'];
        $product_sku = $p['sku'];
      }
    }
/*
    foreach ($products as $p) {
      $p_options = explode('~~', $p['options']);
      $diff_options_a = fast_array_diff(explode('~~', $option_result['signature']), $p_options);

      if (empty($diff_options_a)) {
        $product_id = $p['product_id'];
      }
    }
*/
  }
  
  // if the product already exists, and has an option string
  // then update its prices and return the product id
  $source_item_options_value = $item['data']->field_item_options->value();
  $source_item_options = json_decode($source_item_options_value, TRUE);
  if ($product_id && !empty($source_item_options_value)) {
    
    // get prices from source
    $base_price = abs($item['data']->field_item_price->amount->value() / 100);
    $sale_price = abs($item['data']->field_item_sale_price->amount->value() / 100);
    $on_sale = $sale_price ? 1 : 0;

    // get additional prices from source
    $matched_option = ydesign_services_cart_match_options($source_item_options, $option_result['option_values']);
    $base_price += $matched_option['additional_price'];
    if ($on_sale) {
      $sale_price += $matched_option['additional_price'];
    }

    if ($matched_option['matched_option']) {
      $product_entity = commerce_product_load($product_id);
      $product_wrap = entity_metadata_wrapper('commerce_product', $product_entity);
      $product_price = $product_wrap->commerce_price->value();
      $product_price['amount'] = $base_price * 100;
      $product_wrap->commerce_price = $product_price;

      $product_saleprice = $product_wrap->field_commerce_saleprice->value();
      $product_saleprice['amount'] = $sale_price * 100;
      $product_wrap->field_commerce_saleprice = $product_saleprice;
      $product_wrap->field_commerce_saleprice_on_sale = $on_sale;
      
      $product_wrap->title = htmlspecialchars_decode($item['data']->title->value());
      $product_wrap->field_code = $item['data']->field_item_code->value();
      $product_wrap->field_vendor_code = $item['data']->field_item_vendor_code->value();

      $product_wrap->save();
    }
  }
  
  return $get_sku ? $product_sku : $product_id;
}

/**
 * Callback to autogenerate a product.
 * 
 * @param type $item 
 */
function ydesign_services_cart_create_product($item, $get_sku = FALSE) {
  
  // Parse options of the added item to the cart
  $item_options = isset($item['options']) ? $item['options'] : array();
  $option_result = ydesign_services_cart_parse_options($item_options);
  $item_site = $item['data']->field_item_site->value();
  
  // Get the product from YCS
  $ycs_item_payload = array(
    'lf_id' => $item['lf_id'],
    'signature' => $option_result['signature'],
    'site' => $item_site,
  );
  $ycs_product = ydesign_services_cart_get_ycs_product($ycs_item_payload);
  
  if (empty($ycs_product)) {
    return FALSE;
  }

  // If product is found, create it in the cart
  $base_price = $ycs_product['price'];
  $sale_price = $ycs_product['sale_price'];
  $on_sale = $ycs_product['on_sale'];

  // Set metadata
  $product_orig = commerce_product_load_by_sku($ycs_product['product_id']);
  $product = $product_orig ? $product_orig : commerce_product_new('product');
  $product->uid = 1;
  $product->status = TRUE;
  $product->title = htmlspecialchars_decode($item['data']->title->value());
  $product->sku = $ycs_product['product_id'];
  $product->created = $product->changed = REQUEST_TIME;
  $item_code = $item['data']->field_item_code->value();
  $product->field_code['und'][0]['value'] = $item_code;
  $product->field_vendor_code['und'][0]['value'] = $item['data']->field_item_vendor_code->value();
  $product->field_id['und'][0]['value'] = $item['lf_id'];
  $product->field_vendor_part_number['und'][0]['value'] = $ycs_product['vendorpartnumber'];
  $product->field_options_string['und'][0]['value'] = $option_result['signature'];
  $product->field_cost['und'][0]['value'] = $ycs_product['cost'];
  
  // Set price
  $price = array(
    LANGUAGE_NONE => array(
      0 => array(
        'amount' => $base_price * 100,
        'currency_code' => commerce_default_currency(),
  )));
  $product->commerce_price = $price;
  
  $sale_price = array(
    LANGUAGE_NONE => array(
      0 => array(
        'amount' => $sale_price * 100,
        'currency_code' => commerce_default_currency(),
  )));
  $product->field_commerce_saleprice = $sale_price;
  $product->field_commerce_saleprice_on_sale['und'][0]['value'] = $on_sale;
  
  // set taxonomy options
  if (count($option_result['option_values'])) {
    $option_count = 1;
    foreach ($option_result['option_values'] as $option_value) {
      $field_name = 'option_' . $option_count;
      $term_id = ydesign_services_cart_get_term_id($field_name, $option_value);
      $product->{'field_' . $field_name}['und'][0]['tid'] = $term_id;
      $option_count++;
    }
  }

  commerce_product_save($product);
  
  // Attach the newly created product to the item
  if ($product->product_id) {
    $item['data']->field_product[] = $product->product_id;
    $item['data']->save();
    return $get_sku ? $ycs_product['product_id'] : $product->product_id;
  }
  
  return FALSE;
}

function ydesign_services_cart_get_ycs_product($item = array()) {
  $data = array(
    'lf_id' => $item['lf_id'],
    'signature' => $item['signature'],
  );
  $options = array(
    'method' => 'POST',
    'data' => drupal_http_build_query($data),
    'timeout' => 5,
    'headers' => array('Content-Type' => 'application/x-www-form-urlencoded'),
  );
  
  // get the product from yl or yv
  $ycs_site_api = 'https://yv.ylightingcs.com/api/get_product.php';
  if (isset($item['site']) && $item['site'] == 'yl') {
    $ycs_site_api = 'http://www.ylightingcs.com/api/get_product.php';
  }

  $response = array();
  $result = drupal_http_request($ycs_site_api, $options);
  if (isset($result->code) && $result->code > 0 && isset($result->data)) {
    $response = json_decode($result->data, TRUE);
  }
  else {
    watchdog('ydesign_services_cart', 'Product was not found or created in YCS: '
      . print_r($item, TRUE));
  }
  
  return $response;
}

function ydesign_services_cart_match_options($source_item_options, $cart_item_options) {
  $additional_price = 0;
  $matched_option = FALSE;
  
  if ($cart_item_options && $source_item_options) {
    foreach ($cart_item_options as $name => $value) {
      if (isset($source_item_options[$name])) {
        foreach ($source_item_options[$name] as $source_value) {
          $source_value_no_price = preg_replace('/(\s\([\+,\-]\$[0-9,]+(\.[0-9]{2})?\))/', '', $source_value);
          if ($source_value_no_price == $value) {
            $matched_option = TRUE;
            $price_matches = array();
            preg_match('/(\s\([\+,\-]\$[0-9,]+(\.[0-9]{2})?\))/', $source_value, $price_matches);
            if (isset($price_matches[0]) && $price_matches[0]) {
              $additional_price += (float) str_replace(array('(', '+', '$', ')'), array('', '', '', ''), $price_matches[0]);
            }
            break;
          }
        }
      }
    }
  }
  elseif (empty($cart_item_options) && empty($source_item_options)) {
    $additional_price = 0;
    $matched_option = TRUE;
  }
  
  return array(
    'additional_price' => $additional_price,
    'matched_option' => $matched_option,
  );
}

function ydesign_services_cart_get_term_id($vocabulary_name, $term_name) {
  // get vocab id
  $query = db_select('taxonomy_vocabulary', 'tv');
  $query->addField('tv', 'vid');
  $query->condition('tv.machine_name', $vocabulary_name);
  $result = $query->execute();
  $result_vid = $result->fetchObject();
  $vid = $result_vid->vid;
  
  // get term id
  $query = db_select('taxonomy_term_data', 'ttd');
  $query->addField('ttd', 'tid');
  $query->condition('ttd.vid', $vid);
  $query->condition('ttd.name', $term_name);
  $result = $query->execute();
  $result_tid = $result->fetchObject();
  $tid = $result_tid ? $result_tid->tid : NULL;
  
  if (empty($tid)) {
    $new_term = array(
      'vid' => $vid,
      'name' => $term_name,
    );
    $new_term = (object) $new_term;
    taxonomy_term_save($new_term);
    return $new_term->tid;
  }
  
  return $tid;
}

function ydesign_services_cart_get_item($lf_id) {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
    ->entityCondition('bundle', 'product_display')
    ->propertyCondition('status', 1)
    ->fieldCondition('field_item_id', 'value', $lf_id);  
  $result = $query->execute();
  
  $item_data = array();
  if (isset($result['node'])) {
    $nid = array(max(array_keys($result['node'])));
    if ($nid) {
      $item_load = entity_load('node', $nid);
      $item_data = entity_metadata_wrapper('node', array_pop($item_load));
    }
  }
  
  return $item_data;
}

function ydesign_services_cart_parse_options($item_options) {
  $options = array();
  $option_values = array();
  $additional_price = 0;
  if (count($item_options)) {
    foreach ($item_options as $name => $value) {
      $price_matches = array();
      preg_match('/(\s\([\+,\-]\$[0-9,]+(\.[0-9]{2})?\))/', $value, $price_matches);
      if (isset($price_matches[0]) && $price_matches[0]) {
        $additional_price += (float) str_replace(array('(', '+', '$', ')'), array('', '', '', ''), $price_matches[0]);
      }

      // Remove prices in option value, like (+$253.00) 
      $value = preg_replace('/(\s\([\+,\-]\$[0-9,]+(\.[0-9]{2})?\))/', '', $value);

      // Convert underscores to spaces in array keys.
      // @see http://www.php.net/manual/en/language.variables.external.php
      $name = str_replace('__', '. ', $name);
      $name = str_replace('_', ' ', $name);
      $options[] = $name . '==' . trim($value);
      $option_values[$name] = $value;
    }
  }
  
  return array(
    'signature' => implode('~~', $options),
    'option_values' => $option_values,
    'additional_price' => $additional_price,
  );
}

/**
 * Generate all possible permutations of the given product options.
 * 
 * @param type $array
 * @return type 
 */
function ydesign_services_cart_options_permutation($array) {
  $list = array();
  $array_count = count($array);

  $number_of_permutations = 1;
  if ($array_count > 1) {
    for ($i = 1; $i <= $array_count; $i++) {
      $number_of_permutations *= $i;
    }
  }

  for ($i=0; count($list) < $number_of_permutations; $i++) {
    shuffle($array);
    $tmp = implode('~~', $array);
    if (!isset($list[$tmp])) {
      $list[$tmp] = 1;
    }
  }

  ksort($list);
  $list = array_keys($list);
  return $list;
}

/**
 * Get products by lf_id.
 * 
 * @param type $id
 * @return type 
 */
function ydesign_services_cart_get_all_products($id, $permutation = '') {
  $query = db_select('field_data_field_id', 'fi');
  $query->leftJoin('field_data_field_options_string', 'fo', 'fi.entity_id = fo.entity_id');
  $query->leftJoin('commerce_product', 'cp', 'cp.product_id = fi.entity_id');
  //$query = db_select('commerce_product', 'cp');
  //$query->leftJoin('field_data_field_options_string', 'fo', 'cp.product_id = fo.entity_id');
  //$query->leftJoin('field_data_field_id', 'fi', 'cp.product_id = fi.entity_id');
  $query->addField('cp', 'product_id');
  $query->addField('cp', 'sku');
  $query->addField('fo', 'field_options_string_value');
  
  $query->condition('cp.status', 1);
  $query->condition('fi.field_id_value', $id);
  
  if ($permutation) {
    $query->condition('fo.field_options_string_value', $permutation);
  }
  $query->orderBy('product_id', 'DESC'); 
  $result = $query->execute();
  
  $products = array();
  foreach ($result->fetchAll() as $product) {
    $products[] = array(
      'product_id' => $product->product_id,
      'options' => $product->field_options_string_value,
      'sku' => $product->sku,
    );
  }
  return $products;
}

/**
 * a faster array_diff 
 */
function fast_array_diff($a, $b) {
  $map = $out = array();
  foreach ($a as $val) {
    $map[$val] = 1;
  }
  foreach ($b as $val) {
    unset($map[$val]);
  }
  return array_keys($map);
}

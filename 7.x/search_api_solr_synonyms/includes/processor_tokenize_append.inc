<?php

/**
 * Processor for tokenizing and appending the values.
 * For example, convert the value from "WAC-T4" to "WAC-T4 WAC T4".
 */
class SearchApiTokenizeAppend extends SearchApiAbstractProcessor {

  protected function processFieldValue(&$value) {
    $value = trim($value);
    if (!empty($value) && is_string($value)) {
      $words = explode(" ", $value);
      $w_set = array();
      foreach ($words as $w) {
        $w_set = array_merge($w_set, explode("-", $w));
      }
      $w_set = array_unique($w_set);
      $value .= ' ' . implode(" ", $w_set);
    }
  }

}

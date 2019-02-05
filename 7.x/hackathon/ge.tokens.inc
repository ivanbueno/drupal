<?php

/**
 * Implements hook_token_info().
 */
function ge_token_info() {
  $info = array();
  
  $info['tokens']['user'] = array(
    'sso' => array(
      'name' => t('SSO ID'),
      'description' => t('9 digit SSO ID of the current user'),
    ),
    'sso-mail' => array(
      'name' => t('SSO E-mail'),
      'description' => t('SSO Email of the current user'),
    ),
    'sso-fullname' => array(
      'name' => t('SSO Full Name'),
      'description' => t('SSO Full Name of the current user'),
    ),
    'sso-businessunit' => array(
      'name' => t('SSO Business Unit'),
      'description' => t('SSO Business Unit of the current user, eg. Global Research - Americas'),
    ),
    'sso-phone' => array(
      'name' => t('SSO Phone'),
      'description' => t('SSO phone of the current user'),
    ),
    'sso-firstname' => array(
      'name' => t('SSO First Name'),
      'description' => t('SSO First Name of the current user'),
    ),
    'sso-lastname' => array(
      'name' => t('SSO Last Name'),
      'description' => t('SSO Last Name of the current user'),
    ),
    'sso-title' => array(
      'name' => t('SSO Title'),
      'description' => t('SSO Title of the current user'),
    ),
    'sso-business-segment' => array(
      'name' => t('SSO Business Segment'),
      'description' => t('SSO Business segment of the current user, eg. Global Research'),
    ),
  );
  
  return $info;
}

/**
 * Implements hook_tokens().
 */
function ge_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $replacements = array();
  
  if ($type == 'user') {
    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'sso':
          $replacements[$original] = isset($_SERVER['HTTP_SM_USER']) ? $_SERVER['HTTP_SM_USER'] : '';
          break;
        
        case 'sso-mail':
          $replacements[$original] = isset($_SERVER['HTTP_SM_MAIL']) ? $_SERVER['HTTP_SM_MAIL'] : '';
          break;
        
        case 'sso-fullname':
          $replacements[$original] = isset($_SERVER['HTTP_SM_FULLNAME']) ? $_SERVER['HTTP_SM_FULLNAME'] : '';
          break;
        
        case 'sso-businessunit':
          $replacements[$original] = isset($_SERVER['HTTP_SM_GESSOBUSINESSUNIT']) ? $_SERVER['HTTP_SM_GESSOBUSINESSUNIT'] : '';
          break;
        
        case 'sso-phone':
          $replacements[$original] = isset($_SERVER['HTTP_SM_TELEPHONENUMBER ']) ? $_SERVER['HTTP_SM_TELEPHONENUMBER '] : '';
          break;
        
        case 'sso-firstname':
          $replacements[$original] = isset($_SERVER['HTTP_SM_FIRST_NAME']) ? $_SERVER['HTTP_SM_FIRST_NAME'] : '';
          break;
        
        case 'sso-lastname':
          $replacements[$original] = isset($_SERVER['HTTP_SM_LAST_NAME']) ? $_SERVER['HTTP_SM_LAST_NAME'] : '';
          break;
        
        case 'sso-title':
          $replacements[$original] = isset($_SERVER['HTTP_SM_TITLE']) ? $_SERVER['HTTP_SM_TITLE'] : '';
          break;
        
        case 'sso-business-segment':
          $replacements[$original] = isset($_SERVER['HTTP_SM_GEHRBUSINESSSEGMENT']) ? $_SERVER['HTTP_SM_GEHRBUSINESSSEGMENT'] : '';
          break;
      }
    }
  }
  
  return $replacements;
}

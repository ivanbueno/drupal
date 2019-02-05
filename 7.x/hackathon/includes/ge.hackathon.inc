<?php

function ge_hackathon_project_join_form($form, &$form_state, $event_node, $project_node) {

  module_load_include('inc', 'ge', 'includes/hackathon.class');

  $project_node_wrapper = entity_metadata_wrapper('node', $project_node);
  $hackathon_project = new HackathonProject($project_node_wrapper);

  $roles_needed = $hackathon_project->get_roles_needed();
  $allow_join = $hackathon_project->allow_project_join() && $hackathon_project->is_project_open();

  $form = array();
  $form['node'] = array(
    '#type' => 'value',
    '#value' => $project_node,
  );
  if (!$allow_join) {
    $form['heading_msg'] = array(
      '#markup' => '<div class="topmargin style-msg errormsg">
        <div class="sb-msg">
          <i class="icon-remove"></i><strong>Oh snap!</strong> Either you are already a member/owner of a project or this project is full.
          <a class="btn" href="/' . drupal_get_path_alias('node/' . $project_node->nid) . '">Go back to the project.</a>
        </div>
      </div>',
    );
  }
  $form['heading'] = array(
    '#markup' => '<h3 class="topmargin-sm"><a href="/' . drupal_get_path_alias('node/' . $event_node->nid) . '">' . $event_node->title . '</a> > <a href="/' . drupal_get_path_alias('node/' . $project_node->nid) . '">' . $project_node_wrapper->title->value() . '</a> > Join</h3>',
  );
  $form['role'] = array(
    '#type' => 'radios',
    '#title' => t('Select your preferred role'),
    '#options' => $roles_needed,
    '#required' => TRUE,
    '#disabled' => !$allow_join,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Join the Project'),
    '#disabled' => !$allow_join,
  );

  return $form;
}


/**
 * The submit function.
 */
function ge_hackathon_project_join_form_submit($form, &$form_state) {
  global $user;
  $role = $form_state['values']['role'];
  $node = $form_state['values']['node'];

  $fc_item = entity_create('field_collection_item', array('field_name' => 'field_project_members'));
  $fc_item->setHostEntity('node', $node);
  $fc_wrapper = entity_metadata_wrapper('field_collection_item', $fc_item);
  $fc_wrapper->field_member_name->set($user->uid);
  $fc_wrapper->field_member_role->set($role);
  $fc_wrapper->save(TRUE);
  node_save($node);

  rules_invoke_event('ge_hackathon_project_member_join', $user, $node);

  drupal_set_message('You have successfully joined the <strong>' . $node->title . '</strong> project.');
}

function ge_hackathon_project_leave_form($form, &$form_state, $event_node, $project_node) {

  global $user;
  module_load_include('inc', 'ge', 'includes/hackathon.class');

  $project_node_wrapper = entity_metadata_wrapper('node', $project_node);
  $hackathon_project = new HackathonProject($project_node_wrapper);
  $allow_leave = $hackathon_project->is_project_member($user->uid);
  $member_instance = $hackathon_project->search_project_member($user->uid);

  $form = array();
  $form['node'] = array(
    '#type' => 'value',
    '#value' => $project_node,
  );
  if (!$allow_leave) {
    $form['heading_msg'] = array(
      '#markup' => '<div class="topmargin style-msg errormsg">
        <div class="sb-msg">
          <i class="icon-remove"></i><strong>Oh snap!</strong> You cannot leave something you are not a part of.
          <a class="btn" href="/' . drupal_get_path_alias('node/' . $project_node->nid) . '">Go back to the project.</a>
        </div>
      </div>',
    );
  }
  $form['heading'] = array(
    '#markup' => '<h3 class="topmargin-sm"><a href="/' . drupal_get_path_alias('node/' . $event_node->nid) . '">' . $event_node->title . '</a> > <a href="/' . drupal_get_path_alias('node/' . $project_node->nid) . '">' . $project_node_wrapper->title->value() . '</a> > Leave</h3>',
  );
  if ($allow_leave) {
    $form['heading_info'] = array(
      '#markup' => '<p>You are one of the <u>' . $member_instance['role'] . '</u> of <a href="/' . drupal_get_path_alias('node/' . $project_node->nid) . '">' . $project_node_wrapper->title->value() . '</a>.</p> <p>Are you sure you want to leave?</p>',
    );
    $form['member_delta'] = array(
      '#type' => 'value',
      '#value' => $member_instance['delta'],
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Leave the team'),
    '#disabled' => !$allow_leave,
  );

  return $form;
}

/**
 * The submit function.
 */
function ge_hackathon_project_leave_form_submit($form, &$form_state) {
  global $user;
  $node = $form_state['values']['node'];
  $member_delta = $form_state['values']['member_delta'];

  $field_collection_item_value = $node->field_project_members[$node->language][$member_delta]['value'];
  entity_delete_multiple('field_collection_item', array($field_collection_item_value));

  rules_invoke_event('ge_hackathon_project_member_leave', $user, $node);

  drupal_set_message('You left the <strong>' . $node->title . '</strong> team.');
}

function ge_hackathon_project_decline_form($form, &$form_state, $event_node, $project_node) {

  global $user;
  module_load_include('inc', 'ge', 'includes/hackathon.class');

  $project_node_wrapper = entity_metadata_wrapper('node', $project_node);
  $hackathon_project = new HackathonProject($project_node_wrapper);
  $is_organizer = $hackathon_project->is_organizer();
  $is_project_owner = $hackathon_project->is_project_owner();

  $action_text = $is_project_owner ? 'Cancel' : 'Decline';

  $form = array();
  $form['node_wrapper'] = array(
    '#type' => 'value',
    '#value' => $project_node_wrapper,
  );
  $form['action_text'] = array(
    '#type' => 'value',
    '#value' => $action_text,
  );
  $form['node'] = array(
    '#type' => 'value',
    '#value' => $project_node,
  );
  if (!($is_organizer || $is_project_owner)) {
    $form['heading_msg'] = array(
      '#markup' => '<div class="topmargin style-msg errormsg">
        <div class="sb-msg">
          <i class="icon-remove"></i><strong>Oh snap!</strong> You are not a project owner or event organizer.
          <a class="btn" href="/' . drupal_get_path_alias('node/' . $project_node->nid) . '">Go back to the project.</a>
        </div>
      </div>',
    );
  }
  $form['heading'] = array(
    '#markup' => '<h3 class="topmargin-sm"><a href="/' . drupal_get_path_alias('node/' . $event_node->nid) . '">' . $event_node->title . '</a> > <a href="/' . drupal_get_path_alias('node/' . $project_node->nid) . '">' . $project_node_wrapper->title->value() . '</a> > ' . $action_text . '</h3>',
  );
  if ($is_organizer || $is_project_owner) {
    $form['heading_info'] = array(
      '#markup' =>  '<p>Are you sure you want to ' . $action_text . ' this project from the event?</p>',
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('!action the project', array('!action' => $action_text)),
    '#disabled' => !($is_organizer || $is_project_owner),
  );

  return $form;
}

/**
 * The submit function.
 */
function ge_hackathon_project_decline_form_submit($form, &$form_state) {
  global $user;
  $node_wrapper = $form_state['values']['node_wrapper'];
  $node_wrapper->field_project_declined = 1;
  $node_wrapper->save();

  rules_invoke_event('ge_hackathon_project_declined', $user, $form_state['values']['node']);

  $action_text_past = $form_state['values']['action_text'] == 'Cancel' ? 'cancelled' : 'declined';
  drupal_set_message('You ' . $action_text_past . ' this project: <strong>' . $node_wrapper->title->value() . '</strong>.');
}

function ge_hackathon_project_accept_form($form, &$form_state, $event_node, $project_node) {

  global $user;
  module_load_include('inc', 'ge', 'includes/hackathon.class');

  $project_node_wrapper = entity_metadata_wrapper('node', $project_node);
  $hackathon_project = new HackathonProject($project_node_wrapper);
  $is_organizer = $hackathon_project->is_organizer();

  $form = array();
  $form['node_wrapper'] = array(
    '#type' => 'value',
    '#value' => $project_node_wrapper,
  );
  $form['node'] = array(
    '#type' => 'value',
    '#value' => $project_node,
  );
  if (!$is_organizer) {
    $form['heading_msg'] = array(
      '#markup' => '<div class="topmargin style-msg errormsg">
        <div class="sb-msg">
          <i class="icon-remove"></i><strong>Oh snap!</strong> You are not an organizer of the event.
          <a class="btn" href="/' . drupal_get_path_alias('node/' . $project_node->nid) . '">Go back to the project.</a>
        </div>
      </div>',
    );
  }
  $form['heading'] = array(
    '#markup' => '<h3 class="topmargin-sm"><a href="/' . drupal_get_path_alias('node/' . $event_node->nid) . '">' . $event_node->title . '</a> > <a href="/' . drupal_get_path_alias('node/' . $project_node->nid) . '">' . $project_node_wrapper->title->value() . '</a> > Accept</h3>',
  );
  if ($is_organizer) {
    $form['heading_info'] = array(
      '#markup' =>  '<p>Are you sure you want to accept this project back to the event?</p>',
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Accept the project'),
    '#disabled' => !$is_organizer,
  );

  return $form;
}

/**
 * The submit function.
 */
function ge_hackathon_project_accept_form_submit($form, &$form_state) {
  global $user;
  $node_wrapper = $form_state['values']['node_wrapper'];
  $node_wrapper->field_project_declined = 0;
  $node_wrapper->save();

  rules_invoke_event('ge_hackathon_project_accepted', $user, $form_state['values']['node']);

  drupal_set_message('You accepted this project: <strong>' . $node_wrapper->title->value() . '</strong>.');
}

function ge_hackathon_project_transfer_form($form, &$form_state, $event_node, $project_node) {

  global $user;
  module_load_include('inc', 'ge', 'includes/hackathon.class');

  $project_node_wrapper = entity_metadata_wrapper('node', $project_node);
  $hackathon_project = new HackathonProject($project_node_wrapper);
  $is_project_owner = $hackathon_project->is_project_owner();
  $is_organizer = $hackathon_project->is_organizer();
  $project_members = $hackathon_project->get_members();
  $new_champions = array();
  foreach ($project_members as $project_m_uid => $project_m) {
    if ($user->uid <> $project_m_uid) {
      $new_champions[$project_m_uid] = $project_m['name'] . ' (' . $project_m['sso'] . ')';
    }
  }
  $has_champions = !empty($new_champions);

  $form = array();
  $form['node_wrapper'] = array(
    '#type' => 'value',
    '#value' => $project_node_wrapper,
  );
  if (!($is_project_owner || $is_organizer)) {
    $form['heading_msg'] = array(
      '#markup' => '<div class="topmargin style-msg errormsg">
        <div class="sb-msg">
          <i class="icon-remove"></i><strong>Oh snap!</strong> You are not the owner of this project.
          <a class="btn" href="/' . drupal_get_path_alias('node/' . $project_node->nid) . '">Go back to the project.</a>
        </div>
      </div>',
    );
  }
  if (!$has_champions) {
    $form['heading_msg'] = array(
      '#markup' => '<div class="topmargin style-msg errormsg">
        <div class="sb-msg">
          <i class="icon-remove"></i><strong>Oh snap!</strong> Your project needs members.
          <a class="btn" href="/' . drupal_get_path_alias('node/' . $project_node->nid) . '">Go back to the project.</a>
        </div>
      </div>',
    );
  }
  $form['heading'] = array(
    '#markup' => '<h3 class="topmargin-sm"><a href="/' . drupal_get_path_alias('node/' . $event_node->nid) . '">' . $event_node->title . '</a> > <a href="/' . drupal_get_path_alias('node/' . $project_node->nid) . '">' . $project_node_wrapper->title->value() . '</a> > Transfer Ownership</h3>',
  );
  $form['champion'] = array(
    '#type' => 'radios',
    '#title' => t('Select the new Project Champion'),
    '#options' => $new_champions,
    '#required' => TRUE,
    '#disabled' => !($is_project_owner || $is_organizer) || !$has_champions,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Transfer ownership'),
    '#disabled' => !($is_project_owner || $is_organizer) || !$has_champions,
  );

  return $form;
}

/**
 * The submit function.
 */
function ge_hackathon_project_transfer_form_submit($form, &$form_state) {
  global $user;
  $node_wrapper = $form_state['values']['node_wrapper'];
  $node_wrapper->author = $form_state['values']['champion'];
  $node_wrapper->save();

  drupal_set_message('You transferred ownership of the project: <strong>' . $node_wrapper->title->value() . '</strong>.');
  drupal_goto('node/' . $node_wrapper->nid->value());
}

function ge_hackathon_event_mass_mail_form($form, &$form_state, $event_node) {
  module_load_include('inc', 'ge', 'includes/hackathon.class');
  $event_node_wrapper = entity_metadata_wrapper('node', $event_node);
  $hackathon_event = new Hackathon($event_node);
  $is_organizer = $hackathon_event->is_organizer();
  $event_participants = $hackathon_event->get_participants();

  $form = array();

  if ($is_organizer) {
    $form['node'] = array(
      '#type' => 'value',
      '#value' => $event_node,
    );
    $form['participants'] = array(
      '#type' => 'value',
      '#value' => $event_participants,
    );
    $form['news'] = array(
      '#type' => 'fieldset',
      '#title' => t('Post an announcement'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['news']['subject'] = array(
      '#type' => 'textfield',
      '#attributes' => array(
        'placeholder' => t('Subject (required)'),
      ),
      '#required' => TRUE,
    );
    $form['news']['message'] = array(
      '#type' => 'textarea',
      '#description' => t('All participants will receive an email notification.'),
      '#attributes' => array(
        'placeholder' => t('Message (required)'),
      ),
      '#required' => TRUE,
    );
    $form['news']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Post news'),
    );
  }

  return $form;
}

function ge_hackathon_event_mass_mail_form_submit($form, &$form_state) {
  global $user;

  $event_node = $form_state['values']['node'];
  $subject = trim($form_state['values']['subject']);
  $message = trim($form_state['values']['message']);
  $participants = $form_state['values']['participants'];

  // save as announcement node
  $node = entity_create('node', array('type' => 'announcement'));
  $node->uid = $user->uid;
  $node_wrapper = entity_metadata_wrapper('node', $node);
  $node_wrapper->title = $subject;
  $node_wrapper->body->set(array('value' => $message));
  $node_wrapper->field_parent_event->set($event_node->nid);
  $node_wrapper->save();

  // send email
  $from = $user->mail;
  $to = variable_get('site_mail', ini_get('sendmail_from'));
  $recipients = array_chunk($participants, 100);
  foreach ($recipients as $recipient_set) {
    $bcc_recipients = implode(', ', $recipient_set);

    $params = array(
      'body' => $message,
      'subject' => 'Hackathon - ' . $subject,
      'headers' => array(
        'Bcc' => $bcc_recipients,
      )
    );

    $success = drupal_mail('ge', 'event_news_notify', $to, language_default(), $params, $from);

    if ($success['result']) {
      watchdog('ge', 'Email (' . $subject . ') sent to participants: <br />' . $bcc_recipients);
    }
  }

}

function ge_hackathon_project_complete_form($form, &$form_state, $event_node, $project_node) {

  global $user;
  module_load_include('inc', 'ge', 'includes/hackathon.class');

  $project_node_wrapper = entity_metadata_wrapper('node', $project_node);
  $hackathon_project = new HackathonProject($project_node_wrapper);
  $is_project_owner = $hackathon_project->is_project_owner();

  $form = array();
  $form['node_wrapper'] = array(
    '#type' => 'value',
    '#value' => $project_node_wrapper,
  );
  $form['node'] = array(
    '#type' => 'value',
    '#value' => $project_node,
  );
  if (!$is_project_owner) {
    $form['heading_msg'] = array(
      '#markup' => '<div class="topmargin style-msg errormsg">
        <div class="sb-msg">
          <i class="icon-remove"></i><strong>Oh snap!</strong> You are not the project owner.
          <a class="btn" href="/' . drupal_get_path_alias('node/' . $project_node->nid) . '">Go back to the project.</a>
        </div>
      </div>',
    );
  }
  $form['heading'] = array(
    '#markup' => '<h3 class="topmargin-sm"><a href="/' . drupal_get_path_alias('node/' . $event_node->nid) . '">' . $event_node->title . '</a> > <a href="/' . drupal_get_path_alias('node/' . $project_node->nid) . '">' . $project_node_wrapper->title->value() . '</a> > Submit for Review</h3>',
  );
  if ($is_project_owner) {
    $form['heading_info'] = array(
      '#markup' =>  '<p>Is the project complete? Are you sure you want to submit this project for review?</p>',
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit the project for review'),
    '#disabled' => !$is_project_owner,
  );

  return $form;
}

/**
 * The submit function.
 */
function ge_hackathon_project_complete_form_submit($form, &$form_state) {
  global $user;
  $node_wrapper = $form_state['values']['node_wrapper'];
  $node_wrapper->field_project_completed = 1;
  $node_wrapper->save();

  rules_invoke_event('ge_hackathon_project_completed', $user, $form_state['values']['node']);

  drupal_set_message('You submitted this project for review: <strong>' . $node_wrapper->title->value() . '</strong>.');
}

function ge_hackathon_project_withdraw_form($form, &$form_state, $event_node, $project_node) {

  global $user;
  module_load_include('inc', 'ge', 'includes/hackathon.class');

  $project_node_wrapper = entity_metadata_wrapper('node', $project_node);
  $hackathon_project = new HackathonProject($project_node_wrapper);
  $is_project_owner = $hackathon_project->is_project_owner();

  $form = array();
  $form['node_wrapper'] = array(
    '#type' => 'value',
    '#value' => $project_node_wrapper,
  );
  $form['node'] = array(
    '#type' => 'value',
    '#value' => $project_node,
  );
  if (!$is_project_owner) {
    $form['heading_msg'] = array(
      '#markup' => '<div class="topmargin style-msg errormsg">
        <div class="sb-msg">
          <i class="icon-remove"></i><strong>Oh snap!</strong> You are not the project owner.
          <a class="btn" href="/' . drupal_get_path_alias('node/' . $project_node->nid) . '">Go back to the project.</a>
        </div>
      </div>',
    );
  }
  $form['heading'] = array(
    '#markup' => '<h3 class="topmargin-sm"><a href="/' . drupal_get_path_alias('node/' . $event_node->nid) . '">' . $event_node->title . '</a> > <a href="/' . drupal_get_path_alias('node/' . $project_node->nid) . '">' . $project_node_wrapper->title->value() . '</a> > Withdraw from Review</h3>',
  );
  if ($is_project_owner) {
    $form['heading_info'] = array(
      '#markup' =>  '<p>Are you sure you want to withdraw this project from review?</p>',
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Withdraw the project from review'),
    '#disabled' => !$is_project_owner,
  );

  return $form;
}

/**
 * The submit function.
 */
function ge_hackathon_project_withdraw_form_submit($form, &$form_state) {
  global $user;
  $node_wrapper = $form_state['values']['node_wrapper'];
  $node_wrapper->field_project_completed = 0;
  $node_wrapper->save();

  rules_invoke_event('ge_hackathon_project_withdrawn', $user, $form_state['values']['node']);

  drupal_set_message('You withdrew this project from review: <strong>' . $node_wrapper->title->value() . '</strong>.');
}